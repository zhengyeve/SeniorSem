#ifndef __CREATUREOBJECT_H__
#define __CREATUREOBJECT_H__

#include "WorldObject.h"

enum CreatureType{CREATURE_PLAYER};

class CreatureObject: public WorldObject
{
public:
	CreatureObject(Ogre::SceneNode* node, float collision_radius);
	bool receiveAction(ActionType action_type, double action_var);
	//hunger counts down to zero eventually, killing the creature unless it eats
	float hunger;
	double speed;
};

#endif