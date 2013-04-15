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

//#include "MyGUI.h"
//#include "MyGUI_OgrePlatform.h"

//using namespace demo;


// variable of total score a player is now having
static unsigned long totalScore = 0;
static float INITIAL_HUNGER = 10;

using namespace PolyVox;
using namespace std;

using namespace PolyVox;
using namespace std;

GameFramework::GameFramework(void)
{
	playerObject = 0;
	mapManager = 0;
}
 
//-------------------------------------------------------------------------------------
GameFramework::~GameFramework(void)
{
	for (int i = 0; i < worldObjects.size(); ++i) {
		delete(worldObjects[i]);
	}
	if (playerObject) {
		delete(playerObject);
	}
	if (mapManager) {
		delete(mapManager);
	}
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
<<<<<<< HEAD
				float offset;
				float scale = 2+(rand()%11)/10.0;
=======
				bool doesBlockMovement = false;
				Ogre::Vector3 scale(2+(rand()%11)/10.0,2+(rand()%11)/10.0,2+(rand()%11)/10.0);
				double height_offset = 0.35;
>>>>>>> 8a872c1afa082e2c5060b20060ad13d3215b250a
				//change the model based on what plant we're placing
				switch(to_place) {
					case PLANT_OAK:
						tree_entity = mSceneMgr->createEntity("oaktree.mesh");
						clear_mat = Ogre::MaterialManager::getSingleton().getByName("Materials/Trees/ClearOak");
<<<<<<< HEAD
						scale /= 12;
						offset = .3;
=======
						doesBlockMovement = true;
>>>>>>> 8a872c1afa082e2c5060b20060ad13d3215b250a
						break;
					case PLANT_PINE:
						tree_entity = mSceneMgr->createEntity("pinetree.mesh");
						clear_mat = Ogre::MaterialManager::getSingleton().getByName("Materials/Trees/ClearPine");
<<<<<<< HEAD
						scale /= 20;
=======
						doesBlockMovement = true;
						scale.y /= 1.7;
						scale.z /= 1.2;
						scale.x /= 1.2;
						height_offset = -0.45;
>>>>>>> 8a872c1afa082e2c5060b20060ad13d3215b250a
						break;
					case PLANT_ROUND_SHROOM:
						tree_entity = mSceneMgr->createEntity("roundshroom.mesh");
						clear_mat = Ogre::MaterialManager::getSingleton().getByName("Materials/ClearGray");
<<<<<<< HEAD
						scale /= 20;
						offset = 0.45;
						break;
					case PLANT_BLUE_DICK:
						tree_entity = mSceneMgr->createEntity("Cube.008.mesh");
						clear_mat = Ogre::MaterialManager::getSingleton().getByName("Materials/Blue_dick_flower.mesh.material");
						scale /= 55;
						offset = .95;
						break;
					case PLANT_BALSAM_ROOT:
						tree_entity = mSceneMgr->createEntity("Cube.022.mesh");
						clear_mat = Ogre::MaterialManager::getSingleton().getByName("Materials/balsam-root.mesh.material");
						scale /= 75;
						offset = .8;
						break;
					case PLANT_CANARY_GRASS:
						tree_entity = mSceneMgr->createEntity("Plane.021.mesh");
						clear_mat = Ogre::MaterialManager::getSingleton().getByName("Materials/canary_grass.mesh.material");
						scale /= 45;
						offset = .75;
						break;
					case PLANT_MULE_EARS:
						tree_entity = mSceneMgr->createEntity("Plane.001.mesh");
						clear_mat = Ogre::MaterialManager::getSingleton().getByName("Materials/Mule Ears.mesh.material");
						scale /= 75;
						offset = .9;
						break;
					case PLANT_FIREWEED:
						tree_entity = mSceneMgr->createEntity("Cube.001.mesh");
						clear_mat = Ogre::MaterialManager::getSingleton().getByName("Materials/fireweed.material");
						scale /= 20;
						offset = .58;
						break;
					case PLANT_WINECUP:
						tree_entity = mSceneMgr->createEntity("Plane.039.mesh");
						clear_mat = Ogre::MaterialManager::getSingleton().getByName("Materials/winecup.material");
						scale /= 70;
						offset = .7;
=======
						height_offset = 0.45;
>>>>>>> 8a872c1afa082e2c5060b20060ad13d3215b250a
						break;
				};
				new_pos.x = temp_x;
				new_pos.z = temp_z;
<<<<<<< HEAD
				new_pos.y = mapManager->getHeightAt(new_pos.x,max_y,new_pos.z,1)+offset;
				Ogre::SceneNode *temp_node = mSceneMgr->getRootSceneNode()->createChildSceneNode(new_pos);
				temp_node->attachObject(tree_entity);
				//scale the model randomly
				
				temp_node->scale(scale,scale,scale);
=======
				new_pos.y = mapManager->getHeightAt(new_pos.x,max_y,new_pos.z,1,2.2)+height_offset;
				Ogre::SceneNode *temp_node = mSceneMgr->getRootSceneNode()->createChildSceneNode(new_pos);
				temp_node->attachObject(tree_entity);
				//scale the model randomly
				temp_node->scale(scale.x,scale.y,scale.z);
				temp_node->scale(0.1,0.1,0.1);
>>>>>>> 8a872c1afa082e2c5060b20060ad13d3215b250a
				//give the tree a random rotation about the y-axis, so the trees aren't all aligned on a grid
				Ogre::Radian rot_angle((Ogre::Real)(rand()%7));
				temp_node->rotate(Ogre::Vector3(0,1,0),rot_angle);
				PlantObject* new_obj = new PlantObject(temp_node,0.3);
				new_obj->subtype = (int)to_place;
				new_obj->doesBlockMovement = doesBlockMovement;
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
    Ogre::Vector3 lightdir(0.55, -0.3, 0.75);
    lightdir.normalise();
 
    Ogre::Light* light = mSceneMgr->createLight("tstLight");
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDirection(lightdir);
    light->setDiffuseColour(Ogre::ColourValue::White);
    light->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));
 
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.2));

	mapManager = new MapManager();
	mapManager->draw(0,0,0,mSceneMgr,true);

	populatePlants(PolyVox::Region(-200,-200,-200,200,200,200));


	// starting of gui configuration

	//	initializeGUI();
	//mGUI->load("Basic.layout");
	//mGUI->findWidget<MyGUI::Window>("Result")->setVisible(false);
	//mGUI->findWidget<MyGUI::Widget>("HelpPanel")->setVisible(true);


	//set up the player node, model, and object
	Ogre::Entity* ninjaEntity = mSceneMgr->createEntity("Ninja", "ninja.mesh");
	Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNode");
	node->attachObject(ninjaEntity);
	node->scale(0.006,0.006,0.006);
	Ogre::Vector3 temp_pos = node->getPosition();
	temp_pos.y = mapManager->getHeightAt(temp_pos.x, mapManager->getMaxHeight()+1, temp_pos.z)+2;
	cout << "Setting player's y to: " << temp_pos.y << " because we checked (" << temp_pos.x << ", " << mapManager->getMaxHeight() << ", " << temp_pos.z << ")\n";
	node->setPosition(temp_pos);
	playerObject = new CreatureObject(node, 0.3);
	playerObject->speed = 400*node->getScale().x;
	playerObject->hunger = INITIAL_HUNGER;

	//set up the selector object
	Ogre::SceneNode* selector_node = node->createChildSceneNode("SelectorNode");
	// This translation may seem redundant since we update the selector's position every time the mouse moves, but if we don't do this
	// then the camera tries to look at the selector which is in the same position as the camera itself, and an acid trip results.
	Ogre::Vector3 temp_vector = Ogre::Vector3::ZERO;
	temp_vector.z -= 250;
	temp_vector.y += 80;
	selector_node->translate(temp_vector, Ogre::Node::TS_LOCAL);
	selector_node->attachObject(mSceneMgr->createEntity("Selector", "RoundShroom.mesh"));
	selector_node->scale(10, 10, 10);
	selector_node->pitch(Ogre::Degree(-20));

	//set up the camera (viewpoint)
	Ogre::Vector3 camera_pos = playerObject->ourNode->getPosition();
	camera_pos.y += 1;
	mCamera->setPosition(camera_pos);
	//this long line is to get the position of the selector object, so the camera will start out looking at it.
	mCamera->lookAt((playerObject->ourNode->getOrientation()*(playerObject->ourNode->getChild("SelectorNode")->getPosition()*playerObject->ourNode->getScale()))+playerObject->ourNode->getPosition());
    mCamera->setNearClipDistance(0.1);
    mCamera->setFarClipDistance(9000);
	
	//set up the fog and skyplane
	Ogre::ColourValue fadeColour(0.9, 0.9, 0.9);
	mSceneMgr->setFog(Ogre::FOG_EXP2, fadeColour, 0.03);
	mWindow->getViewport(0)->setBackgroundColour(fadeColour);
	Ogre::Plane plane;
	plane.d = 20;
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Y;
	mSceneMgr->setSkyPlane(true, plane, "Examples/CloudySky", 500, 1000, true, 0.5, 150, 150);

	//check for collisions where the player spawned. If the player spawned on some plants, destroy them
	Ogre::Vector3 temp = node->getPosition();
	int collision_index = checkForCollision(&temp, COLLISION_MODE_MOVEMENT);
	while (collision_index != -1) {
		removeWorldObject(collision_index);
		collision_index = checkForCollision(&temp, COLLISION_MODE_MOVEMENT);
	}
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

	static float timeSinceLastRedraw = 0;
	timeSinceLastRedraw += evt.timeSinceLastFrame;
	if ((playerObject) && timeSinceLastRedraw > 1.5) {
		timeSinceLastRedraw = 0;
		Region render_area = mapManager->draw(playerObject->ourNode->getPosition().x, playerObject->ourNode->getPosition().y, playerObject->ourNode->getPosition().z, mSceneMgr);
		updateVisibleObjects(render_area);
	}




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

			string resultText("Starved to death! :c\nYour total Score is: "), resultScore;
			
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

