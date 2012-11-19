/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.cpp
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _ 
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/                              
      Tutorial Framework
      http://www.ogre3d.org/tikiwiki/
-----------------------------------------------------------------------------
*/
#include "GameFramework.h"
#include <OgreMath.h>
#include "TreeObject.h"
#include "PlantManager.h"

GameFramework::GameFramework(void)
{
}
 
//-------------------------------------------------------------------------------------
GameFramework::~GameFramework(void)
{
	for (int i = 0; i < worldObjects.size(); ++i) {
		delete(worldObjects[i]);
	}
}

void GameFramework::destroyScene(void)
{
	OGRE_DELETE mTerrainGroup;
    OGRE_DELETE mTerrainGlobals;
}
//-------------------------------------------------------------------------------------
void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img)
{
	img.load("myTerrain.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    if (flipX)
        img.flipAroundY();
    if (flipY)
        img.flipAroundX();
}

void GameFramework::defineTerrain(long x, long y)
{
	Ogre::String filename = mTerrainGroup->generateFilename(x, y);

	//If the map resource file exists, load it. If not, make it.
    if (Ogre::ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), filename))
    {
        mTerrainGroup->defineTerrain(x, y);
    }
    else
    {
        Ogre::Image img;
        getTerrainImage(x % 2 != 0, y % 2 != 0, img);
        mTerrainGroup->defineTerrain(x, y, &img);
        mTerrainsImported = true;
    }
}

void GameFramework::initBlendMaps(Ogre::Terrain* terrain)
{
	//in this case, blendMap0 is the dirt texture and blendMap1 is the rocky texture, with the base being grass. May change in the future.
	Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
    Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
	//minimum heights that the textures will appear at. The fadeDist vars are for how far they spread out. Anything below the minimum heights
	//will be the base texture (in this case grass)
    Ogre::Real minHeight0 = 40;
    Ogre::Real fadeDist0 = 40;
    Ogre::Real minHeight1 = 75;
    Ogre::Real fadeDist1 = 40;
    float* pBlend0 = blendMap0->getBlendPointer();
    float* pBlend1 = blendMap1->getBlendPointer();
    for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
    {
        for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
        {
            Ogre::Real tx, ty;
 
            blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
            Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
            Ogre::Real val = (height - minHeight0) / fadeDist0;
            val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
            *pBlend0++ = val;
 
            val = (height - minHeight1) / fadeDist1;
            val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
            *pBlend1++ = val;
        }
    }
    blendMap0->dirty();
    blendMap1->dirty();
    blendMap0->update();
    blendMap1->update();
}
//-------------------------------------------------------------------------------------
void GameFramework::configureTerrainDefaults(Ogre::Light* light)
{
	// Configure global
    mTerrainGlobals->setMaxPixelError(0);
    // testing composite map
    mTerrainGlobals->setCompositeMapDistance(350);
 
    // Important to set these so that the terrain knows what to use for derived (non-realtime) data
    mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
    mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
    mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());
 
    // Configure default import settings for if we use imported image
    Ogre::Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
    defaultimp.terrainSize = 129;
    defaultimp.worldSize = 3000.0f;
    defaultimp.inputScale = 150;
    defaultimp.minBatchSize = 33;
    defaultimp.maxBatchSize = 65;
    // textures
    defaultimp.layerList.resize(3);
    defaultimp.layerList[0].worldSize = 100;
    defaultimp.layerList[0].textureNames.push_back("grass_green-01_diffusespecular.dds");
    defaultimp.layerList[0].textureNames.push_back("grass_green-01_normalheight.dds");
    defaultimp.layerList[1].worldSize = 30;
    defaultimp.layerList[1].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
    defaultimp.layerList[1].textureNames.push_back("dirt_grayrocky_normalheight.dds");
    defaultimp.layerList[2].worldSize = 50;
    defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
    defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
}

