/*******************************************************************************
Project       : TreeList
Purpose       : Tree List Abstraction and GUI Representation in QT
Creation Date : February 2009
Author        : Martin Christen
Copyright     : Copyright (c) 2009 by Martin Christen. All rights reserved.
*******************************************************************************/

#ifndef _NODE_H
#define _NODE_H

#include "og.h"
#include <boost/shared_ptr.hpp>


// Interface GUI <-> List
// It is possible to connect a variable with a name
//#include <boost/shared_ptr.hpp>
#include <list>
#include <vector>
#include <queue>
#include <deque>
#include <cstring>


class OPENGLOBE_API Container;


class OPENGLOBE_API BaseNode
{
   friend class Container;
public:
   //! \brief Constructor
   BaseNode();

   //! \brief Destructor

   virtual ~BaseNode();

	//! \brief   Returns name of BaseNode
	//! \return name of BaseNode
	std::string&  GetName(){return _sNodeName;}


   //! \brief   Returns classname of BaseNode
   //! \return name of BaseNode
   std::string&  ClassName(){return _sNodeClassName;}

   //! \brief Set name of BaseNode.
   //! \param sNodeName Name of BaseNode 
   void SetName(std::string sNodeName);
   
   //! \brief Set name of BaseNode class.
   //! \param sClassName name of class
   void SetClassName(std::string sClassName);

   //! \brief  Adds a new BaseNode to the parent BaseNode of this BaseNode
   void AddNode(boost::shared_ptr<BaseNode>  qNode);

   //! \brief  Inserts a new child BaseNode to this BaseNode
   void InsertNode(boost::shared_ptr<BaseNode>  qNode);

   //! \brief  Inserts a new child BaseNode to this BaseNode at a specific index
   //! \param qNode BaseNode to be inserted
   //! \param qDestParent Destination group where BaseNode will be inserted
   //! \param index index in destination group
   void InsertNodeAt(boost::shared_ptr<BaseNode>  qNode, boost::shared_ptr<BaseNode>  qDestParent, int index);

   //! \brief  Returns the childnode at the given index or a zero pointer if index
   //! is out of bounds.
   //! \param index Index of the wanted Child BaseNode.
   boost::shared_ptr<BaseNode> GetChildByIndex(int index);

   //! \brief  Returns the list of all nodes of this BaseNode
   std::list<boost::shared_ptr<BaseNode> >& GetNodeList(){return _lstChildNodes;}

	//! \brief  This function returns true if it is a group BaseNode (children) can be added)
	//! \return true if this is a group BaseNode
   virtual bool IsGroup() {return true;}

   //! \brief  This method is called after BaseNode was created.
   //! \param bWriteInActionQueue Writes it's creation in the Action list if true.
   virtual void OnCreate();

   //! \brief  This method is called before BaseNode is destroyed
   //! \param bWriteInActionQueue Writes it's destruction in the Action list if true.
   virtual void OnDestroy();

   //! \brief Remove this BaseNode and its children
   void Destroy();

   //! \brief Receieve internal Type Identification
   virtual int GetTypeId(){return _iTypeId;}

   //! \brief Get BaseNode Id
   unsigned int  GetId(void){return _nId;}
	
   //! \brief Move BaseNode to dest. If dest is a group, it is appended, otherwise it is put after the BaseNode.
   //! \param qParentNode "Destination Group"
   //! \param nIndex Position in "Destination Group"
   void MoveNodeTo(boost::shared_ptr<BaseNode> qParentNode, int nIndex);

   //! \brief Set the Container
   void SetContainer(Container* pContainer);

   //! \brief Returns a non-shared-pointer of parent BaseNode.
   BaseNode* GetParentNode(){return _pParentNode;}

   //! \brief Set parent BaseNode.
   void  SetParentNode(BaseNode* pParent){_pParentNode = pParent;}

   //! \brief Set BaseNode Id
   void  SetId(unsigned int nId);

   //! \brief returns current instance of treelist BaseNode as shared pointer
   //! \return shared pointer corresponding to "this"
   boost::shared_ptr<BaseNode> GetSharedThis();

   // This method is called when BaseNode is selected (usually from a GUI)
   virtual void OnNodeSelected(){}

   //! \brief Adjusts properties depending on the rights the current user has. used in collaboration only.
   void AdjustPropertysToRights();

   //! \brief general purpose process function (example: rendering, ...)
   virtual void OnProcess(){}

protected:
   void _RemoveFromTree(); //!< \interal

   std::string                                 _sNodeName;          //!< \internal
   std::string                                 _sNodeClassName;     //!< \internal
   std::list<boost::shared_ptr<BaseNode> >         _lstChildNodes;      //!< \internal
   int											        _iTypeId;            //!< \internal
   BaseNode*                                       _pParentNode;        //!< \internal
   Container*                                  _pContainer;         //!< \internal
   unsigned int                                _nId;                //!< \internal

};

//-----------------------------------------------------------------------------

#endif