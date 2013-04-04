#include "CreatureObject.h"

using namespace std;

CreatureObject::~CreatureObject() {
	for (unsigned int i = 0; i < inventory.size(); ++i) {
		delete inventory[i];
	}
}

CreatureObject::CreatureObject(Ogre::SceneNode* node, float collision_radius) {
	ourNode = node;
	collisionRadius = collision_radius;
	isClear = false;
	objectType = OBJECT_CREATURE;
	hunger = 50;
	speed = 1;
	momentum.x = momentum.y = momentum.z = 0;
	numObjects = 1;
	maxInventorySize = 10;
}

vector<Action> CreatureObject::receiveAction(Action action) {
	Action return_action(ACTION_REMOVE_SELF, 20);
	vector<Action> to_return;
	to_return.push_back(return_action);
	return to_return;
}

bool CreatureObject::operator==(const WorldObject& other) {
	//honestly, we don't want people stacking foxes or whatever. Those animals should all have individual characteristics
	return false;
}

int CreatureObject::obtainObject(WorldObject* to_add) {
	if (inventory.size() == maxInventorySize) {
		return 0;
	} else {
		bool foundit = false;
		for (unsigned int i = 0; i < inventory.size(); ++i) {
			if (*inventory[i] == *to_add) {
				inventory[i]->numObjects++;
				foundit = true;
				cout << "FOUNDIT AHOIUGAIUSGDLKGIUGSIDUGASOIUDGOIUGAIOUSGIUAGSDIUAGIUSGDoiAShd\n";
				return 2;
			}
		}
		if (!foundit) {
			inventory.push_back(to_add);
			cout << "Adding new object to inventory.\n";
			return 1;
		}
	}
}

void CreatureObject::listInventory(void) {
	for (unsigned int i = 0; i < inventory.size(); ++i) {
		cout << inventory[i]->getName() << "\t" << inventory[i]->numObjects << endl;
	}
}