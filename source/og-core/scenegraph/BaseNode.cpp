/*******************************************************************************
Project       : TreeList
Purpose       : Tree List Abstraction and GUI Representation in QT
Creation Date : February 2009
Author        : Martin Christen
Copyright     : Copyright (c) 2009 by Martin Christen. All rights reserved.
*******************************************************************************/

#include "scenegraph/BaseNode.h"
#include "scenegraph/Container.h"
#include <cassert>
#include "string/StringUtils.h"

BaseNode::BaseNode()
{
   SetName("Unnamed Entity");
   SetClassName("Unnamed Class");
   _iTypeId = -1;
   _pParentNode = 0;
   _pContainer = 0;
   _nId = 0xFFFFFFFF;   // "invalid ID"
}

//-----------------------------------------------------------------------------

BaseNode::~BaseNode()
{ 
   _lstChildNodes.clear(); 
}

//-----------------------------------------------------------------------------

void BaseNode::OnDestroy()
{

}

//-----------------------------------------------------------------------------

void BaseNode::SetName(std::string sNodeName)
{
   _sNodeName = sNodeName;
}

//-----------------------------------------------------------------------------

void BaseNode::SetClassName(std::string sClassName)
{
   _sNodeClassName = sClassName;
}

//-----------------------------------------------------------------------------

void BaseNode::SetId(unsigned int nId)
{
   _nId = nId;
}

//-----------------------------------------------------------------------------

void BaseNode::AddNode(boost::shared_ptr<BaseNode>  qNode)
{
	if (_pParentNode)
	{
		std::list<boost::shared_ptr<BaseNode> >::iterator i = _pParentNode->GetNodeList().begin();

		while (i!=_pParentNode->GetNodeList().end())
		{
			if ((*i).get() == this)
			{
				_pParentNode->GetNodeList().insert(++i, qNode); // insert before ++i (which may be end)
				qNode->_pParentNode = _pParentNode;
				qNode->_pContainer = _pContainer;
				qNode->OnCreate();

				// Call the Update function to update a possibly connected GUI. 
				
				break;
			}
			i++;
		}
	}
	else
	{
		assert(false);
	}

}

//-----------------------------------------------------------------------------

void BaseNode::InsertNode(boost::shared_ptr<BaseNode>  qNode)
{
	qNode->_pParentNode = this;
	qNode->_pContainer = _pContainer;
  
	_lstChildNodes.push_back(qNode);
  
	qNode->OnCreate();
}

//-----------------------------------------------------------------------------

void BaseNode::InsertNodeAt(boost::shared_ptr<BaseNode>  qNode, boost::shared_ptr<BaseNode>  qDestParent, int index)
{
   if (!qDestParent)
   {
      assert(false);
      return;
   }

   qNode->_pParentNode = qDestParent.get();
	qNode->_pContainer = _pContainer;

   int idx = 0;
   std::list<boost::shared_ptr<BaseNode> >& lst = qDestParent->GetNodeList();

   // list is empty, insert directly
   if (lst.size() == 0)
   {
      lst.push_back(qNode);
      qNode->OnCreate();

      return;
   }
 
   std::list<boost::shared_ptr<BaseNode> >::iterator it = lst.begin();
   for(int i = 0; i < index; i++)
   {
      ++it;
   }

     lst.insert(it, qNode);
  
     qNode->OnCreate();
   	
}

//-----------------------------------------------------------------------------

boost::shared_ptr<BaseNode> BaseNode::GetChildByIndex(int index)
{
   std::list<boost::shared_ptr<BaseNode> >::iterator it = _lstChildNodes.begin();

   while(it != _lstChildNodes.end() && index > 0)
   {
      index--;
      it++;
   }
   if (index == 0 && it != _lstChildNodes.end())
   {
      return (*it);
   }
   return boost::shared_ptr<BaseNode>();
}

//-----------------------------------------------------------------------------

void BaseNode::OnCreate()
{
   if (_nId == 0xFFFFFFFF && _pContainer)
   {
      SetId(_pContainer->GetCurrentId());
      _pContainer->IncrementId();
   }
}

//-----------------------------------------------------------------------------

void BaseNode::Destroy()
{
	// Keep a copy of the item to be destroyed, otherwise "this" is destroyed in the middle of the algorithm.
	boost::shared_ptr<BaseNode> pDestroy;

   if (_pParentNode)
   {
      std::list<boost::shared_ptr<BaseNode> >::iterator i = _pParentNode->GetNodeList().begin();

      while (i!=_pParentNode->GetNodeList().end())
      {
         if ((*i).get() == this)
         {
            (*i)->OnDestroy();
			pDestroy = *i;
            i = _pParentNode->GetNodeList().erase(i);
            break;
         }
         i++;
      }
   }
   else
   {
      // Destroying root is not possible (EVERY graph has a root of type "BaseNode")
   }

}

//-----------------------------------------------------------------------------

void BaseNode::MoveNodeTo(boost::shared_ptr<BaseNode> qDestParent, int index)
{
   if (_pContainer)
   {
      boost::shared_ptr<BaseNode> qThis = GetSharedThis();
      this->_RemoveFromTree();

      assert(qDestParent->IsGroup());

      InsertNodeAt(qThis, qDestParent, index);
   }
}


//-----------------------------------------------------------------------------

void BaseNode::_RemoveFromTree()
{
   if (!_pParentNode)
   {
      assert(false);
      return;
   }

   std::list<boost::shared_ptr<BaseNode> >& lst = _pParentNode->GetNodeList();
 
   std::list<boost::shared_ptr<BaseNode> >::iterator it = lst.begin();
   while (it != lst.end())
   {
      if ((*it).get() == this)
      {
         lst.erase(it);
         return;
      }
      ++it;
   }

   assert(false); // your tree is corrupted/broken!


}

//-----------------------------------------------------------------------------

boost::shared_ptr<BaseNode> BaseNode::GetSharedThis()
{
   if (!_pParentNode)
   {
      assert(false);
      return boost::shared_ptr<BaseNode>();
   }

   std::list<boost::shared_ptr<BaseNode> >& lst = _pParentNode->GetNodeList();
 
   std::list<boost::shared_ptr<BaseNode> >::iterator it = lst.begin();
   while (it != lst.end())
   {
      if ((*it).get() == this)
      {
         return (*it);
      }
      ++it;
   }

   assert(false); // your tree is corrupted/broken!
   return boost::shared_ptr<BaseNode>();
}

//-----------------------------------------------------------------------------

