#include "DiskCache.h"
#include "http/Downloader.h"
#include "io/FileSystem.h"
#include "string/FilenameUtils.h"
#include "xml/xml.h"
#include <cstdio>
#include <fstream>
#include <iostream>

//------------------------------------------------------------------------------

class CacheEntry
{
public:
   CacheEntry(){}
   virtual ~CacheEntry(){}

   std::string url;
   std::string localfile;
   size_t      size;
};

//------------------------------------------------------------------------------

BeginPropertyMap(CacheEntry);
   XMLProperty(CacheEntry, "url", url);
   XMLProperty(CacheEntry, "local", localfile);
   XMLProperty(CacheEntry, "filesize", size);
EndPropertyMap(CacheEntry);


//------------------------------------------------------------------------------

MemoryEntry::~MemoryEntry()
{
   if (_pData)
   {
      delete _pData;
      _pData = 0;
      _nDataSize = 0;
   }
}

//------------------------------------------------------------------------------

FileEntry::~FileEntry()
{
}


//------------------------------------------------------------------------------

Cache::Cache(const std::string& basedir, double diskcap, double memcap, unsigned int numDownloadThreads)  
   : _basedir(basedir), _maxDiskCacheCapacity(diskcap), 
     _maxMemoryCacheCapacity(memcap),  
     _curDiskCacheCapacity(0.0), 
     _curMemoryCacheCapacity(0.0),
     _maxDiskCacheEntries(16777216),
     _maxMemoryCacheEntries(16777216),
     _numDownloadThreads(numDownloadThreads),
     _maxProcess(1),_curProcess(0), _increment(1)
{ 
   atomic_set(_bThreadsDone, false);

   _basedir = FilenameUtils::DelimitPath(_basedir);

   // Create Threads
   boost::function0<void> f =  boost::bind( &Cache::ProcessDownloads, this );
   for (unsigned int i=0;i<_numDownloadThreads;++i)
   {
      _threads.create_thread(f);
   }


   // Check if directory exists:
   if (!FileSystem::DirExists(_basedir))
   {
      FileSystem::makeallsubdirs(_basedir);
   }

   std::vector<std::string> files = FileSystem::GetFilesInDirectory(_basedir, ".cache");

   for (size_t i=0;i<files.size();i++)
   {
      //std::cout << "*CACHE FILE FOUND : " << files[i] << "\n";

      std::ifstream fin;
      fin.open(files[i].c_str(), std::ios::in|std::ios::binary);

      if (fin.good())
      {
         CacheEntry* pCE = (CacheEntry*)access::Class::FromXML(fin, "CacheEntry");
         if (pCE)
         {
             FileEntry* v= new FileEntry();
             v->_SetLocalFile(pCE->localfile);
             v->_SetFilesize(pCE->size);
             v->_SetReady();
             insertDiskEntry(pCE->url,v);

             _curDiskCacheCapacity += (double)v->FileSize() / 1024.0 / 1024.0; // in MB

            delete pCE;
         }
      }

   }


}

//------------------------------------------------------------------------------

Cache::~Cache()
{
   wait();

   //---------------------------------------------------------------------------
   // DELETE DISK CACHE:

   DiskCache_t::right_const_iterator it = _diskcache.right.begin();

   while (it != _diskcache.right.end())
   {
      //std::string s1 = it->second; // key
      FileEntry* f = it->info;
      delete f; // free memory;
      ++it;
   }

   _diskcache.clear();

   //---------------------------------------------------------------------------
   // DELETE MEMORY CACHE

   MemoryCache_t::right_const_iterator jt = _memcache.right.begin();

   while (jt != _memcache.right.end())
   {
      MemoryEntry* m = jt->info;
      delete m; // free memory;
      ++jt;
   }

   _memcache.clear();
}

