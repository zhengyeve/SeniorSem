/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.h
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

#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include "BaseApplication.h"
#include "CreatureObject.h"
#include "WorldObject.h"
#include <vector>
<<<<<<< HEAD


=======
#include "MapChunk.h"
#include "MapManager.h"
>>>>>>> origin/Matt's-Branch
 
using namespace std;

class GameFramework : public BaseApplication
{
public:
    GameFramework(void);
    virtual ~GameFramework(void);
    
private:
	//DATA MEMBERS

	//text labels for displaying player conditions
	OgreBites::Label* scoreLabel;
	OgreBites::Label* hungerLabel;

	//stores all the objects in the world, such as trees
	vector<WorldObject*> worldObjects;

	//If this is > 0, it's in the process of a countdown to exit
	float exitTimer;

	//the player's object
	CreatureObject* playerObject;

	//what contains the voxel map
	MapManager* mapManager;

	//FUNCTIONS
	//handles all input, like key presses or mouse clicks
	bool processUnbufferedInput(const Ogre::FrameEvent& evt);

	//returns the index of the collided object in the worldObjects vector, or -1 if no collision occurred.
	int checkForCollision(Ogre::Vector3* to_check);

	//deletes an object in the world
	void removeWorldObject(int index, bool do_delete=true);

	//covers the world in plants
	void populatePlants(PolyVox::Region region);

	//updates world objects so those outside the region are invisible
	void updateVisibleObjects(PolyVox::Region region);
	
	void handleAction(Action action, WorldObject* target, WorldObject* actor);

	// GUI initialization
	
//	void initializeGUI();

protected:
	//creates a frame listener base object which runs every time another frame is queued. Later we register the "frameRenderingQueued" function as our listener function
	//with this base object, and then the base object will call our function whenever a frame is queued for rendering. A frame is a still picture rendered to the screen,
	//usually at a rate of aroud 50-60 per second for our game, giving the illusion of smooth-ish movement. There are different frame listeners for when the frame is starting
	//to be made, is finished being prepared and about to be rendered to the screen, and another for when the rendering is done. The reason we choose the one for when it's
	//about to be drawn to the screen ("frameRenderingQueued") is so we can do computationally expensive tasks with the CPU while the drawing goes on with the GPU or graphics card,
	//as opposed to doing the expensive tasks while the GPU is idled and not making full use of both.
    virtual void createFrameListener(void);

	//sets up everything before the game starts running, kinda like a main function for our game
    virtual void createScene(void);

	//creates the scene. 
    virtual void destroyScene(void);

	//our frame listener function we use for updating code stuff many times a second.
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

};

