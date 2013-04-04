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
#include "PlantObject.h"
#include "CreatureObject.h"
#include "PlantManager.h"
//#include "ToolTip.h"

//#include "glew/glew.h"

//#include "MyGUI.h"
//#include "MyGUI_OgrePlatform.h"

//using namespace demo;


// variable of total score a player is now having
static unsigned long totalScore = 0;
static float INITIAL_HUNGER = 10;

using namespace PolyVox;
using namespace std;

GameFramework::GameFramework(void)
{
}
 
//-------------------------------------------------------------------------------------
GameFramework::~GameFramework(void)
{
	for (int i = 0; i < worldObjects.size(); ++i) {
		delete(worldObjects[i]);
	}
	delete(playerObject);
	delete(mapManager);
}

void GameFramework::destroyScene(void)
{
	delete mapManager;
}
//-------------------------------------------------------------------------------------

void GameFramework::updateVisibleObjects(PolyVox::Region region) {
	for (unsigned int i = 0; i < worldObjects.size(); ++i) {
		Ogre::Vector3 obj_pos = worldObjects[i]->ourNode->getPosition();
		if (region.containsPoint(PolyVox::Vector3DInt32(obj_pos.x, obj_pos.y, obj_pos.z))) {
			worldObjects[i]->ourNode->setVisible(true);
		} else {
			worldObjects[i]->ourNode->setVisible(false);
		}
	}
}

void GameFramework::populatePlants(PolyVox::Region region) {
	cout << "Populating plants...\n";
	Ogre::Vector3 new_pos;
	PlantManager plantManager;
	int min_plant_spacing = 2;
	int min_x = region.getLowerCorner().getX();
	int max_x = region.getUpperCorner().getX();
	int min_z = region.getLowerCorner().getZ();
	int max_z = region.getUpperCorner().getZ();
	int max_y = region.getUpperCorner().getY();

	//goes from -(1/2 map size) to +(1/2 map size), incrementing by the minimum plant spacing each time, which prevents putting like 80 trees within a foot of one another
	for (int x = min_x; x < max_x; x+=min_plant_spacing) {
		for (int z = min_z; z < max_z; z+=min_plant_spacing) {
			int temp_x = x + rand()%2;
			int temp_z = z + rand()%2;
			double cur_height = mapManager->getHeightAt(temp_x,max_y,temp_z);
			/*double height_diff = abs(cur_height - mapManager->getHeightAt(temp_x+min_plant_spacing,max_y,temp_z+min_plant_spacing))+1;
			height_diff += abs(cur_height - mapManager->getHeightAt(temp_x-min_plant_spacing,max_y,temp_z+min_plant_spacing));
			height_diff += abs(cur_height - mapManager->getHeightAt(temp_x-min_plant_spacing,max_y,temp_z-min_plant_spacing));
			height_diff += abs(cur_height - mapManager->getHeightAt(temp_x+min_plant_spacing,max_y,temp_z-min_plant_spacing));
			height_diff /= min_plant_spacing;*/
			double height_diff = abs(cur_height - mapManager->getHeightAt(temp_x+1,max_y,temp_z+1))+1;
			height_diff += abs(cur_height - mapManager->getHeightAt(temp_x-1,max_y,temp_z+1));
			height_diff += abs(cur_height - mapManager->getHeightAt(temp_x-1,max_y,temp_z-1));
			height_diff += abs(cur_height - mapManager->getHeightAt(temp_x+1,max_y,temp_z-1));

			//get what kind of plant we are making here from the plant manager
			PlantType to_place = plantManager.getFlora(cur_height,1.0/height_diff);
			//if the plant manager actually gave us a plant, create it
			if (to_place != PLANT_NONE) {
				//initialize this before the switch statement, since you can't do that inside...
				Ogre::Entity* tree_entity;
				Ogre::ResourcePtr clear_mat;
				//change the model based on what plant we're placing
				switch(to_place) {
					case PLANT_OAK:
						tree_entity = mSceneMgr->createEntity("oaktree.mesh");
						clear_mat = Ogre::MaterialManager::getSingleton().getByName("Materials/Trees/ClearOak");
						break;
					case PLANT_PINE:
						tree_entity = mSceneMgr->createEntity("pinetree.mesh");
						clear_mat = Ogre::MaterialManager::getSingleton().getByName("Materials/Trees/ClearPine");
						break;
					case PLANT_ROUND_SHROOM:
						tree_entity = mSceneMgr->createEntity("roundshroom.mesh");
						clear_mat = Ogre::MaterialManager::getSingleton().getByName("Materials/ClearGray");
						break;
				};
				new_pos.x = temp_x;
				new_pos.z = temp_z;
				new_pos.y = mapManager->getHeightAt(new_pos.x,max_y,new_pos.z,1)+0.45;
				Ogre::SceneNode *temp_node = mSceneMgr->getRootSceneNode()->createChildSceneNode(new_pos);
				temp_node->attachObject(tree_entity);
				//scale the model randomly
				temp_node->scale(2+(rand()%11)/10.0,2+(rand()%11)/10.0,2+(rand()%11)/10.0);
				temp_node->scale(0.1,0.1,0.1);
				//give the tree a random rotation about the y-axis, so the trees aren't all aligned on a grid
				Ogre::Radian rot_angle((Ogre::Real)(rand()%7));
				temp_node->rotate(Ogre::Vector3(0,1,0),rot_angle);
				PlantObject* new_obj = new PlantObject(temp_node,0.3);
				new_obj->subtype = (int)to_place;
				if (to_place == PLANT_ROUND_SHROOM) {
					new_obj->setEatable(true, 10);
				}
				worldObjects.push_back(new_obj);
			}
		}
	}
	worldObjects.resize(worldObjects.size());
	cout << "Populated " << worldObjects.size() << " plants.\n";
}