//------------------------------------------------------------------------------
void Cache::_doRead(MemoryEntry* pEntry, const std::string& sFilename)
{
   size_t nSize;
   unsigned char* pData;

   if (FileSystem::FileToAllocedMemory(sFilename, pData, nSize))
   {
      pEntry->_SetData(pData, nSize);
      pEntry->_SetReady();
      _curMemoryCacheCapacity += (double)nSize / 1024.0 / 1024.0; // in MB
   }
   else
   {
      pEntry->_SetFailed();
   }
}

// Retrieve Data
MemoryEntry* Cache::_LoadLocalFile(const std::string& k, bool async, bool relative, boost::function<void(const std::string&, MemoryEntry*, void*)> callback, boost::function<void(const std::string&, MemoryEntry*, void*)> callback_onthread, const std::string& origURL, void* userdata)
{
   const MemoryCache_t::left_iterator it =_memcache.left.find(k); 

   if (it==_memcache.left.end()) 
   { 
      // We don't have it: 
      // Evaluate function and create new record 
      MemoryEntry* v= new MemoryEntry();
      insertMemoryEntry(k,v); 

      std::string sFilename;
      if (relative)
      {
         sFilename = _basedir + k;
      }
      else
      {
         sFilename = k;
      }

      // async download: create a "read from disk job"
      if (async)
      {
          // create new job
          DownloadJob job;
          job.vMem = v;
          job.vFile = 0;
          job.local_file = sFilename;
          job.url = origURL;
          job.onload = callback;
          job.onthread = callback_onthread;
          job.userdata = userdata;
          _downloadJobs.push(job);  // push job
          _post();
      }
      else
      {
         // synchronous read...
         _doRead(v, sFilename);
         if (callback_onthread)
         {
            callback_onthread(sFilename, v, userdata);
         }
         
         if (callback)
         {
            callback(sFilename, v, userdata);
         }

         
      }
      
      return v;
   }
   else
   {
      // We do have it: 
      // Update the access record view. 
      if (!it->info->IsFailed())
         _memcache.right.relocate(_memcache.right.end(), _memcache.project_right(it)); 
      return it->info; 
   }
}

//-----------------------------------------------------------------------------

inline std::string _GenerateFilename()
{
   std::string sTempFile = std::string((char*)(tmpnam(0)));
   for (size_t i=0;i<sTempFile.length();i++)
   {
      if (sTempFile[i] == '.')
      {
         sTempFile[i] = 'o';
      }
      else if (sTempFile[i] == '\\' || sTempFile[i] == '/' || sTempFile[i] == ':')
      {
         sTempFile[i] = 'c';
      }
   }

   return sTempFile;
}

//------------------------------------------------------------------------------

void Cache::_doDownload(const std::string& url, const std::string& sTempFile, FileEntry* pFileEntry, MemoryEntry* result)
{
   std::string sTargetFilename = _basedir + sTempFile;
   downloadTask tsk(url, sTargetFilename);
   if (tsk.download())
   {
      pFileEntry->_SetFilesize((size_t)tsk.getDownloaded());
      std::string sTargetFilenameMeta = sTargetFilename + ".cache";  // _basedir + sTempFile + ".cache"; 
      std::ofstream metafile;
      metafile.open(sTargetFilenameMeta.c_str());

      CacheEntry ce;
      ce.url = url;
      ce.localfile = sTempFile;
      ce.size = pFileEntry->FileSize();
      /*const HeaderFieldDefinition& hi = tsk.GetHeaderFieldDefinitions();
      for (size_t i=0;i<hi.size();i++)
      {
         std::cout << "[" << hi[i].first << "] [" << hi[i].second << "]\n";
      }*/

      access::Class::ToXML(metafile, "CacheEntry", (void*)&ce, false); // XML without header
      
      metafile.close();
      pFileEntry->_SetReady();

      _curDiskCacheCapacity += (double)pFileEntry->FileSize() / 1024.0 / 1024.0;
      
      _doRead(result, _basedir + sTempFile);
   }
   else
   {
      pFileEntry->_SetFailed();
      result->_SetFailed();
      insertMemoryEntry(pFileEntry->LocalFile(),result);
   }
}

