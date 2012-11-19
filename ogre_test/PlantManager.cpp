#include "PlantManager.h"
#include <iostream>

PlantManager::PlantManager(void) {
	//TODO: Use an XML file or something to read in these plant values
	//Average flatness is about 0.2 on the test map
	PlantProfile oak;
	oak.plantType = PLANT_OAK;
	oak.frequency = 100;
	oak.idealHeight = 45;
	oak.rangeHeight = 40;
	oak.weightHeight = 1.0;
	oak.idealFlatness = 0.2;
	oak.rangeFlatness = 0.1;
	oak.weightFlatness = 0.7;
	plants.push_back(oak);

	PlantProfile pine;
	pine.plantType = PLANT_PINE;
	pine.frequency = 75;
	pine.idealHeight = 95;
	pine.rangeHeight = 50;
	pine.weightHeight = 1.0;
	pine.idealFlatness = 0.18;
	pine.rangeFlatness = 0.15;
	pine.weightFlatness = 0.5;
	plants.push_back(pine);

	PlantProfile round_shroom;
	round_shroom.plantType = PLANT_ROUND_SHROOM;
	round_shroom.frequency = 150;
	round_shroom.idealHeight = 45;
	round_shroom.rangeHeight = 45;
	round_shroom.weightHeight = 0.1;
	round_shroom.idealFlatness = 0.23;
	round_shroom.rangeFlatness = 0.06;
	round_shroom.weightFlatness = 3.0;
	plants.push_back(round_shroom);
}

PlantType PlantManager::getFlora(double height, float terrain_flatness) {
	//cull about half... 
	/*if (rand()%2) {
		return PLANT_NONE;
	}*/

	/*static float average_flatness = 0;
	static double num_evaluated = 0;

	if (terrain_flatness < 0.9 && terrain_flatness > 0.01) {
		average_flatness = ((num_evaluated * average_flatness) + terrain_flatness) / (num_evaluated + 1.0);
		num_evaluated++;
		std::cout << "Average flatness: " << average_flatness << "\n";
		std::cout << "Number evaluated: " << num_evaluated << "\n";
	}*/

	PlantType winner = PLANT_NONE;
	float highest_score = 20; //increase number to decrease the chance plants will spawn
	float cur_score = 0;

	for (unsigned int i = 0; i < plants.size(); ++i) {
		cur_score = (plants[i].rangeHeight - abs(plants[i].idealHeight-height)) / plants[i].rangeHeight;
		cur_score *= plants[i].weightHeight;
		//if it's negative (outside possible range), skip the next calculation step. In case multiple negative numbers combine to make a positive one...
		if (cur_score < 0) {
			cur_score = 0;
		} else {
			cur_score *= ((plants[i].rangeFlatness - abs(plants[i].idealFlatness-terrain_flatness)) / plants[i].rangeFlatness) * plants[i].weightFlatness;
			cur_score /= plants[i].weightHeight * plants[i].weightFlatness;
		}
		/*if (plants[i].plantType == PLANT_ROUND_SHROOM && cur_score > 0) {
			std::cout << "cur_score: " << cur_score << "\tcur flatness: " << terrain_flatness << "\n";
		}*/
		//make sure this hasn't become a negative number
		if (cur_score < 0) {
			cur_score = 0;
		}
		//at this point, cur_score should be in the range 0.0-1.0
		cur_score *= rand()%plants[i].frequency;
		if (cur_score > highest_score) {
			highest_score = cur_score;
			winner = plants[i].plantType;
		}
	}

	return winner;
}