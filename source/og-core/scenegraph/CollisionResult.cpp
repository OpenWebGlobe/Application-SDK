

#include "scenegraph/CollisionResult.h"



CollisionResult::CollisionResult()
{

}

CollisionResult::~CollisionResult()
{

}

void CollisionResult::AddCollidingNode(boost::shared_ptr<BaseNode>& qNode)
{
   _lstCollidingNodes.push_back(qNode);
}