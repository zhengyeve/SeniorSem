#include "MapManager.h"
#include <string>
#include <math.h>
#include "PolyVoxCore\DefaultMarchingCubesController.h"
#include "PolyVoxCore\LowPassFilter.h"

using namespace std;
using namespace PolyVox;

/*template<typename VoxelType>
class ExtractorController
{
public:
    typedef VoxelType DensityType;
    typedef float MaterialType;
   
    ExtractorController(void) {
        m_tThreshold = ((std::numeric_limits<DensityType>::min)() + (std::numeric_limits<DensityType>::max)()) / 2;
    }
   
    ExtractorController(DensityType tThreshold) {
        m_tThreshold = tThreshold;
    }
   
    DensityType convertToDensity(VoxelType voxel) {
		if (voxel != 0) {
			return 120;
		} else {
			return 0;
		}
    }
   
    MaterialType convertToMaterial(VoxelType voxel) {
        return voxel;
    }
   
    DensityType getThreshold(void) {
        return m_tThreshold;
    }
   
private:
    DensityType m_tThreshold;
};*/

class ExtractorController
{
public:
	typedef uint8_t DensityType;
    typedef uint8_t MaterialType;

    ExtractorController(void) {
        m_tThreshold = ((MaterialDensityPair88::getMinDensity()) + (MaterialDensityPair88::getMaxDensity())) / 2;
    }

	ExtractorController(uint16_t tThreshold) {
        m_tThreshold = tThreshold;
    }
   
    uint16_t convertToDensity(MaterialDensityPair88 voxel) {
		return voxel.getDensity();
    }
   
    uint16_t convertToMaterial(MaterialDensityPair88 voxel) {
		return voxel.getMaterial();
    }
   
    uint16_t getThreshold(void) {
        return m_tThreshold;
    }
   
private:
    uint16_t m_tThreshold;
};

/*void MapManager::smoothTerrain(void) {
	PolyVox::Vector3DInt32 bot_left = mapData->getEnclosingRegion().getLowerCorner();
	PolyVox::Vector3DInt32 top_right = mapData->getEnclosingRegion().getUpperCorner();

	for (int x = bot_left.getX(); x < top_right.getX(); ++x) {
		for (int y = bot_left.getY(); y < top_right.getY(); ++y) {
			for (int z = bot_left.getZ(); z < top_right.getZ(); ++z) {
			}
		}
	}
}*/

MapManager::MapManager(void) {
	maxDrawDist = 100;
	maxHeight = -100;
	mapData = new LargeVolume<MaterialDensityPair88>(Region(Vector3DInt32(-200,-200,-200), Vector3DInt32(200,200,200)));
	cout << "Creating map.\n";
	for (int x = -200; x < 200; ++x) {
		for (int y = -200; y < 200; ++y) {
			for (int z = -200; z < 200; ++z) {
				if ((y+sin((double)z*0.05)*10) < 30) {
					setMaterialAt(x,y,z,255,false);
				} else {
					setMaterialAt(x,y,z,0,false);
				}
				//cout << "Map data at : " << mapData->getVoxelAt(x,y,z) << endl;
			}
		}
	}
	//LowPassFilter< LargeVolume<MaterialDensityPair88>, LargeVolume<MaterialDensityPair88>, MaterialDensityPair88 > pass1(mapData, mapData->getEnclosingRegion(), mapData, mapData->getEnclosingRegion(), 3);
	//pass1.execute();
	cout << "Finished map creation.\n";
	lastPlayerChunk = Vector3DInt32(-999,-999,-999);
}

MapManager::~MapManager(void) {
	delete mapData;
}

string getChunkName(Vector3DInt32 chunk_pos) {
	//build the chunk name, so we can reference the manual object and scene node by this string. Should be in the format "chunk_32_99_200" or such
	string chunk_name = "chunk";
	chunk_name.append(to_string((long long)chunk_pos.getX()));
	chunk_name.append("_");
	chunk_name.append(to_string((long long)chunk_pos.getY()));
	chunk_name.append("_");
	chunk_name.append(to_string((long long)chunk_pos.getZ()));
	return chunk_name;
}

