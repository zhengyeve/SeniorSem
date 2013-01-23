#include "MapChunk.h"
#include "PolyVoxCore\MeshDecimator.h"
#include <vector>
#include <math.h>

using namespace std;
using namespace Ogre;
using namespace PolyVox;

MapChunk::MapChunk(string file_name, Ogre::Vector3 chunk_position, int num_voxels) {
	chunkPosition = chunk_position;
	filename = file_name;
	isLoaded = true;
	//tells the draw function to redraw our geometry
	changedSinceDrawn = true;
	//HACKY thing to tell the draw function not to delete the manual object on the first run...
	lastLOD = -1;
	chunkData = new SimpleVolume<uint8_t>(PolyVox::Region(Vector3DInt32(0,0,0), Vector3DInt32(num_voxels-1, num_voxels-1, num_voxels-1)));
	//just to have something to delete in the future. Otherwise it throws an error when trying to delete the old simplifiedData that happens to be uninitialized
	simplifiedData = new SimpleVolume<uint8_t>(PolyVox::Region(Vector3DInt32(0,0,0), Vector3DInt32(7, 7, 7)));
	cout << "depth: " << chunkData->getDepth() << endl;
	for (int z = 0; z < chunkData->getDepth(); z++)
	{
		for (int y = 0; y < chunkData->getHeight(); y++)
		{
			for (int x = 0; x < chunkData->getWidth(); x++)
			{
				//if (y < chunkData->getHeight()/2){
					//if ((x+z)%5 != 0) {
						chunkData->setVoxelAt(x, y, z, 255);
					//}
				//} else {
					//chunkData->setVoxelAt(x, y, z, 0);
				//}
			}
		}
	}
}

MapChunk::~MapChunk(void) {
	delete chunkData;
	delete simplifiedData;
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
				//cout << "x_group: " << x_group << " depth: " << chunkData->getDepth() << " voxel_grouping: " << voxel_grouping << endl;
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
				//cout << "Setting voxel at " << x_group/voxel_grouping << ", " << y_group/voxel_grouping << ", " << z_group/voxel_grouping << ") to " << winner << " with a volume size of " << new_chunk_size << endl;
				temp_data->setVoxelAt(x_group/voxel_grouping, y_group/voxel_grouping, z_group/voxel_grouping, 255);
			}
		}
	}
	delete simplifiedData;
	simplifiedData = temp_data;
	cout << "DECIMATION SUCCESSFUL. SIZE OF RESULTING VOLUME: " << temp_data->getDepth() << endl;
}

