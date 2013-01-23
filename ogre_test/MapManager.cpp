#include "MapManager.h"
#include <string>

using namespace std;
using namespace PolyVox;

MapChunk* MapManager::getChunkAt(int32_t x, int32_t y, int32_t z, Ogre::Vector3& pos_within_chunk) {
	cout << "getting chunk at (" << x << ", " << y << ", " << z << ")\n";
	int32_t chunk_x, chunk_y, chunk_z;
	pos_within_chunk.x = x % chunkSize;
	pos_within_chunk.y = y % chunkSize;
	pos_within_chunk.z = z % chunkSize;
	chunk_x = x - pos_within_chunk.x;
	chunk_y = y - pos_within_chunk.y;
	chunk_z = z - pos_within_chunk.z;
	for (unsigned int i = 0; i < mapChunks.size(); ++i) {
		if ((mapChunks[i]->chunkPosition.x == chunk_x) && (mapChunks[i]->chunkPosition.y == chunk_y) && (mapChunks[i]->chunkPosition.z == chunk_z)) {
			cout << "Found chunk with position (" << mapChunks[i]->chunkPosition.x << ", " << mapChunks[i]->chunkPosition.y << ", " << mapChunks[i]->chunkPosition.z << ")\n";
			return mapChunks[i];
		}
	}
	cout << "Found no chunk\n";
	return 0;
}

MapManager::MapManager(unsigned int chunk_size) {
	chunkSize = chunk_size;
	maxDrawDist = 200;
	mapData = new LargeVolume<uint8_t>(Region(Vector3DInt32(0,0,0), Vector3DInt32(4000,4000,4000)));
	cout << "Creating map.\n";
	for (int x = 0; x < 100; ++x) {
		for (int y = 0; y < 100; ++y) {
			for (int z = 0; z < 100; ++z) {
				mapData->setVoxelAt(x,y,z,255);
				//cout << "Map data at : " << mapData->getVoxelAt(x,y,z) << endl;
			}
		}
	}
	cout << "Finished map creation.\n";
}

MapManager::~MapManager(void) {
	for (unsigned int i = 0; i < mapChunks.size(); ++i) {
		delete mapChunks[i];
	}
	delete mapData;
}

/*void MapManager::draw(int32_t player_x, int32_t player_y, int32_t player_z, Ogre::SceneManager* mSceneMgr) {
	cout << "Drawing!\n";
	Ogre::Vector3 pos_within_chunk;
	MapChunk* cur_chunk;
	//check every chunk within max draw range
	for (int32_t area_x = player_x - maxDrawDist; area_x < player_x + maxDrawDist; area_x += chunkSize) {
		//for (int32_t area_y = player_y - maxDrawDist; area_y < player_y + maxDrawDist; area_y += chunkSize) {
		for (int32_t area_y = player_y - chunkSize; area_y < player_y + chunkSize; area_y += chunkSize) {
			//cout << "area_y: " << area_y << endl;
			for (int32_t area_z = player_z - maxDrawDist; area_z < player_z + maxDrawDist; area_z += chunkSize) {
				//cout << "area_z: " << area_z << endl;
				cur_chunk = getChunkAt(area_x, area_y, area_z, pos_within_chunk);
				if (cur_chunk == 0) {
					Ogre::Vector3 chunk_pos (area_x-pos_within_chunk.x, area_y-pos_within_chunk.y, area_z-pos_within_chunk.z);
					string filename = "chunk";
					filename.append(to_string((long long)chunk_pos.x));
					filename.append("_");
					filename.append(to_string((long long)chunk_pos.y));
					filename.append("_");
					filename.append(to_string((long long)chunk_pos.z));
					filename.append(".chk");
					mapChunks.push_back(new MapChunk(filename,chunk_pos,chunkSize/VOXEL_SCALE));
					cout << "Creating child scene node " << filename << endl;
					mSceneMgr->getRootSceneNode()->createChildSceneNode(filename, chunk_pos);
					cur_chunk = mapChunks[mapChunks.size()-1];
				}
				//add LOD
				cur_chunk->draw(mSceneMgr, 5);
			}
		}
	}
	for (unsigned int i = 0; i < mapChunks.size(); ++i) {
		//clean up
	}
}*/