void BaseApplication::mousePressedExit(MyGUI::Widget* _widget)	// EXIT button
{
	mShutDown = true;
}




void GameFramework::createScene(void)
{ 
	//create a console so we can print debug statements
	AllocConsole();
	freopen("CONOUT$","wb",stdout);
	srand((int)std::time(NULL));
	totalScore = 0;

	


	//make sure the exit timer hasn't started
	exitTimer = -1;

	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_TRILINEAR);
    //Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(7);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../../myMedia","FileSystem");
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8, 2000, false);
 
    Ogre::Vector3 lightdir(0.55, -0.3, 0.75);
    lightdir.normalise();
 
    Ogre::Light* light = mSceneMgr->createLight("tstLight");
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDirection(lightdir);
    light->setDiffuseColour(Ogre::ColourValue::White);
    light->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));
 
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.2));


 ////
 ////   mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
 ////   mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(mSceneMgr, Ogre::Terrain::ALIGN_X_Z, 257, 3000.0f);
 ////   mTerrainGroup->setFilenameConvention(Ogre::String("TestTerrain1"), Ogre::String("dat"));
 ////   mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);
 ////
 ////   configureTerrainDefaults(light);
 ////
 ////   for (long x = 0; x <= 0; ++x)
 ////       for (long y = 0; y <= 0; ++y)
 ////           defineTerrain(x, y);


 ////
 ////   // sync load since we want everything in place when we start
 ////   mTerrainGroup->loadAllTerrains(true);
	//// 
 ////   if (mTerrainsImported)
 ////   {
 ////       Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
 ////       while(ti.hasMoreElements())
 ////       {
 ////           Ogre::Terrain* t = ti.getNext()->instance;
 ////           initBlendMaps(t);
 ////       }
 ////   }

	//////mSceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));


	mapManager = new MapManager();
	mapManager->draw(0,0,0,mSceneMgr,true);

	populatePlants(PolyVox::Region(-200,-200,-200,200,200,200));


	// starting of gui configuration

//	initializeGUI();
	//mGUI->load("Basic.layout");
	//mGUI->findWidget<MyGUI::Window>("Result")->setVisible(false);
	//mGUI->findWidget<MyGUI::Widget>("HelpPanel")->setVisible(true);





	// starting of gui configuration