Vector3DInt32 getChunkAt(int32_t x, int32_t y, int32_t z) {
	Vector3DInt32 to_return;
	if (x < 0) {
		to_return.setX(x + (-32 - (x%32)));
	} else {
		to_return.setX(x - (x%32));
	}
	if (y < 0) {
		to_return.setY(y + (-32 - (y%32)));
	} else {
		to_return.setY(y - (y%32));
	}
	if (z < 0) {
		to_return.setZ(z + (-32 - (z%32)));
	} else {
		to_return.setZ(z - (z%32));
	}
	return to_return;
}

void MapManager::drawChunk(int32_t chunk_x, int32_t chunk_y, int32_t chunk_z, Ogre::SceneManager* mSceneMgr, bool remove_old) {
	SurfaceMesh<PositionMaterialNormal> map_mesh;
	Region render_region(Vector3DInt32(chunk_x,chunk_y,chunk_z), Vector3DInt32(chunk_x+32,chunk_y+32,chunk_z+32));
	mapData->prefetch(render_region);
	cout << "Beginning chunk surface extraction...\n";
	//DefaultMarchingCubesController<uint8_t> controller = ExtractorController<uint8_t>(1);
	//DefaultMarchingCubesController<int32_t> controller();
	MarchingCubesSurfaceExtractor< LargeVolume<MaterialDensityPair88>, ExtractorController > surfaceExtractor(mapData, render_region, &map_mesh, ExtractorController());
	//CubicSurfaceExtractorWithNormals< LargeVolume<uint8_t> > surfaceExtractor(mapData, render_region, &map_mesh);
	//Execute the surface extractor.
	surfaceExtractor.execute();
	cout << "Finished surface extraction.\n";

	string chunk_name = getChunkName(Vector3DInt32(chunk_x, chunk_y, chunk_z));

	//If this isn't the first time we've drawn this chunk, there's some cleanup required.
	if (remove_old) {
		mSceneMgr->destroyManualObject(chunk_name);
	}

	// create a base object to add our data to
	Ogre::ManualObject* ogreMesh = mSceneMgr->createManualObject(chunk_name);

	// Tell ogre we will be changing this object later
	//ogreMesh->setDynamic(true);

	cout << "drawing map chunk with " << map_mesh.getNoOfVertices() << " vertices at " << getChunkName(Vector3DInt32(chunk_x, chunk_y, chunk_z)) << endl;

	//map_mesh.scaleVertices(MAP_SCALE);
	
	//begin defining the object
	ogreMesh->begin("TerrainMaterial", Ogre::RenderOperation::OT_TRIANGLE_LIST);
	{	
		//want to scale this by the LOD too
		float textureScale = 100;//TEXTURE_SCALE * pow((double)2, curLOD);
		//convenient pointer to the vertices in the map mash
		const std::vector<PolyVox::PositionMaterialNormal>& vecVertices = map_mesh.getVertices();
		//convenient pointer to the indices in the map mash
		const std::vector<uint32_t>& vecIndices = map_mesh.getIndices();
		unsigned int uLodLevel = 0; //Note to self: check on what this lod stuff does...
		int beginIndex = map_mesh.m_vecLodRecords[uLodLevel].beginIndex;
		int endIndex = map_mesh.m_vecLodRecords[uLodLevel].endIndex;
		//cout << "beginIndex: " << beginIndex << " endIndex: " << endIndex << endl;
		//cout << "Vec#: " << vecVertices.size() << endl;

		int last_mat = 0;
		int vert_count = 0;
		for(int index = beginIndex; index < endIndex; ++index) {
			const PolyVox::PositionMaterialNormal& vertex = vecVertices[vecIndices[index]];
			const PolyVox::Vector3DFloat& v3dVertexPos = vertex.getPosition();
			const PolyVox::Vector3DFloat& v3dVertexNormal = vertex.getNormal();
			//const PolyVox::Vector3DFloat v3dRegionOffset(uRegionX * g_uRegionSideLength, uRegionY * g_uRegionSideLength, uRegionZ * g_uRegionSideLength);
			const PolyVox::Vector3DFloat v3dFinalVertexPos = v3dVertexPos + static_cast<PolyVox::Vector3DFloat>(map_mesh.m_Region.getLowerCorner());
			//ogreMesh->position(v3dVertexPos.getX()-scale_offset, v3dVertexPos.getY()-scale_offset, v3dVertexPos.getZ()-scale_offset);
			ogreMesh->position(v3dFinalVertexPos.getX(), v3dFinalVertexPos.getY(), v3dFinalVertexPos.getZ());
			ogreMesh->normal(v3dVertexNormal.getX(), v3dVertexNormal.getY(), v3dVertexNormal.getZ());
			int tex_x = v3dVertexPos.getX()*textureScale;
			int tex_y = v3dVertexPos.getZ()*textureScale;
			tex_x %= 256;
			tex_y %= 256;
			//tex_x += 64;
			//tex_y += 64;
			//cout << "Tex x: " << tex_x << " Tex y: " << tex_y << endl;
			if ((vert_count%3) == 0) {
				last_mat = vecVertices[vecIndices[index]].getMaterial();
			}
			if (last_mat == 254) {
				//cout << "CHANGING TEX\n";
				tex_x += 768;
				tex_y += 768;
			}
			ogreMesh->textureCoord(tex_x/1024.0, tex_y/1024.0);
			++vert_count;
		}
	}
	//tell ogre we're done defining the object
	ogreMesh->end();

	//attach our object to the scene via a child node
	Ogre::SceneNode* scene_node;
	if (remove_old) {
		scene_node = mSceneMgr->getSceneNode(chunk_name);
	} else {
		scene_node = mSceneMgr->getRootSceneNode()->createChildSceneNode(chunk_name);
	}
	//ogreNode->scale(5,5,5);
	scene_node->attachObject(ogreMesh);
	scene_node->needUpdate();

	cout << "map chunk drawing finished\n";
}

