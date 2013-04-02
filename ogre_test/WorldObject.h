
#include <OgreEntity.h>
#include <string>

#ifndef __WORLDOBJECT_H__
#define __WORLDOBJECT_H__

using namespace std;

enum ObjectType{OBJECT_NONE, OBJECT_PLANT, OBJECT_CREATURE};
enum ActionType{ACTION_NONE, ACTION_CHOP, ACTION_REMOVE_SELF, ACTION_FEED, ACTION_MODIFY_VOXELS, ACTION_DROP_OBJECT, ACTION_DROP_SELF};
static string ActionNames[6] = {"No Action", "Chop", "Self Destruct", "Feed", "Change Terrain", "Drop Object"};

class Action {
public:
	ActionType actionType;
	double actionVar, actionVar2;
	Action(ActionType type, double action_val=0, double action_val2=0) { 
		actionType = type;
		actionVar = action_val;
		actionVar2 = action_val2;
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
	unsigned int numObjects;

	WorldObject(void);
	virtual vector<Action> receiveAction(Action action);
	virtual bool operator==(const WorldObject& other)=0;
	virtual string getName();
	bool isNone(void);
};

#endif