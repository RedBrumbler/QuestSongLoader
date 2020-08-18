#include "../include/mod_interface.hpp"
#include <unordered_set>
#include "../extern/beatsaber-hook/utils/utils.h"
#include "../extern/beatsaber-hook/utils/logging.hpp"
#include "../extern/modloader/modloader.hpp"
#include "../extern/beatsaber-hook/utils/il2cpp-utils.hpp" 
#include "../extern/beatsaber-hook/utils/il2cpp-functions.hpp"
#include "../extern/beatsaber-hook/utils/typedefs.h"
#include "../extern/beatsaber-hook/config/config-utils.hpp"

namespace CustomSong
{
    class MapData
    {
        public:
            std::string version;
            std::vector<CustomSong::Event> events;
            std::vector<CustomSong::Note> notes;
            std::vector<CustomSong::Obstacle> obstacles;
    };

    class Event
    {
        public:
            float time;
            int type;
            int value;
    };

    class Note
    {
        public:
            float time;
            int lineIndex;
            int lineLayer;
            int type;
            int cutDirection;
    };

    class Obstacle
    {
        public:
            float time;
            int lineIndex;
            int type;
            float duration;
            int width;
    };

    class Info
    {
        public:
            std::string version;
            std::string songName;
            std::string songSubName;
            std::string songAuthorName;
            std::string levelAuthorName;
            int beatsPerMinute;
            int shuffle;
            float shufflePeriod;
            int previewStartTime;
            float previewDuration;
            std::string songFilename;
            std::string coverImageFilename;
            std::string environmentName;
            int songTimeOffset;
            std::vector<CustomSong::DifficultyBeatmapSet> difficultyBeatmapSets;
    };

    class DifficultyBeatmapSet
    {
        public:
            std::string beatmapCharacteristicName;
            std::vector<CustomSong::DifficultyBeatmap> difficultyBeatmap;
    };

    class DifficultyBeatmap
    {
        public:
            std::string difficuly;
            int difficultyRank;
            std::string beatmapFilename;
            int noteJumpMovementSpeed;
            float noteJumpStartBeatOffset;
    };

}