//	initializeGUI();
	//mGUI->load("Basic.layout");
	//mGUI->findWidget<MyGUI::Window>("Result")->setVisible(false);
	//mGUI->findWidget<MyGUI::Widget>("HelpPanel")->setVisible(true);




	Ogre::Entity* ninjaEntity = mSceneMgr->createEntity("Ninja", "ninja.mesh");
	Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNode");
	node->attachObject(ninjaEntity);
	node->scale(0.006,0.006,0.006);
	Ogre::Vector3 temp_pos = node->getPosition();
	temp_pos.y = mapManager->getHeightAt(temp_pos.x, mapManager->getMaxHeight()+1, temp_pos.z)+2;
	cout << "Setting player's y to: " << temp_pos.y << " because we checked (" << temp_pos.x << ", " << mapManager->getMaxHeight() << ", " << temp_pos.z << ")\n";
	node->setPosition(temp_pos);
	Ogre::SceneNode* selector_node = node->createChildSceneNode("SelectorNode");
	Ogre::Vector3 temp_vector = Ogre::Vector3::ZERO;
	temp_vector.z -= 250;
	temp_vector.y += 80;
	selector_node->translate(temp_vector, Ogre::Node::TS_LOCAL);
	selector_node->attachObject(mSceneMgr->createEntity("Selector", "RoundShroom.mesh"));
	selector_node->scale(10, 10, 10);

	playerObject = new CreatureObject(node, 0.3);
	playerObject->speed = 400*node->getScale().x;

	playerObject->hunger = INITIAL_HUNGER;


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

	//mGUI->findWidget<MyGUI::Widget>("LeftClickAction")->setProperty("ImageTexture","shovel.png");
}
//-------------------------------------------------------------------------------------
void GameFramework::createFrameListener(void)
{
	BaseApplication::createFrameListener();
 
 //   scoreLabel = mTrayMgr->createLabel(OgreBites::TL_TOP, "ScoreBox", "", 200);
	//hungerLabel = mTrayMgr->createLabel(OgreBites::TL_BOTTOMRIGHT,"HungerBox","",200);
	//mTrayMgr->createLabel(OgreBites::TL_TOP,"DeathBox","YOU ARE DEAD!",300);
	//mTrayMgr->getWidget("DeathBox")->hide();
	//mTrayMgr->removeWidgetFromTray("DeathBox");
}

//-------------------------------------------------------------------------------------
bool GameFramework::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if (mWindow->isClosed()) return false;
    if (mShutDown) return false;
    mKeyboard->capture();
    mMouse->capture();
    mTrayMgr->frameRenderingQueued(evt);






	if (exitTimer > 0) {
		exitTimer -= evt.timeSinceLastFrame;
		if (exitTimer < 0.001) {
//			mShutDown = true;
//			delete mCameraMan;
//			mCameraMan = 0;

			mGUI->findWidget<MyGUI::Window>("Result_Window")->setVisible(true);
			MyGUI::ButtonPtr exitButton = mGUI->findWidget<MyGUI::Button>("Exit");
			exitButton->eventMouseButtonClick = MyGUI::newDelegate(static_cast<BaseApplication*>(this), &BaseApplication::mousePressedExit);

		}
	}

	//mTrayMgr->moveWidgetToTray(scoreLabel, OgreBites::TL_TOP, 0);
 //   scoreLabel->show();
	//mTrayMgr->moveWidgetToTray(hungerLabel, OgreBites::TL_BOTTOMRIGHT, 0);
 //   hungerLabel->show();

	//uncomment this once we want folks to actually starve to death
	//playerObject->hunger -= evt.timeSinceLastFrame;
	if (playerObject->hunger < 0) {
		if (exitTimer < 0) {
			exitTimer = 5;

			string resultText("Starved to death! :c\nYou total Score is: "), resultScore;
			
			stringstream tmp;
			tmp << totalScore;
			tmp >> resultScore;

			resultText += resultScore;
//			resultText += "\nApplication will end in 5 seconds";

			//mTrayMgr->removeWidgetFromTray(hungerLabel);
			//hungerLabel->hide();
			//mTrayMgr->removeWidgetFromTray(scoreLabel);
			//scoreLabel->hide();

			mGUI->findWidget<MyGUI::EditBox>("Result_Text")->castType<MyGUI::TextBox>()->setCaption(resultText);
			mGUI->findWidget<MyGUI::Window>("Result_Window")->setVisible(true);
/*
			MyGUI::ButtonPtr exitButton = mGUI->findWidget<MyGUI::Button>("Exit");
			exitButton->eventMouseButtonClick = MyGUI::newDelegate(static_cast<BaseApplication*>(this), &BaseApplication::mousePressedExit);
*/
			//hungerLabel->setCaption("Starved to death! :c");
			//mTrayMgr->moveWidgetToTray("DeathBox", OgreBites::TL_CENTER, 0);
			

		}
	} else {
		char hungerStr[10];
		itoa((int) floor(playerObject->hunger + 0.5),hungerStr,10);
		//char caption[20] = "Hunger: ";
		//strcat(caption,hungerStr);
		mGUI->findWidget<MyGUI::EditBox>("Current_Hunger")->castType<MyGUI::TextBox>()->setCaption(hungerStr);
	}

    bool ret = BaseApplication::frameRenderingQueued(evt);

    //if (!mTerrainGroup->isDerivedDataUpdateInProgress())
    //{
