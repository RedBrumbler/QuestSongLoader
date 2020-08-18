#include "SongLoader.hpp"

ModInfo SongLoader::Loader::modInfo;
std::vector<GlobalNamespace::CustomPreviewBeatmapLevel*> SongLoader::Loader::previewBeatMaps;
GlobalNamespace::BeatmapLevelPackCollectionSO* SongLoader::Loader::customPackCollection;
std::map<std::string, GlobalNamespace::CustomPreviewBeatmapLevel*> SongLoader::Loader::previewCache;
std::map<std::string, Utils::Audio*> SongLoader::Loader::soundLoader;
namespace SongLoader
{
    const Logger& Loader::getLogger() 
    {
        static const Logger logger(modInfo);
        return logger;
    }

    void Loader::OnLoad(ModInfo modInfo)
    {
        Loader::modInfo = modInfo;
        Loader::modInfo.id += "::SongLoader";
    }

    Loader::Loader()
    {
        getLogger().info("Loader Instantiated");
        MenuLoaded();
        RefreshSongs();
    }

    void Loader::MenuLoaded()
    {
        getLogger().info("MenuLoaded Called");
        if (AreSongsLoading)
        {
            //Songs are loading and the menu load was called again? this could cause crashes and we should stop loading

            cancelLoading = true;
            AreSongsLoading = false;
            LoadingProgress = 0;
            getLogger().error("Song loading was cancelled by player becasue they loaded a new scene");
        }
    }
    
    void Loader::RefreshSongs(bool fullRefresh)
    {
        getLogger().info("RefreshSongs Called");
        if (AreSongsLoading) return;

        AreSongsLoading = true;
        AreSongsLoaded = false;
        LoadingProgress = 0;
        loadingCancelled = false;

        RetrieveAllSongs(fullRefresh);
    }
    
    
    void Loader::RetrieveAllSongs(bool fullrefresh)
    {
        getLogger().info("RetrieveAllSongs Called");
        if (fullrefresh)
        {
            //clear all the things
        }


        //Load Customs
        std::vector<std::string> songFolders;
        Utils::File::GetDirectoriesInDir(customSongPath, songFolders);

        int i = 0;
        for (auto dir : songFolders)
        {
            i++;
            GlobalNamespace::StandardLevelInfoSaveData* saveData = GetStandardLevelInfoSaveData(string_format(FILE_PATH_FORMAT, dir.c_str(), "Info.dat"));
            std::string hash;
            GlobalNamespace::CustomPreviewBeatmapLevel* customLevel = LoadSong(saveData, string_format(SONG_PATH_FORMAT, dir.c_str()), hash);
            getLogger().info("Pushing Back level");
            previewBeatMaps.push_back(customLevel);
            getLogger().info("End of %s", dir.c_str());
            
            if (!previewCache.contains(hash))
            {
                previewCache.insert_or_assign(hash, customLevel);
            }

            LoadSound(string_format(FILE_PATH_FORMAT, dir.c_str(), to_utf8(csstrtostr(saveData->songFilename)).c_str()), hash);
        }
        
        for (auto level : previewBeatMaps)
        {
            getLogger().info("Loaded: %s CustomBeatmapLevel", to_utf8(csstrtostr(level->get_songName())).c_str());
        }
        CreatePack();
        AddPack();
    }

    void Loader::LoadSound(std::string filePath, std::string hash)
    {
        getLogger().info("Loadsound from %s", filePath.c_str());
        soundLoader.insert_or_assign(hash, new Utils::Audio());
        soundLoader.find(hash)->second->filePath = filePath;
        soundLoader.find(hash)->second->songHash = hash;
        soundLoader.find(hash)->second->load();
    }

