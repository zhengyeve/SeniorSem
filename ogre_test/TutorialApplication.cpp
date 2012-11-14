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
#include "TutorialApplication.h"
#include <OgreMath.h>
 
BasicTutorial4::BasicTutorial4(void)
{
}
 
//-------------------------------------------------------------------------------------
BasicTutorial4::~BasicTutorial4(void)
{
}

void BasicTutorial4::destroyScene(void)
{
	OGRE_DELETE mTerrainGroup;
    OGRE_DELETE mTerrainGlobals;
}
//-------------------------------------------------------------------------------------
void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img)
{
	img.load("terrain.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    if (flipX)
        img.flipAroundY();
    if (flipY)
        img.flipAroundX();
}
//-------------------------------------------------------------------------------------
void BasicTutorial4::defineTerrain(long x, long y)
{
	Ogre::String filename = mTerrainGroup->generateFilename(x, y);
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
//-------------------------------------------------------------------------------------
void BasicTutorial4::initBlendMaps(Ogre::Terrain* terrain)
{
	Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
    Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
    Ogre::Real minHeight0 = 40;
    Ogre::Real fadeDist0 = 40;
    Ogre::Real minHeight1 = 40;
    Ogre::Real fadeDist1 = 15;
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
void BasicTutorial4::configureTerrainDefaults(Ogre::Light* light)
{
	// Configure global
    mTerrainGlobals->setMaxPixelError(8);
    // testing composite map
    mTerrainGlobals->setCompositeMapDistance(10000);
 
    // Important to set these so that the terrain knows what to use for derived (non-realtime) data
    mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
    mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
    mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());
 
    // Configure default import settings for if we use imported image
    Ogre::Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
    defaultimp.terrainSize = 513;
    defaultimp.worldSize = 3000.0f;
    defaultimp.inputScale = 150;
    defaultimp.minBatchSize = 33;
    defaultimp.maxBatchSize = 65;
    // textures
    defaultimp.layerList.resize(3);
    defaultimp.layerList[0].worldSize = 100;
    defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
    defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
    defaultimp.layerList[1].worldSize = 30;
    defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
    defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
    defaultimp.layerList[2].worldSize = 50;
    defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
    defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
}
//-------------------------------------------------------------------------------------
void BasicTutorial4::createScene(void)
{ 
    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
    Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(7);
 
    Ogre::Vector3 lightdir(0.55, -0.3, 0.75);
    lightdir.normalise();
 
    Ogre::Light* light = mSceneMgr->createLight("tstLight");
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDirection(lightdir);
    light->setDiffuseColour(Ogre::ColourValue::White);
    light->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));
 
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.2));
 
    mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
    mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(mSceneMgr, Ogre::Terrain::ALIGN_X_Z, 513, 3000.0f);
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

	Ogre::Vector3 new_pos;
	int map_size = mTerrainGroup->getTerrainWorldSize();
	//map_size = sqrt((map_size * map_size) / 2.0);

	for (int i = 0; i < 1000; ++i) {
		Ogre::Entity* tree_entity = mSceneMgr->createEntity("tree.mesh");
		new_pos.x = rand() % map_size - (map_size / 2.0);
		new_pos.z = rand() % map_size - (map_size / 2.0);
		//new_pos.y = 10000; //crude hack to make sure it'll be above the terrain
		new_pos.y = mTerrainGroup->getHeightAtWorldPosition(new_pos);
		/*if (new_pos.y < 0.001) {
			new_pos.y = 1000;
		}*/
		
		Ogre::SceneNode *temp_node = mSceneMgr->getRootSceneNode()->createChildSceneNode(new_pos);

		temp_node->attachObject(tree_entity);
		temp_node->scale(4,4,4);
		worldObjects.push_back(WorldObject(temp_node,5));
	}

	Ogre::Entity* ninjaEntity = mSceneMgr->createEntity("Ninja", "ninja.mesh");
	Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("NinjaNode");
	node->attachObject(ninjaEntity);
	node->scale(0.1,0.1,0.1);

	mCamera->setPosition(Ogre::Vector3(0, 50, 50));
	mCamera->lookAt(node->getPosition());
    mCamera->setNearClipDistance(0.1);
    mCamera->setFarClipDistance(50000);

	Ogre::Vector3 temp = node->getPosition();
	int collision_index = checkForCollision(&temp);
	while (collision_index != -1) {
		worldObjects[collision_index] = worldObjects[worldObjects.size()-1];
		worldObjects.pop_back();
		collision_index = checkForCollision(&temp);
	}

	//ninja_node->setMaterial(Ogre::MaterialManager.getSingleton().getByName("Glass.material"));
	//Ogre::MaterialManager::getSingleton().getResour
	//Ogre::ResourceGroupManager::getSingleton().openResource("materials/scripts/Glass.material");
	//Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	/*mm = ogre.MaterialManager.getSingleton()
    rm = ogre.ResourceGroupManager.getSingleton()
    tm = ogre.TextureManager.getSingleton()
    meshm=ogre.MeshManager.getSingleton()

    tm.setDefaultNumMipmaps (5)
    mm.setDefaultTextureFiltering(ogre.TFO_ANISOTROPIC)
    mm.setDefaultAnisotropy(2)
    rm.initialiseAllResourceGroups()


    mesh=meshm.load('data/Mesh/robot.mesh',ogre.ResourceGroupManager.DEFAULT_RESOURCE_GROUP_NAME)
    mesh.setSkeletonName("data/Skeleton/robot.skeleton")
    ds = rm.openResource("data/Material/robot.material", "General", True)
    mm.parseScript(ds, "General")
    material=mm.getByName("Robot")
      
    texture=tm.load("data/Map/r2skin.jpg", "General")

    mpass=material.getTechnique(1).getPass(0)
    mpass.getTextureUnitState(texture.getName())
    entity = sceneManager.createEntity('robot', mesh.getName())
    entity.setMaterialName(material.getName())
    sceneManager.getRootSceneNode().attachObject(entity)
    state=entity.getAnimationState('Walk')*/
}
//-------------------------------------------------------------------------------------
void BasicTutorial4::createFrameListener(void)
{
	BaseApplication::createFrameListener();
 
    mInfoLabel = mTrayMgr->createLabel(OgreBites::TL_TOP, "TInfo", "", 350);
}
//-------------------------------------------------------------------------------------
bool BasicTutorial4::frameRenderingQueued(const Ogre::FrameEvent& evt)
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