int GameFramework::checkForCollision(Ogre::Vector3* to_check, CollisionMode mode) {
	for (int i = 0; i < worldObjects.size(); ++i) {
		if (to_check->squaredDistance(worldObjects[i]->ourNode->getPosition()) < Ogre::Math::Sqr(worldObjects[i]->collisionRadius + playerObject->collisionRadius)) {
			if (worldObjects[i]->blocksMovement() || (mode != COLLISION_MODE_MOVEMENT)) {
				return i;
			}
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
			int coll_index = checkForCollision(&selector_pos, COLLISION_MODE_ACTION);
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
		Ogre::Vector3 voxel_offset(0,0,0);
		if (selector_pos.x > 0) {
			voxel_offset.x += 0.5;
		}
		if (selector_pos.y > 0) {
			voxel_offset.y += 0.5;
		}
		if (selector_pos.z > 0) {
			voxel_offset.z += 0.5;
		}
		//setVoxelAt() returns true if a change was made (i.e. setting a previously empty space to solid), so if it returns true we want to ask the map to redraw itself.
		if (mapManager->setVoxelAt(selector_pos.x+voxel_offset.x, selector_pos.y+voxel_offset.y, selector_pos.z+voxel_offset.z, action.actionVar, action.actionVar2)) {
			//tell the map manager to redraw at the current position
			Region render_area = mapManager->draw(playerObject->ourNode->getPosition().x, playerObject->ourNode->getPosition().y, playerObject->ourNode->getPosition().z, mSceneMgr);
			updateVisibleObjects(render_area);
		}
	} else if (action.actionType == ACTION_DROP_SELF) {
		if (target->objectType == OBJECT_CREATURE) {
			CreatureObject* receiver_creature = (CreatureObject*) target;
			if (receiver_creature->obtainObject(actor)) {
				totalScore += 10;
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

bool GameFramework::mouseMoved( const OIS::MouseEvent &arg )
{
	//if inventory opened, do  this...
	//if (mTrayMgr->injectMouseMove(arg)) return true;
	//mTrayMgr->getCursorImage()->show();

	static double cursor_dist = 500;
	// When the mouse is moved, yaw the player and camera accordingly. (rotate from side to side)
	// arg.state contains all the movement information from the mouse. X keeps track of changes in the cursor's absolute X position, and Y keeps track of the Y.
	// the .rel is the relative movement since the last time mouseMoved was called.
	playerObject->ourNode->yaw(Ogre::Degree(-arg.state.X.rel * 0.15f));
	mCamera->yaw(Ogre::Degree(-arg.state.X.rel * 0.15f));

	// When the mouse is moved, pitch the camera accordingly. (rotate up and down)
	mCamera->pitch(Ogre::Degree(-arg.state.Y.rel * 0.15f));

	// arg.state.Z keeps track of the scroll wheel. If the player moves the scroll wheel, change how far the cursor is from them.
	if (arg.state.Z.rel < -1) {
		cursor_dist += arg.state.Z.rel / 15 - cursor_dist / 10.0;
	} else if (arg.state.Z.rel > 1) {
		cursor_dist += arg.state.Z.rel / 15 + cursor_dist / 10.0;
	}

	// Now we need to update the selector thing to move with our view.
	Ogre::Node* selector_node = playerObject->ourNode->getChild("SelectorNode");
	// Rotate it appropriately. It is important that this is the same amount that mCamera was pitched by, otherwise it will get out of sync.
	selector_node->pitch(Ogre::Degree(-arg.state.Y.rel * 0.15f));
	// Move it back to where our viewpoint is
	selector_node->setPosition(Ogre::Vector3(0,1.0/playerObject->ourNode->getScale().y,0));
	// Move it "forward" according to the local reference point. If we used "forward" according to the player's reference point, it wouldn't take into account
	// any changes in the orientation of the node.
	selector_node->translate(0,0,-cursor_dist,Ogre::Node::TS_LOCAL);

	// Because yes.
    return true;
}

bool GameFramework::processUnbufferedInput(const Ogre::FrameEvent& evt)
{
	static bool last_left_mouse = false;      // If the left mouse button was depressed last frame
	static bool last_right_mouse = false;     // If the right mouse button was depressed last frame
	static float time_since_left_click = 0;
	static float time_since_right_click = 0;
	static float click_delay = 0.2;		  // Allows continued actions while the mouse buttons are depressed every click_delay seconds.
	static bool affixCamera = true;			  // Keeps the camera stuck to the player. If the player turns this off, they will still be able to move, but the camera won't follow.
    static Ogre::Real mRotate = 0.13;   // The rotate constant
	static Ogre::Real mMove = playerObject->speed;      // The movement constant
	static bool has_jumped = false;
	static Action primary_action(ACTION_CHOP, 5);
	static Action secondary_action(ACTION_MODIFY_VOXELS, 254, 20);
	
	bool cur_left_mouse = mMouse->getMouseState().buttonDown(OIS::MB_Left);
	bool cur_right_mouse = mMouse->getMouseState().buttonDown(OIS::MB_Right);

	//if the mouse is pressed (currMouse) and wasn't pressed last frame, then they clicked
	if (cur_left_mouse && !last_left_mouse) {
		PlantObject temp_obj(playerObject->ourNode,0);
		temp_obj.objectType = OBJECT_NONE;
		handleAction(primary_action, &temp_obj, playerObject);
		time_since_left_click = 0;
	} else if (cur_left_mouse) {
		if (time_since_left_click >= click_delay) {
			PlantObject temp_obj(playerObject->ourNode,0);
			temp_obj.objectType = OBJECT_NONE;
			handleAction(primary_action, &temp_obj, playerObject);
			time_since_left_click = 0;
		} else {
			time_since_left_click += evt.timeSinceLastFrame;
		}
	}
	//record the mouse state for the next frame's use
	last_left_mouse = cur_left_mouse;
	

	if (cur_right_mouse && !last_right_mouse) {
		PlantObject temp_obj(playerObject->ourNode,0);
		temp_obj.objectType = OBJECT_NONE;
		handleAction(secondary_action, &temp_obj, playerObject);
		time_since_right_click = 0;
	} else if (cur_right_mouse) {
		if (time_since_right_click >= click_delay) {
			PlantObject temp_obj(playerObject->ourNode,0);
			temp_obj.objectType = OBJECT_NONE;
			handleAction(secondary_action, &temp_obj, playerObject);
			time_since_right_click = 0;
		} else {
			time_since_right_click += evt.timeSinceLastFrame;
		}
	}
	last_right_mouse = cur_right_mouse;

	//hacky material change code for until we get the inventory working.
	static float mat_change_timer = 0;
	if (mKeyboard->isKeyDown(OIS::KC_X) && (mat_change_timer < 0.01)) {
		if ((primary_action.actionType == ACTION_MODIFY_VOXELS) && (primary_action.actionVar > 240)) {
			primary_action.actionVar--;
			cout << "Material changed to " << primary_action.actionVar << endl;
			mat_change_timer = 0.5;
		} else if ((secondary_action.actionType == ACTION_MODIFY_VOXELS) && (secondary_action.actionVar > 240)) {
			secondary_action.actionVar--;
			cout << "Material changed to " << secondary_action.actionVar << endl;
			mat_change_timer = 0.5;
		}
	} else if (mKeyboard->isKeyDown(OIS::KC_Z) && (mat_change_timer < 0.01)) {
		if ((primary_action.actionType == ACTION_MODIFY_VOXELS) && (primary_action.actionVar < 255) && (primary_action.actionVar >= 240)) {
			primary_action.actionVar++;
			cout << "Material changed to " << primary_action.actionVar << endl;
			mat_change_timer = 0.5;
		} else if ((secondary_action.actionType == ACTION_MODIFY_VOXELS) && (secondary_action.actionVar < 255) && (secondary_action.actionVar >= 240)) {
			secondary_action.actionVar++;
			cout << "Material changed to " << secondary_action.actionVar << endl;
			mat_change_timer = 0.5;
		}
	}
	if (mat_change_timer > 0) {
		mat_change_timer -= evt.timeSinceLastFrame;
	}

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
	} else if (mKeyboard->isKeyDown(OIS::KC_2)) { //remove voxels
		if ((mKeyboard->isKeyDown(OIS::KC_LSHIFT)) || (mKeyboard->isKeyDown(OIS::KC_RSHIFT))) {
			secondary_action.actionType = ACTION_MODIFY_VOXELS;
			secondary_action.actionVar = 0; //the material (air)
			secondary_action.actionVar2 = 20; //the amount we want to change by
			mGUI->findWidget<MyGUI::Widget>("RightClickAction")->setProperty("ImageTexture","shovel.png");
		} else {
			primary_action.actionType = ACTION_MODIFY_VOXELS;
			primary_action.actionVar = 0; //the material (air)
			primary_action.actionVar2 = 20; //the amount we want to change by
			mGUI->findWidget<MyGUI::Widget>("LeftClickAction")->setProperty("ImageTexture","shovel.png");
		}
	} else if (mKeyboard->isKeyDown(OIS::KC_3)) { //add voxels
		if ((mKeyboard->isKeyDown(OIS::KC_LSHIFT)) || (mKeyboard->isKeyDown(OIS::KC_RSHIFT))) {
			secondary_action.actionType = ACTION_MODIFY_VOXELS;
			secondary_action.actionVar = 254; //the material (stone?)
			secondary_action.actionVar2 = 20; //the amount we want to change by
			mGUI->findWidget<MyGUI::Widget>("RightClickAction")->setProperty("ImageTexture","brick.png");
		} else {
			primary_action.actionType = ACTION_MODIFY_VOXELS;
			primary_action.actionVar = 254; //the material (stone?)
			primary_action.actionVar2 = 20; //the amount we want to change by
			mGUI->findWidget<MyGUI::Widget>("LeftClickAction")->setProperty("ImageTexture","brick.png");
		}
	}

	//use key C and c to toggle fixed camera on and off
	if (mKeyboard->isKeyDown(OIS::KC_C) && ((mKeyboard->isKeyDown(OIS::KC_LSHIFT) || (mKeyboard->isKeyDown(OIS::KC_RSHIFT))))) {
		affixCamera = true;
	} else if (mKeyboard->isKeyDown(OIS::KC_C)) {
		affixCamera = false;
	}

	float acceleration = 0.17;
	float max_speed = 5;
	float rest_threshold = 0.001;
	if (mKeyboard->isKeyDown(OIS::KC_W)) // Forward key pressed
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
	if (mKeyboard->isKeyDown(OIS::KC_S)) // Backward key pressed
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
	if (mKeyboard->isKeyDown(OIS::KC_Q)) // Left yaw
	{
		playerObject->ourNode->yaw(Ogre::Degree(mRotate * 10));
	}
	if (mKeyboard->isKeyDown(OIS::KC_A)) // Left strafe
	{
		if (-playerObject->momentum.x < max_speed/2) {
			playerObject->momentum.x -= acceleration;
		}
	} else if (-playerObject->momentum.x > rest_threshold) {
		playerObject->momentum.x += acceleration;
		if (playerObject->momentum.x > rest_threshold) {
			playerObject->momentum.x = 0;
		}
	}
	if (mKeyboard->isKeyDown(OIS::KC_E)) // Right yaw
	{
		playerObject->ourNode->yaw(Ogre::Degree(-mRotate * 10));
	}
	if (mKeyboard->isKeyDown(OIS::KC_D)) // Right strafe
	{
		if (playerObject->momentum.x < max_speed/2) {
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
		cout << "Player pos:\t" << playerObject->ourNode->getPosition() << "\nPlayer momentum:\t" << playerObject->momentum << "\nPlayer inventory:\n";
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
		if (checkForCollision(&player_pos, COLLISION_MODE_MOVEMENT) == -1) {
			double height_diff = mapManager->getAveragedHeightAt(player_pos.x,player_pos.y+1.5,player_pos.z) - player_pos.y + player_height;
			if (height_diff > 0.2) {
				height_diff = max(height_diff, (mapManager->getAveragedHeightAt(player_pos.x,player_pos.y+2.5,player_pos.z) - player_pos.y + player_height));
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

	/*if (affixCamera) {
		mCamera->lookAt((playerObject->ourNode->getOrientation()*(playerObject->ourNode->getChild("SelectorNode")->getPosition()*playerObject->ourNode->getScale()))+playerObject->ourNode->getPosition());
	}*/



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
