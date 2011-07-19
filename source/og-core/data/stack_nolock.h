/*******************************************************************************
Project       : Example of a lockfree stack using std::atomic (boost::atomic)
Version       : 1.0
Author        : Martin Christen, martin.christen@fhnw.ch
Copyright     : (c) 2006-2010 by FHNW/IVGI. All Rights Reserved

$License$
*******************************************************************************/

#ifndef _STACK_NOLOCK_H
#define _STACK_NOLOCK_H

#include <boost/atomic.hpp>

template<typename T>
class stack_nolock
{
public:
   template<typename T>
   struct Node 
   {
      T data;
      Node* next;
   };

   stack_nolock()
   {
      _head = 0;
   }

   typedef Node<T> node_t; 

   void push(const T& data)
   {
      node_t* n = new node_t; 
      n->data = data;

      node_t* stale_head=_head.load(boost::memory_order_relaxed);
      do 
      {
         n->next=stale_head;
      } 
      while(!_head.compare_exchange_weak(stale_head, n, boost::memory_order_release));
   }

   node_t* popall()
   {
      node_t* last = pop_all_reverse();
      node_t* first = 0;
  
      while(last) 
      {
         node_t* tmp=last;
         last=last->next;
         tmp->next=first;
         first=tmp;
      }
      
      return first;
   }

   bool pop(node_t*& node, T& element)
   {
      if (!node)
      {
         return false;
      }

      element = node->data;
      node = node->next;
      return true;
   }


protected:
   boost::atomic<node_t*> _head;

   
   node_t* pop_all_reverse(void)
   {
      return _head.exchange(0, boost::memory_order_consume);
   }
};



#endif

