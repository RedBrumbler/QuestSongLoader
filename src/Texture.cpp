#include "include/Utils/Texture.hpp"
ModInfo Utils::Texture::modInfo;
//Adapted from rugtveits audio clip loader from https://github.com/Rugtveit/QuestSounds


void Utils::Texture::load()
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
        return;
    }

    //Stage 1
    Il2CppString* filePathStr = il2cpp_utils::createcsstr("file:///" + filePath);

    TextureRequest = CRASH_UNLESS(il2cpp_utils::RunMethod("UnityEngine.Networking", "UnityWebRequestTexture", "GetTexture", filePathStr));
    TextureAsync = CRASH_UNLESS(il2cpp_utils::RunMethod(TextureRequest, "SendWebRequest"));

    //Stage 2
    const MethodInfo* addCompletedMethod = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(TextureAsync, "add_completed", 1));
    auto action = CRASH_UNLESS(il2cpp_utils::MakeAction(addCompletedMethod, 0, this, TextureCompleted));
    //auto* action = CRASH_UNLESS(il2cpp_utils::MakeAction(addCompletedMethod, 0, new TextureCallback(callback), TextureCompleted));
    CRASH_UNLESS(il2cpp_utils::RunMethod(TextureAsync, addCompletedMethod, action));
}

void Utils::Texture::TextureCompleted(Texture* obj, Il2CppObject* asyncOp)
{   

    getLogger().info("Image loaded");
    auto* tex = CRASH_UNLESS(il2cpp_utils::RunMethod<UnityEngine::Texture*>("UnityEngine.Networking", "DownloadHandlerTexture", "GetContent", reinterpret_cast<UnityEngine::Networking::UnityWebRequest*>(obj->TextureRequest)));;//UnityEngine::Networking::DownloadHandlerTexture::GetContent(reinterpret_cast<UnityEngine::Networking::UnityWebRequest*>(textureRequest));

    // Stage 1
   
    getLogger().info("Creating tex2D from image");
    obj->texture = UnityEngine::Texture2D::CreateExternalTexture(tex->get_width(), tex->get_height(), UnityEngine::TextureFormat::ARGB32, false, false, tex->GetNativeTexturePtr());
    getLogger().info("generating sprite from texture data");
    UnityEngine::Texture2D* useableTexture = UnityEngine::Texture2D::CreateExternalTexture(obj->texture->get_width(), obj->texture->get_height(), UnityEngine::TextureFormat::ARGB32, false, false, obj->texture->GetNativeTexturePtr());

    UnityEngine::Rect rect = *UnityEngine::Rect::New_ctor(0, 0, 50, 50);//tempCharCollection->beatmapCharacteristics->values[0]->icon->get_rect();
    UnityEngine::Vector2 pivot = UnityEngine::Vector2::get_zero();//tempCharCollection->beatmapCharacteristics->values[0]->icon->get_pivot(); 
    float pixelsPerUnit = 50;//tempCharCollection->beatmapCharacteristics->values[0]->icon->get_pixelsPerUnit();
    uint extrude = 1;
    UnityEngine::SpriteMeshType meshType = 1;
    UnityEngine::Vector4 border = UnityEngine::Vector4::get_zero();//tempCharCollection->beatmapCharacteristics->values[0]->icon->get_border();
    obj->characteristic->icon = UnityEngine::Sprite::Create(useableTexture, rect, pivot, pixelsPerUnit, extrude, meshType, border, true);

    obj->loaded = true;
    // Finished
}

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