//------------------------------------------------------------------------------

MemoryEntry* Cache::_LoadRemoteFile(const std::string& url, FileEntry* pFileEntry, bool async, boost::function<void(const std::string&, MemoryEntry*, void*)> callback, boost::function<void(const std::string&, MemoryEntry*, void*)> callback_onthread, void* userdata)
{
   // (1) download file from url (update FileEntry)
   // (2) load file to memory (create memory entry)

   MemoryEntry* result= 0;

   // check if entry already exists for memory entry
   const MemoryCache_t::left_iterator it =_memcache.left.find(pFileEntry->LocalFile()); 
   if (it!=_memcache.left.end()) 
   {
      result = it->info;
      return result;
   }

   std::string sTempFile = _GenerateFilename();
   pFileEntry->_SetLocalFile(sTempFile);

   if (async)
   {
      // async generation
      // this is a little tricky, because when creating an async representation
      // the cache entry and memory entry must be done at the same time.
      result = new MemoryEntry();
      insertMemoryEntry(sTempFile,result); // "empty" memory entry

      // make download job with url, tempfile, fileentry, mementry
      DownloadJob job;
      job.vMem = result;
      job.vFile = pFileEntry;
      job.local_file = sTempFile;
      job.url = url;
      job.onload = callback;
      job.onthread = callback_onthread;
      job.userdata = userdata;
      _downloadJobs.push(job);  // push job
      _post();
      return result;

   }
   else
   {
      //**************************************************
      // synchronous download and cache generation of file
      //**************************************************
      //std::cout << "**SYNCHRONOUS DOWNLOAD: " << url << "\n";
      result= new MemoryEntry();
      insertMemoryEntry(sTempFile,result);
      _doDownload(url, sTempFile, pFileEntry, result);
      
      if (callback_onthread)
      {
         callback_onthread(url, result, userdata);
      }
      
      if (callback)
      {
         callback(url, result, userdata);
      }      
      return result;

   }

   return 0;
}


MemoryEntry* Cache::_RequestData(const std::string& url, boost::function<void(const std::string&, MemoryEntry*, void*)> callback, bool async, void* userdata, boost::function<void(const std::string&, MemoryEntry*, void*)> callback_onthread)
{
   MemoryEntry* result = 0;
   int n = url.find("http://");
	if(n != 0)
   {
      result = _LoadLocalFile(url, async, false, callback, callback_onthread, url, userdata);
      return result;
   }
   else
   {
      // file on web -> disk cache required
      // Attempt to find record of the URL in disk cache
      const DiskCache_t::left_iterator it =_diskcache.left.find(url); 

      if (it==_diskcache.left.end()) 
      { 
          // create new record 
          FileEntry* v= new FileEntry();
          insertDiskEntry(url,v); 

          result = _LoadRemoteFile(url, v, async, callback, callback_onthread, userdata);
          return result;
      }
      else
      {
         // We do have it: 
         // Update the access record view. 
          
         FileEntry* entry = it->info; 
         if (!entry->IsFailed()) // failed entries never get higher
            _diskcache.right.relocate(_diskcache.right.end(), _diskcache.project_right(it));
         
         result = _LoadLocalFile(entry->LocalFile(), async, true, callback, callback_onthread, url, userdata);

         return result;
      }

   }

   return 0;
}

//------------------------------------------------------------------------------

MemoryEntry* Cache::RequestDataAsync(const std::string& url, boost::function<void(const std::string&, MemoryEntry*, void*)> callback, void* userdata)
{
   return _RequestData(url, callback, true, userdata, 0);
}

//------------------------------------------------------------------------------


MemoryEntry* Cache::RequestData(const std::string& url, boost::function<void(const std::string&, MemoryEntry*, void*)> callback, void* userdata)
{
   return _RequestData(url, callback, false, userdata, 0);
}

//------------------------------------------------------------------------------

