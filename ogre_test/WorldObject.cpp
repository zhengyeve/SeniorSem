#include "WorldObject.h"

WorldObject::WorldObject(void) {
	objectType = OBJECT_NONE;
}

vector<Action> WorldObject::receiveAction(Action action) {
	vector<Action> to_return;
	return to_return;
}
/*bool WorldObject::operator==(const WorldObject& other) {
	return (other.objectType == OBJECT_NONE);
}*/

string WorldObject::getName(void) {
	return "Empty Object";
}

bool WorldObject::isNone(void) {
	return (objectType == OBJECT_NONE);
}

bool WorldObject::blocksMovement(void) {
	return doesBlockMovement;
}