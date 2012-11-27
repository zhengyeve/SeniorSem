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
 
using namespace std;

class GameFramework : public BaseApplication
{
public:
    GameFramework(void);
    virtual ~GameFramework(void);
    
private:
	//DATA MEMBERS
	//stores global variables for rendering the terrain, like how detailed it should be, etc.
    Ogre::TerrainGlobalOptions* mTerrainGlobals;

	//stores the terrain pieces themselves
    Ogre::TerrainGroup* mTerrainGroup;

	//whether the terrain has been sucessfully imported
    bool mTerrainsImported;

	//a text label for displaying messages at the top of the screen (e.g. "Please wait while terrain is generated...")
	OgreBites::Label* mInfoLabel;

	//stores all the objects in the world, such as trees
	vector<WorldObject*> worldObjects;

	//If this is > 0, it's in the process of a countdown to exit
	float exitTimer;

	//the player's object
	CreatureObject* playerObject;

	//FUNCTIONS
	//checks to see if the the terrain files have been generated or not, and loads them in if they have. If they haven't, it takes a while to generate them.
	//for this reason, the game will take a while to start up the first time you run it on a particular map, but subsequent loads will be much faster.
    void defineTerrain(long x, long y);

	//blends different terrain textures together based on terrain height (rocky up high, more dirt down low, etc) and saves that blend map for quick
	//future use, so Ogre doesn't have to blend the textures every time it wants to render the terrain.
    void initBlendMaps(Ogre::Terrain* terrain);

	//configures default terrain options for us. Some of the defaults we override later, but it's good to have a base.
    void configureTerrainDefaults(Ogre::Light* light);

	//handles all input, like key presses or mouse clicks
	bool processUnbufferedInput(const Ogre::FrameEvent& evt);

	//returns the index of the collided object in the worldObjects vector, or -1 if no collision occurred.
	int checkForCollision(Ogre::Vector3* to_check);

	//deletes an object in the world
	void removeWorldObject(int index);

	//covers the world in plants
	void populatePlants(void);

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

