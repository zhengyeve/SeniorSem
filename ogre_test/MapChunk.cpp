#include "MapChunk.h"
#include "PolyVoxCore\MeshDecimator.h"
#include <vector>
#include <math.h>

using namespace std;
using namespace Ogre;
using namespace PolyVox;

MapChunk::MapChunk(char* file_name) {
	filename = file_name;
	isLoaded = true;
	//tells the draw function to redraw our geometry
	changedSinceDrawn = true;
	//HACKY thing to tell the draw function not to delete the manual object on the first run...
	lastLOD = -1;
	chunkData = new SimpleVolume<uint8_t>(PolyVox::Region(Vector3DInt32(0,0,0), Vector3DInt32(63, 63, 63)));
	//just to have something to delete in the future. Otherwise it throws an error when trying to delete the old simplifiedData that happens to be uninitialized
	simplifiedData = new SimpleVolume<uint8_t>(PolyVox::Region(Vector3DInt32(0,0,0), Vector3DInt32(7, 7, 7)));
	for (int z = 0; z < chunkData->getDepth(); z++)
	{
		for (int y = 0; y < chunkData->getHeight(); y++)
		{
			for (int x = 0; x < chunkData->getWidth(); x++)
			{
				if (x*y < chunkData->getHeight()){
					if ((x+z)%5 != 0) {
						chunkData->setVoxelAt(x, y, z, 255);
					}
				} else {
					chunkData->setVoxelAt(x, y, z, 0);
				}
			}
		}
	}
}

MapChunk::~MapChunk(void) {
	delete chunkData;
}

void MapChunk::reduceDetail(int LOD_level) {
	if (LOD_level == 0) {
		return;
	}

	cout << "BEGINNING DECIMATION OF LEVEL " << LOD_level << endl;
	//the level of detail voxel grouping always has to be a power of two, so that we can be sure it divides evenly into a map with a size that's a power of two
	int voxel_grouping = pow((double)2, LOD_level);

	//make sure that our voxel grouping leaves us with at least a 2x2x2 cube
	if (voxel_grouping > (chunkData->getWidth()/2)) {
		voxel_grouping = chunkData->getWidth()/2;
	}

	//assuming that each chunk is a cube
	int new_chunk_size = (chunkData->getWidth()/voxel_grouping)-1;

	//create a volume to store the reduced map in
	SimpleVolume<uint8_t>* temp_data = new SimpleVolume<uint8_t>(PolyVox::Region(Vector3DInt32(0,0,0), Vector3DInt32(new_chunk_size, new_chunk_size, new_chunk_size)));
	std::vector<uint8_t> materials;
	std::vector<unsigned int> counts;

	//loop through every cluster of voxels, find the average material (weighted), and set the voxel in the new simplified chunk map to this average
	for (unsigned int z_group = 0; z_group < chunkData->getDepth(); z_group+=voxel_grouping)
	{
		for (unsigned int y_group = 0; y_group < chunkData->getHeight(); y_group+=voxel_grouping)
		{
			for (unsigned int x_group = 0; x_group < chunkData->getWidth(); x_group+=voxel_grouping)
			{
				//clear these vectors in preparation for compiling a list of all materials in the volume, and their weighted counts
				materials.clear();
				counts.clear();
				for (unsigned int z_individual = z_group; z_individual < (z_group + voxel_grouping); ++z_individual) {
					for (unsigned int y_individual = y_group; y_individual < (y_group + voxel_grouping); ++y_individual) {
						for (unsigned int x_individual = x_group; x_individual < (x_group + voxel_grouping); ++x_individual) {
							int this_count = 0;
							uint8_t this_mat = chunkData->getVoxelAt(x_group+x_individual, y_group+y_individual, z_group+z_individual);
							bool foundit = false;
							for (int i = 0; i < materials.size(); ++i) {
								if (materials[i] == this_mat) {
									this_count = counts[i];
									//if this voxel is not air...
									if (this_mat != 0) {
										//weight it according to it's z-height, giving a 3x higher weight to non-air materials
										this_count += z_individual*3;
									} else {
										++this_count;
									}
									counts[i] = this_count;
									foundit = true;
									break;
								}
							}
							if (!foundit) {
								materials.push_back(this_mat);
								//if this voxel is not air...
								if (this_mat != 0) {
									//weight it according to it's z-height, giving a 3x higher weight to non-air materials
									counts.push_back(z_individual*3);
								} else {
									counts.push_back(this_count);
								}
							}
						}
					}
				}
				uint8_t winner;
				int win_count = 0;
				for (unsigned int i = 0; i < counts.size(); ++i) {
					if (counts[i] > win_count) {
						win_count = counts[i];
						winner = materials[i];
					}
				}
				//cout << "Setting voxel at " << x_group << ", " << y_group << ", " << z_group << ") to " << winner << endl;
				temp_data->setVoxelAt(x_group/voxel_grouping, y_group/voxel_grouping, z_group/voxel_grouping, winner);
			}
		}
	}
	delete simplifiedData;
	simplifiedData = temp_data;
	cout << "DECIMATION SUCCESSFUL\n";
}