void MapManager::draw(int32_t player_x, int32_t player_y, int32_t player_z, Ogre::SceneManager* mSceneMgr, bool force_redraw) {
	static bool hasBeenDrawn = false;

	cout << "Starting to draw map chunks...\n";

	//calculate the new central chunk and region
	Vector3DInt32 new_center = getChunkAt(player_x, player_y, player_z);
	Region render_region(new_center - Vector3DInt32(32,32,32), new_center + Vector3DInt32(63,63,63));

	//if we've moved we need to draw the new close chunks
	if (force_redraw || (new_center != lastPlayerChunk)) {
		//waaerawerawre
		Region old_region(lastPlayerChunk - Vector3DInt32(32,32,32), lastPlayerChunk + Vector3DInt32(63,63,63));

		//hacky way to make it draw everything the first time around
		if (!hasBeenDrawn) {
			old_region = Region(Vector3DInt32(-999,-999,-999),Vector3DInt32(-999,-999,-999));
		}

		//clear the chunk cache of chunks not in the current render region, or the previous render region.
		vector<Vector3DInt32> temp_cache;
		for (unsigned int i = 0; i < chunkCache.size(); ++i) {
			if (!render_region.containsPoint(chunkCache[i]) && !old_region.containsPoint(chunkCache[i])) {
				mSceneMgr->destroyManualObject(getChunkName(chunkCache[i]));
				cout << "Trying to destroy a scene node in the cache.\n";
				mSceneMgr->destroySceneNode(getChunkName(chunkCache[i]));
				cout << "Successfully destroyed said scene node.\n";
			} else {
				temp_cache.push_back(chunkCache[i]);
			}
		}
		chunkCache = temp_cache;

		//check every chunk coordinate in our current region...
		for (int x = render_region.getLowerCorner().getX(); x < render_region.getUpperCorner().getX(); x+=32) {
			for (int y = render_region.getLowerCorner().getY(); y < render_region.getUpperCorner().getY(); y+=32) {
				for (int z = render_region.getLowerCorner().getZ(); z < render_region.getUpperCorner().getZ(); z+=32) {
					//if this was a new chunk (not shared with old region or the chunk cache), we need to draw it
					if (!old_region.containsPoint(Vector3DInt32(x,y,z))) {
						bool foundit = false;
						//check to see if it's in the chunk cache already
						for (unsigned int i = 0; i < chunkCache.size(); ++i) {
							if ((chunkCache[i].getX() == x) && (chunkCache[i].getY() == y) && (chunkCache[i].getZ() == z)) {
								foundit = true;
								break;
							}
						}
						//if it isn't, draw it
						if (!foundit) {
							//the "false" indicates that this is new and does not require a cleanup
							drawChunk(x,y,z,mSceneMgr,false);
						} else if (force_redraw) {
							//redraw it anyway if we're forcing a redraw
							drawChunk(x,y,z,mSceneMgr,true);
						}
					}
					//if it was actually in the old region, we need to make sure this chunk was not changed. If it was changed, we need to redraw it
					else {					
						bool changed = false;
						//if we're forcing a redraw, set this to true so it will redraw it
						if (force_redraw) {
							changed = true;
						}
						//check to see if it is among the changed chunks
						for (unsigned int i = 0; i < changedChunks.size(); ++i) {
							if ((changedChunks[i].getX() == x) && (changedChunks[i].getY() == y) && (changedChunks[i].getZ() == z)) {
								changed = true;
								break;
							}
						}
						if (changed) {
							//the "true" indicates that this chunk had already been drawn and requires a cleanup
							drawChunk(x,y,z,mSceneMgr,true);
						}
					}
				}
			}
		}

		//check every chunk coordinate in the old region...
		for (int x = old_region.getLowerCorner().getX(); x < old_region.getUpperCorner().getX(); x+=32) {
			for (int y = old_region.getLowerCorner().getY(); y < old_region.getUpperCorner().getY(); y+=32) {
				for (int z = old_region.getLowerCorner().getZ(); z < old_region.getUpperCorner().getZ(); z+=32) {
					//if it's not in the new render region, it needs to be added to the chunk cache (unless we haven't drawn before, in which case it's probably a garbage value)
					if (!render_region.containsPoint(Vector3DInt32(x,y,z)) && hasBeenDrawn) {
						//check to make sure it's not already in there
						bool foundit = false;
						for (unsigned int i = 0; i < chunkCache.size(); ++i) {
							if ((chunkCache[i].getX() == x) && (chunkCache[i].getY() == y) && (chunkCache[i].getZ() == z)) {
								foundit = true;
								break;
							}
						}
						if (!foundit) {
							cout << "Caching point (" << x << ", " << y << ", " << z << ")\n";
							chunkCache.push_back(Vector3DInt32(x,y,z));
						}
					}
				}
			}
		}

		//check every chunk that was changed...
		for (unsigned int i = 0; i < changedChunks.size(); ++i) {
			//if it wasn't in the new render area, we need to redraw it if it's in the cache. If it was, we already took care of it above.
			if (!render_region.containsPoint(changedChunks[i])) {
				bool foundit = false;
				//check to see if it's in the chunk cache
				for (unsigned int j = 0; j < chunkCache.size(); ++j) {
					if ((chunkCache[j].getX() == changedChunks[i].getX()) && (chunkCache[j].getY() == changedChunks[i].getY()) && (chunkCache[j].getZ() == changedChunks[i].getZ())) {
						foundit = true;
						break;
					}
				}
				//if it was in the chunk cache, we need to redraw it.
				if (foundit) {
					//the "true" indicates that this chunk had already been drawn and requires a cleanup
					drawChunk(changedChunks[i].getX(),changedChunks[i].getY(),changedChunks[i].getZ(),mSceneMgr,true);
				}
			}
		}

		//we can now assume we've taken care of every changed chunk that's visible, and clear the vector
		changedChunks.clear();
		//update this, so we can keep track of where we were last in future calls to this function
		lastPlayerChunk = new_center;
		hasBeenDrawn = true;
	} 
	//if we've changed chunks but not moved, we need to update those changed chunks (provided they're visible)
	else if (changedChunks.size() != 0) {
		//for each changed chunk...
		for (unsigned int i = 0; i < changedChunks.size(); ++i) {
			//check to see if it's within the standard render area, if yes, redraw it
			if (render_region.containsPoint(changedChunks[i])) {
				//the "true" tells it that the chunk has already been drawn, and needs cleaning up
				drawChunk(changedChunks[i].getX(),changedChunks[i].getY(),changedChunks[i].getZ(),mSceneMgr,true);
			}

			//check to see if it's in the chunk cache. If yes, redraw it.
			bool foundit = false;
			for (unsigned int j = 0; j < chunkCache.size(); ++j) {
				if ((chunkCache[j].getX() == changedChunks[j].getX()) && (chunkCache[j].getY() == changedChunks[j].getY()) && (chunkCache[j].getZ() == changedChunks[j].getZ())) {
					foundit = true;
					break;
				}
			}
			if (foundit) {
				//the "true" tells it that the chunk has already been drawn, and needs cleaning up
				drawChunk(changedChunks[i].getX(),changedChunks[i].getY(),changedChunks[i].getZ(),mSceneMgr,true);
			}
		}
		//clear the changed chunk vector, since we've taken care of them all.
		changedChunks.clear();
	}

	cout << "Finished drawing all map chunks\n";
}


