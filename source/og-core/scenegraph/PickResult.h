#ifndef _PICKRESULT_H
#define _PICKRESULT_H

#include "og.h"
#include <list>
#include <string>
#include <boost/shared_ptr.hpp>
#include "math/vec3.h"
#include "scenegraph/BaseNode.h"

//-----------------------------------------------------------------------------
// Picking Nodes:
//-----------------------------------------------------------------------------

struct OPENGLOBE_API NodeHitRecord
{
   double t;   // point
   boost::shared_ptr<BaseNode>  qNode;
};

//-----------------------------------------------------------------------------

class OPENGLOBE_API NodePickResult
{
public:
   NodePickResult();
   virtual ~NodePickResult();

   void  AddPickResult(double h, boost::shared_ptr<BaseNode>  qNode);

   void  Sort();
   void  Reset();

   bool   GetFirstPickPos(vec3<double>& pos);
   void   GetNodePickPos(vec3<double>& pos, NodeHitRecord& a);
   bool   GetFirstHitRecord(double& h, boost::shared_ptr<BaseNode>&  qNode);

   vec3<double>&  GetRayPos(){ return _vRayPos; }
   vec3<double>&  GetRayDir(){ return _vRayDir; }

   void  SetRayPos(vec3<double>& r){_vRayPos = r;}
   void  SetRayDir(vec3<double>& d){_vRayDir = d;}

   std::list<NodeHitRecord>& GetHitPoints(){ return _hitpoints; }

protected:
   vec3<double>   _vRayPos;               // Start Position of Ray
   vec3<double>   _vRayDir;               // Direction of Ray
   std::list<NodeHitRecord> _hitpoints; // List of hitpoints t: _vRayPos + _vRayDir * t;

};

//-----------------------------------------------------------------------------
// Picking Terrain:
//-----------------------------------------------------------------------------

struct OPENGLOBE_API TerrainHitRecord
{
   double t;   // point / distance
   double lng; // longitude
   double lat; // latitude
   double elv; // elevation
   int    lod; // level of detail of picking
};

//-----------------------------------------------------------------------------

class OPENGLOBE_API TerrainPickResult
{
public:
   TerrainPickResult();
   virtual ~TerrainPickResult();

   void  AddPickResult(double h, int lod);

   void  Sort();
   void  Reset();

   bool   GetFirstPickPos(vec3<double>& pos);
   void   GetNodePickPos(vec3<double>& pos, TerrainHitRecord& a);
   bool   GetFirstHitRecord(double& h);

   vec3<double>&  GetRayPos(){ return _vRayPos; }
   vec3<double>&  GetRayDir(){ return _vRayDir; }

   void  SetRayPos(vec3<double>& r){_vRayPos = r;}
   void  SetRayDir(vec3<double>& d){_vRayDir = d;}

   std::list<TerrainHitRecord>& GetHitPoints(){ return _hitpoints; }

protected:
   vec3<double>   _vRayPos;               // Start Position of Ray
   vec3<double>   _vRayDir;               // Direction of Ray
   std::list<TerrainHitRecord> _hitpoints; // List of hitpoints t: _vRayPos + _vRayDir * t;

};


//-----------------------------------------------------------------------------
// Picking Named Objects:
//-----------------------------------------------------------------------------

struct OPENGLOBE_API ObjectHitRecord
{
   double t;   // point / distance
   std::string sName;
};

//-----------------------------------------------------------------------------

class OPENGLOBE_API ObjectPickResult
{
public:
   ObjectPickResult();
   virtual ~ObjectPickResult();

   void  AddPickResult(double h, std::string sName);

   void  Sort();
   void  Reset();

   bool   GetFirstPickPos(vec3<double>& pos);
   void   GetNodePickPos(vec3<double>& pos, ObjectHitRecord& a);
   bool   GetFirstHitRecord(double& h, std::string& sName);

   vec3<double>&  GetRayPos(){ return _vRayPos; }
   vec3<double>&  GetRayDir(){ return _vRayDir; }

   void  SetRayPos(vec3<double>& r){_vRayPos = r;}
   void  SetRayDir(vec3<double>& d){_vRayDir = d;}

   std::list<ObjectHitRecord>& GetHitPoints(){ return _hitpoints; }

protected:
   vec3<double>   _vRayPos;               // Start Position of Ray
   vec3<double>   _vRayDir;               // Direction of Ray
   std::list<ObjectHitRecord> _hitpoints; // List of hitpoints t: _vRayPos + _vRayDir * t;

};

//-----------------------------------------------------------------------------
#endif