    void Loader::CreatePack()
    {
        getLogger().info("Creating Pack");
        GlobalNamespace::BeatmapLevelsModel* beatmapLevelsModel = (GlobalNamespace::BeatmapLevelsModel*)Utils::Unity::GetFirstObjectOfType(il2cpp_utils::GetClassFromName("", "BeatmapLevelsModel"));
        GlobalNamespace::AlwaysOwnedContentContainerSO* alwaysOwnedContentContainer = (GlobalNamespace::AlwaysOwnedContentContainerSO*)Utils::Unity::GetFirstObjectOfType(il2cpp_utils::GetClassFromName("", "AlwaysOwnedContentContainerSO"));
        if (beatmapLevelsModel == nullptr) 
        {
            getLogger().error("beatmapLevelsModel was null, error in creating custom level pack");
            return;
        }

        if (alwaysOwnedContentContainer == nullptr) 
        {
            getLogger().error("alwaysOwnedContentContainer was null, error in creating custom level pack");
            return;
        }

        auto levels = *Loader::GetLevels();
        
        Array<GlobalNamespace::CustomPreviewBeatmapLevel*>* customlevels = reinterpret_cast<Array<GlobalNamespace::CustomPreviewBeatmapLevel*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "CustomPreviewBeatmapLevel"), levels.size()));        

        int i = 0;
        for (auto level : levels)
        {
            customlevels->values[i] = level;
            alwaysOwnedContentContainer->alwaysOwnedBeatmapLevelIds->Add(level->get_levelID());
            i++;
        }
        
        GlobalNamespace::CustomBeatmapLevelCollection* constructedCollection = GlobalNamespace::CustomBeatmapLevelCollection::New_ctor(customlevels);

        GlobalNamespace::CustomBeatmapLevelPack* constructedPack = GlobalNamespace::CustomBeatmapLevelPack::New_ctor(
            il2cpp_utils::createcsstr("custom_levelpack_whatever"),
            il2cpp_utils::createcsstr("myCustomPack"),
            il2cpp_utils::createcsstr("custom"),
            beatmapLevelsModel->ostAndExtrasPackCollection->beatmapLevelPacks->values[0]->get_coverImage(),
            constructedCollection);

        GlobalNamespace::BeatmapLevelPackCollectionSO* tempCollection = UnityEngine::ScriptableObject::CreateInstance<GlobalNamespace::BeatmapLevelPackCollectionSO*>();
        tempCollection->beatmapLevelPacks = reinterpret_cast<Array<GlobalNamespace::BeatmapLevelPackSO*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "BeatmapLevelPackSO"), 1));
        tempCollection->previewBeatmapLevelPack = reinterpret_cast<Array<GlobalNamespace::PreviewBeatmapLevelPackSO*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "PreviewBeatmapLevelPackSO"), 1));
        tempCollection->allBeatmapLevelPacks = reinterpret_cast<Array<GlobalNamespace::IBeatmapLevelPack*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "IBeatmapLevelPack"), 1));
        
        tempCollection->beatmapLevelPacks->values[0] = reinterpret_cast<GlobalNamespace::BeatmapLevelPackSO *>(constructedPack);
        tempCollection->previewBeatmapLevelPack->values[0] = reinterpret_cast<GlobalNamespace::PreviewBeatmapLevelPackSO*>(constructedPack);
        tempCollection->allBeatmapLevelPacks->values[0] = reinterpret_cast<GlobalNamespace::IBeatmapLevelPack*>(constructedPack);

        alwaysOwnedContentContainer->alwaysOwnedPacksIds->Add(tempCollection->beatmapLevelPacks->values[0]->get_packID());

        customPackCollection = tempCollection;
        
    }
    
    void Loader::AddPack()
    {
        getLogger().info("Adding Pack");
        GlobalNamespace::BeatmapLevelsModel* beatmapLevelsModel = (GlobalNamespace::BeatmapLevelsModel*)Utils::Unity::GetFirstObjectOfType(il2cpp_utils::GetClassFromName("", "BeatmapLevelsModel"));

        if (beatmapLevelsModel == nullptr) 
        {
            getLogger().error("beatmapLevelsModel was null, error in creating custom level pack");
            return;
        }

        getLogger().info("Getting original length");
        int arrayLength = beatmapLevelsModel->ostAndExtrasPackCollection->get_beatmapLevelPacks()->Length();
        
        getLogger().info("Making new collection");
        GlobalNamespace::BeatmapLevelPackCollectionSO* ost = UnityEngine::ScriptableObject::CreateInstance<GlobalNamespace::BeatmapLevelPackCollectionSO*>();
        
        getLogger().info("Setting array length");
        ost->beatmapLevelPacks = reinterpret_cast<Array<GlobalNamespace::BeatmapLevelPackSO*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "BeatmapLevelPackSO"), arrayLength + 1));
        
        getLogger().info("setting all packs into array");
        for (int i = 0; i < arrayLength; i++)
        { 
            GlobalNamespace::BeatmapLevelPackSO* pack = beatmapLevelsModel->ostAndExtrasPackCollection->beatmapLevelPacks->values[i];
            getLogger().info("Adding %s", to_utf8(csstrtostr(pack->get_packID())).c_str());
            ost->beatmapLevelPacks->values[i] = pack;
        }

        getLogger().info("Adding custom pack");
        ost->beatmapLevelPacks->values[arrayLength] = Loader::customPackCollection->beatmapLevelPacks->values[0];

        getLogger().info("setting customlevelpackcollection");
        beatmapLevelsModel->customLevelPackCollection = reinterpret_cast<GlobalNamespace::IBeatmapLevelPackCollection*>(Loader::customPackCollection);
        
        getLogger().info("setting ost back");
        beatmapLevelsModel->ostAndExtrasPackCollection = ost;

        getLogger().info("run some methods");
        beatmapLevelsModel->UpdateAllLoadedBeatmapLevelPacks();
        beatmapLevelsModel->UpdateLoadedPreviewLevels();

        getLogger().info("End of add pack");
        packAdded = true; 
    }

    GlobalNamespace::CustomPreviewBeatmapLevel* Loader::LoadSong(GlobalNamespace::StandardLevelInfoSaveData* saveData, std::string songPath, std::string& hash)
    {

        getLogger().info("LoadSong Called, loading %s", songPath.c_str());
        GlobalNamespace::CustomPreviewBeatmapLevel* result;

        getLogger().info("Getting Hash");
        hash = Utils::Hashing::GetCustomLevelHash(saveData, songPath + "/Info.dat");
        getLogger().info("Got Hash");

        Il2CppString* levelID = il2cpp_utils::createcsstr("custom_level_" + hash);
        Il2CppString* songName = saveData->get_songName();
        Il2CppString* songSubName = saveData->get_songSubName();
        Il2CppString* songAuthorName = saveData->get_songAuthorName();
        Il2CppString* levelAuthorName = saveData->get_levelAuthorName();

        float beatsPerMinute = saveData->get_beatsPerMinute();
        float songTimeOffset = saveData->get_songTimeOffset();
        float shuffle = saveData->get_shuffle();
        float shufflePeriod = saveData->get_shufflePeriod();
        float previewStartTime = saveData->get_previewStartTime();
        float previewDuration = saveData->get_previewDuration();

        getLogger().info("Getting environmentScene info");
        GlobalNamespace::EnvironmentInfoSO* environmentSceneInfo = LoadEnvironmentInfo(saveData->get_environmentName(), false);
        GlobalNamespace::EnvironmentInfoSO* allDirectionEnvironmentInfo = LoadEnvironmentInfo(saveData->get_allDirectionsEnvironmentName(), true);
        getLogger().info("Gotten environmentSceneInfo");

        Array<GlobalNamespace::PreviewDifficultyBeatmapSet*>* diffList = reinterpret_cast<Array<GlobalNamespace::PreviewDifficultyBeatmapSet*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "PreviewDifficultyBeatmapSet"), saveData->get_difficultyBeatmapSets()->Length()));;

        Loader::beatmapCharacteristicCollection = (GlobalNamespace::BeatmapCharacteristicCollectionSO*)CRASH_UNLESS(Utils::Unity::GetFirstObjectOfType(il2cpp_utils::GetClassFromName("", "BeatmapCharacteristicCollectionSO")));
        
        
        for (int i = 0; i < saveData->get_difficultyBeatmapSets()->Length(); i++)
        {
            GlobalNamespace::StandardLevelInfoSaveData::DifficultyBeatmapSet* difficultyBeatmapSet = saveData->get_difficultyBeatmapSets()->values[i];
            GlobalNamespace::BeatmapCharacteristicSO* beatmapCharacteristicBySerializedName = beatmapCharacteristicCollection->GetBeatmapCharacteristicBySerializedName(difficultyBeatmapSet->get_beatmapCharacteristicName());
            if (beatmapCharacteristicBySerializedName != nullptr)
            {   
                Array<GlobalNamespace::BeatmapDifficulty>* array = reinterpret_cast<Array<GlobalNamespace::BeatmapDifficulty>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "BeatmapDifficulty"), difficultyBeatmapSet->get_difficultyBeatmaps()->Length()));
                getLogger().info("Found %d difficulties, parsing info", array->Length());
                for (int j = 0; j < difficultyBeatmapSet->get_difficultyBeatmaps()->Length(); j++)
                {
                    GlobalNamespace::BeatmapDifficulty beatmapDifficulty;
                    std::string diffname = to_utf8(csstrtostr(difficultyBeatmapSet->get_difficultyBeatmaps()->values[j]->get_difficulty()));
                    BeatmapDifficultyFromSerializedName(diffname, beatmapDifficulty);
                    array->values[j] = beatmapDifficulty;
                }
                getLogger().info("Before: serializedName: %s, array length: %d", to_utf8(csstrtostr(beatmapCharacteristicBySerializedName->get_serializedName())).c_str(), array->Length());
                diffList->values[i] = GlobalNamespace::PreviewDifficultyBeatmapSet::New_ctor(beatmapCharacteristicBySerializedName, array);
            }
        }
        
        getLogger().info("diffList length: %d", diffList->Length());
        
        if (Loader::cachedMediaAsyncLoaderSO == nullptr)
        {
            GlobalNamespace::CachedMediaAsyncLoader* temp = (GlobalNamespace::CachedMediaAsyncLoader*)Utils::Unity::GetFirstObjectOfType(il2cpp_utils::GetClassFromName("", "CachedMediaAsyncLoader"));
            getLogger().info((temp != nullptr) ? "Found a medialoader" : "Did not find a medialoader");

            getLogger().info("Making new media async loader");
            Loader::cachedMediaAsyncLoaderSO = (temp == nullptr) ? GlobalNamespace::CachedMediaAsyncLoader::New_ctor() : temp;
        }
        /*
        getLogger().info("checking cached media loader");
        if (Loader::cachedMediaAsyncLoaderSO == nullptr)
        {
            getLogger().info("Making new media async loader, getting reference to appcore installer");
            GlobalNamespace::AppCoreInstaller* appCoreInstaller = (GlobalNamespace::AppCoreInstaller*)Utils::Unity::GetFirstObjectOfType(il2cpp_utils::GetClassFromName("", "AppCoreInstaller"));
            getLogger().info("Instantiating media loader, appcore was null: %d", appCoreInstaller == nullptr);
            Loader::cachedMediaAsyncLoaderSO = UnityEngine::Object::Instantiate(appCoreInstaller->cachedMediaAsyncLoaderPrefab);
        }
        */
        std::string modifiedPath = songPath;
        modifiedPath.erase(0,1);

        getLogger().info("modified path: %s", modifiedPath.c_str());

        getLogger().info("Got media loader");
        getLogger().info("Generating result");
        result = GlobalNamespace::CustomPreviewBeatmapLevel::New_ctor(
                    UnityEngine::Texture2D::get_whiteTexture(),
                    saveData,
                    il2cpp_utils::createcsstr(modifiedPath),
                    /*(GlobalNamespace::IAudioClipAsyncLoader*)*/reinterpret_cast<GlobalNamespace::IMediaAsyncLoader*>(Loader::cachedMediaAsyncLoaderSO),
                    /*(GlobalNamespace::IImageAsyncLoader*)*/reinterpret_cast<GlobalNamespace::IMediaAsyncLoader*>(Loader::cachedMediaAsyncLoaderSO),
                    levelID,
                    songName,
                    songSubName,
                    songAuthorName,
                    levelAuthorName,
                    beatsPerMinute,
                    songTimeOffset,
                    shuffle,
                    shufflePeriod,
                    previewStartTime,
                    previewDuration,
                    environmentSceneInfo,
                    allDirectionEnvironmentInfo,
                    diffList);

        getLogger().info("End of LoadSong");
        return result;
    }

    GlobalNamespace::StandardLevelInfoSaveData* Loader::GetStandardLevelInfoSaveData(std::string path)
    {
        getLogger().info("GetStandardLevelInfoSaveData Called");

        std::string info = readfile(path);

        return GlobalNamespace::StandardLevelInfoSaveData::DeserializeFromJSONString(il2cpp_utils::createcsstr(info));
    }

    GlobalNamespace::EnvironmentInfoSO* Loader::LoadEnvironmentInfo(Il2CppString* environmentName, bool allDirections)
    {
        getLogger().info("LoadEnvironmentInfo Called, 360: %d", allDirections);
        GlobalNamespace::EnvironmentInfoSO* result;
        getLogger().info("Getting collection pointer");
        GlobalNamespace::EnvironmentsListSO* environmentSceneInfoCollection = (GlobalNamespace::EnvironmentsListSO*)CRASH_UNLESS(Utils::Unity::GetFirstObjectOfType(il2cpp_utils::GetClassFromName("", "EnvironmentsListSO")));
        getLogger().info("Getting result");
        result = environmentSceneInfoCollection->GetEnviromentInfoBySerializedName(environmentName);
        getLogger().info("returning result");
        return result;
    }

    bool Loader::BeatmapDifficultyFromSerializedName(std::string name, GlobalNamespace::BeatmapDifficulty &difficulty)
    {
        getLogger().info("BeatMapDifficultyFromSerializedName Called, trying to resolve: %s", name.c_str());

        if (name == "Easy")
		{
			difficulty = GlobalNamespace::BeatmapDifficulty::Easy;
			return true;
		}
		if (name == "Normal")
		{
			difficulty = GlobalNamespace::BeatmapDifficulty::Normal;
			return true;
		}
		if (name == "Hard")
		{
			difficulty = GlobalNamespace::BeatmapDifficulty::Hard;
			return true;
		}
		if (name == "Expert")
		{
			difficulty = GlobalNamespace::BeatmapDifficulty::Expert;
			return true;
		}
		if (name == "Expert+" || name == "ExpertPlus")
		{
			difficulty = GlobalNamespace::BeatmapDifficulty::ExpertPlus;
			return true;
		}
		difficulty = GlobalNamespace::BeatmapDifficulty::Normal;
		return false;
    }

    std::vector<GlobalNamespace::CustomPreviewBeatmapLevel*>* Loader::GetLevels()
    {
        return &Loader::previewBeatMaps;
    }
}
