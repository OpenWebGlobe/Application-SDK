/*******************************************************************************
Project       : Cross Platform URL Download + Disk Cache using boost::bimap
                idea based on article "LRU cache implementation in C++" 
                by Tim Day http://www.bottlenose.demon.co.uk/article/lru.htm
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

// Diskcache: Level 1 Cache for OpenWebGlobe

#ifndef _LRU_diskcache_H
#define _LRU_diskcache_H

#include "og.h"
#include <string>
#include <queue>
#include <boost/bimap.hpp> 
#include <boost/bimap/list_of.hpp> 
#include <boost/bimap/set_of.hpp> 
#include <boost/function.hpp>
#include <boost/atomic.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <csignal>
#include "data/stack_nolock.h"

//------------------------------------------------------------------------------
// using boost::atomic for atomic. In future this can be replaced with std::atomic
typedef boost::atomic<bool> atomic_bool;
#define atomic_read(v)  ((v).load())
#define atomic_set(v, i)   ((v).store(i))
//------------------------------------------------------------------------------

class OPENGLOBE_API MemoryEntry
{
public:
   MemoryEntry() { _pData = 0; _nDataSize = 0; atomic_set(_isReady,false); atomic_set(_isFailed,false);}
   virtual ~MemoryEntry();

   // Restriction for data and size: only access if "IsReady()" returns true (thread)
   inline unsigned char* Data()     { return _pData;}
   inline size_t       DataSize()   { return _nDataSize; }
   
   inline bool IsReady()    { return atomic_read(_isReady);}
   inline bool IsFailed()   { return atomic_read(_isFailed);} 

   inline void _SetData(unsigned char* pData, size_t nSize){ _pData = pData; _nDataSize = nSize; }
   inline void _SetReady() { atomic_set(_isReady,true); }
   inline void _SetFailed() { atomic_set(_isFailed,true);}

protected:
   volatile atomic_bool _isReady;    
   volatile atomic_bool _isFailed;
   //bool _isLocked; // file is locked and can't be removed at the moment, because memory cache requested it. (TODO)
   unsigned char* _pData;
   size_t        _nDataSize;

};

//------------------------------------------------------------------------------

class OPENGLOBE_API FileEntry
{
public:
   FileEntry(){ _nFileSize = 0; atomic_set(_isReady,false); atomic_set(_isFailed,false);}
   virtual ~FileEntry();

   // Filename of local file
   inline std::string& LocalFile()  { return _sLocalFile; }
   inline size_t       FileSize()   { return _nFileSize; }
   inline bool IsReady()    { return atomic_read(_isReady);}
   inline bool IsFailed()   { return atomic_read(_isFailed);} 

   inline void _SetReady() { atomic_set(_isReady,true); }
   inline void _SetFailed() { atomic_set(_isFailed,true);}
   inline void _SetLocalFile(const std::string& s){_sLocalFile = s;}
   inline void _SetFilesize(size_t n){ _nFileSize = n;}
protected:
   volatile atomic_bool _isReady;  
   volatile atomic_bool _isFailed;
   std::string _sLocalFile;   // local file
   size_t      _nFileSize;    // file size (in Bytes)

};

//------------------------------------------------------------------------------

struct DownloadJob
{
   MemoryEntry*   vMem;
   FileEntry*     vFile;
   std::string    url;
   std::string    local_file; // url or local file
   boost::function<void(const std::string&, MemoryEntry*, void*)> onload;
   boost::function<void(const std::string&, MemoryEntry*, void*)> onthread;
   void*          userdata; // custom data
};

//------------------------------------------------------------------------------

class OPENGLOBE_API Cache 
{ 
   public: 


   //---------------------------------------------------------------------------
   typedef int dummy_type; 
   typedef boost::bimaps::bimap< 
      boost::bimaps::set_of<std::string>,    // URL (as std::string)
      boost::bimaps::list_of<dummy_type>, 
      boost::bimaps::with_info<FileEntry*>   // File Entry
   > DiskCache_t; 

   typedef boost::bimaps::bimap< 
      boost::bimaps::set_of<std::string>,    // local file (as std::string)
      boost::bimaps::list_of<dummy_type>, 
      boost::bimaps::with_info<MemoryEntry*>   // Data Entry
   > MemoryCache_t; 
   //---------------------------------------------------------------------------
   
   // ctor specifies the target directory for downloads and the maximal cache size in Megabytes
   // please not that the maximal cache size is not guaranteed, because the file size is only known
   // after downloading.
   // Example: if you download 10x4 GB files at the same time and have a 1 MB buffer - downloads are not stopped because of file size.
   Cache(const std::string& basedir, double diskcachecapacity_MB, double memcachecapacity_MB, unsigned int numDownloadThreads=2);
   
   // dtor
   virtual ~Cache();

   // Limit the total number of disk cache entries (Default are 2^24 entries.)
   void SetMaxDiskCacheEntries(size_t m) { _maxDiskCacheEntries = m; }

   // Limit the total number of mempry cache entries. (Default are 2^24 entries.)
   void SetMaxMemoryCacheEntries(size_t m) { _maxMemoryCacheEntries = m; }
   
   /*
       boost::function<void(const std::string&, MemoryEntry*, void*)> f =  boost::bind( &myClass::OnLoad, this );
   */

   // Request Data from URL.
   MemoryEntry* RequestData(const std::string& url, boost::function<void(const std::string&, MemoryEntry*, void*)> callback_onload = 0, void* userdata = 0);
   MemoryEntry* RequestDataAsync(const std::string& url, boost::function<void(const std::string&, MemoryEntry*, void*)> callback_onload = 0, void* userdata = 0);
   
   MemoryEntry* RequestDataThreaded(const std::string& url, boost::function<void(const std::string&, MemoryEntry*, void*)> callback_onload = 0, boost::function<void(const std::string&, MemoryEntry*, void*)> callback_onthread = 0, void* userdata = 0);


   //---------------------------------------------------------------------------
   // Request Data from URL using the () operator. 
   //void operator()(const std::string& url) { return RequestData(url);}
   //---------------------------------------------------------------------------
   void process();  // call this from time to time to process finished jobs (call callbacks)
   //---------------------------------------------------------------------------
   // wait for all download jobs to finish.
   void wait();
   //---------------------------------------------------------------------------
   void dump(bool memonly = false);
   //---------------------------------------------------------------------------
