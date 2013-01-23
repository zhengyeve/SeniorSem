#ifndef __MAPMANAGER_H__
#define __MAPMANAGER_H__

#include "MapChunk.h"
#include "PolyVoxCore/LargeVolume.h"
#include <vector>

#define MAP_SCALE 5

using namespace std;

class MapManager
{
private:
	vector<MapChunk*> mapChunks;
	PolyVox::LargeVolume<uint8_t>* mapData;
	int32_t chunkSize;
	int32_t maxDrawDist;

	MapChunk* getChunkAt(int32_t x, int32_t y, int32_t z, Ogre::Vector3& pos_within_chunk);

public:
	MapManager(unsigned int chunk_size);
	~MapManager(void);
	void draw(int32_t player_x, int32_t player_y, int32_t player_z, Ogre::SceneManager* mSceneMgr);
	void setMaxDrawDist(unsigned int dist_from_player);
	int32_t getHeightAt(int32_t x, int32_t y, int32_t z, unsigned int column_width);
	void setVoxelAt(int32_t x, int32_t y, int32_t z, uint8_t value);
	uint8_t getVoxelAt(int32_t x, int32_t y, int32_t z);
};

#endif