void MapManager::draw(int32_t player_x, int32_t player_y, int32_t player_z, Ogre::SceneManager* mSceneMgr) {
	SurfaceMesh<PositionMaterialNormal> map_mesh;
	Region render_region(Vector3DInt32(player_x-maxDrawDist,player_y-maxDrawDist,player_z-maxDrawDist), Vector3DInt32(player_x+maxDrawDist,player_y+maxDrawDist,player_z+maxDrawDist));
	mapData->prefetch(render_region);
	cout << "Map size: " << mapData->getDepth() << endl;
	//Create a surface extractor. Comment out one of the following two lines to decide which type gets created.
	//CubicSurfaceExtractorWithNormals< SimpleVolume<uint8_t> > surfaceExtractor(chunkData, chunkData->getEnclosingRegion(), &chunkMesh);
	cout << "Render region: (" << player_x-maxDrawDist << ", " << player_y-maxDrawDist << ", " << player_z-maxDrawDist << ") to (" << player_x+maxDrawDist << ", " << player_y+maxDrawDist << ", " << player_z+maxDrawDist << ")\n";
	cout << "Beginning surface extraction.\n";
	MarchingCubesSurfaceExtractor< LargeVolume<uint8_t> > surfaceExtractor(mapData, render_region, &map_mesh);
	//MarchingCubesSurfaceExtractor< SimpleVolume<uint8_t> > surfaceExtractor(chunkData, PolyVox::Region(Vector3DInt32(0,0,0), Vector3DInt32(vol_size-1, vol_size-1, vol_size-1)), &chunkMesh);
	//Execute the surface extractor.
	surfaceExtractor.execute();
	cout << "Finished surface extraction.\n";

	//hacky check to see if this is the first time this has run, and therefore there's no cleanup required
	//if (lastLOD != -1) {
		//remove the last instance of this chunk I guess. In the future we probably want to just update a dynamic mesh
		//mSceneMgr->destroyManualObject(filename);
	//}

	// create a base object to add our data to
	Ogre::ManualObject* ogreMesh = mSceneMgr->createManualObject("map");

	// Tell ogre we will be changing this object later
	//ogreMesh->setDynamic(true);

	cout << "drawing map with " << map_mesh.getNoOfVertices() << " vertices\n";		

	//map_mesh.scaleVertices(MAP_SCALE);
	
	//begin defining the object
	ogreMesh->begin("Examples/BeachStones", Ogre::RenderOperation::OT_TRIANGLE_LIST);
	{	
		//want to scale this by the LOD too
		float textureScale = 0.1;//TEXTURE_SCALE * pow((double)2, curLOD);
		//convenient pointer to the vertices in the map mash
		const std::vector<PolyVox::PositionMaterialNormal>& vecVertices = map_mesh.getVertices();
		//convenient pointer to the indices in the map mash
		const std::vector<uint32_t>& vecIndices = map_mesh.getIndices();
		unsigned int uLodLevel = 0; //Note to self: check on what this lod stuff does...
		int beginIndex = map_mesh.m_vecLodRecords[uLodLevel].beginIndex;
		int endIndex = map_mesh.m_vecLodRecords[uLodLevel].endIndex;
		//cout << "beginIndex: " << beginIndex << " endIndex: " << endIndex << endl;
		//cout << "Vec#: " << vecVertices.size() << endl;

		for(int index = beginIndex; index < endIndex; ++index) {
			const PolyVox::PositionMaterialNormal& vertex = vecVertices[vecIndices[index]];
			const PolyVox::Vector3DFloat& v3dVertexPos = vertex.getPosition();
			const PolyVox::Vector3DFloat& v3dVertexNormal = vertex.getNormal();
			//const PolyVox::Vector3DFloat v3dRegionOffset(uRegionX * g_uRegionSideLength, uRegionY * g_uRegionSideLength, uRegionZ * g_uRegionSideLength);
			const PolyVox::Vector3DFloat v3dFinalVertexPos = v3dVertexPos + static_cast<PolyVox::Vector3DFloat>(map_mesh.m_Region.getLowerCorner());
			//ogreMesh->position(v3dVertexPos.getX()-scale_offset, v3dVertexPos.getY()-scale_offset, v3dVertexPos.getZ()-scale_offset);
			ogreMesh->position(v3dFinalVertexPos.getX(), v3dFinalVertexPos.getY(), v3dFinalVertexPos.getZ());
			ogreMesh->normal(v3dVertexNormal.getX(), v3dVertexNormal.getY(), v3dVertexNormal.getZ());
			ogreMesh->textureCoord(v3dVertexPos.getX()*textureScale, v3dVertexPos.getY()*textureScale);
		}
	}
	//tell ogre we're done defining the object
	ogreMesh->end();

	//attach our object to the scene via a child node
	Ogre::SceneNode* ogreNode = mSceneMgr->getRootSceneNode();
	ogreNode->attachObject(ogreMesh);

	cout << "map drawing finished\n";
}


void MapManager::setMaxDrawDist(unsigned int dist_from_player) {
	maxDrawDist = dist_from_player;
}

int32_t MapManager::getHeightAt(int32_t x, int32_t y, int32_t z, unsigned int column_size) {
	Ogre::Vector3 interior_pos;
	MapChunk* cur_chunk = getChunkAt(x,y,z,interior_pos);
	while (cur_chunk != 0) {
		//for every height, going downwards from the starting position...
		for (int32_t check_y = interior_pos.y; check_y >= 0; --check_y) {
			//check whether a column_size by column_size square has any solid voxels in it. If so, return that value
			for (int32_t check_z = interior_pos.z - column_size + 1; check_z < interior_pos.z + column_size; ++check_z) {
				for (int32_t check_x = interior_pos.x - column_size + 1; check_x < interior_pos.x + column_size; ++check_x) {
					//will have to change this if we implement other non-solid voxel values (such as smoke)
					if (cur_chunk->getVoxel(check_x, check_y, check_z) != 0) {
						return cur_chunk->chunkPosition.y+check_y;
					}
				}
			}
		}
		//get the next lower chunk
		cur_chunk = getChunkAt(x,cur_chunk->chunkPosition.y-1,z,interior_pos);
	}
}

void MapManager::setVoxelAt(int32_t x, int32_t y, int32_t z, uint8_t value) {
	Ogre::Vector3 interior_pos;
	MapChunk* chunk = getChunkAt(x,y,z,interior_pos);
	chunk->setVoxel(interior_pos.x, interior_pos.y, interior_pos.z, value);
}

uint8_t MapManager::getVoxelAt(int32_t x, int32_t y, int32_t z) {
	Ogre::Vector3 interior_pos;
	MapChunk* chunk = getChunkAt(x,y,z,interior_pos);
	return chunk->getVoxel(interior_pos.x, interior_pos.y, interior_pos.z);
}