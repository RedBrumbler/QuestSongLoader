#pragma once
#include <dlfcn.h>
#include "../extern/beatsaber-hook/shared/utils/utils.h"
#include "../extern/beatsaber-hook/shared/utils/logging.hpp"
#include "../extern/modloader/shared/modloader.hpp"
#include "../extern/beatsaber-hook/shared/utils/typedefs.h"
#include "../extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "../extern/beatsaber-hook/shared/utils/il2cpp-functions.hpp"
//Adapted from rugtveit's audio loader in https://github.com/Rugtveit/QuestSounds

namespace Utils
{
    class Audio : Il2CppObject
    {
        public:

            /// @brief activated on first menu load
            static void OnLoad(ModInfo modInfo);

            //static Logger logger;
            std::string filePath;
            int audioType = 14;
            Il2CppObject* audioSource;
            Il2CppObject* audioClip;
            std::string songHash;
            Il2CppObject* getClip(); //Audioclip
            bool loaded = false;
            bool load();
        private: 
            static ModInfo modInfo;
            static const Logger& getLogger();
            Il2CppObject* audioClipAsync;
            Il2CppObject* audioClipRequest;
            static void audioClipCompleted(Audio* obj, Il2CppObject* asyncOp);
    };
}