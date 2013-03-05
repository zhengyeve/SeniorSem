#ifndef __CREATUREOBJECT_H__
#define __CREATUREOBJECT_H__

#include "WorldObject.h"

using namespace std;

enum CreatureType{CREATURE_PLAYER};
static string CreatureNames[1] = {"Player"};

class CreatureObject: public WorldObject
{
private:
	vector<WorldObject*> inventory;
	
public:
	CreatureObject(Ogre::SceneNode* node, float collision_radius);
	~CreatureObject();
	vector<Action> receiveAction(Action action);
	//hunger counts down to zero eventually, killing the creature unless it eats
	float hunger;
	double speed;
	unsigned int maxInventorySize;
	Ogre::Vector3 momentum;
	bool operator==(const WorldObject& other);
	//returns 0 if adding the object failed, 1 if we used the object pointer, 2 if we added it to a stack
	int obtainObject(WorldObject* to_add);
	string getName() {return CreatureNames[subtype];}
	void listInventory(void);
};

#endif