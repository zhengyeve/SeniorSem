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
#include "CreatureObject.h"
#include "PlantManager.h"
#include "glew/glew.h"

static unsigned long totalScore = 0;

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

void GameFramework::populatePlants(PolyVox::Region region) {
	cout << "Populating plants...\n";
	Ogre::Vector3 new_pos;
	PlantManager plantManager;
	int min_plant_spacing = 1;
	int min_x = region.getLowerCorner().getX();
	int max_x = region.getUpperCorner().getX();
	int min_z = region.getLowerCorner().getZ();
	int max_z = region.getUpperCorner().getZ();
	int max_y = region.getUpperCorner().getY();

	//goes from -(1/2 map size) to +(1/2 map size), incrementing by the minimum plant spacing each time, which prevents putting like 80 trees within a foot of one another
	for (int x = min_x; x < max_x; x+=min_plant_spacing) {
		for (int z = min_z; z < max_z; z+=min_plant_spacing) {
			double cur_height = mapManager->getHeightAt(x,max_y,z,2);
			double height_diff = abs(cur_height - mapManager->getHeightAt(x+min_plant_spacing,max_y,z+min_plant_spacing,1))+1;
			height_diff += abs(cur_height - mapManager->getHeightAt(x-min_plant_spacing,max_y,z+min_plant_spacing,1));
			height_diff += abs(cur_height - mapManager->getHeightAt(x-min_plant_spacing,max_y,z-min_plant_spacing,1));
			height_diff += abs(cur_height - mapManager->getHeightAt(x+min_plant_spacing,max_y,z-min_plant_spacing,1));
			height_diff *= 5;

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
				new_pos.x = x;
				new_pos.z = z;
				new_pos.y = mapManager->getHeightAt(new_pos.x,max_y,new_pos.z,1)+0.4;
				Ogre::SceneNode *temp_node = mSceneMgr->getRootSceneNode()->createChildSceneNode(new_pos);
				temp_node->attachObject(tree_entity);
				//scale the model randomly
				temp_node->scale(2+(rand()%11)/10.0,2+(rand()%11)/10.0,2+(rand()%11)/10.0);
				temp_node->scale(0.1,0.1,0.1);
				//give the tree a random rotation about the y-axis, so the trees aren't all aligned on a grid
				Ogre::Radian rot_angle((Ogre::Real)(rand()%7));
				temp_node->rotate(Ogre::Vector3(0,1,0),rot_angle);
				TreeObject* new_obj = new TreeObject(temp_node,0.3);
				new_obj->clearMat = clear_mat;
				new_obj->primaryMat = tree_entity->getSubEntity(0)->getMaterial();
				new_obj->subtype = (int)to_place;
				worldObjects.push_back(new_obj);
			}
		}
	}
	worldObjects.resize(worldObjects.size());
	cout << "Populated " << worldObjects.size() << " plants.\n";
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
	//mSceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));

	mapManager = new MapManager();
	mapManager->draw(0,0,0,mSceneMgr,true);

	//populatePlants(PolyVox::Region(-150,-150,-150,150,150,150));

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
	playerObject->hunger = 100;

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
 
    scoreLabel = mTrayMgr->createLabel(OgreBites::TL_TOP, "ScoreBox", "", 200);
	hungerLabel = mTrayMgr->createLabel(OgreBites::TL_BOTTOMRIGHT,"HungerBox","",200);
	mTrayMgr->createLabel(OgreBites::TL_TOP,"DeathBox","YOU ARE DEAD!",300);
	mTrayMgr->getWidget("DeathBox")->hide();
	mTrayMgr->removeWidgetFromTray("DeathBox");
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
			mShutDown = true;
		}
	}

	mTrayMgr->moveWidgetToTray(scoreLabel, OgreBites::TL_TOP, 0);
    scoreLabel->show();
	mTrayMgr->moveWidgetToTray(hungerLabel, OgreBites::TL_BOTTOMRIGHT, 0);
    hungerLabel->show();

	//uncomment this once we want folks to actually starve to death
	//playerObject->hunger -= evt.timeSinceLastFrame;
	if (playerObject->hunger < 0) {
		if (exitTimer < 0) {
			exitTimer = 2;
			hungerLabel->setCaption("Starved to death! :c");
			mTrayMgr->moveWidgetToTray("DeathBox", OgreBites::TL_CENTER, 0);
			mTrayMgr->getWidget("DeathBox")->show();
		}
	} else {
		char hungerStr[10];
		itoa((int) floor(playerObject->hunger + 0.5),hungerStr,10);
		char caption[20] = "Hunger: ";
		strcat(caption,hungerStr);
		hungerLabel->setCaption(caption);
	}

    bool ret = BaseApplication::frameRenderingQueued(evt);

	string str = "Your current score: ";
	stringstream strstm;
	strstm << totalScore;
	str += strstm.str();
    scoreLabel->setCaption(str);

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

