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
#include "GlobalNamespace/BeatmapLevelsModel.hpp"

#include "GlobalNamespace/BeatmapLevelsModel.hpp"
#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/StandardLevelInfoSaveData.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/PreviewDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/BeatmapCharacteristicCollectionSO.hpp"
#include "GlobalNamespace/EnvironmentInfoSO.hpp"
#include "GlobalNamespace/EnvironmentsListSO.hpp"
#include "GlobalNamespace/StandardLevelInfoSaveData_DifficultyBeatmapSet.hpp"
#include "GlobalNamespace/StandardLevelInfoSaveData_DifficultyBeatmap.hpp"
#include "GlobalNamespace/CachedMediaAsyncLoader.hpp"
#include "GlobalNamespace/IImageAsyncLoader.hpp"
#include "GlobalNamespace/IAudioClipAsyncLoader.hpp"
#include "GlobalNamespace/IMediaAsyncLoader.hpp"
#include "GlobalNamespace/AppCoreInstaller.hpp"

#include "GlobalNamespace/BeatmapLevelsModel.hpp"
#include "GlobalNamespace/LevelFilteringNavigationController.hpp"
#include "GlobalNamespace/LevelFilteringNavigationController_TabBarData.hpp"
#include "GlobalNamespace/IBeatmapLevelPackCollection.hpp"
#include "GlobalNamespace//BeatmapLevelPackCollectionSO.hpp"
#include "System/Threading/Tasks/Task.hpp"
#include "GlobalNamespace/BeatmapLevelPackSO.hpp"
#include "GlobalNamespace/PreviewBeatmapLevelPackSO.hpp"
#include "GlobalNamespace/IBeatmapLevelPack.hpp"
#include "GlobalNamespace/BeatmapLevelsModel.hpp"
#include "GlobalNamespace/CustomBeatmapLevelCollection.hpp"
#include "GlobalNamespace/CustomBeatmapLevelPack.hpp"
#include "GlobalNamespace/IBeatmapLevelCollection.hpp"
#include "GlobalNamespace/BeatmapLevelCollectionSO.hpp"
#include "GlobalNamespace/IAnnotatedBeatmapLevelCollection.hpp"
#include "GlobalNamespace/AlwaysOwnedContentContainerSO.hpp"
#include "UnityEngine/ScriptableObject.hpp"
#include "System/Collections/Generic/HashSet_1.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"

#include "UnityEngine/Object.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/Object.hpp"
#include "Utils/File.hpp"
#include "Utils/Unity.hpp"
#include "Utils/Hashing.hpp"
#include "Utils/Audio.hpp"
#include <map>

#define SONG_PATH_FORMAT "/sdcard/BeatSaberSongs/%s"
#define FILE_PATH_FORMAT "/sdcard/BeatSaberSongs/%s/%s"

namespace SongLoader
{
    class Loader
    {
        public:
            // cache of loaded preview maps
            static std::map<std::string, GlobalNamespace::CustomPreviewBeatmapLevel*> previewCache;

            // cache to sound pointers
            static std::map<std::string, Utils::Audio*> soundLoader;

            /// @brief get a reference to the custompreviewbeatmaplevel* vector
            /// @return custompreviewbeatmaplevel* vector
            static std::vector<GlobalNamespace::CustomPreviewBeatmapLevel*>* GetLevels();

            /// @brief activated on first menu load
            static void OnLoad(ModInfo modInfo);

            /// @brief empty ctor
            Loader();

            /// @brief Runs each time the menu is loaded
            static void MenuLoaded();

            static ModInfo modInfo;

            static inline bool packAdded = false;

            /// @brief get the difficulty enum from the given name
            /// @param name Name to search for
            /// @param difficulty output difficulty variable
            /// @return bool succesfully found diff
            static bool BeatmapDifficultyFromSerializedName(std::string name, GlobalNamespace::BeatmapDifficulty &difficulty);

            
            static GlobalNamespace::BeatmapCharacteristicSO* GetBeatmapCharacteristicBySerializedName(Il2CppString* serializedName);
            
        private:

            static inline Array<GlobalNamespace::BeatmapCharacteristicSO*>* beatmapCharacteristics = nullptr;

            static void SetupCharacteristics();

            static const Logger& getLogger();

            /// @brief used to Refresh all the songs;
            /// @param fullRefresh used to determine wether or not a full refresh will be done
            static void RefreshSongs(bool fullRefresh = true);

            /// @brief Actually retrieves all songs
            /// @param fullRefresh determines full refresh or not
            static void RetrieveAllSongs(bool fullRefresh);

            /// @brief Loads a song
            /// @param saveData The data loaded from the info.dat of the song
            /// @param songPath the full folder path of the song
            /// @param hash the hash that gets written to
            /// @return beatmap preview object
            static GlobalNamespace::CustomPreviewBeatmapLevel* LoadSong(GlobalNamespace::StandardLevelInfoSaveData* saveData, std::string songPath, std::string& hash);
            
            /// @brief Loads a standardlevelinfosavedata from file, basically the info.dat in a class representation
            /// @param path the path to the info.dat
            /// @return C# object StandardLevelInfoSaveData
            static GlobalNamespace::StandardLevelInfoSaveData* GetStandardLevelInfoSaveData(std::string path);

            /// @brief own implementation of CustomLevelLoader.LoadEnvironmentInfo due to it not existing on quest
            /// @param environmentName desired environment name
            /// @param allDirections bool if 360 degrees
            /// @return EnvironmentInfo
            static GlobalNamespace::EnvironmentInfoSO* LoadEnvironmentInfo(Il2CppString* environmentName, bool allDirections);

            /// @brief creates pack out of the previewBeatMaps array generated by retrieve songs
            static void CreatePack();

            /// @brief adds created pack to the beatmaplevelsmodel and calls functions to update stuff
            static void AddPack();

            static inline std::string customSongPath = string_format(SONG_PATH_FORMAT, "");

            /// @brief Loads in all music files in 
            static void LoadSound(std::string filePath, std::string hash);
            
            static inline bool cancelLoading = false;
            static inline bool AreSongsLoading = false;
            static inline bool AreSongsLoaded = false;
            static inline bool loadingCancelled = false;
            static inline float LoadingProgress = 0;
            
            //Il2CppNamespace::Loader* loader;

            static std::vector<GlobalNamespace::CustomPreviewBeatmapLevel*> previewBeatMaps;
            static GlobalNamespace::BeatmapLevelPackCollectionSO* customPackCollection;
            static inline GlobalNamespace::BeatmapCharacteristicCollectionSO* beatmapCharacteristicCollection = nullptr;
            static inline GlobalNamespace::CachedMediaAsyncLoader* cachedMediaAsyncLoaderSO = nullptr;
    };
}