void MapManager::setMaxDrawDist(unsigned int dist_from_player) {
	maxDrawDist = dist_from_player;
}

//tries to interpolate height values at a given point
double MapManager::getAveragedHeightAt(double x, double y, double z) {
	//use integer precision dropping to get the integer values on either side of the target. E.g. if going for an x value of 2.3, we want to check x=2 and x=3
	int32_t primary_x = x;
	int32_t primary_z = z;
	float remainder_x = x - primary_x;
	float remainder_z = z - primary_z;
	int32_t secondary_x = x + (1.0 - remainder_x);
	int32_t secondary_z = z + (1.0 - remainder_z);
	int32_t primary_height = getHeightAt(primary_x, y, primary_z);
	int32_t secondary_height = getHeightAt(secondary_x, y, secondary_z);
	//if the two points are far apart, don't interpolate the values, just get the higher of the two. For example, if you're walking on a narrow bridge above a chasm,
	//you don't want it to interpolate values on the edge of the bridge, since that would give you a point halfway to the bottom of the chasm.
	if (abs(secondary_height-primary_height) > 1) {
		return max(primary_height, secondary_height);
	} else {
		return (((remainder_x * secondary_height) + ((1.0 - remainder_x) * primary_height) + (remainder_z * secondary_height) + ((1.0 - remainder_z) * primary_height)) / 2.0);
	}
}

