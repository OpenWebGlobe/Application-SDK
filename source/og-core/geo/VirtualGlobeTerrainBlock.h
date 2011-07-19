
#ifndef _VIRTUALGLOBE_TERRAINBLOCK_H
#define _VIRTUALGLOBE_TERRAINBLOCK_H

#include "og.h"

#include "math/vec3.h"
#include "math/mat4.h"
#include "math/ElevationPoint.h"
#include "math/aabb3.h"
#include "renderer/IIndexBuffer.h"
#include "renderer/IVertexBuffer.h"
#include "renderer/ITexture.h"
#include "renderer/IRenderEngine.h"
#include "geo/MercatorQuadtree.h"
#include "math/TextureTransformation.h"
#include "geo/ElevationTile.h"
#include "geo/layer/IImageLayer.h"
#include "geo/layer/IElevationLayer.h"
#include <boost/shared_array.hpp>
#include <list>

class OPENGLOBE_API VirtualGlobeCache;
struct VirtualGlobeRenderOptions;

class /*OPENGLOBE_API*/ VirtualGlobeTerrainBlock
{
public:
   // ctor
   VirtualGlobeTerrainBlock(IRenderEngine* pEngine, const std::string& sQuadcode,  const boost::shared_ptr<MercatorQuadtree>& qQuadtree);
   
   // dtor
   virtual ~VirtualGlobeTerrainBlock();

   // Retrieve Quadcode of this Terrain Block
   std::string GetQuadcode(){return _sQuadcode;}

   // returns true if specified WGS84 coordinate is inside this block!
   bool IsInside(double lng, double lat);

   // returns true if all data is available
   bool IsAvailable();

   // Render Tile
   void Render(const mat4<double>& oModelMatrix, const mat4<double>& oViewMatrix, boost::shared_ptr<VirtualGlobeCache>& qCache/*, const VirtualGlobeRenderOptions& oRenderOptions*/);

   // Set RAW-Elevation Data
   void SetElevationDataBlock(boost::shared_array<float>& data, unsigned int nBlocksize);

   // Set TIN-based Elevation Data
   void SetElevationData(ElevationTile& oElevationTile);
   
   // internally used to mark data as available
   void SetReady();

   // Calculate Visible Pixel Size
   double GetPixelSize(const mat4<double>& mModelViewProjection, const int& nWidth, const int& nHeight);

   // Calculate size of a block (cell size)
   double GetBlockSize();

   // Calc exact distance to another point
   double CalcDistanceTo(vec3<double>& vWhere, vec3<double>& outHitpoint);
   
   // Return bounding box, only valid if "IsAvailable()" returns true!
   aabb3<double>&                      GetBoundingBox() {return _BoundingBox;}

   // Return offset of tile (virtual camera!)
   vec3<double>&                       GetOffset() {return _vOffset;}

   // Calculate Tile Points
   void CalcLocation();

   // Returns texture of block (or null if unavailable)
   ITexture*      GetTexture() {return _qTexture;}

   boost::shared_ptr<IIndexBuffer>     GetIndexBuffer(){return _qIB;}    //!< returns Index Buffer
   boost::shared_ptr<IVertexBuffer>    GetVertexBuffer(){return _qVB;}    //!< returns Vertex Buffer

   // returns true if block is TIN geometry (and not regular grid)
   bool                                IsTin(){return !_bIsBlock;}


   // returns true if no more merges are required
   bool                                IsMergeFinished(){return _bFinishMerge;}

   // returns level of detail of this block
   int GetLod(){return _sQuadcode.length();}
   
   const vec3<double>& GetNormal() {return _vNormal;}
   const vec3<double>& GetCenter() {return _vCenter;}

   void Request(std::vector< boost::shared_ptr<IImageLayer> >&  lstImg, std::vector< boost::shared_ptr<IElevationLayer> >& lstElv);

protected:
   IRenderEngine* _pEngine;
   void _CreateBuffers();
   void _CreateVertexBuffer();
   void _CreateIndexBuffer();
   void _CreateTexture();

   bool _bBuffersCreated;
   void _MergeImageData(boost::shared_array<unsigned char>&data, unsigned int nChannels, unsigned int nTextureSize);

