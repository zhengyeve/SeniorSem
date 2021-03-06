

#include "PlantManager.h"
#include <iostream>

PlantManager::PlantManager(void) {
	//TODO: Use an XML file or something to read in these plant values

	//See the PlantManager.h for a description of each variable.
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
	//An average height calculation for debugging.
	/*static double average_height = 0;
	static double num_evaluated = 0;
	if (height > 0) {
		average_height = ((num_evaluated * average_height) + height) / (num_evaluated + 1.0);
		num_evaluated++;
		std::cout << "Average height: " << average_height << "\n";
		std::cout << "Number evaluated: " << num_evaluated << "\n";
	}*/

	PlantType winner = PLANT_NONE;
	float highest_score = 20; //increase number to decrease the chance plants will spawn / decrease the overall number of plants

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