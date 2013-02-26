
#ifndef __ACTION_H__
#define __ACTION_H__

enum ActionType{ACTION_CHOP, ACTION_REMOVE_SELF, ACTION_FEED};
extern WorldObject;

class Action
{
public:
	ActionType actionType;
	double actionVar;
	WorldObject actionObject;
};

#endif