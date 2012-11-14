
#include <OgreEntity.h>


#ifndef __WORLDOBJECT_H__
#define __WORLDOBJECT_H__

class WorldObject
{
public:
	WorldObject(Ogre::SceneNode* node, float collision_radius);
	float collisionRadius;
	Ogre::SceneNode* ourNode;
	bool isClear;
};

#endif