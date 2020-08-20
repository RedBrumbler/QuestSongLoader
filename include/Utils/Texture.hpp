#pragma once
#include <dlfcn.h>
#include "../extern/beatsaber-hook/shared/utils/utils.h"
#include "../extern/beatsaber-hook/shared/utils/logging.hpp"
#include "../extern/modloader/shared/modloader.hpp"
#include "../extern/beatsaber-hook/shared/utils/typedefs.h"
#include "../extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "../extern/beatsaber-hook/shared/utils/il2cpp-functions.hpp"
//Adapted from rugtveit's audio loader in https://github.com/Rugtveit/QuestSounds
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/Texture.hpp"
#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/Networking/UnityWebRequest.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector4.hpp"
#include "UnityEngine/Rect.hpp"
#include "UnityEngine/SpriteMeshType.hpp"
#include "UnityEngine/TextureFormat.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "UnityEngine/Networking/DownloadHandlerTexture.hpp"

typedef std::function<void(UnityEngine::Texture*)> TextureCallback;

namespace Utils
{
    class Texture : Il2CppObject
    {
        public:
            /// @brief constructs new Texture loader that immediately starts loading
            /// @param filePath path to the image file to read from
            /// @param spriteToSet pointer to the sprite pointer that needs to be set
            Texture(std::string filePath, GlobalNamespace::BeatmapCharacteristicSO* characteristic)//, TextureCallback callback)
            {
                this->filePath = filePath;
                this->characteristic = characteristic;
                this->load();
            };

            GlobalNamespace::BeatmapCharacteristicSO* characteristic;

            /// @brief activated on first menu load
            static void OnLoad(ModInfo modInfo);

            //static Logger logger;
            std::string filePath;
            UnityEngine::Texture* texture;
            //UnityEngine::Texture2D* getImage();
            bool loaded = false;
            void load();
        private: 
            static ModInfo modInfo;
            static const Logger& getLogger();
            
            Il2CppObject* TextureAsync = nullptr;
            Il2CppObject* TextureRequest;
            static void TextureCompleted(Texture* obj, Il2CppObject* asyncOp);
    };
}