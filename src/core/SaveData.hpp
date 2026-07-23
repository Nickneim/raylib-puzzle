#pragma once

#include <string>
#include <vector>

class SaveData {
public:
    void markLevelAsSolved(int levelIndex);
    bool levelIsSolved(int levelIndex) const;
    void setLevelAmount(int levelAmount);
    int currentLevelIndex{ -1 };


    void load();
    void save();
    void resetLevelProgress();
private:
    std::vector<bool> solvedLevels{};
    std::string savefileName{ "save.dat" };
};