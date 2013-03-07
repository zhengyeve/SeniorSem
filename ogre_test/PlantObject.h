#ifndef __PLANTOBJECT_H__
#define __PLANTOBJECT_H__

#include "WorldObject.h"

using namespace std;

enum PlantType{PLANT_NONE, PLANT_OAK, PLANT_PINE, PLANT_ROUND_SHROOM};
static string PlantNames[4] = {"No Plant", "Oak", "Pine", "Round Mushroom"};

class PlantObject: public WorldObject
{
private:
	bool isEatable;
	int foodValue;

public:
	PlantObject(Ogre::SceneNode* node, float collision_radius);
	vector<Action> receiveAction(Action action);
	void setEatable(bool eatable, int hunger_restored = 0);
	virtual bool operator==(const WorldObject& other);
	string getName() {return PlantNames[subtype];}
};

#endif