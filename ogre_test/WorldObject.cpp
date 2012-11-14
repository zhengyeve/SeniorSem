#include "WorldObject.h"

WorldObject::WorldObject(Ogre::SceneNode* node, float collision_radius) {
	ourNode = node;
	collisionRadius = collision_radius;
	isClear=false;
}