MemoryEntry* Cache::RequestDataThreaded(const std::string& url, boost::function<void(const std::string&, MemoryEntry*, void*)> callback_onload, boost::function<void(const std::string&, MemoryEntry*, void*)> callback_onthread, void* userdata)
{
   return _RequestData(url, callback_onload, true, userdata, callback_onthread);
}

//------------------------------------------------------------------------------

void Cache::insertDiskEntry(const std::string& k, FileEntry* v) 
{ 
   // If necessary, make space 
   while (_diskcache.size()>0 && 
         (_diskcache.size()>=_maxDiskCacheEntries || _curDiskCacheCapacity > _maxDiskCacheCapacity))
   {
      std::cout << "[CACHE] Deleting Disk Cache Entry: curSize = " << _curDiskCacheCapacity << ", max = " << _maxDiskCacheCapacity << "\n";

      FileEntry* pEntry = _diskcache.right.begin()->info;
      if (!pEntry->IsReady() && !pEntry->IsFailed())
      {
         return; // can't delete at the moment
      }
      
      // by purging the least-recently-used element 
      _curDiskCacheCapacity -= ((double)pEntry->FileSize() / 1024.0 / 1024.0);
      if (_curDiskCacheCapacity < 0)  _curDiskCacheCapacity = 0;

      // delete Cache entry from disk:
      std::string sFilename = _basedir + pEntry->LocalFile();
      std::string sCacheFile = sFilename + ".cache";
      
      if (!FileSystem::rm(sCacheFile)) std::cout << "**ERROR: Failed deleting: " << sCacheFile << "\n";  
      if (!FileSystem::rm(sFilename)) std::cout << "**ERROR: Failed deleting: " << sFilename << "\n";  

      // delete entry from list
      _diskcache.right.erase(_diskcache.right.begin()); 
   } 

   _diskcache.insert(DiskCache_t::value_type(k,0,v)); 
} 

//------------------------------------------------------------------------------

void Cache::insertMemoryEntry(const std::string& k, MemoryEntry* v) 
{ 
   while (_memcache.size()>0 && 
         (_memcache.size()>=_maxMemoryCacheEntries || _curMemoryCacheCapacity > _maxMemoryCacheCapacity))
   {

      MemoryEntry* pEntry = _memcache.right.begin()->info;
      if (!pEntry->IsReady() && !pEntry->IsFailed())
      {
         return; // can't delete at the moment
      }

      std::cout << "[CACHE] Deleting Memory Cache Entry: curSize = " << _curMemoryCacheCapacity << ", max = " << _maxMemoryCacheCapacity << "\n";

      _curMemoryCacheCapacity -= ((double)pEntry->DataSize() / 1024.0 / 1024.0);
      if (_curMemoryCacheCapacity < 0) _curMemoryCacheCapacity = 0;

      delete pEntry;

      _memcache.right.erase(_memcache.right.begin()); 
  
   }

      _memcache.insert(MemoryCache_t::value_type(k,0,v)); 
} 

//------------------------------------------------------------------------------

