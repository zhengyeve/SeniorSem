
#include "TreeObject.h"

TreeObject::TreeObject(Ogre::SceneNode* node, float collision_radius) {
	ourNode = node;
	collisionRadius = collision_radius;
	isClear = false;
	objectType = OBJECT_PLANT;
}

bool TreeObject::receiveAction(ActionType action_type, double action_var) {
	return true;
}