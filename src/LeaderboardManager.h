#pragma once
#include <vector>
#include <string>

struct LeaderboardEntry {
    int score;
    float time;
};

class LeaderboardManager {
private:
    std::vector<LeaderboardEntry> entries;
    std::string filename = "leaderboard.txt";

public:
    void load();
    void save();

    void addScore(int score, float time);

    const std::vector<LeaderboardEntry>& getEntries() const;
};