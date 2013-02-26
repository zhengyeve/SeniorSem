#include "CreatureObject.h"

using namespace std;

CreatureObject::CreatureObject(Ogre::SceneNode* node, float collision_radius) {
	ourNode = node;
	collisionRadius = collision_radius;
	isClear = false;
	objectType = OBJECT_CREATURE;
	hunger = 50;
	speed = 1;
	momentum.x = momentum.y = momentum.z = 0;
}

vector<Action> CreatureObject::receiveAction(Action action) {
	Action return_action(ACTION_REMOVE_SELF, 20);
	vector<Action> to_return;
	to_return.push_back(return_action);
	return to_return;
}