

#include "PlantManager.h"
#include <iostream>

PlantManager::PlantManager(void) {
	//TODO: Use an XML file or something to read in these plant values

	//See the PlantManager.h for a description of each variable.
	PlantProfile temp_plant;
	temp_plant.plantType = PLANT_OAK;
	temp_plant.frequency = 100;
	temp_plant.idealHeight = 30;
	temp_plant.rangeHeight = 50;
	temp_plant.weightHeight = 1.0;
	temp_plant.idealFlatness = 0.85;
	temp_plant.rangeFlatness = 0.50;
	temp_plant.weightFlatness = 0.4;
	plants.push_back(temp_plant);

	temp_plant.plantType = PLANT_PINE;
	temp_plant.frequency = 75;
	temp_plant.idealHeight = 55;
	temp_plant.rangeHeight = 50;
	temp_plant.weightHeight = 1.0;
	temp_plant.idealFlatness = 0.70;
	temp_plant.rangeFlatness = 0.35;
	temp_plant.weightFlatness = 0.6;
	plants.push_back(temp_plant);

	temp_plant.plantType = PLANT_ROUND_SHROOM;
	temp_plant.frequency = 120;
	temp_plant.idealHeight = 35;
	temp_plant.rangeHeight = 75;
	temp_plant.weightHeight = 0.1;
	temp_plant.idealFlatness = 0.95;
	temp_plant.rangeFlatness = 0.20;
	temp_plant.weightFlatness = 3.0;
	plants.push_back(temp_plant);

	temp_plant.plantType = PLANT_BLUE_DICK;
	temp_plant.frequency = 100;
	temp_plant.idealHeight = 30;
	temp_plant.rangeHeight = 50;
	temp_plant.weightHeight = 1.0;
	temp_plant.idealFlatness = 0.85;
	temp_plant.rangeFlatness = 0.50;
	temp_plant.weightFlatness = 0.4;
	plants.push_back(temp_plant);

	temp_plant.plantType = PLANT_BALSAM_ROOT;
	temp_plant.frequency = 100;
	temp_plant.idealHeight = 30;
	temp_plant.rangeHeight = 50;
	temp_plant.weightHeight = 1.0;
	temp_plant.idealFlatness = 0.85;
	temp_plant.rangeFlatness = 0.50;
	temp_plant.weightFlatness = 0.4;
	plants.push_back(temp_plant);

	temp_plant.plantType = PLANT_CANARY_GRASS;
	temp_plant.frequency = 100;
	temp_plant.idealHeight = 30;
	temp_plant.rangeHeight = 50;
	temp_plant.weightHeight = 1.0;
	temp_plant.idealFlatness = 0.85;
	temp_plant.rangeFlatness = 0.50;
	temp_plant.weightFlatness = 0.4;
	plants.push_back(temp_plant);

	temp_plant.plantType = PLANT_MULE_EARS;
	temp_plant.frequency = 100;
	temp_plant.idealHeight = 30;
	temp_plant.rangeHeight = 50;
	temp_plant.weightHeight = 1.0;
	temp_plant.idealFlatness = 0.85;
	temp_plant.rangeFlatness = 0.50;
	temp_plant.weightFlatness = 0.4;
	plants.push_back(temp_plant);

	temp_plant.plantType = PLANT_FIREWEED;
	temp_plant.frequency = 100;
	temp_plant.idealHeight = 30;
	temp_plant.rangeHeight = 50;
	temp_plant.weightHeight = 1.0;
	temp_plant.idealFlatness = 0.85;
	temp_plant.rangeFlatness = 0.50;
	temp_plant.weightFlatness = 0.4;
	plants.push_back(temp_plant);

	temp_plant.plantType = PLANT_WINECUP;
	temp_plant.frequency = 100;
	temp_plant.idealHeight = 30;
	temp_plant.rangeHeight = 50;
	temp_plant.weightHeight = 1.0;
	temp_plant.idealFlatness = 0.85;
	temp_plant.rangeFlatness = 0.50;
	temp_plant.weightFlatness = 0.4;
	plants.push_back(temp_plant);
}

PlantType PlantManager::getFlora(double height, float terrain_flatness) {
	//An average height calculation for debugging.
	/*static double average_flatness = 0;
	static double average_height = 0;
	static double num_evaluated = 0;
	if (height > 0) {
		average_flatness = ((num_evaluated * average_flatness) + terrain_flatness) / (num_evaluated + 1.0);
		average_height = ((num_evaluated * average_height) + height) / (num_evaluated + 1.0);
		num_evaluated++;
		std::cout << "Average flatness: " << average_flatness << "\n";
		std::cout << "Average height: " << average_height << "\n";
		std::cout << "Number evaluated: " << num_evaluated << "\n";
	}*/

	/*if (rand() % 2 == 0) {
		return PLANT_NONE;
	}*/

	PlantType winner = PLANT_NONE;
	float highest_score = 35; //increase number to decrease the chance plants will spawn / decrease the overall number of plants

	for (unsigned int i = 0; i < plants.size(); ++i) {
		//subtracts the difference between the height and the ideal height from the range, and divides it by the range.
		//thus, if the distance from the ideal is zero, the calculation will evaluate to a 1. If the difference is
		//as large as the range, the calculation will evaluate to a 0. So, it will be a number from 0-1 that represents
		//how close to ideal it is.
		float cur_score = (plants[i].rangeHeight - abs(plants[i].idealHeight-height)) / plants[i].rangeHeight;
		//multiply it by the weight to scale it by it's importance
		cur_score *= plants[i].weightHeight;
		//if it's negative (outside possible range), skip the next calculation step. So multiple negative numbers won't combine to make a positive one...
		if (cur_score < 0) {
			cur_score = 0;
		} else {
			//just like the height calculation.
			cur_score *= ((plants[i].rangeFlatness - abs(plants[i].idealFlatness-terrain_flatness)) / plants[i].rangeFlatness) * plants[i].weightFlatness;
			//divide by the weights, so the resulting number will be between 0 and 1.
			cur_score /= plants[i].weightHeight * plants[i].weightFlatness;
		}
		//make sure this hasn't become a negative number by being outside the possible range
		if (cur_score < 0) {
			cur_score = 0;
		}
		//multiply the score by a random value between zero and the plant's frequency. Here's where a higher frequency can help
		//result in a higher score. The randomness is so that plant zones will interpenetrate somewhat, and not have a strict
		//line in between them.
		cur_score *= rand()%plants[i].frequency;
		//if this is better than what we've gotten in previous runs, make it the new high score and winner
		if (cur_score > highest_score) {
			highest_score = cur_score;
			winner = plants[i].plantType;
		}
	}

	return winner;
}