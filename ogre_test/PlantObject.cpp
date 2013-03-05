
#include "PlantObject.h"

PlantObject::PlantObject(Ogre::SceneNode* node, float collision_radius) {
	ourNode = node;
	collisionRadius = collision_radius;
	isClear = false;
	objectType = OBJECT_PLANT;
	isEatable = false;
	numObjects = 1;
}

vector<Action> PlantObject::receiveAction(Action action) {
	vector<Action> to_return;
	if (action.actionType == ACTION_CHOP) {
		/*to_return.push_back(return_action);
		return_action.actionType = ACTION_DROP_OBJECT;
		return_action.actionVar = objectType;
		return_action.actionVar2 = subtype;*/
		
		if (isEatable) {
			to_return.push_back(Action(ACTION_FEED,foodValue));
		}
		//drop self MUST be the last action!
		to_return.push_back(Action(ACTION_DROP_SELF));
	}
	return to_return;
}

void PlantObject::setEatable(bool eatable, int hunger_restored) {
	isEatable = eatable;
	foodValue = hunger_restored;
}

bool PlantObject::operator==(const WorldObject& other) {
	if (other.objectType != objectType) {
		return false;
	} else {
		const PlantObject& other_plant = (const PlantObject&) other;

		if ((objectType == other_plant.subtype) && (isEatable == other_plant.isEatable) && (foodValue == other_plant.foodValue)) {
			return true;
		} else {
			return false;
		}
	}
}