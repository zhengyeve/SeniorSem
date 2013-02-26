#ifndef __TREEOBJECT_H__
#define __TREEOBJECT_H__

#include "WorldObject.h"

using namespace std;

class TreeObject: public WorldObject
{
private:
	bool isEatable;
	int hungerValue;

public:
	TreeObject(Ogre::SceneNode* node, float collision_radius);
	vector<Action> receiveAction(Action action);
	void setEatable(bool eatable, int hunger_restored = 0);
};

#endif