#pragma once
#include "GlobalNamespace/StandardLevelInfoSaveData.hpp"
#include <string>
#include <map>
#include "../include/mod_interface.hpp"
#include <unordered_set>
#include "../extern/beatsaber-hook/shared/utils/utils.h"
#include "../extern/beatsaber-hook/shared/utils/logging.hpp"
#include "../extern/modloader/shared/modloader.hpp"
#include "../extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp" 
#include "../extern/beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "../extern/beatsaber-hook/shared/utils/typedefs.h"
#include "../extern/beatsaber-hook/shared/config/config-utils.hpp"
#include "System/Text/Encoding.hpp"
#include "System/Security/Cryptography/SHA1.hpp"
#include "System/BitConverter.hpp"
#include "System/IO/File.hpp"
#include "Utils/File.hpp"
#include "System/Collections/Generic/IEnumerable_1.hpp"
#include "GlobalNamespace/StandardLevelInfoSaveData_DifficultyBeatmapSet.hpp"
#include "GlobalNamespace/StandardLevelInfoSaveData_DifficultyBeatmap.hpp"
namespace Utils
{   
    class SongHashData
    {
        public:
            long directoryHash;
            std::string songHash;

            SongHashData()
            {
                this->directoryHash = 0;
                this->songHash = "default";
            }

            SongHashData(long directoryHash, std::string songHash)
            {
                this->directoryHash = directoryHash;
                this->songHash = songHash;
            }
    };

    class Hashing
    {
        public:
            //static std::map<std::string, Utils::SongHashData> cachedSongHashData;
            

            static void OnLoad(ModInfo modInfo);

            static ModInfo modInfo;
            
            /// @brief generate hash from leveldata and custom song path
            /// @param level leveldata of the song to generate hash for
            /// @param customLevelPath path to the custom level
            /// @return string hash
            static std::string GetCustomLevelHash(GlobalNamespace::StandardLevelInfoSaveData* level, std::string customLevelPath);
            
            /// @brief generates a string sha1 hash from input byte vector
            /// @param input byte vector input
            /// @return string sha1 hash
            static std::string CreateSha1FromBytes(Array<uint8_t>* input);
            
        private:

            static const Logger& getLogger();

            /// @brief runs through the std::map to retrieve the song's hash by it's path
            /// @param customLevelPath string customlevelpath to use to search with
            /// @param directoryHash output for dir hash
            /// @param cachedSongHash output for song hash
            /// @return success
            static bool GetCachedSongData(std::string customLevelPath, long &directoryHash, std::string &cachedSongHash);
    };

    

}