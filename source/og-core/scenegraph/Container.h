/*******************************************************************************
Project       : TreeList
Purpose       : Tree List Abstraction
Creation Date : February 2009
Author        : Martin Christen
Copyright     : Copyright (c) 2009 by Martin Christen. All rights reserved.
*******************************************************************************/

#ifndef _CONTAINER_H
#define _CONTAINER_H

#include "og.h"
#include "scenegraph/BaseNode.h"
#include <boost/shared_ptr.hpp>

//-----------------------------------------------------------------------------
//! \class Container
//! \brief Container class holding (property based) nodes for a tree.
//! \author Martin Christen, martin.christen@fhnw.ch
//! \ingroup treelist
class OPENGLOBE_API Container
{
   friend class BaseNode;
public:
   //! \brief Constructor
   Container();

   //! \brief Destructor
   virtual ~Container();

   //! \brief Add a new BaseNode into root
   void AddNode(boost::shared_ptr<BaseNode> qNode);

   //! \brief Set Root BaseNode. Warning: A call to this method may destroy the whole tree!
   void SetRootNode(boost::shared_ptr<BaseNode> qNode);

   //! \brief Returns the root BaseNode
   boost::shared_ptr<BaseNode>& GetRootNode(){return _qRootNode;}

   unsigned int   GetCurrentId(){return _nCurrentId;}
   void           IncrementId(){_nCurrentId++;}


   // This must be called after creating a BaseNode list without using
   // "AddNode", i.e. after deserialization.
   void  OnCreate(BaseNode* pNode);

   //Cleanup calls OnExit() in every BaseNode. It is essential that this function is called
   //before the rendering context gets deleted.
   void CleanUp();

protected:
   boost::shared_ptr<BaseNode> _qRootNode;

   void _connect(BaseNode* pNode);
   void _update(BaseNode* pNode);
   void _remove(BaseNode* pNode);

   unsigned int   _nCurrentId;

};


#endif

