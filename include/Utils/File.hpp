#pragma once
#include <dirent.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <fstream>
#include "../extern/beatsaber-hook/shared/utils/utils.h"
#include "../extern/beatsaber-hook/shared/utils/typedefs.h"
#include "../extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp" 
#include "../extern/beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "../extern/modloader/shared/modloader.hpp"

namespace Utils
{
    class File
    {
        public:

            static void OnLoad(ModInfo modInfo);
            
            static ModInfo modInfo;

            /// @brief gets all the dir names
            /// @param dir directory to check in
            /// @param out array to output to, only outputs the "local" names of the directories
            /// @return bool find at least 1
            static bool GetDirectoriesInDir(std::string dir, std::vector<std::string> &out);

            /// @brief gets the info.dat from the given directory, returns null if not found
            /// @param dir directory to search in
            /// @return contents of info.dat
            static std::string GetInfoFromDirectory(std::string dir);

            /// @brief reads all bytes from file
            /// @param path full file path
            /// @return Arrat of C# bytes
            static Array<uint8_t>* ReadAllBytes(std::string path);
        private:
            static const Logger& getLogger();
    };
}