int BasicTutorial4::checkForCollision(Ogre::Vector3* to_check) {
	for (int i = 0; i < worldObjects.size(); ++i) {
		if (to_check->squaredDistance(worldObjects[i].ourNode->getPosition()) < Ogre::Math::Sqr(worldObjects[i].collisionRadius + 5)) {
			return i;
		}
	}
	return -1;
}

//-------------------------------------------------------------------------------------
bool BasicTutorial4::processUnbufferedInput(const Ogre::FrameEvent& evt)
{
	static bool mMouseDown = false;     // If a mouse button is depressed
	static bool affixCamera = true;
    static Ogre::Real mToggle = 0.0;    // The time left until next toggle
    static Ogre::Real mRotate = 0.13;   // The rotate constant
	Ogre::SceneNode* ninja_node = mSceneMgr->getSceneNode("NinjaNode");
	static Ogre::Real mMove = 400*ninja_node->getScale().x;      // The movement constant
	bool currMouse = mMouse->getMouseState().buttonDown(OIS::MB_Left);

	/*if (currMouse && ! mMouseDown) {
		Ogre::Light* light = mSceneMgr->getLight("pointLight");
		light->setVisible(! light->isVisible());
	}

	mMouseDown = currMouse;*/

	mToggle -= evt.timeSinceLastFrame;
	if ((mToggle < 0.0f ) && mKeyboard->isKeyDown(OIS::KC_1))
	{
		mToggle  = 0.5;
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

	
	ninja_node->translate(transVector * evt.timeSinceLastFrame, Ogre::Node::TS_LOCAL);

	Ogre::Vector3 ninja_pos = ninja_node->getPosition();
	ninja_pos.y = mTerrainGroup->getHeightAtWorldPosition(ninja_pos);
	if (checkForCollision(&ninja_pos) == -1) {
		ninja_node->setPosition(ninja_pos);

		ninja_pos = ninja_node->getPosition();

		if (affixCamera) {
			mCamera->setPosition(ninja_pos - Ogre::Vector3(0, -(1200*ninja_node->getScale().x), 1200*ninja_node->getScale().x));
			mCamera->lookAt(ninja_pos);
		}
	} else {
		ninja_node->translate(-1 * transVector * evt.timeSinceLastFrame, Ogre::Node::TS_LOCAL);
	}
	//Ogre::ResourceGroupManager::
	for (int i = 0; i < worldObjects.size(); ++i) {
		Ogre::Entity* temp = static_cast<Ogre::Entity*>(worldObjects[i].ourNode->getAttachedObject(0));
		if (ninja_node->getPosition().squaredDistance(mCamera->getPosition()) > mCamera->getPosition().squaredDistance(worldObjects[i].ourNode->getPosition())) {
			/*if (!Ogre::MaterialManager::getSingleton().resourceExists("Materials/ClearLeaves")) {
				temp->setVisible(false);
			} else {*/
			if (!worldObjects[i].isClear) {
				temp->setMaterial(Ogre::MaterialManager::getSingleton().getByName("Materials/ClearLeaves"));
				worldObjects[i].isClear = true;
			}
			//Ogre::MaterialManager::getSingleton
			
			//Ogre::MaterialManager mm& mm = Ogre::MaterialManager::getSingleton();
			
			//temp->setVisible(false);
		} else {
			/*if (!Ogre::MaterialManager::getSingleton().resourceExists("Material")) {
				temp->setVisible(true);
			} else {*/
			if (worldObjects[i].isClear) {
				temp->setMaterial(Ogre::MaterialManager::getSingleton().getByName("Material"));
				worldObjects[i].isClear = false;
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
        BasicTutorial4 app;

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
