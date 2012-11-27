
#include <OgreEntity.h>


#ifndef __WORLDOBJECT_H__
#define __WORLDOBJECT_H__

//in the future this will have all the different types of actions
enum ActionType{ACTION_CHOP};
enum ObjectType{OBJECT_PLANT, OBJECT_CREATURE};

class WorldObject
{
public:
	float collisionRadius;
	Ogre::SceneNode* ourNode;
	bool isClear;
	Ogre::ResourcePtr primaryMat, clearMat;
	ObjectType objectType;
	int subtype;


	//how we tell an object an action is being done to it. The "action_type" is the name of the type of action, like say ACTION_CHOP
	//and the action_var is used for passing various data. For ACTION_CHOP, for example, it would be how much damage the chop does.
	//returns whether or not the game framework is to destroy the object after returning.
	virtual bool receiveAction(ActionType action_type, double action_var)=0;
};

#endif