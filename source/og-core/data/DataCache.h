/*******************************************************************************
Project       : i3D OpenGlobe SDK - Reference Implementation
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2011 by FHNW/IVGI. All Rights Reserved
Boost LRU Cache from article: http://www.bottlenose.demon.co.uk/article/lru.htm

$License$
*******************************************************************************/

#ifndef _DATACACHE_H
#define _DATACACHE_H

#include <boost/bimap.hpp> 
#include <boost/bimap/list_of.hpp> 
#include <boost/bimap/set_of.hpp> 
#include <boost/function.hpp> 
 
// Class providing fixed-size (by number of records) 
// LRU-replacement cache of a function with signature 
// V f(K) 
template <typename K,typename V> class DataCache 
{ 
public: 
   typedef int dummy_type; 
   //---------------------------------------------------------------------------
   // Bimap with key access on left view, key access 
   // history on right view, and associated value. 
   typedef boost::bimaps::bimap< 
     boost::bimaps::set_of<K>, 
     boost::bimaps::list_of<dummy_type>, 
     boost::bimaps::with_info<V> > cache_type; 
 
   //---------------------------------------------------------------------------
   // Constuctor specifies the cached function and 
   // the maximum number of records to be stored. 
   DataCache(const boost::function<V(const K&)>& f, size_t c) 
    : _fn(f), _capacity(c) 
   { 
      assert(_capacity!=0); 
   } 
   //---------------------------------------------------------------------------
   // Obtain value of the cached function for k 
   V Request(const K& k) 
   { 
      // Attempt to find existing record 
      const typename cache_type::left_iterator it = _cache.left.find(k); 
 
      if (it==_cache.left.end()) 
      { 
         // We don't have it: 
         // Evaluate function and create new record 
         const V v=_fn(k); 
         insert(k,v); 
         return v; 
      } 
      else 
      { 
         // We do have it: 
         // Update the access record view. 
         _cache.right.relocate(_cache.right.end(), _cache.project_right(it) ); 
 
         return it->info; 
      } 
   } 
   //---------------------------------------------------------------------------
   // Get Entry if already cached, (don't create new one!)
   bool GetCachedEntry(const K& k, V& out_value)
   {
      // Attempt to find existing record 
      const typename cache_type::left_iterator it = _cache.left.find(k); 

      if (it==_cache.left.end()) 
      { 
         // entry doesn't exist. Return false!
         return false;
      } 
      else 
      { 
         // We do have it: 
         // Update the access record view. 
         _cache.right.relocate(_cache.right.end(), _cache.project_right(it) ); 
 
         out_value = it->info;
         return true; 
      } 
  
   } 
   //---------------------------------------------------------------------------
   // Obtain the cached keys, most recently used element 
   // at head, least recently used at tail. 
   // This method is provided purely to support testing. 
   template <typename IT> void get_keys(IT dst) const 
   { 
      typename cache_type::right_const_reverse_iterator src =_cache.right.rbegin(); 
      while (src!=_cache.right.rend()) 
      { 
         ⋆dst++=(⋆src++).second; 
      } 
   }
   //--------------------------------------------------------------------------- 
 
private: 
   void insert(const K& k,const V& v) 
   { 
      assert(_cache.size()<=_capacity); 
 
      // If necessary, make space 
      if (_cache.size()==_capacity) 
      { 
         // by purging the least-recently-used element 
         _cache.right.erase(_cache.right.begin()); 
      } 
 
      // Create a new record from the key, a dummy and the value 
      _cache.insert(typename cache_type::value_type(k,0,v)); 
   } 
   //---------------------------------------------------------------------------
   const boost::function<V(const K&)> _fn; 
   const size_t _capacity; 
   cache_type _cache; 
};


#endif

