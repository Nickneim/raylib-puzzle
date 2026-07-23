#include "core/SaveData.hpp"

#include <fstream>
#include <iostream>
#include <string>

void SaveData::markLevelAsSolved(int levelIndex)
{
	if (levelIndex >= 0 && levelIndex < static_cast<int>(solvedLevels.size())) {
		solvedLevels[levelIndex] = true;
	}
}

bool SaveData::levelIsSolved(int levelIndex) const
{
	if (levelIndex >= 0 && levelIndex < static_cast<int>(solvedLevels.size())) {
		return solvedLevels[levelIndex];
	}
	return false;
}

void SaveData::setLevelAmount(int levelAmount)
{
	if (levelAmount >= 0) {
		solvedLevels.resize(levelAmount, false);
	}
}

void SaveData::load()
{
	std::ifstream file{ savefileName };

	if (!file.is_open()) {
		std::cerr << "Failed to load savefile: " << savefileName << std::endl;
		return;
	}

	std::string line;
	std::getline(file, line);

	for (size_t i = 0; i < solvedLevels.size() && i < line.size(); ++i) {
		solvedLevels[i] = (line[i] == '1');
	}
}

void SaveData::save()
{
	std::ofstream file{ savefileName };

	if (!file.is_open()) {
		std::cerr << "Failed to save savefile: " << savefileName << std::endl;
		return;
	}

	for (bool solved : solvedLevels) {
		file << (solved ? '1' : '0');
	}
}


void SaveData::resetLevelProgress() {
	for (size_t i = 0; i < solvedLevels.size(); ++i) {
		solvedLevels[i] = false;
	}
}