   void _OnTileReady(const std::string& sQuadcode, ImageTile* pTile);
   void _OnTileFailed(const std::string& sQuadcode);

   
   void                                _CreateVertexBufferBlock();
   void                                _CreateVertexBufferTIN();
   void                                _CreateVertexBufferTINCurtainNorth();
   void                                _CreateVertexBufferTINCurtainEast();
   void                                _CreateVertexBufferTINCurtainSouth();
   void                                _CreateVertexBufferTINCurtainWest();
   void                                _CreateIndexBufferTIN();
   void                                _CreateIndexBufferTINCurtain();
   void                                _SetTexture(ITexture* qTexture, bool bLinear);
   bool                                _bIsBlock;
   void                                _UpdateBoundingBox();
   boost::shared_array<unsigned short> _CreateIndices(int& nIndexCount, int a, int b, int c, int d, int x_start, int y_start, int x_end, int y_end, int LOD);
   void                                _QuadKeyMatrixTraversal(std::string& sLowQuadcode, std::string& sHighQuadcode, TextureTransformation<double>& outTexTrans);
   std::string _sQuadcode;
   boost::shared_ptr<MercatorQuadtree> _qQuadtree;
   boost::shared_ptr<IIndexBuffer>     _qIB;    //!< Index Buffer
   boost::shared_ptr<IVertexBuffer>    _qVB;    //!< Vertex Buffer

   boost::shared_ptr<IIndexBuffer>     _qIBStichN;    //!< Index Buffer for stiching
   boost::shared_ptr<IVertexBuffer>    _qVBStichN;    //!< Vertex Buffer for stitching

   boost::shared_ptr<IIndexBuffer>     _qIBStichE;    //!< Index Buffer for stiching
   boost::shared_ptr<IVertexBuffer>    _qVBStichE;    //!< Vertex Buffer for stitching

   boost::shared_ptr<IIndexBuffer>     _qIBStichS;    //!< Index Buffer for stiching
   boost::shared_ptr<IVertexBuffer>    _qVBStichS;    //!< Vertex Buffer for stitching

   boost::shared_ptr<IIndexBuffer>     _qIBStichW;    //!< Index Buffer for stiching
   boost::shared_ptr<IVertexBuffer>    _qVBStichW;    //!< Vertex Buffer for stitching

   ITexture*         _qTexture;   //!< Texture

   bool                                _bReady;
   //mutex _M;

   boost::shared_array<float>                _qElevationData;
   unsigned int                              _nElevationBlockSize;
   boost::shared_array<unsigned char>        _qImageData;
   unsigned int                              _nImageDataChannels; // 3 for RGB, 4 for RGBA
   unsigned int                              _nTextureSize;
   vec3<double>                              _vTilePoints[5]; // coordinate of corner and middle of tile
   aabb3<double>                             _BoundingBox;
   vec3<double>                              _vNormal;
   vec3<double>                              _vCenter;

   vec3<double>                              _vOffset;
   double                                    _dBlockSize;

   double _lng0, _lat0, _lng1, _lat1;

   std::vector< vec3<float> >                  _lstElevationPointWGS84;  // Elevation Points (Cartesian WGS84 minus offset)
   std::vector< vec2<float> >                  _lstTexCoord;             //
   std::vector<int>                            _lstPosIdx;              // Indices

   
   // From Tile Data:
   std::vector<ElevationPoint>               _vCornerPoints;
   std::vector<ElevationPoint>               _vNorthPoints;
   std::vector<ElevationPoint>               _vEastPoints;
   std::vector<ElevationPoint>               _vSouthPoints; 
   std::vector<ElevationPoint>               _vWestPoints;

   bool                                      _bFinishMerge;
   double                                    _dCurtainElevation;
   int                                       _curLayer;
   int                                       _nLod;
   std::vector< boost::shared_ptr<IImageLayer> > _lstImg;
   std::vector< boost::shared_ptr<IElevationLayer> > _lstElv;
   void  _ContinueDownload();

private:
   VirtualGlobeTerrainBlock(){}
};



#endif