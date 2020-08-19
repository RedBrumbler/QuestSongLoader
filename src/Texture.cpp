#include "include/Utils/Texture.hpp"
ModInfo Utils::Texture::modInfo;
//Adapted from rugtveits audio clip loader from https://github.com/Rugtveit/QuestSounds


bool Utils::Texture::load(TextureCallback callback)
{

    getLogger().info("Loading Texture File");
    getLogger().info("fileexists: %d", fileexists(filePath.c_str()));
    //TODO figure out how to get logging working here!
    //Stage 0 
    bool fileError = fileexists(filePath.c_str());
    bool error = (TextureAsync != nullptr || !fileError);
    if(error)
    {
        if(!fileError)
        {
            getLogger().error("this loader is already loading a file");
        } else 
        {
            getLogger().error("File did not exist");
        }
        return false;
    }

    //Stage 1
    Il2CppString* filePathStr = il2cpp_utils::createcsstr("file:///" + filePath);

    TextureRequest = CRASH_UNLESS(il2cpp_utils::RunMethod("UnityEngine.Networking", "UnityWebRequestTexture", "GetTexture", filePathStr));
    TextureAsync = CRASH_UNLESS(il2cpp_utils::RunMethod(TextureRequest, "SendWebRequest"));

    //Stage 2
    const MethodInfo* addCompletedMethod = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(TextureAsync, "add_completed", 1));
    auto* action = RET_0_UNLESS(il2cpp_utils::MakeAction(addCompletedMethod, 0, new TextureCallback(callback), TextureCompleted));
    CRASH_UNLESS(il2cpp_utils::RunMethod(TextureAsync, addCompletedMethod, action));
    return true;
}

void Utils::Texture::TextureCompleted(TextureCallback* callback, UnityEngine::Networking::UnityWebRequestAsyncOperation* textureRequest)
{
    getLogger().info("Image loaded");
    auto* texture = UnityEngine::Networking::DownloadHandlerTexture::GetContent(reinterpret_cast<UnityEngine::Networking::UnityWebRequest*>(textureRequest));
    getLogger().info("Calling Callback");
    (*callback)(texture);
}   
    /*// Stage 1
   
    getLogger().info("Creating tex2D from image");
    obj->texture = UnityEngine::Texture2D::CreateExternalTexture(tex->get_width(), tex->get_height(), UnityEngine::TextureFormat::ARGB32, false, false, tex->GetNativeTexturePtr());
    getLogger().info("generating sprite from texture data");
    UnityEngine::Rect rect;
    UnityEngine::Vector2 pivot;
    UnityEngine::Vector4 border;
    UnityEngine::Sprite* sprite = UnityEngine::Sprite::Create(obj->texture, rect, pivot, 20, 0, 0, border, true);
    getLogger().info("Setting sprite");
    obj->characteristic->icon = sprite;
    getLogger().info("Loaded sprite has been set");
    obj->loaded = true;
    // Finished
}
*/
/*
UnityEngine::Texture2D* Utils::Texture::getImage()
{
    if(texture != nullptr && loaded)
    {
        return texture;
    } else
    {
        return nullptr;
    }
}
*/
const Logger& Utils::Texture::getLogger() 
    {
        static const Logger logger(modInfo);
        return logger;
    }

    void Utils::Texture::OnLoad(ModInfo modInfo)
    {
        Utils::Texture::modInfo = modInfo;
        Utils::Texture::modInfo.id += "::Texture";
    }

