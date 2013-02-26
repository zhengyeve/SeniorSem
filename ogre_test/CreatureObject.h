#ifndef __CREATUREOBJECT_H__
#define __CREATUREOBJECT_H__

#include "WorldObject.h"

using namespace std;

enum CreatureType{CREATURE_PLAYER};

class CreatureObject: public WorldObject
{
public:
	CreatureObject(Ogre::SceneNode* node, float collision_radius);
	vector<Action> receiveAction(Action action);
	//hunger counts down to zero eventually, killing the creature unless it eats
	float hunger;
	double speed;
	Ogre::Vector3 momentum;
};

#endif