void MapChunk::draw(Ogre::SceneManager* mSceneMgr, int curLOD) {
	//if the map hasn't been loaded into memory, load it
	if (!isLoaded) {
		load();
	}

	//if the map has changed since it was last drawn, recreate it
	if (changedSinceDrawn || curLOD != lastLOD) {
		//reduce the detail of the map (for chunks far away from the player)
		reduceDetail(curLOD);

		//if the detail is reduced, run the mesh extractor on the simplified version of the data set.
		if (curLOD != 0) {
			int vol_size = simplifiedData->getDepth();
			MarchingCubesSurfaceExtractor< SimpleVolume<uint8_t> > surfaceExtractor(simplifiedData, PolyVox::Region(Vector3DInt32(-1,-1,-1), Vector3DInt32(vol_size+1, vol_size+1, vol_size+1)), &chunkMesh);
			//MarchingCubesSurfaceExtractor< SimpleVolume<uint8_t> > surfaceExtractor(simplifiedData, PolyVox::Region(Vector3DInt32(0,0,0), Vector3DInt32(vol_size-1, vol_size-1, vol_size-1)), &chunkMesh);
			//Execute the surface extractor.
			surfaceExtractor.execute();
		} else {
			int vol_size = chunkData->getDepth();
			//Create a surface extractor. Comment out one of the following two lines to decide which type gets created.
			//CubicSurfaceExtractorWithNormals< SimpleVolume<uint8_t> > surfaceExtractor(chunkData, chunkData->getEnclosingRegion(), &chunkMesh);
			MarchingCubesSurfaceExtractor< SimpleVolume<uint8_t> > surfaceExtractor(chunkData, PolyVox::Region(Vector3DInt32(-1,-1,-1), Vector3DInt32(vol_size+1, vol_size+1, vol_size+1)), &chunkMesh);
			//MarchingCubesSurfaceExtractor< SimpleVolume<uint8_t> > surfaceExtractor(chunkData, PolyVox::Region(Vector3DInt32(0,0,0), Vector3DInt32(vol_size-1, vol_size-1, vol_size-1)), &chunkMesh);
			//Execute the surface extractor.
			surfaceExtractor.execute();
		}

		//hacky check to see if this is the first time this has run, and therefore there's no cleanup required
		if (lastLOD != -1) {
			//remove the last instance of this chunk I guess. In the future we probably want to just update a dynamic mesh
			mSceneMgr->destroyManualObject(filename);
		}

		cout << "Attempting to add object to " << filename << endl;
		// create a base object to add our data to
		Ogre::ManualObject* ogreMesh = mSceneMgr->createManualObject(filename);

		// Tell ogre we will be changing this object later
		//ogreMesh->setDynamic(true);

		cout << "drawing map with " << chunkMesh.getNoOfVertices() << " vertices\n";		
		double scale_offset = VOXEL_SCALE*pow((double)2, curLOD);

		//scale the vertices by the LOD scale
		chunkMesh.scaleVertices(scale_offset);
		//chunkMesh.scaleVertices(600);
	
		//begin defining the object
		ogreMesh->begin("Examples/BeachStones", Ogre::RenderOperation::OT_TRIANGLE_LIST);
		{	
			//want to scale this by the LOD too
			float textureScale = 0.1;//TEXTURE_SCALE * pow((double)2, curLOD);
			//convenient pointer to the vertices in the map mash
			const std::vector<PolyVox::PositionMaterialNormal>& vecVertices = chunkMesh.getVertices();
			//convenient pointer to the indices in the map mash
			const std::vector<uint32_t>& vecIndices = chunkMesh.getIndices();
			unsigned int uLodLevel = 0; //Note to self: check on what this lod stuff does...
			int beginIndex = chunkMesh.m_vecLodRecords[uLodLevel].beginIndex;
			int endIndex = chunkMesh.m_vecLodRecords[uLodLevel].endIndex;
			//cout << "beginIndex: " << beginIndex << " endIndex: " << endIndex << endl;
			//cout << "Vec#: " << vecVertices.size() << endl;

			for(int index = beginIndex; index < endIndex; ++index) {
				const PolyVox::PositionMaterialNormal& vertex = vecVertices[vecIndices[index]];
				const PolyVox::Vector3DFloat& v3dVertexPos = vertex.getPosition();
				const PolyVox::Vector3DFloat& v3dVertexNormal = vertex.getNormal();
				//const PolyVox::Vector3DFloat v3dRegionOffset(uRegionX * g_uRegionSideLength, uRegionY * g_uRegionSideLength, uRegionZ * g_uRegionSideLength);
				const PolyVox::Vector3DFloat v3dFinalVertexPos = v3dVertexPos + static_cast<PolyVox::Vector3DFloat>(chunkMesh.m_Region.getLowerCorner());
				//ogreMesh->position(v3dVertexPos.getX()-scale_offset, v3dVertexPos.getY()-scale_offset, v3dVertexPos.getZ()-scale_offset);
				ogreMesh->position(v3dFinalVertexPos.getX(), v3dFinalVertexPos.getY(), v3dFinalVertexPos.getZ());
				ogreMesh->normal(v3dVertexNormal.getX(), v3dVertexNormal.getY(), v3dVertexNormal.getZ());
				ogreMesh->textureCoord(v3dVertexPos.getX()*textureScale, v3dVertexPos.getY()*textureScale);
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
	return chunkData->getVoxelAt(x, y, z);
}