

#include "scenegraph/PickResult.h"
#include "math/GeoCoord.h"


//-----------------------------------------------------------------------------

NodePickResult::NodePickResult()
{
}

//-----------------------------------------------------------------------------

NodePickResult::~NodePickResult()
{
}

//-----------------------------------------------------------------------------

inline bool CompareHitRecord(NodeHitRecord& a, NodeHitRecord& b) 
{
   return a.t < b.t;
} 

//-----------------------------------------------------------------------------

void  NodePickResult::Sort()
{
   _hitpoints.sort(CompareHitRecord);
   //std::sort(_hitpoints.begin(), _hitpoints.end(), CompareHitRecord); 
}

//-----------------------------------------------------------------------------

void  NodePickResult::Reset()
{
   _hitpoints.clear();
   _vRayPos = vec3<double>(0,0,0);
   _vRayDir = vec3<double>(0,0,0);
}

//-----------------------------------------------------------------------------

void  NodePickResult::AddPickResult(double h, boost::shared_ptr<BaseNode>  qNode)
{
   NodeHitRecord HR;
   HR.t = h;
   HR.qNode = qNode;
   _hitpoints.push_back(HR);
}

//-----------------------------------------------------------------------------

bool   NodePickResult::GetFirstPickPos(vec3<double>& pos)
{
   if (_hitpoints.size()<1)
      return false;

   std::list<NodeHitRecord>::iterator i = _hitpoints.begin();

   pos = _vRayPos + _vRayDir * (*i).t;
   return true;
}

//-----------------------------------------------------------------------------

void   NodePickResult::GetNodePickPos(vec3<double>& pos, NodeHitRecord& a)
{
   pos = _vRayPos + _vRayDir * a.t;
}

//-----------------------------------------------------------------------------

bool NodePickResult::GetFirstHitRecord(double& h, boost::shared_ptr<BaseNode>&  qNode)
{
   if (_hitpoints.size()<1)
      return false;
   std::list<NodeHitRecord>::iterator i = _hitpoints.begin();

   h = (*i).t;
   qNode = (*i).qNode;
   return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

inline bool CompareHitRecordTerrain(TerrainHitRecord& a, TerrainHitRecord& b) 
{
   return a.t < b.t;
} 

//-----------------------------------------------------------------------------

TerrainPickResult::TerrainPickResult()
{
}

//-----------------------------------------------------------------------------

TerrainPickResult::~TerrainPickResult()
{
}

//-----------------------------------------------------------------------------

void  TerrainPickResult::Sort()
{
   _hitpoints.sort(CompareHitRecordTerrain);
   //std::sort(_hitpoints.begin(), _hitpoints.end(), CompareHitRecord); 
}

//-----------------------------------------------------------------------------

void  TerrainPickResult::Reset()
{
   _hitpoints.clear();
   _vRayPos = vec3<double>(0,0,0);
   _vRayDir = vec3<double>(0,0,0);
}

//-----------------------------------------------------------------------------

void  TerrainPickResult::AddPickResult(double h, int lod)
{
   TerrainHitRecord HR;
   HR.t = h;

   vec3<double> pos;
   pos = _vRayPos + _vRayDir * h;

   GeoCoord oGeoCoord;
   oGeoCoord.FromCartesian(pos.x, pos.y, pos.z);
   HR.lng = oGeoCoord.GetLongitude();
   HR.lat = oGeoCoord.GetLatitude();
   HR.elv = oGeoCoord.GetEllipsoidHeight();
   HR.lod = lod;

   _hitpoints.push_back(HR);
}

//-----------------------------------------------------------------------------

bool TerrainPickResult::GetFirstPickPos(vec3<double>& pos)
{
   if (_hitpoints.size()<1)
      return false;

   std::list<TerrainHitRecord>::iterator i = _hitpoints.begin();

   pos = _vRayPos + _vRayDir * (*i).t;
   return true;
}

//-----------------------------------------------------------------------------

void TerrainPickResult::GetNodePickPos(vec3<double>& pos, TerrainHitRecord& a)
{
   pos = _vRayPos + _vRayDir * a.t;
}

//-----------------------------------------------------------------------------

bool TerrainPickResult::GetFirstHitRecord(double& h)
{
   if (_hitpoints.size()<1)
      return false;
   std::list<TerrainHitRecord>::iterator i = _hitpoints.begin();

   h = (*i).t;
   return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

inline bool CompareHitRecordObject(ObjectHitRecord& a, ObjectHitRecord& b) 
{
   return a.t < b.t;
} 

//-----------------------------------------------------------------------------

ObjectPickResult::ObjectPickResult()
{
}

//-----------------------------------------------------------------------------

ObjectPickResult::~ObjectPickResult()
{
}

//-----------------------------------------------------------------------------

void  ObjectPickResult::Sort()
{
   _hitpoints.sort(CompareHitRecordObject);
   //std::sort(_hitpoints.begin(), _hitpoints.end(), CompareHitRecord); 
}

//-----------------------------------------------------------------------------

void  ObjectPickResult::Reset()
{
   _hitpoints.clear();
   _vRayPos = vec3<double>(0,0,0);
   _vRayDir = vec3<double>(0,0,0);
}

//-----------------------------------------------------------------------------

void ObjectPickResult::AddPickResult(double h, std::string sName)
{
   ObjectHitRecord HR;
   HR.t = h;
   HR.sName = sName;
   _hitpoints.push_back(HR);
}

//-----------------------------------------------------------------------------

bool ObjectPickResult::GetFirstPickPos(vec3<double>& pos)
{
   if (_hitpoints.size()<1)
      return false;

   std::list<ObjectHitRecord>::iterator i = _hitpoints.begin();

   pos = _vRayPos + _vRayDir * (*i).t;
   return true;
}

//-----------------------------------------------------------------------------

void  ObjectPickResult::GetNodePickPos(vec3<double>& pos, ObjectHitRecord& a)
{
   pos = _vRayPos + _vRayDir * a.t;
}

//-----------------------------------------------------------------------------

bool ObjectPickResult::GetFirstHitRecord(double& h, std::string& sName)
{
   if (_hitpoints.size()<1)
      return false;
   std::list<ObjectHitRecord>::iterator i = _hitpoints.begin();

   h = (*i).t;
   sName = (*i).sName;
   return true;
}

//-----------------------------------------------------------------------------


