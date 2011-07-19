/*******************************************************************************
Project       : TreeList
Purpose       : Tree List Abstraction and GUI Representation in QT
Creation Date : February 2009
Author        : Martin Christen
Copyright     : Copyright (c) 2009 by Martin Christen. All rights reserved.
*******************************************************************************/

#include "scenegraph/Container.h"

//-----------------------------------------------------------------------------
Container::Container()
{ 
   _nCurrentId = 1;
}

//------------------------------------------------------------------------------

Container::~Container()
{ 
   // Call "OnDestroy" for each BaseNode in container!
   //_remove(_qRootNode.get());

   // Delete all nodes!
   if (_qRootNode)
      _qRootNode.reset();
}

//------------------------------------------------------------------------------

void Container::OnCreate(BaseNode* pNode)
{
   if (pNode == 0)
      return;

   pNode->OnCreate();

   std::list<boost::shared_ptr<BaseNode> >::iterator i = pNode->GetNodeList().begin();
   while (i!=pNode->GetNodeList().end())
   {
      OnCreate((*i).get());
      i++;
   }
}

//------------------------------------------------------------------------------

void Container::CleanUp()
{
   _remove(_qRootNode.get());
}

//------------------------------------------------------------------------------

void Container::_update(BaseNode* pNode)
{
   pNode->_pContainer = this;

   if (pNode->GetId() >= _nCurrentId && pNode->GetId() != 0xFFFFFFFF)
      _nCurrentId = pNode->GetId()+1;

   std::list<boost::shared_ptr<BaseNode> >::iterator i = pNode->GetNodeList().begin();

   while (i!=pNode->GetNodeList().end())
   {
      (*i)->_pParentNode = pNode;
      _update((*i).get());
      i++;
   }
}

//------------------------------------------------------------------------------

void Container::SetRootNode(boost::shared_ptr<BaseNode> qNode)
{
   if (_qRootNode)
      _qRootNode.reset();

   _qRootNode = qNode;
   _qRootNode->_pContainer = this;
   _qRootNode->OnCreate();
}

//------------------------------------------------------------------------------

void Container::AddNode(boost::shared_ptr<BaseNode> qNode)
{
   if (!_qRootNode)
   {
      boost::shared_ptr<BaseNode> qRoot(new BaseNode);
      qRoot->SetName("Root");
      _qRootNode = qRoot;
      _qRootNode->OnCreate();
   }

   qNode->_pContainer = this;
   _qRootNode->AddNode(qNode);
   qNode->OnCreate();
}

//------------------------------------------------------------------------------

void Container::_remove(BaseNode* pNode)
{
   if (!pNode)
      return;

   pNode->OnDestroy();

   std::list<boost::shared_ptr<BaseNode> >::iterator i = pNode->GetNodeList().begin();

   while (i!=pNode->GetNodeList().end())
   {
      _remove((*i).get());
      i++;
   }
}


//------------------------------------------------------------------------------