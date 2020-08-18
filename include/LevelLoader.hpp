#pragma once

#include "../include/mod_interface.hpp"
#include <unordered_set>
#include "../extern/beatsaber-hook/shared/utils/utils.h"
#include "../extern/beatsaber-hook/shared/utils/logging.hpp"
#include "../extern/modloader/shared/modloader.hpp"
#include "../extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp" 
#include "../extern/beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "../extern/beatsaber-hook/shared/utils/typedefs.h"
#include "../extern/beatsaber-hook/shared/config/config-utils.hpp"

#include "GlobalNamespace/CustomBeatmapLevel.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"  
#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/BeatmapLevelData.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/CustomDifficultyBeatmap.hpp"
#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/StandardLevelInfoSaveData.hpp"
#include "System/Threading/CancellationToken.hpp"
#include "GlobalNamespace/CustomDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/BeatmapCharacteristicCollectionSO.hpp"
#include "GlobalNamespace/StandardLevelInfoSaveData_DifficultyBeatmapSet.hpp"
#include "GlobalNamespace/StandardLevelInfoSaveData_DifficultyBeatmap.hpp"
#include "GlobalNamespace/BeatmapDataLoader.hpp"
#include "UnityEngine/AudioClip.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "Utils/Unity.hpp"
#include "SongLoader.hpp"
#include "Utils/Audio.hpp"
#include "UnityEngine/Networking/UnityWebRequest.hpp"
#include "System/Threading/CancellationToken.hpp"
#include "System/Threading/CancellationTokenSource.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "System/Runtime/CompilerServices/TaskAwaiter_1.hpp"
#include "vorbisLib/stb_vorbis.h"

namespace SongLoader
{
    class LevelLoader
    {
        public:

            /// @brief activated on first menu load
            static void OnLoad(ModInfo modInfo);

            static GlobalNamespace::CustomBeatmapLevel* LoadCustomBeatmapLevel(GlobalNamespace::CustomPreviewBeatmapLevel* customPreviewBeatmapLevel);

        private: 

            static const Logger& getLogger();

            static ModInfo modInfo;

            static GlobalNamespace::BeatmapLevelData* LoadBeatmapLevelData(std::string customLevelPath, GlobalNamespace::CustomBeatmapLevel* customBeatmapLevel, GlobalNamespace::StandardLevelInfoSaveData* standardLevelInfoSaveData, UnityEngine::AudioClip* previewAudioClip);

            static Array<GlobalNamespace::IDifficultyBeatmapSet*>* LoadDifficultyBeatmapSets(std::string customLevelPath, GlobalNamespace::CustomBeatmapLevel* customBeatmapLevel, GlobalNamespace::StandardLevelInfoSaveData* standardLevelInfoSaveData);

            static GlobalNamespace::IDifficultyBeatmapSet* LoadDifficultyBeatmapSet(std::string customLevelPath, GlobalNamespace::CustomBeatmapLevel* customBeatmapLevel, GlobalNamespace::StandardLevelInfoSaveData* standardLevelInfoSaveData, GlobalNamespace::StandardLevelInfoSaveData::DifficultyBeatmapSet* difficultyBeatmapSetSaveData);
            
            static GlobalNamespace::CustomDifficultyBeatmap* LoadDifficultyBeatmap(std::string customLevelPath, GlobalNamespace::CustomBeatmapLevel* parentCustomBeatmapLevel, GlobalNamespace::CustomDifficultyBeatmapSet* parentDifficultyBeatmapSet, GlobalNamespace::StandardLevelInfoSaveData* standardLevelInfoSaveData, GlobalNamespace::StandardLevelInfoSaveData::DifficultyBeatmap* difficultyBeatmapSaveData);

            static GlobalNamespace::BeatmapData* LoadBeatmapData(std::string customLevelPath, std::string difficultyFileName, GlobalNamespace::StandardLevelInfoSaveData* standardLevelInfoSaveData);

            static GlobalNamespace::BeatmapData* LoadBeatmapDataBeatmapData(std::string customLevelPath, std::string difficultyFileName, GlobalNamespace::StandardLevelInfoSaveData* standardLevelInfoSaveData);
            
            static UnityEngine::AudioClip* LoadAudioClip(GlobalNamespace::CustomPreviewBeatmapLevel* customPreviewBeatmapLevel);
            
            static inline GlobalNamespace::BeatmapDataLoader* beatmapLevelLoader = nullptr;

            
    };
}