void MapChunk::draw(Ogre::SceneManager* mSceneMgr, int curLOD) {
	//if the map hasn't been loaded into memory, load it
	if (!isLoaded) {
		load();
	}

	//if the map has changed since it was last drawn, recreate it
	if (changedSinceDrawn) {
		//reduce the detail of the map (for chunks far away from the player)
		reduceDetail(curLOD);

		//if the detail is reduced, run the mesh extractor on the simplified version of the data set.
		if (curLOD != 0) {
			MarchingCubesSurfaceExtractor< SimpleVolume<uint8_t> > surfaceExtractor(simplifiedData, simplifiedData->getEnclosingRegion(), &chunkMesh);
			//Execute the surface extractor.
			surfaceExtractor.execute();
		} else {
			//Create a surface extractor. Comment out one of the following two lines to decide which type gets created.
			//CubicSurfaceExtractorWithNormals< SimpleVolume<uint8_t> > surfaceExtractor(chunkData, chunkData->getEnclosingRegion(), &chunkMesh);
			MarchingCubesSurfaceExtractor< SimpleVolume<uint8_t> > surfaceExtractor(chunkData, chunkData->getEnclosingRegion(), &chunkMesh);//Execute the surface extractor.
			surfaceExtractor.execute();
		}

		//hacky check to see if this is the first time this has run, and therefore there's no cleanup required
		if (lastLOD != -1) {
			//remove the last instance of this chunk I guess. In the future we probably want to just update a dynamic mesh
			mSceneMgr->destroyManualObject(filename);
		}

		// create a base object to add our data to
		Ogre::ManualObject* ogreMesh = mSceneMgr->createManualObject(filename);

		// Tell ogre we will be changing this object later
		//ogreMesh->setDynamic(true);

		cout << "drawing map with " << chunkMesh.getNoOfVertices() << " vertices\n";
	
		//begin defining the object
		ogreMesh->begin("Examples/BeachStones", Ogre::RenderOperation::OT_TRIANGLE_LIST);
		{	
			// the bigger this is, the bigger the voxel volume will be. Scale the map by the LOD so that simplified chunks won't be smaller
			double mapScale = MAP_SCALE*pow((double)2,curLOD);
			//want to scale this by the LOD too
			float textureScale = TEXTURE_SCALE*curLOD;
			//convenient pointer to the vertices in the map mash
			const std::vector<PolyVox::PositionMaterialNormal>& vecVertices = chunkMesh.getVertices();
			//convenient pointer to the indices in the map mash
			const std::vector<uint32_t>& vecIndices = chunkMesh.getIndices();
			unsigned int uLodLevel = 0; //Note to self: check on what this lod stuff does...
			int beginIndex = chunkMesh.m_vecLodRecords[uLodLevel].beginIndex;
			int endIndex = chunkMesh.m_vecLodRecords[uLodLevel].endIndex;
			for(int index = beginIndex; index < endIndex; ++index) {
				const PolyVox::PositionMaterialNormal& vertex = vecVertices[vecIndices[index]];
				const PolyVox::Vector3DFloat& v3dVertexPos = vertex.getPosition();
				const PolyVox::Vector3DFloat& v3dVertexNormal = vertex.getNormal();
				//const PolyVox::Vector3DFloat v3dRegionOffset(uRegionX * g_uRegionSideLength, uRegionY * g_uRegionSideLength, uRegionZ * g_uRegionSideLength);
				const PolyVox::Vector3DFloat v3dFinalVertexPos = v3dVertexPos + static_cast<PolyVox::Vector3DFloat>(chunkMesh.m_Region.getLowerCorner());
				ogreMesh->position(v3dFinalVertexPos.getX()*mapScale, v3dFinalVertexPos.getY()*mapScale, v3dFinalVertexPos.getZ()*mapScale);
				ogreMesh->normal(v3dVertexNormal.getX(), v3dVertexNormal.getY(), v3dVertexNormal.getZ());
				ogreMesh->textureCoord(v3dFinalVertexPos.getX()*textureScale, v3dFinalVertexPos.getY()*textureScale);
			}
		}
		//tell ogre we're done defining the object
		ogreMesh->end();

		//attach our object to the scene via a child node
		Ogre::SceneNode* ogreNode = mSceneMgr->getSceneNode(filename);
		ogreNode->attachObject(ogreMesh);

		cout << "map drawing finished\n";
		changedSinceDrawn = false;
		lastLOD = curLOD;
	}
}

void MapChunk::unload(void) {
	//save the map to file
	isLoaded = false;
}

void MapChunk::load(void) {
	//get the chunk data from file
	isLoaded = true;
	//force a re-generation of the map mesh
	changedSinceDrawn = true;
}

void MapChunk::setVoxel(int32_t x, int32_t y, int32_t z, uint8_t data) {
	chunkData->setVoxelAt(x, y, z, data);
	changedSinceDrawn = true;
}

uint8_t MapChunk::getVoxel(int32_t x, int32_t y, int32_t z) {
	chunkData->getVoxelAt(x, y, z);
}