void GameFramework::populatePlants(void) {
	Ogre::Vector3 new_pos;
	PlantManager plantManager;
	int map_size = mTerrainGroup->getTerrainWorldSize();
	int min_plant_spacing = 5;
	//goes from -(1/2 map size) to +(1/2 map size), incrementing by the minimum plant spacing each time, which prevents putting like 80 trees within a foot of one another
	for (int i = -0.5*map_size; i < map_size*0.5; i+=min_plant_spacing) {
		for (int j = -0.5*map_size; j < map_size*0.5; j+=min_plant_spacing) {
			double cur_height = mTerrainGroup->getHeightAtWorldPosition(i*min_plant_spacing,0,j*min_plant_spacing);
			double height_diff = abs(cur_height - mTerrainGroup->getHeightAtWorldPosition((i+1)*min_plant_spacing,0,(j+1)*min_plant_spacing))+1;
			height_diff += abs(cur_height - mTerrainGroup->getHeightAtWorldPosition((i-1)*min_plant_spacing,0,(j+1)*min_plant_spacing));
			height_diff += abs(cur_height - mTerrainGroup->getHeightAtWorldPosition((i-1)*min_plant_spacing,0,(j-1)*min_plant_spacing));
			height_diff += abs(cur_height - mTerrainGroup->getHeightAtWorldPosition((i+1)*min_plant_spacing,0,(j-1)*min_plant_spacing));

			//get what kind of plant we are making here from the plant manager
			PlantType to_place = plantManager.getFlora(cur_height,1.0/height_diff);
			//if the plant manager actually gave us a plant, create it
			if (to_place != PLANT_NONE) {
				//initialize this before the switch statement, since you can't do that inside...
				Ogre::Entity* tree_entity;
				//change the model based on what plant we're placing
				switch(to_place) {
					case PLANT_OAK:
						tree_entity = mSceneMgr->createEntity("tree.mesh");
						break;
					case PLANT_PINE:
						tree_entity = mSceneMgr->createEntity("pinetree.mesh");
						break;
					case PLANT_ROUND_SHROOM:
						tree_entity = mSceneMgr->createEntity("roundshroom.mesh");
						break;
				};
				new_pos.x = i*min_plant_spacing;
				new_pos.z = j*min_plant_spacing;
				new_pos.y = mTerrainGroup->getHeightAtWorldPosition(new_pos);
				Ogre::SceneNode *temp_node = mSceneMgr->getRootSceneNode()->createChildSceneNode(new_pos);
				temp_node->attachObject(tree_entity);
				//scale the model randomly
				temp_node->scale(2+(rand()%11)/10.0,2+(rand()%11)/10.0,2+(rand()%11)/10.0);
				//give the tree a random rotation about the y-axis, so the trees aren't all aligned on a grid
				Ogre::Radian rot_angle((Ogre::Real)(rand()%7));
				temp_node->rotate(Ogre::Vector3(0,1,0),rot_angle);
				worldObjects.push_back(new TreeObject(temp_node,2));
			}
		}
	}
	worldObjects.resize(worldObjects.size());
}

