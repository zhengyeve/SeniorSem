#ifndef __MAPMANAGER_H__
#define __MAPMANAGER_H__

#include "MapChunk.h"
#include "PolyVoxCore/LargeVolume.h"
#include <vector>
#include "PolyVoxCore/MaterialDensityPair.h"

using namespace std;

class MapManager
{
private:
	PolyVox::LargeVolume<MaterialDensityPair88>* mapData;
	int32_t maxDrawDist;
	int32_t maxHeight;
	//a list of all the chunks that have been changed since the last draw operation. They need to be redrawn.
	vector<PolyVox::Vector3DInt32> changedChunks;
	//a cache of chunks (up to 10?), so if we rapidly move back and forth on a chunk border, we won't have to delete and redraw three chunks every frame
	vector<PolyVox::Vector3DInt32> chunkCache;
	//the last player-inhabited chunk. If they've moved substantially, we need to change which chunks are drawn.
	PolyVox::Vector3DInt32 lastPlayerChunk;

	void drawChunk(int32_t chunk_x, int32_t chunk_y, int32_t chunk_z, Ogre::SceneManager* mSceneMgr, bool remove_old);
	void smoothTerrain(void);

public:
	MapManager(void);
	~MapManager(void);
	//the force_redraw parameter makes the function redraw every little chunk, regardless of whether the LOD has changed or if it has been modified. This
	//is computationally expensive, so use only when strictly necessary.
	void draw(int32_t player_x, int32_t player_y, int32_t player_z, Ogre::SceneManager* mSceneMgr, bool force_redraw = false);
	void setMaxDrawDist(unsigned int dist_from_player);
	//gets the highest voxel in a column of size column_width, going downwards from the point
	int32_t getHeightAt(int32_t x, int32_t y, int32_t z, int32_t column_width = 1);
	//gets an averaged height for a point, going downwards from (x, y, z)
	double getAveragedHeightAt(double x, double y, double z);
	//the queue_update tells the map manager whether or not this should add the affected chunk to the "need to be re-drawn" list. This is not desireable if,
	//for example, you're setting a very large number of blocks in a chunk that's going to be updated anyway (ex; world-gen). If you want to force a global redraw,
	//use the draw() function with force_redraw = true. Returns whether an update happened.
	bool setMaterialAt(int32_t x, int32_t y, int32_t z, uint8_t value, bool queue_update = true);
	MaterialDensityPair88 getVoxelAt(int32_t x, int32_t y, int32_t z);
	uint32_t getMaxHeight(void);
};

#endif