void Cache::dump(bool memonly)
{
   if (!memonly)
   {
   // DUMPING DISK CACHE
 
   std::cout << "***********************\n";
   std::cout << "*** DISK CACHE INFO ***\n";
   std::cout << "***********************\n";
   std::cout << "Capacity: " << _curDiskCacheCapacity << "\n";
   std::cout << "Disk cache has " << _diskcache.right.size() << " entries\n";

   DiskCache_t::right_const_iterator it = _diskcache.right.begin();

   while (it != _diskcache.right.end())
   {
      FileEntry* f = it->info;
      if (f->IsReady())
      {
        std::cout << "[READY] " << it->second << " (" << f->LocalFile() << ")\n";
      }
      else
      {
         if (f->IsFailed())
         {
            std::cout << "[FAILED] " << it->second << "\n";
         }
         else
         {
            std::cout << "[WAIT] " << it->second << "\n";
         } 
      }

      ++it;
   }

   }

   std::cout << "*************************\n";
   std::cout << "*** MEMORY CACHE INFO ***\n";
   std::cout << "*************************\n";
   std::cout << "Capacity: " << _curMemoryCacheCapacity << "\n";
   std::cout << "Memory cache has " << _memcache.right.size() << " entries\n";

   MemoryCache_t::right_const_iterator jt = _memcache.right.begin();

   while (jt != _memcache.right.end())
   {
      MemoryEntry* m = jt->info;

      if (m->IsReady())
      {
         std::cout << "[READY] " << jt->second << "  (" <<  m->DataSize() << " bytes)\n";
      }
      else
      {
         if (m->IsFailed())
         {
            std::cout << "[FAILED] " << jt->second << "\n";
         }
         else
         {
            std::cout << "[WAIT] " << jt->second << "\n";
         }
      }

      jt++;
   }

}

//------------------------------------------------------------------------------

void Cache::wait()
{
   if (atomic_read(_bThreadsDone))
      return;

   atomic_set(_bThreadsDone, true);
   _post_all();
   _threads.join_all(); // wait for threads to finish...
   process();           // process last time...
}

//------------------------------------------------------------------------------
void Cache::_do_consume()
{
   DownloadJob job;
   stack_nolock<DownloadJob>::node_t* pNode = _downloadJobs.popall();
   while(_downloadJobs.pop(pNode, job))
   {
      // process job:
      // is this a "file read job?"
      if (job.vFile == 0 )
      {
         if (job.vMem)
         {
            _doRead(job.vMem, job.local_file); // read file and mark it "ready"
            if (job.onthread)
            {
                  job.onthread(job.url, job.vMem, job.userdata);
            }
            _finishedJobs.push(job);
         }
      }
      else
      {
         if (job.vMem)
         {
            _doDownload(job.url, job.local_file, job.vFile, job.vMem);
            if (job.onthread)
            {
                  job.onthread(job.url, job.vMem, job.userdata);
            }
            _finishedJobs.push(job);
         }
      }

   }
}
//------------------------------------------------------------------------------
void Cache::ProcessDownloads()
{   
   while(!atomic_read(_bThreadsDone))
   {
      // todo: <wait>   // Condition var: http://www.boost.org/doc/libs/1_46_1/doc/html/thread/synchronization.html#thread.synchronization.condvar_ref
      boost::unique_lock<boost::mutex> lock(_mutex);
      _condition.wait(lock);      
      _do_consume();
   }
   _do_consume();
}

//------------------------------------------------------------------------------

void Cache::process()
{
   DownloadJob job;
   stack_nolock<DownloadJob>::node_t* pNode = _finishedJobs.popall();

   int cnt=0;
   while(_finishedJobs.pop(pNode, job))
   {
      _queuedJobs.push(job);
      cnt++;
   }

   /*if (cnt>0)
   {
      std::cout << "new jobs: " << cnt << " [";
      std::cout << "queued: " << _queuedJobs.size() << "]\n";
   }*/

   _curProcess += _increment;

   while (_curProcess>=_maxProcess)
   {
      _curProcess -= 1;

      if (_queuedJobs.size()>0)
      {
         job = _queuedJobs.front();
         _queuedJobs.pop();
         // process job:
         // is this a "file read job?"
         if (job.vFile == 0)
         {
            if (job.vMem)
            {
               if (job.onload)
               {
                  job.onload(job.url, job.vMem, job.userdata);
               }
            }
          }
         else
         {
            if (job.vMem)
            {
               if (job.onload)
               {
                  job.onload(job.url, job.vMem, job.userdata);
               }
            }
         }
      }
   }


}

//-----------------------------------------------------------------------------

void Cache::_post()
{
   _condition.notify_one(); 
}

//-----------------------------------------------------------------------------

void Cache::_post_all()
{
   _condition.notify_all();
}

//------------------------------------------------------------------------------