void GameFramework::createScene(void)
{ 
	//create a console so we can print debug statements
	AllocConsole();
	freopen("CONOUT$","wb",stdout);
	srand((int)std::time(NULL));

    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
    Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(7);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../myMedia","FileSystem");
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
 
    Ogre::Vector3 lightdir(0.55, -0.3, 0.75);
    lightdir.normalise();
 
    Ogre::Light* light = mSceneMgr->createLight("tstLight");
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDirection(lightdir);
    light->setDiffuseColour(Ogre::ColourValue::White);
    light->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));
 
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.2));
 
    mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
    mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(mSceneMgr, Ogre::Terrain::ALIGN_X_Z, 257, 3000.0f);
    mTerrainGroup->setFilenameConvention(Ogre::String("TestTerrain1"), Ogre::String("dat"));
    mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);
 
    configureTerrainDefaults(light);
 
    for (long x = 0; x <= 0; ++x)
        for (long y = 0; y <= 0; ++y)
            defineTerrain(x, y);
 
    // sync load since we want everything in place when we start
    mTerrainGroup->loadAllTerrains(true);
	 
    if (mTerrainsImported)
    {
        Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
        while(ti.hasMoreElements())
        {
            Ogre::Terrain* t = ti.getNext()->instance;
            initBlendMaps(t);
        }
    }

    mTerrainGroup->freeTemporaryResources();

	populatePlants();

	Ogre::Entity* ninjaEntity = mSceneMgr->createEntity("Ninja", "ninja.mesh");
	Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("NinjaNode");
	node->attachObject(ninjaEntity);
	node->scale(0.06,0.06,0.06);
	Ogre::Vector3 temp_pos = node->getPosition();
	temp_pos.y = mTerrainGroup->getHeightAtWorldPosition(temp_pos);
	node->setPosition(temp_pos);

	mCamera->setPosition(Ogre::Vector3(0, 50, 50));
	mCamera->lookAt(node->getPosition());
    mCamera->setNearClipDistance(0.1);
    mCamera->setFarClipDistance(5000);

	Ogre::Vector3 temp = node->getPosition();
	int collision_index = checkForCollision(&temp);
	while (collision_index != -1) {
		removeWorldObject(collision_index);
		collision_index = checkForCollision(&temp);
	}
}
//-------------------------------------------------------------------------------------
void GameFramework::createFrameListener(void)
{
	BaseApplication::createFrameListener();
 
    mInfoLabel = mTrayMgr->createLabel(OgreBites::TL_TOP, "TInfo", "", 350);
}
//-------------------------------------------------------------------------------------
bool GameFramework::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    bool ret = BaseApplication::frameRenderingQueued(evt);

    if (mTerrainGroup->isDerivedDataUpdateInProgress())
    {
        mTrayMgr->moveWidgetToTray(mInfoLabel, OgreBites::TL_TOP, 0);
        mInfoLabel->show();
        if (mTerrainsImported)
        {
            mInfoLabel->setCaption("Building terrain, please wait...");
        }
        else
        {
            mInfoLabel->setCaption("Updating textures, patience...");
        }
    }
    else
    {
        mTrayMgr->removeWidgetFromTray(mInfoLabel);
        mInfoLabel->hide();
        if (mTerrainsImported)
        {
            mTerrainGroup->saveAllTerrains(true);
            mTerrainsImported = false;
        }
    }

    if(!processUnbufferedInput(evt)) return false;
 
    return ret;
}

int GameFramework::checkForCollision(Ogre::Vector3* to_check) {
	for (int i = 0; i < worldObjects.size(); ++i) {
		if (to_check->squaredDistance(worldObjects[i]->ourNode->getPosition()) < Ogre::Math::Sqr(worldObjects[i]->collisionRadius + 2)) {
			return i;
		}
	}
	return -1;
}

void GameFramework::removeWorldObject(int index) {
	WorldObject* temp = worldObjects[index];
	worldObjects[index] = worldObjects[worldObjects.size()-1];
	worldObjects.pop_back();
	temp->ourNode->detachAllObjects();
	delete(temp);
}

