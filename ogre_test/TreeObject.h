#ifndef __TREEOBJECT_H__
#define __TREEOBJECT_H__

#include "WorldObject.h"

class TreeObject: public WorldObject
{
public:
	TreeObject(Ogre::SceneNode* node, float collision_radius);
	bool receiveAction(ActionType action_type, double action_var);
};

#endif