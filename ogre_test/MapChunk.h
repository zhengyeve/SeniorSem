#ifndef __MAPCHUNK_H__
#define __MAPCHUNK_H__

#include "PolyVoxCore/SimpleVolume.h"
#include "PolyVoxCore/CubicSurfaceExtractorWithNormals.h"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"
#include "PolyVoxCore/SurfaceMesh.h"
#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/LargeVolume.h"
#include <OgreSceneManager.h>
#include <OgreManualObject.h>
#include <string>

#define VOXEL_SCALE 5
#define TEXTURE_SCALE 0.5

using namespace PolyVox;
using namespace std;

class MapChunk
{
private:
	SimpleVolume<uint8_t>* chunkData;
	SimpleVolume<uint8_t>* simplifiedData;
	SurfaceMesh<PositionMaterialNormal> chunkMesh;
	bool changedSinceDrawn, isLoaded;
	int lastLOD;
	string filename;

public:
	Ogre::Vector3 chunkPosition;
	MapChunk(string file_name, Ogre::Vector3 chunk_position, int num_voxels);
	~MapChunk(void);
	void draw(Ogre::SceneManager* mSceneMgr, int curLOD);
	void unload();
	void load();
	void setVoxel(int32_t x, int32_t y, int32_t z, uint8_t data);
	uint8_t getVoxel(int32_t x, int32_t y, int32_t z);
	void reduceDetail(int LOD_level);
};

#endif