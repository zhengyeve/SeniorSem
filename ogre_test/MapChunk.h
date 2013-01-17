#ifndef __MAPCHUNK_H__
#define __MAPCHUNK_H__

#include "PolyVoxCore/SimpleVolume.h"
#include "PolyVoxCore/CubicSurfaceExtractorWithNormals.h"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"
#include "PolyVoxCore/SurfaceMesh.h"
#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/SimpleVolume.h"
#include <OgreSceneManager.h>
#include <OgreManualObject.h>

#define MAP_SCALE 10
#define TEXTURE_SCALE 0.5

using namespace PolyVox;

class MapChunk
{
private:
	SimpleVolume<uint8_t>* chunkData;
	SimpleVolume<uint8_t>* simplifiedData;
	SurfaceMesh<PositionMaterialNormal> chunkMesh;
	bool changedSinceDrawn, isLoaded;
	int lastLOD;
	char* filename;

public:
	MapChunk(char* filename);
	~MapChunk(void);
	void draw(Ogre::SceneManager* mSceneMgr, int curLOD);
	void unload();
	void load();
	void setVoxel(int32_t x, int32_t y, int32_t z, uint8_t data);
	uint8_t getVoxel(int32_t x, int32_t y, int32_t z);
	void reduceDetail(int LOD_level);
};

#endif