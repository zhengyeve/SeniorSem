#include "CreatureObject.h"

CreatureObject::CreatureObject(Ogre::SceneNode* node, float collision_radius) {
	ourNode = node;
	collisionRadius = collision_radius;
	isClear = false;
	objectType = OBJECT_CREATURE;
	hunger = 50;
	speed = 1;
	momentum.x = momentum.y = momentum.z = 0;
}

bool CreatureObject::receiveAction(ActionType action_type, double action_var) {
	return true;
}