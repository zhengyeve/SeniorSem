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
#include "WorldObject.h"
#include <vector>
 
using namespace std;

class BasicTutorial4 : public BaseApplication
{
public:
    BasicTutorial4(void);
    virtual ~BasicTutorial4(void);
    
private:
    Ogre::TerrainGlobalOptions* mTerrainGlobals;
    Ogre::TerrainGroup* mTerrainGroup;
    bool mTerrainsImported;
 
    void defineTerrain(long x, long y);
    void initBlendMaps(Ogre::Terrain* terrain);
    void configureTerrainDefaults(Ogre::Light* light);
	OgreBites::Label* mInfoLabel;
	bool processUnbufferedInput(const Ogre::FrameEvent& evt);
	vector<WorldObject> worldObjects;
	//returns the index of the collided object
	int checkForCollision(Ogre::Vector3* to_check);

protected:
    virtual void createScene(void);
    virtual void createFrameListener(void);
    virtual void destroyScene(void);
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
};

