#ifndef __PLANTMANAGER_H__
#define __PLANTMANAGER_H__

#include <vector>

enum PlantType{PLANT_NONE, PLANT_OAK, PLANT_PINE, PLANT_ROUND_SHROOM};

struct PlantProfile {
	PlantType plantType;
	//the ideal value for each category
	double idealHeight;
	float idealFlatness;
	//the range for each category
	double rangeHeight;
	float rangeFlatness;
	//the weights will be how important each category is
	float weightHeight, weightFlatness;
	//frequency of plant occurence (rarity)
	int frequency;
};

class PlantManager
{
private:
	std::vector<PlantProfile> plants;

public:
	PlantManager(void);
	PlantType getFlora(double height, float terrain_flatness);
};

#endif