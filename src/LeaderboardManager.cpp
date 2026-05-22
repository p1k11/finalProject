#include "LeaderboardManager.h"
#include <fstream>
#include <algorithm>

void LeaderboardManager::load() {
    entries.clear();

    std::ifstream file(filename);

    if (!file.is_open()) {
        return;
    }

    int savedScore;
    float savedTime;

    while (file >> savedScore >> savedTime) {
        entries.push_back({ savedScore, savedTime });
    }

    file.close();
}

void LeaderboardManager::save() {
    std::ofstream file(filename);

    for (auto& entry : entries) {
        file << entry.score << " " << entry.time << "\n";
    }

    file.close();
}

void LeaderboardManager::addScore(int score, float time) {
    entries.push_back({ score, time });

    std::sort(entries.begin(), entries.end(),
        [](const LeaderboardEntry& a, const LeaderboardEntry& b) {
            if (a.score == b.score) {
                return a.time < b.time;
            }

            return a.score > b.score;
        });

    if (entries.size() > 5) {
        entries.resize(5);
    }

    save();
}

const std::vector<LeaderboardEntry>& LeaderboardManager::getEntries() const {
    return entries;
}