void GameFramework::removeWorldObject(int index) {
	WorldObject* temp = worldObjects[index];
	worldObjects[index] = worldObjects[worldObjects.size()-1];
	worldObjects.pop_back();
	temp->ourNode->detachAllObjects();
	delete(temp);
}

bool GameFramework::processUnbufferedInput(const Ogre::FrameEvent& evt)
{
	static bool last_left_mouse = false;     // If the left mouse button is depressed
	static bool last_right_mouse = false;     // If the right mouse button is depressed
	static bool affixCamera = true;
    static Ogre::Real mRotate = 0.13;   // The rotate constant
	static Ogre::Real mMove = playerObject->speed;      // The movement constant
	
	bool cur_left_mouse = mMouse->getMouseState().buttonDown(OIS::MB_Left);
	bool cur_right_mouse = mMouse->getMouseState().buttonDown(OIS::MB_Right);

	//if the mouse is pressed (currMouse) and wasn't pressed last frame, then they clicked
	if (cur_left_mouse) {
		/*//check for collisions with that new point, and if something was hit we apply an action to it
		int coll_index = checkForCollision(&ninja_pos);
		if (coll_index != -1) {
			cout << "Performed action on " << coll_index << endl; //debug statement
			//apply the action. ACTION_CHOP is the only one so far, so use it
			if (worldObjects[coll_index]->receiveAction(ACTION_CHOP,5)) {
				if (worldObjects[coll_index]->objectType == OBJECT_PLANT && worldObjects[coll_index]->subtype == (int)PLANT_ROUND_SHROOM) {
					playerObject->hunger += 10;
				}
				//an object returns true when it wants to be destroyed
				removeWorldObject(coll_index);
				totalScore += 5;
				cout << "You've successfully killed a tree! Your current score is: "<<totalScore<<endl<<endl;
			}
		} else {
			cout << "Nothing within action range.\n"; //debug statement
		}*/
		Ogre::Vector3 selector_pos = (playerObject->ourNode->getOrientation()*(playerObject->ourNode->getChild("SelectorNode")->getPosition()*playerObject->ourNode->getScale()))+playerObject->ourNode->getPosition();
		
		double offset = 0.5;
		//setVoxelAt() returns true if a change was made (i.e. setting a previously empty space to solid), so if it returns true we want to ask the map to redraw itself.
		if (mapManager->setVoxelAt(selector_pos.x+offset, selector_pos.y+0.2+offset, selector_pos.z+offset, 0)) {
			//tell the map manager to redraw at the current position
			mapManager->draw(playerObject->ourNode->getPosition().x, playerObject->ourNode->getPosition().y, playerObject->ourNode->getPosition().z, mSceneMgr);
		}
	}
	//record the mouse state for the next frame's use
	last_left_mouse = cur_left_mouse;

	if (cur_right_mouse) {
		Ogre::Vector3 selector_pos = (playerObject->ourNode->getOrientation()*(playerObject->ourNode->getChild("SelectorNode")->getPosition()*playerObject->ourNode->getScale()))+playerObject->ourNode->getPosition();
		double offset = 0.5;
		//set the voxel to solid where the action is. The setVoxelAt(...) function returns true if a change was made.
		if (mapManager->setVoxelAt(selector_pos.x+offset, selector_pos.y+0.2+offset, selector_pos.z+offset, 255)) {
			//tell the map manager to redraw at the current position
			mapManager->draw(playerObject->ourNode->getPosition().x, playerObject->ourNode->getPosition().y, playerObject->ourNode->getPosition().z, mSceneMgr);
		}
	}
	last_right_mouse = cur_right_mouse;

	//use key 1 and 2 to toggle fixed camera on and off
	if (mKeyboard->isKeyDown(OIS::KC_1))
	{
		affixCamera = true;
	} else if(mKeyboard->isKeyDown(OIS::KC_2))
	{
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
	float jump_speed = 1.5;
	if (mKeyboard->isKeyDown(OIS::KC_SPACE) && (playerObject->momentum.y < jump_speed)) // Space bar (jump)
	{
		playerObject->momentum.y += acceleration;
	}

	if ((abs(playerObject->momentum.x) > rest_threshold) || (abs(playerObject->momentum.y) > rest_threshold) || (abs(playerObject->momentum.z) > rest_threshold)) {
		//move the ninja in whatever direction
		playerObject->ourNode->translate(playerObject->momentum * evt.timeSinceLastFrame, Ogre::Node::TS_LOCAL);
		//set the new position's height to be the height of the terrain at that location (so it doesn't fly when it walks off a cliff)
		Ogre::Vector3 player_pos = playerObject->ourNode->getPosition();
		float player_height = playerObject->ourNode->getScale().y * 80; //* 170;
		//player_pos.y = mapManager->getHeightAt(player_pos.x,player_pos.y+1,player_pos.z)+player_height;

		//if the new position isn't colliding with an object, make that our position and update the camera. Otherwise, move back to where we were.
		if (checkForCollision(&player_pos) == -1) {
			double height_diff = mapManager->getAveragedHeightAt(player_pos.x,player_pos.y+1,player_pos.z) - player_pos.y + player_height;
			if (abs(height_diff) > 0.2) {
				height_diff = max(height_diff, (mapManager->getAveragedHeightAt(player_pos.x,player_pos.y+2,player_pos.z) - player_pos.y + player_height));
			}
			cout << "Height diff: " << height_diff << endl;
			float hop_threshold = 0.1;
			//if it's a small height difference, make the player hop up
			if ((height_diff >= hop_threshold) && (height_diff < 1)) {
				playerObject->momentum.y+=acceleration;
				cout << "Fly, you fool!\n";
			} //if the height difference is higher, stop the player
			else if (height_diff > 1) {
				playerObject->ourNode->translate(-playerObject->momentum * evt.timeSinceLastFrame, Ogre::Node::TS_LOCAL);
				playerObject->momentum.x = 0;
				playerObject->momentum.y = 0;
				playerObject->momentum.z = 0;
			} else if ((height_diff < hop_threshold) && (height_diff > -hop_threshold)) {
				playerObject->momentum.y = 0;
				Ogre::Vector3 temp_vec(0,height_diff/100,0);
				playerObject->ourNode->translate(temp_vec);
				cout << "Levelling\n";
			} else {
				if ((-playerObject->momentum.y < max_speed) && (!mKeyboard->isKeyDown(OIS::KC_SPACE))) {
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
		}

		//Check to see if each object is closer to the camera than the player's character, and if so change the material to something transparent
		for (int i = 0; i < worldObjects.size(); ++i) {
			Ogre::Entity* temp = static_cast<Ogre::Entity*>(worldObjects[i]->ourNode->getAttachedObject(0));
			double camToObj = mCamera->getPosition().squaredDistance(worldObjects[i]->ourNode->getPosition());
			//if distance from ninja to camera is greater than the distance from camera to object...
			if (affixCamera && playerObject->ourNode->getPosition().squaredDistance(mCamera->getPosition()) > camToObj) {
				//if it's not already transparent, make it so
				if (!worldObjects[i]->isClear) {
					temp->setMaterial(worldObjects[i]->clearMat);
					worldObjects[i]->isClear = true;
				}
			} else if (worldObjects[i]->isClear) {
				temp->setMaterial(worldObjects[i]->primaryMat);
				worldObjects[i]->isClear = false;
			}
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
