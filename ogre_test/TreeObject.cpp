
#include "TreeObject.h"

TreeObject::TreeObject(Ogre::SceneNode* node, float collision_radius) {
	ourNode = node;
	collisionRadius = collision_radius;
	isClear = false;
	objectType = OBJECT_PLANT;
	isEatable = false;
}

vector<Action> TreeObject::receiveAction(Action action) {
	vector<Action> to_return;
	if (action.actionType == ACTION_CHOP) {
		Action return_action(ACTION_REMOVE_SELF, 10);
		to_return.push_back(return_action);
		if (isEatable) {
			return_action.actionType = ACTION_FEED;
			return_action.actionVar = hungerValue;
			to_return.push_back(return_action);
		}
	}
	return to_return;
}

void TreeObject::setEatable(bool eatable, int hunger_restored) {
	isEatable = eatable;
	hungerValue = hunger_restored;
}