int32_t MapManager::getHeightAt(int32_t x, int32_t y, int32_t z, int32_t column_size) {
	//cout << "Checking height (" << x << ", " << y << ", " << z << ")\n";
	Vector3DInt32 point(x, y, z);
	if (!mapData->getEnclosingRegion().containsPoint(point)) {
		cout << "Tried to height-check point (" << x << ", " << y << ", " << z << ") which is outside the volume.\n";
		return -999;
	}
	//for every height, going downwards from the starting position...
	for (int32_t check_y = y; check_y >= mapData->getEnclosingRegion().getLowerCorner().getY(); --check_y) {
		//check whether a column_size by column_size square has any solid voxels in it. If so, return that value
		for (int32_t check_z = z - column_size + 1; check_z < z + column_size; ++check_z) {
			for (int32_t check_x = x - column_size + 1; check_x < x + column_size; ++check_x) {
				//will have to change this if we implement other non-solid voxel values (such as smoke)
				//cout << "Checking height (" << check_x << ", " << check_y << ", " << check_z << ")\n";
				MaterialDensityPair88 voxel_val = getVoxelAt(check_x, check_y, check_z);
				if (voxel_val.getMaterial() != 0) {
					//cout << "Result: " << check_y << endl;
					return (check_y - ((255 - voxel_val.getDensity())/255.0));
				}
			}
		}
	}
	//didn't find a solid voxel in that column, so return the default. Kinda hacky, but should help in debugging since the ninja will all of a sudden appear far beneath the surface.
	//cout << "No height found there, so returning -999.\n";
	return -999;
}