protected:
   void _post();
   void _post_all(); // notify all threads to drop lock
   // Retrieve Data (local filename). NEVER use this for URL. This operation is
   // synchronous!
   MemoryEntry* _LoadLocalFile(const std::string& filename, bool async, bool relative, boost::function<void(const std::string&, MemoryEntry*, void*)> callback, boost::function<void(const std::string&, MemoryEntry*, void*)> callback_onthread, const std::string& origURL, void* userdata);
   MemoryEntry* _LoadRemoteFile(const std::string& url, FileEntry* pFileEntry, bool async, boost::function<void(const std::string&, MemoryEntry*, void*)> callback, boost::function<void(const std::string&, MemoryEntry*, void*)> callback_onthread, void* userdata);

   void ProcessDownloads();

private:
   Cache() : _maxDiskCacheCapacity(0) {} // default ctor private

   void _doRead(MemoryEntry* pEntry, const std::string& sFilename);
   void _doDownload(const std::string& url, const std::string& sTempFile, FileEntry* pFileEntry, MemoryEntry* pMemoryEntry);
   MemoryEntry* _RequestData(const std::string& url, boost::function<void(const std::string&, MemoryEntry*, void*)> callback, bool async, void* userdata, boost::function<void(const std::string&, MemoryEntry*, void*)> callback_onthread);

   void insertDiskEntry(const std::string& k, FileEntry* v);
   void insertMemoryEntry(const std::string& k, MemoryEntry* v);
   std::string _basedir;

   double   _maxDiskCacheCapacity;        // Max disk cache capacity in Megabytes (MB) 
   double   _maxMemoryCacheCapacity;      // Max memory cache capacity in Megabytes (MB)

   double   _curDiskCacheCapacity;     // current disk cache capacity
   double   _curMemoryCacheCapacity;   // current mempry cache capacity

   size_t _maxDiskCacheEntries;  // maximal disk cache entries (default: 16777216)
   size_t _maxMemoryCacheEntries; // maximal memory cache entries (default: 16777216)
   unsigned int _numDownloadThreads;

   //---------------------------------------------
   void _do_consume();
   stack_nolock<DownloadJob>  _downloadJobs;    // stack containing downloaded jobs
   stack_nolock<DownloadJob>  _finishedJobs;    // stack containing finished jobs
   std::queue<DownloadJob>    _queuedJobs;      // queued jobs
   double                     _maxProcess;        // number of elements to process per call to "process"
   double                     _curProcess;
   double                     _increment;
   boost::thread_group        _threads;         // threads (for download and file loading)
   atomic_bool                _bThreadsDone;    // to finish all threads
   //---------------------------------------------

   DiskCache_t     _diskcache;
   MemoryCache_t   _memcache; 


   boost::condition_variable  _condition;
   boost::mutex               _mutex;
};


#endif
