#ifndef _COLLISIONRESULT_H
#define _COLLISIONRESULT_H

#include "og.h"
#include <list>
#include <string>
#include <boost/shared_ptr.hpp>
#include <math/vec3.h>
#include <scenegraph/BaseNode.h>


class OPENGLOBE_API CollisionResult
{
public:
   CollisionResult();
   virtual ~CollisionResult();

   void AddCollidingNode(boost::shared_ptr<BaseNode>& qNode);

   std::list<boost::shared_ptr<BaseNode> >& GetCollisionList() {return _lstCollidingNodes;}


protected:
   std::list<boost::shared_ptr<BaseNode> > _lstCollidingNodes;
};




#endif