//        mTrayMgr->removeWidgetFromTray(mInfoLabel);
//        mInfoLabel->hide();

	// score display handling
		string str;
		stringstream strstm;
		strstm << totalScore;
		str = strstm.str();
//        scoreLabel->setCaption(str);
		mGUI->findWidget<MyGUI::EditBox>("Current_Score")->castType<MyGUI::TextBox>()->setCaption(str);

    if(!processUnbufferedInput(evt)) return false;
 
    return ret;
}

int GameFramework::checkForCollision(Ogre::Vector3* to_check) {
	for (int i = 0; i < worldObjects.size(); ++i) {
		if (to_check->squaredDistance(worldObjects[i]->ourNode->getPosition()) < Ogre::Math::Sqr(worldObjects[i]->collisionRadius + playerObject->collisionRadius)) {
			return i;
		}
	}
	return -1;
}

void GameFramework::removeWorldObject(int index, bool do_delete) {
	WorldObject* temp = worldObjects[index];
	worldObjects[index] = worldObjects[worldObjects.size()-1];
	worldObjects.pop_back();
	if (do_delete) {
		temp->ourNode->detachAllObjects();
		delete(temp);
	} else {
		temp->ourNode->setVisible(false);
	}
}

void GameFramework::handleAction(Action action, WorldObject* target, WorldObject* actor) {
	cout << "Performing action " << action.getName() << ".\n";
	Ogre::Vector3 selector_pos = (playerObject->ourNode->getOrientation()*(playerObject->ourNode->getChild("SelectorNode")->getPosition()*playerObject->ourNode->getScale()))+playerObject->ourNode->getPosition();
	if (action.actionType == ACTION_CHOP) {
		//WorldObject* actual_target;
		if (target->isNone()) {
			//check for collisions with that new point, and if something was hit we apply an action to it
			int coll_index = checkForCollision(&selector_pos);
			if (coll_index != -1) {
				target = worldObjects[coll_index];
			}
		}
		if (!target->isNone()) {
			//apply the action
			vector<Action> result_actions = target->receiveAction(action);
			for (unsigned int i = 0; i < result_actions.size(); ++i) {
				handleAction(result_actions[i],actor,target);
			}
		} else {
			cout << "Nothing within action range.\n"; //debug statement
		}
	} else if (action.actionType == ACTION_MODIFY_VOXELS) {
		double offset = 0;// 0.5;
		//setVoxelAt() returns true if a change was made (i.e. setting a previously empty space to solid), so if it returns true we want to ask the map to redraw itself.
		if (mapManager->setMaterialAt(selector_pos.x+offset, selector_pos.y+0.2+offset, selector_pos.z+offset, action.actionVar)) {
			//tell the map manager to redraw at the current position
			Region render_area = mapManager->draw(playerObject->ourNode->getPosition().x, playerObject->ourNode->getPosition().y, playerObject->ourNode->getPosition().z, mSceneMgr);
			updateVisibleObjects(render_area);
		}
	} else if (action.actionType == ACTION_DROP_SELF) {
		if (target->objectType == OBJECT_CREATURE) {
			CreatureObject* receiver_creature = (CreatureObject*) target;
			if (receiver_creature->obtainObject(actor)) {
				bool foundit = false;
				for (unsigned int i = 0; i < worldObjects.size(); ++i) {
					if (worldObjects[i] == actor) {
						removeWorldObject(i, false);
						foundit = true;
						break;
					}
				}
				if (!foundit) {
					cout << "Remove self from drop failed!\n";
				}
			}
		}		
	} else if (action.actionType == ACTION_REMOVE_SELF) {
		bool foundit = false;
		for (unsigned int i = 0; i < worldObjects.size(); ++i) {
			if (worldObjects[i] == actor) {
				removeWorldObject(i);
				foundit = true;
				break;
			}
		}
		if (!foundit) {
			cout << "Remove self failed!\n";
		}
	} else if (action.actionType == ACTION_FEED) {
		CreatureObject* target_creature = (CreatureObject*) target;
		target_creature->hunger += action.actionVar;
	} else {
		cout << "Unhandled action attempted!\n";
	}
}