bool MapManager::setMaterialAt(int32_t x, int32_t y, int32_t z, uint8_t value, bool queue_update) {
	/*Vector3DInt32 point(x, y, z);
	if (!mapData->getEnclosingRegion().containsPoint(point)) {
		cout << "Tried to set point (" << x << ", " << y << ", " << z << ") which is outside the volume.\n";
		return;
	}*/
	MaterialDensityPair88 voxel_val = getVoxelAt(x,y,z);

	if (voxel_val.getMaterial() != value) {
		if ((y > maxHeight) && (value > 0)) {
			//cout << "New max height: " << y << " from setting point (" << x << ", " << y << ", " << z << ") to " << (unsigned int)value << ".\n";
			maxHeight = y;
		}
		if (queue_update) {
			for (int32_t chunk_x = x - 1; chunk_x < x + 2; ++chunk_x) {
				for (int32_t chunk_y = y - 1; chunk_y < y + 2; ++chunk_y) {
					for (int32_t chunk_z = z - 1; chunk_z < z + 2; ++chunk_z) {
						//add the affected chunk to the list of changed chunks, so we know to re-draw it (unless it's already in there)
						bool foundit = false;
						Vector3DInt32 changed_chunk = getChunkAt(chunk_x,chunk_y,chunk_z);
						for (unsigned int i = 0; i < changedChunks.size(); ++i) {
							if ((changedChunks[i].getX() == changed_chunk.getX()) && (changedChunks[i].getY() == changed_chunk.getY()) && (changedChunks[i].getZ() == changed_chunk.getZ())) {
								foundit = true;
								break;
							}
						}
						//if it wasn't already present in the list of changed chunks, add it in.
						if (!foundit) {
							changedChunks.push_back(changed_chunk);
							//cout << "Setting voxel at (" << x << ", " << y << ", " << z << ") which is in " << getChunkName(changed_chunk) << endl;
							cout << "Adding chunk " << getChunkName(changed_chunk) << " to the changed list.\n";
						}
					}
				}
			}
		}
		
		if (value > 0) {
			voxel_val.setDensity(255);
			voxel_val.setMaterial(value);
		} else {
			if (voxel_val.getDensity() > 1) {
				voxel_val.setDensity(voxel_val.getDensity()-1);
			} else {
				voxel_val.setMaterial(value);
				voxel_val.setDensity(0);
			}
		}
		mapData->setVoxelAt(x, y, z, voxel_val);
		return true;
	}
	return false;
}

 MaterialDensityPair88 MapManager::getVoxelAt(int32_t x, int32_t y, int32_t z) {
	/*if (!mapData->getEnclosingRegion().containsPoint(point)) {
		cout << "Tried to get point (" << x << ", " << y << ", " << z << ") which is outside the volume.\n";
		return 0;
	}*/
	return mapData->getVoxelAt(x, y, z);
}

uint32_t MapManager::getMaxHeight(void) {
	return maxHeight;
}