#ifndef __PLANTMANAGER_H__
#define __PLANTMANAGER_H__

#include <vector>
#include "PlantObject.h"

struct PlantProfile {
	PlantType plantType;
	//the ideal value for each category. The average flatness for the test map is 0.2 and average height is 57.
	double idealHeight;
	float idealFlatness;
	//the maximum range for each category. If a plant has an idealHeight=50 and a rangeHeight=10, they won't be generated below 40 or above 60,
	//and will be much more common at 50 that at 40.
	double rangeHeight;
	float rangeFlatness;
	//the weights will be how important each category is, relative to one another. The exact numbers don't matter, rather the ratio is what
	//determines the relative weight. weightHeight=1 and weightFlatness=2 is equivalent to weightHeight=10 and weightFlatness=20. The higher
	//the number the more important the category is.
	float weightHeight, weightFlatness;
	//frequency of plant occurence (rarity). The higher the number, the more frequent it will be.
	int frequency;
};

class PlantManager
{
private:
	std::vector<PlantProfile> plants;

public:
	//initializes the plant profiles
	PlantManager(void);
	//Pass this the terrain height and flatness at a location and it will return to you a plant that occurs there. Sometimes returns PLANT_NONE
	//if no plant ought to be placed at all.
	PlantType getFlora(double height, float terrain_flatness);
};

#endif