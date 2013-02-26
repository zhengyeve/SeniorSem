
#include <OgreEntity.h>
#include <string>

#ifndef __WORLDOBJECT_H__
#define __WORLDOBJECT_H__

using namespace std;

enum ObjectType{OBJECT_PLANT, OBJECT_CREATURE};
enum ActionType{ACTION_NONE, ACTION_CHOP, ACTION_REMOVE_SELF, ACTION_FEED, ACTION_MODIFY_VOXELS};
static string ActionNames[5] = {"No Action", "Chop", "Self Destruct", "Feed", "Change Terrain"};

class Action {
public:
	ActionType actionType;
	double actionVar, actionVar2;
	Action(ActionType type, double action_val) { 
		actionType = type;
		actionVar = action_val;
	}
	string getName() {
		return ActionNames[actionType];
	}
};

class WorldObject
{
public:
	float collisionRadius;
	Ogre::SceneNode* ourNode;
	bool isClear;
	ObjectType objectType;
	int subtype;

	virtual vector<Action> receiveAction(Action action)=0;
};

#endif