bool GameFramework::processUnbufferedInput(const Ogre::FrameEvent& evt)
{
	static bool last_left_mouse = false;     // If the left mouse button is depressed
	static bool last_right_mouse = false;     // If the right mouse button is depressed
	static bool affixCamera = true;
    static Ogre::Real mRotate = 0.13;   // The rotate constant
	static Ogre::Real mMove = playerObject->speed;      // The movement constant
	static bool has_jumped = false;
	static Action primary_action(ACTION_CHOP, 5);
	static Action secondary_action(ACTION_MODIFY_VOXELS, 254);
	
	bool cur_left_mouse = mMouse->getMouseState().buttonDown(OIS::MB_Left);
	bool cur_right_mouse = mMouse->getMouseState().buttonDown(OIS::MB_Right);

	//if the mouse is pressed (currMouse) and wasn't pressed last frame, then they clicked
	if (cur_left_mouse) {
		WorldObject temp;
		handleAction(primary_action, &temp, playerObject);
	}
	//record the mouse state for the next frame's use
	last_left_mouse = cur_left_mouse;

	if (cur_right_mouse) {
		WorldObject temp;
		handleAction(secondary_action, &temp, playerObject);
	}
	last_right_mouse = cur_right_mouse;

	//use the num keys to switch between actions
	if (mKeyboard->isKeyDown(OIS::KC_1)) {
		if ((mKeyboard->isKeyDown(OIS::KC_LSHIFT)) || (mKeyboard->isKeyDown(OIS::KC_RSHIFT))) {
			secondary_action.actionType = ACTION_CHOP;
			primary_action.actionVar = 5;
			mGUI->findWidget<MyGUI::Widget>("RightClickAction")->setProperty("ImageTexture","axe.png");
		} else {
			primary_action.actionType = ACTION_CHOP;
			primary_action.actionVar = 5;
			mGUI->findWidget<MyGUI::Widget>("LeftClickAction")->setProperty("ImageTexture","axe.png");
		}
	} else if (mKeyboard->isKeyDown(OIS::KC_2)) {
		if ((mKeyboard->isKeyDown(OIS::KC_LSHIFT)) || (mKeyboard->isKeyDown(OIS::KC_RSHIFT))) {
			secondary_action.actionType = ACTION_MODIFY_VOXELS;
			secondary_action.actionVar = 0;
			mGUI->findWidget<MyGUI::Widget>("RightClickAction")->setProperty("ImageTexture","shovel.png");
		} else {
			primary_action.actionType = ACTION_MODIFY_VOXELS;
			primary_action.actionVar = 0;
			mGUI->findWidget<MyGUI::Widget>("LeftClickAction")->setProperty("ImageTexture","shovel.png");
		}
	} else if (mKeyboard->isKeyDown(OIS::KC_3)) {
		if ((mKeyboard->isKeyDown(OIS::KC_LSHIFT)) || (mKeyboard->isKeyDown(OIS::KC_RSHIFT))) {
			secondary_action.actionType = ACTION_MODIFY_VOXELS;
			secondary_action.actionVar = 254;
			mGUI->findWidget<MyGUI::Widget>("RightClickAction")->setProperty("ImageTexture","brick.png");
		} else {
			primary_action.actionType = ACTION_MODIFY_VOXELS;
			primary_action.actionVar = 254;
			mGUI->findWidget<MyGUI::Widget>("LeftClickAction")->setProperty("ImageTexture","brick.png");
		}
	}

	//use key C and c to toggle fixed camera on and off
	if (mKeyboard->isKeyDown(OIS::KC_C) && ((mKeyboard->isKeyDown(OIS::KC_LSHIFT) || (mKeyboard->isKeyDown(OIS::KC_RSHIFT))))) {
		affixCamera = true;
	} else if (mKeyboard->isKeyDown(OIS::KC_C)) {
		affixCamera = false;
	}

	//use x and z to lower and raise the selector
	if (mKeyboard->isKeyDown(OIS::KC_Z)) {
		Ogre::Vector3 temp_vector = Ogre::Vector3::ZERO;
		temp_vector.y += 2;
		playerObject->ourNode->getChild("SelectorNode")->translate(temp_vector, Ogre::Node::TS_LOCAL);
	} else if(mKeyboard->isKeyDown(OIS::KC_X))
	{
		Ogre::Vector3 temp_vector = Ogre::Vector3::ZERO;
		temp_vector.y -= 2;
		playerObject->ourNode->getChild("SelectorNode")->translate(temp_vector, Ogre::Node::TS_LOCAL);
	}
	float acceleration = 0.17;
	float max_speed = 5;
	float rest_threshold = 0.001;
	if (mKeyboard->isKeyDown(OIS::KC_I)) // Forward key pressed
	{
		if (-playerObject->momentum.z < max_speed) {
			playerObject->momentum.z -= acceleration;
		}
	} else if (playerObject->momentum.z < -rest_threshold) {
		playerObject->momentum.z += acceleration;
		if (playerObject->momentum.z > rest_threshold) {
			playerObject->momentum.z = 0;
		}
	}
	if (mKeyboard->isKeyDown(OIS::KC_K)) // Backward key pressed
	{
		if (playerObject->momentum.z < max_speed) {
			playerObject->momentum.z += acceleration;
		}
	} else if (playerObject->momentum.z > rest_threshold) {
		playerObject->momentum.z -= acceleration;
		if (playerObject->momentum.z < -rest_threshold) {
			playerObject->momentum.z = 0;
		}
	}
	if (mKeyboard->isKeyDown(OIS::KC_J)) // Left yaw
	{
		playerObject->ourNode->yaw(Ogre::Degree(mRotate * 10));
	}
	if (mKeyboard->isKeyDown(OIS::KC_U)) // Left strafe
	{
		if (-playerObject->momentum.x < max_speed) {
			playerObject->momentum.x -= acceleration;
		}
	} else if (-playerObject->momentum.x > rest_threshold) {
		playerObject->momentum.x += acceleration;
		if (playerObject->momentum.x > rest_threshold) {
			playerObject->momentum.x = 0;
		}
	}
	if (mKeyboard->isKeyDown(OIS::KC_L)) // Right yaw
	{
		playerObject->ourNode->yaw(Ogre::Degree(-mRotate * 10));
	}
	if (mKeyboard->isKeyDown(OIS::KC_O)) // Right strafe
	{
		if (playerObject->momentum.x < max_speed) {
			playerObject->momentum.x += acceleration;
		}
	} else if (playerObject->momentum.x > rest_threshold) {
		playerObject->momentum.x -= acceleration;
		if (playerObject->momentum.x < -rest_threshold) {
			playerObject->momentum.x = 0;
		}
	}
	float jump_speed = 7.5;
	if (mKeyboard->isKeyDown(OIS::KC_SPACE)) // Space bar (jump)
	{
		if ((playerObject->momentum.y < jump_speed) && (playerObject->momentum.y > -rest_threshold) && !has_jumped) {
			playerObject->momentum.y += 5*acceleration;
		} else if (playerObject->momentum.y >= jump_speed) {
			has_jumped = true;
		}
	}


	if (mKeyboard->isKeyDown(OIS::KC_0)) {
		cout << "Player pos: " << playerObject->ourNode->getPosition() << " player momentum: " << playerObject->momentum << " player inventory: ";
		playerObject->listInventory();
		cout << endl;
	}

	if ((abs(playerObject->momentum.x) > rest_threshold) || (abs(playerObject->momentum.y) > rest_threshold) || (abs(playerObject->momentum.z) > rest_threshold)) {

		//move the ninja in whatever direction
		playerObject->ourNode->translate(playerObject->momentum * evt.timeSinceLastFrame, Ogre::Node::TS_LOCAL);
		//set the new position's height to be the height of the terrain at that location (so it doesn't fly when it walks off a cliff)
		Ogre::Vector3 player_pos = playerObject->ourNode->getPosition();
		float player_height = playerObject->ourNode->getScale().y * 80;
		//player_pos.y = mapManager->getHeightAt(player_pos.x,player_pos.y+1,player_pos.z)+player_height;

		//if the new position isn't colliding with an object, make that our position and update the camera. Otherwise, move back to where we were.
		if (checkForCollision(&player_pos) == -1) {
			double height_diff = mapManager->getAveragedHeightAt(player_pos.x,player_pos.y+1,player_pos.z) - player_pos.y + player_height;
			if (height_diff > 0.2) {
				height_diff = max(height_diff, (mapManager->getAveragedHeightAt(player_pos.x,player_pos.y+2,player_pos.z) - player_pos.y + player_height));
			}
			//cout << "Height diff: " << height_diff << endl;
			float hop_threshold = 0.1;
			//if it's a small height difference, make the player hop up
			if ((height_diff >= hop_threshold) && (height_diff < 1)) {
				playerObject->momentum.y+=acceleration;
				//cout << "Fly, you fool!\n";
			} //if the height difference is higher, stop the player
			else if (height_diff > 1) {
				playerObject->ourNode->translate(-playerObject->momentum * evt.timeSinceLastFrame, Ogre::Node::TS_LOCAL);
				playerObject->momentum.x = 0;
				playerObject->momentum.y = 0;
				playerObject->momentum.z = 0;
			} else if ((height_diff < hop_threshold) && (height_diff > -hop_threshold)) {
				if ((playerObject->momentum.y > 0) && !(mKeyboard->isKeyDown(OIS::KC_SPACE))) {
					playerObject->momentum.y = 0;
					has_jumped = false;
				} else if (playerObject->momentum.y < 0) {
					playerObject->momentum.y = 0;
					has_jumped = false;
				}
				Ogre::Vector3 temp_vec(0,height_diff/100,0);
				playerObject->ourNode->translate(temp_vec);
				//cout << "Levelling\n";
			} else {
				if ((-playerObject->momentum.y < max_speed*10) && ((!mKeyboard->isKeyDown(OIS::KC_SPACE) || has_jumped))) {
					playerObject->momentum.y -= acceleration;
				}
			}
			//player_pos = playerObject->ourNode->getPosition();
			//playerObject->ourNode->setPosition(player_pos);
			//if the sticky camera is toggled, move it with the ninja
			if (affixCamera) {
				//changes the distance according to the subject's scale, so it's not right up against a giant object and super far from a tiny one.
				//mCamera->setPosition(player_pos - Ogre::Vector3(0, -(1200*playerObject->ourNode->getScale().x), 1200*playerObject->ourNode->getScale().x));
				//mCamera->lookAt(player_pos);
				Ogre::Vector3 camera_pos = player_pos;
				camera_pos.y += 1;
				mCamera->setPosition(camera_pos);
			}
		} else {
			playerObject->ourNode->translate(-playerObject->momentum * evt.timeSinceLastFrame, Ogre::Node::TS_LOCAL);
			playerObject->momentum = Ogre::Vector3(0,0,0);
		}
	}

	if (affixCamera) {
		mCamera->lookAt((playerObject->ourNode->getOrientation()*(playerObject->ourNode->getChild("SelectorNode")->getPosition()*playerObject->ourNode->getScale()))+playerObject->ourNode->getPosition());
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