bool GameFramework::processUnbufferedInput(const Ogre::FrameEvent& evt)
{
	static bool mMouseDown = false;     // If a mouse button is depressed
	static bool affixCamera = true;
    static float key_cooldown = 0.0;    // Cooldown remaining for key presses
    static Ogre::Real mRotate = 0.13;   // The rotate constant
	Ogre::SceneNode* ninja_node = mSceneMgr->getSceneNode("NinjaNode");
	static Ogre::Real mMove = 400*ninja_node->getScale().x;      // The movement constant
	bool currMouse = mMouse->getMouseState().buttonDown(OIS::MB_Left);

	//if the mouse is pressed (currMouse) and wasn't pressed last frame, then they clicked
	if (currMouse && ! mMouseDown) {
		//kinda hacky way to check where our action is projecting, we move the player forward a tad, record the new position, and then move back.
		Ogre::Vector3 tempVector = Ogre::Vector3::ZERO;
		tempVector.z -= 2;
		ninja_node->translate(tempVector, Ogre::Node::TS_LOCAL);
		Ogre::Vector3 ninja_pos = ninja_node->getPosition();
		ninja_node->translate(-1 * tempVector, Ogre::Node::TS_LOCAL);

		//check for collisions with that new point, and if something was hit we apply an action to it
		int coll_index = checkForCollision(&ninja_pos);
		if (coll_index != -1) {
			cout << "Collided with " << coll_index << endl; //debug statement
			//apply the action. ACTION_CHOP is the only one so far, so use it
			if (worldObjects[coll_index]->receiveAction(ACTION_CHOP,5)) {
				//an object returns true when it wants to be destroyed
				removeWorldObject(coll_index);
			}
		} else {
			cout << "No collision.\n"; //debug statement
		}
	}
	//record the mouse state for the next frame's use
	mMouseDown = currMouse;

	//key_cooldown is a timer to prevent the system from registering a million keypresses during the short period of time a key is held down after a keypress.
	key_cooldown -= evt.timeSinceLastFrame;
	//if it has been more than a half second and the key "1" is pressed down...
	if ((key_cooldown < 0 ) && mKeyboard->isKeyDown(OIS::KC_1))
	{
		//reset the cooldown and toggle the variable that determined whether the camera is stuck to the ninja
		key_cooldown  = 0.5;
		affixCamera = !affixCamera;
	}

	Ogre::Vector3 transVector = Ogre::Vector3::ZERO;

	if (mKeyboard->isKeyDown(OIS::KC_I)) // Forward
	{
		transVector.z -= mMove;
	}
	if (mKeyboard->isKeyDown(OIS::KC_K)) // Backward
	{
		transVector.z += mMove;
	}

	if (mKeyboard->isKeyDown(OIS::KC_J)) // Left yaw
	{
			mSceneMgr->getSceneNode("NinjaNode")->yaw(Ogre::Degree(mRotate * 20));
	}
	if (mKeyboard->isKeyDown(OIS::KC_U)) // Left strafe
	{
		transVector.x -= mMove;
	}
	if (mKeyboard->isKeyDown(OIS::KC_L)) // Right yaw
	{
		mSceneMgr->getSceneNode("NinjaNode")->yaw(Ogre::Degree(-mRotate * 20));
	}
	if (mKeyboard->isKeyDown(OIS::KC_O)) // Right strafe
	{
		transVector.x += mMove;
	}
	if (mKeyboard->isKeyDown(OIS::KC_U)) // Up
	{
		transVector.y += mMove;
	}
	if (mKeyboard->isKeyDown(OIS::KC_O)) // Down
	{
		transVector.y -= mMove;
	}

	//move the ninja in whatever direction
	ninja_node->translate(transVector * evt.timeSinceLastFrame, Ogre::Node::TS_LOCAL);
	//set the new position's height to be the height of the terrain at that location (so it doesn't fly when it walks off a cliff)
	Ogre::Vector3 ninja_pos = ninja_node->getPosition();
	ninja_pos.y = mTerrainGroup->getHeightAtWorldPosition(ninja_pos);

	//if the new position isn't colliding with an object, make that our position and update the camera. Otherwise, move back to where we were.
	if (checkForCollision(&ninja_pos) == -1) {
		ninja_node->setPosition(ninja_pos);
		
		//if the sticky camera is toggled, move it with the ninja
		if (affixCamera) {
			//changes the distance according to the subject's scale, so it's not right up against a giant object and super far from a tiny one.
			mCamera->setPosition(ninja_pos - Ogre::Vector3(0, -(1200*ninja_node->getScale().x), 1200*ninja_node->getScale().x));
			mCamera->lookAt(ninja_pos);
		}
	} else {
		ninja_node->translate(-1 * transVector * evt.timeSinceLastFrame, Ogre::Node::TS_LOCAL);
	}

	//Check to see if each object is closer to the camera than the player's character, and if so change the material to something transparent
	for (int i = 0; i < worldObjects.size(); ++i) {
		Ogre::Entity* temp = static_cast<Ogre::Entity*>(worldObjects[i]->ourNode->getAttachedObject(0));
		//if distance from ninja to camera is greater than the distance from camera to object...
		if (ninja_node->getPosition().squaredDistance(mCamera->getPosition()) > mCamera->getPosition().squaredDistance(worldObjects[i]->ourNode->getPosition())) {
			//if it's not already transparent, make it so
			if (!worldObjects[i]->isClear) {
				temp->setMaterial(Ogre::MaterialManager::getSingleton().getByName("Materials/ClearLeaves"));
				worldObjects[i]->isClear = true;
			}
		} else {
			if (worldObjects[i]->isClear) {
				temp->setMaterial(Ogre::MaterialManager::getSingleton().getByName("Material"));
				worldObjects[i]->isClear = false;
			}
		}
	}
    return true;
}
//-------------------------------------------------------------------------------------



#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        GameFramework app;

        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif
