#include "SongLoader.hpp"

ModInfo SongLoader::Loader::modInfo;
GlobalNamespace::BeatmapLevelPackCollectionSO* SongLoader::Loader::customPackCollection;
std::map<std::string, GlobalNamespace::CustomPreviewBeatmapLevel*> SongLoader::Loader::previewCache;

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
        //dispatch refresh songs as an async thread, letting the unity thread continueing loading the game
        //std::thread refreshSongs([]{RefreshSongs();});
        //refreshSongs.detach();
        //auto fuck = std::async(std::launch::async, []{RefreshSongs();});
    }

    void Loader::MenuLoaded()
    {
        getLogger().info("Menu Loaded called");
        if (AreSongsLoading)
        {
            cancelLoading = true;
            AreSongsLoading = false;
            LoadingProgress = 0;
            getLogger().error("Menu loading was called while songs were loading, cancelling load");
            return;
        }
        //std::thread refreshSongs([]{RefreshSongs();});
        //refreshSongs.detach();
        RefreshSongs(false);
    }

    void Loader::RefreshSongs(bool fullRefresh)
    {
        getLogger().info("Refreshing Songs");
        if (AreSongsLoading)
        { 
            getLogger().error("Refresh songs called while already loading songs, skipping...");
            return;
        }
        AreSongsLoading = true;
        LoadingProgress = 0;
        AreSongsLoaded = false;
        loadingCancelled = false;

        RetrieveAllSongs(fullRefresh);
    }
    
    void Loader::RetrieveAllSongs(bool fullRefresh)
    {
        if (fullRefresh)
        {
            // if a full refresh is done things need to be cleared
            previewCache.clear();
        }
        // Get all folders in the song Directory
        std::vector<std::string> folders;
        Utils::File::GetDirectoriesInDir(customSongPath, folders);

        // Setup custom Characteristics (Lawless and Lightshow)
        SetupCharacteristics();

        for (auto folder : folders)
        {
            if (cancelLoading)
            {   
                cancelLoading = false;
                AreSongsLoading = false;
                return;
            }

            // for each folder there's gonna be a path and a hash, and each folder contains 1 song
            std::string path = string_format(SONG_PATH_FORMAT, folder.c_str());
            std::string hash;
            // load the custom level
            GlobalNamespace::CustomPreviewBeatmapLevel* level = LoadSong(path, hash);
            // if not in the cache already, add it
            if (!previewCache.contains(hash)) previewCache.insert_or_assign(hash, level);

            LoadingProgress += 0.8 / folders.size();
        }

        // Create custom Song pack out of the loaded data, and add it to the game
        CreatePack();
        AddPack();
        getLogger().info("All Songs loaded!");
    }

    void Loader::SetupCharacteristics()
    {
        //get base game characteristics
        //create own 2 characteristics -> names and images and perhaps can even do display names? (probably something with localization?)
        //put all together in an array
        //use that to get the characteristics at runtime

        GlobalNamespace::BeatmapCharacteristicCollectionSO* tempCharCollection = (GlobalNamespace::BeatmapCharacteristicCollectionSO*)CRASH_UNLESS(Utils::Unity::GetFirstObjectOfType(il2cpp_utils::GetClassFromName("", "BeatmapCharacteristicCollectionSO")));
        GlobalNamespace::BeatmapCharacteristicSO* lawless = UnityEngine::ScriptableObject::CreateInstance<GlobalNamespace::BeatmapCharacteristicSO*>();
        GlobalNamespace::BeatmapCharacteristicSO* lightshow = UnityEngine::ScriptableObject::CreateInstance<GlobalNamespace::BeatmapCharacteristicSO*>();

        /*
        std::vector<char> lawlessAsChar = readbytes("/sdcard/Pictures/Lawless.png");
        std::vector<uint8_t> lawlessBytesVector;
        
        for (auto c : lawlessAsChar)
        {
            lawlessBytesVector.push_back(c);
        }

        Array<uint8_t>* lawlessBytes = il2cpp_utils::vectorToArray(lawlessBytesVector);
        
        UnityEngine::Texture2D* lawlessTexture = UnityEngine::Texture2D::New_ctor(500, 500);
        UnityEngine::ImageConversion::LoadImage(lawlessTexture, lawlessBytes, false);
        UnityEngine::Rect lawlessRect = tempCharCollection->beatmapCharacteristics->values[0]->icon->get_rect();
        UnityEngine::Vector2 lawlessPivot = tempCharCollection->beatmapCharacteristics->values[0]->icon->get_pivot(); 
        float lawlessPixelsPerUnit = tempCharCollection->beatmapCharacteristics->values[0]->icon->get_pixelsPerUnit();
        uint lawlessExtrude = 1;
        UnityEngine::SpriteMeshType lawlessMeshType = 1;
        UnityEngine::Sprite* lawlessSprite = UnityEngine::Sprite::Create(lawlessTexture, lawlessRect, lawlessPivot, lawlessPixelsPerUnit, lawlessExtrude, lawlessMeshType, UnityEngine::Vector4::get_zero(), true);
        
        std::vector<char> lightshowAsChar = readbytes("/sdcard/Pictures/Lightshow.png");
        std::vector<uint8_t> lightshowBytesVector;
        
        for (auto c : lightshowAsChar)
        {
            lightshowBytesVector.push_back(c);
        }

        Array<uint8_t>* lightshowBytes = il2cpp_utils::vectorToArray(lightshowBytesVector);

        UnityEngine::Texture2D* lightshowTexture = UnityEngine::Texture2D::New_ctor(512, 512);
        UnityEngine::ImageConversion::LoadImage(lightshowTexture, lightshowBytes, false);
        UnityEngine::Rect lightshowRect = tempCharCollection->beatmapCharacteristics->values[0]->icon->get_rect();
        UnityEngine::Vector2 lightshowPivot = tempCharCollection->beatmapCharacteristics->values[0]->icon->get_pivot(); 
        float lightshowPixelsPerUnit = tempCharCollection->beatmapCharacteristics->values[0]->icon->get_pixelsPerUnit();
        uint lightshowExtrude = 1;
        UnityEngine::SpriteMeshType lightshowMeshType = 1;
        UnityEngine::Sprite* lightshowSprite = UnityEngine::Sprite::Create(lightshowTexture, lightshowRect, lightshowPivot, lightshowPixelsPerUnit, lightshowExtrude, lightshowMeshType, UnityEngine::Vector4::get_zero(), true);
        
        */
        lawless->serializedName = il2cpp_utils::createcsstr("Lawless");
        lightshow->serializedName = il2cpp_utils::createcsstr("Lightshow");
        /*
        lawless->icon = lawlessSprite;
        lightshow->icon = lightshowSprite;
        */
        /*
        Utils::Texture("sdcard/Pictures/Lawless.png", lawless);

        Utils::Texture("sdcard/Pictures/Lightshow.png", lightshow);
        */
        int originalLength = tempCharCollection->get_beatmapCharacteristics()->Length();
        Array<GlobalNamespace::BeatmapCharacteristicSO*>* editedArray = reinterpret_cast<Array<GlobalNamespace::BeatmapCharacteristicSO*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "BeatmapCharacteristicSO"), originalLength + 2));

        for (int i = 0; i < originalLength; i++)
        {
            editedArray->values[i] = tempCharCollection->get_beatmapCharacteristics()->values[i];
        }

        editedArray->values[originalLength] = lawless;
        editedArray->values[originalLength + 1] = lightshow;

        tempCharCollection->beatmapCharacteristics = editedArray;
        Loader::beatmapCharacteristicCollection = tempCharCollection;
        LoadingProgress = 0.1;
    }

    void Loader::CreatePack()
    {
        // getting references to beatmaplevels model and alwaysownedcontentcontainer, the levels model is where all the available songs are stored, the content container is where the user owned content is stored, this is where wee add our level IDs and pack IDs so that we can always play the added songs
        GlobalNamespace::BeatmapLevelsModel* beatmapLevelsModel = (GlobalNamespace::BeatmapLevelsModel*)Utils::Unity::GetFirstObjectOfType(il2cpp_utils::GetClassFromName("", "BeatmapLevelsModel"));
        GlobalNamespace::AlwaysOwnedContentContainerSO* alwaysOwnedContentContainer = (GlobalNamespace::AlwaysOwnedContentContainerSO*)Utils::Unity::GetFirstObjectOfType(il2cpp_utils::GetClassFromName("", "AlwaysOwnedContentContainerSO"));
        
        if (beatmapLevelsModel == nullptr) 
        {
            // if not defined it's useless to continue
            getLogger().error("beatmapLevelsModel was null, error in creating custom level pack");
            return;
        }

        if (alwaysOwnedContentContainer == nullptr) 
        {
            // if not defined it's useless to continue
            getLogger().error("alwaysOwnedContentContainer was null, error in creating custom level pack");
            return;
        }
        
        // load all the beatmaps from preview cache into an array to make the beatmaplevelcollection
        Array<GlobalNamespace::CustomPreviewBeatmapLevel*>* customlevels = reinterpret_cast<Array<GlobalNamespace::CustomPreviewBeatmapLevel*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "CustomPreviewBeatmapLevel"), previewCache.size()));
        int i = 0;
        for (auto pair : previewCache)
        {
            getLogger().info("Adding %s to custom levels", to_utf8(csstrtostr(pair.second->get_songName())).c_str());
            // our levels are the second values in the pairs of the previewCache map
            customlevels->values[i] = pair.second;
            // add to always owned IDs, that way it won't ask us to buy the song
            alwaysOwnedContentContainer->alwaysOwnedBeatmapLevelIds->Add(pair.second->get_levelID());
            i++;
        }
        // make the beatmaplevelcollection
        GlobalNamespace::CustomBeatmapLevelCollection* constructedCollection = GlobalNamespace::CustomBeatmapLevelCollection::New_ctor(customlevels);
        // construct new pack to add to the custom pack collection later on
        GlobalNamespace::CustomBeatmapLevelPack* constructedPack = GlobalNamespace::CustomBeatmapLevelPack::New_ctor(
            il2cpp_utils::createcsstr("custom_levelpack_whatever"),
            il2cpp_utils::createcsstr("Custom Levels"),
            il2cpp_utils::createcsstr("Custom Levels"),
            beatmapLevelsModel->ostAndExtrasPackCollection->beatmapLevelPacks->values[3]->get_coverImage(),
            constructedCollection);

        // create new pack collection
        GlobalNamespace::BeatmapLevelPackCollectionSO* tempCollection = UnityEngine::ScriptableObject::CreateInstance<GlobalNamespace::BeatmapLevelPackCollectionSO*>();
        // initialize the arrays in the new instance
        tempCollection->beatmapLevelPacks = reinterpret_cast<Array<GlobalNamespace::BeatmapLevelPackSO*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "BeatmapLevelPackSO"), 1));
        tempCollection->previewBeatmapLevelPack = reinterpret_cast<Array<GlobalNamespace::PreviewBeatmapLevelPackSO*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "PreviewBeatmapLevelPackSO"), 1));
        tempCollection->allBeatmapLevelPacks = reinterpret_cast<Array<GlobalNamespace::IBeatmapLevelPack*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "IBeatmapLevelPack"), 1));
        
        // set the values on the packs with a cast to their specific datatypes
        tempCollection->beatmapLevelPacks->values[0] = reinterpret_cast<GlobalNamespace::BeatmapLevelPackSO *>(constructedPack);
        tempCollection->previewBeatmapLevelPack->values[0] = reinterpret_cast<GlobalNamespace::PreviewBeatmapLevelPackSO*>(constructedPack);
        tempCollection->allBeatmapLevelPacks->values[0] = reinterpret_cast<GlobalNamespace::IBeatmapLevelPack*>(constructedPack);

        // add our pack ID to the always owned pack IDs so that the game will not try to make us buy the songs
        alwaysOwnedContentContainer->alwaysOwnedPacksIds->Add(tempCollection->beatmapLevelPacks->values[0]->get_packID());

        // store pointer for later use in AddPack
        customPackCollection = tempCollection;   
        
        //Increasing Load progress
        LoadingProgress += 0.1;
    }
    
    void Loader::AddPack()
    {
        // Getting reference to beatmaplevelsmodel because that is where the pack needs to be added
        GlobalNamespace::BeatmapLevelsModel* beatmapLevelsModel = (GlobalNamespace::BeatmapLevelsModel*)Utils::Unity::GetFirstObjectOfType(il2cpp_utils::GetClassFromName("", "BeatmapLevelsModel"));

        if (beatmapLevelsModel == nullptr) 
        {
            // if it's not defined it's useless to continue
            getLogger().error("beatmapLevelsModel was null, error in adding custom level pack");
            return;
        }
        // setting the custom pack collection
        beatmapLevelsModel->customLevelPackCollection = reinterpret_cast<GlobalNamespace::IBeatmapLevelPackCollection*>(Loader::customPackCollection);

        // idk why I run this but kyle said so
        beatmapLevelsModel->UpdateAllLoadedBeatmapLevelPacks();
        beatmapLevelsModel->UpdateLoadedPreviewLevels();

        // Bools fixed
        LoadingProgress = 1;
        AreSongsLoading = false;
        AreSongsLoaded = true;
        packAdded = true;
    }

    GlobalNamespace::CustomPreviewBeatmapLevel* Loader::LoadSong(std::string songPath, std::string& hash)
    {
        // get info.dat as data that the game can use
        GlobalNamespace::StandardLevelInfoSaveData* saveData = GetStandardLevelInfoSaveData(songPath);
        getLogger().info("Loading song Called, loading %s", to_utf8(csstrtostr(saveData->songName)).c_str());
        
        // declaring result pointer ahead of time
        GlobalNamespace::CustomPreviewBeatmapLevel* result;
        
        // Generating hash from savedata and songPath
        hash = Utils::Hashing::GetCustomLevelHash(saveData, songPath);

        // Setting all the various data points for the constructor later
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

        // Getting the environment info for the regular and 360 environments
        GlobalNamespace::EnvironmentInfoSO* environmentSceneInfo = LoadEnvironmentInfo(saveData->get_environmentName(), false);
        GlobalNamespace::EnvironmentInfoSO* allDirectionEnvironmentInfo = LoadEnvironmentInfo(saveData->get_allDirectionsEnvironmentName(), true);

        // Declaring and initializing the difficulty set list
        Array<GlobalNamespace::PreviewDifficultyBeatmapSet*>* diffList = reinterpret_cast<Array<GlobalNamespace::PreviewDifficultyBeatmapSet*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "PreviewDifficultyBeatmapSet"), saveData->get_difficultyBeatmapSets()->Length()));
        
        // for each difficulty beatmapset get its characteristic and accompanying characteristics
        for (int i = 0; i < saveData->get_difficultyBeatmapSets()->Length(); i++)
        {
            // get set from savedata
            GlobalNamespace::StandardLevelInfoSaveData::DifficultyBeatmapSet* difficultyBeatmapSet = saveData->get_difficultyBeatmapSets()->values[i];

            // get characteristic by it's serialized name
            GlobalNamespace::BeatmapCharacteristicSO* beatmapCharacteristicBySerializedName = beatmapCharacteristicCollection->GetBeatmapCharacteristicBySerializedName(difficultyBeatmapSet->get_beatmapCharacteristicName());
            
            // if it's not a nullptr (doesn't exist), do the rest
            if (beatmapCharacteristicBySerializedName != nullptr)
            {   
                // declare and initalize a beatmapdiff array to use to make the set later
                Array<GlobalNamespace::BeatmapDifficulty>* array = reinterpret_cast<Array<GlobalNamespace::BeatmapDifficulty>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "BeatmapDifficulty"), difficultyBeatmapSet->get_difficultyBeatmaps()->Length()));
                
                // for each diff in the set, do this
                for (int j = 0; j < difficultyBeatmapSet->get_difficultyBeatmaps()->Length(); j++)
                {
                    // make a difficulty
                    GlobalNamespace::BeatmapDifficulty beatmapDifficulty;
                    // get it's name 
                    std::string diffname = to_utf8(csstrtostr(difficultyBeatmapSet->get_difficultyBeatmaps()->values[j]->get_difficulty()));
                    // get it's diff from the name
                    BeatmapDifficultyFromSerializedName(diffname, beatmapDifficulty);
                    // add it to the diff array we made just before
                    array->values[j] = beatmapDifficulty;
                }

                // for each set we get the name of the characteristic, and we add the difficulties that it has to it
                diffList->values[i] = GlobalNamespace::PreviewDifficultyBeatmapSet::New_ctor(beatmapCharacteristicBySerializedName, array);
            }
        }
        
        // if we did not already have a media loader do this
        if (Loader::cachedMediaAsyncLoaderSO == nullptr)
        {
            // try to find a pre existing media loader
            GlobalNamespace::CachedMediaAsyncLoader* temp = (GlobalNamespace::CachedMediaAsyncLoader*)Utils::Unity::GetFirstObjectOfType(il2cpp_utils::GetClassFromName("", "CachedMediaAsyncLoader"));

            // if we found one, that's great! set our own media loader to that. Else we create a new loader and use that instead in the preview constructor
            Loader::cachedMediaAsyncLoaderSO = (temp == nullptr) ? GlobalNamespace::CachedMediaAsyncLoader::New_ctor() : temp;
        }
        // modifying song path to be better and not fail the load later
        std::string modifiedPath = songPath;
        modifiedPath.erase(0,1);

        // now we actually can construct the result
        result = GlobalNamespace::CustomPreviewBeatmapLevel::New_ctor(
                    UnityEngine::Texture2D::get_whiteTexture(),
                    saveData,
                    il2cpp_utils::createcsstr(modifiedPath),
                    reinterpret_cast<GlobalNamespace::IMediaAsyncLoader*>(Loader::cachedMediaAsyncLoaderSO), // The cast is neccesary, otherwise it will complain. Also the reinterpret is there because otherwise it will crash
                    reinterpret_cast<GlobalNamespace::IMediaAsyncLoader*>(Loader::cachedMediaAsyncLoaderSO),
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
        return result;
    }

    GlobalNamespace::StandardLevelInfoSaveData* Loader::GetStandardLevelInfoSaveData(std::string path)
    {
        // try to find Info.dat in the specified path, if it doesn't exist then it must be info.dat
        std::string newPath = path + "/Info.dat";
        if (!fileexists(path)) newPath = path + "info.dat";

        // read the text in the file (it's really just a json)
        std::string info = readfile(newPath);

        // deserialize the json data with the game's own method for it
        return GlobalNamespace::StandardLevelInfoSaveData::DeserializeFromJSONString(il2cpp_utils::createcsstr(info));
    }

    GlobalNamespace::EnvironmentInfoSO* Loader::LoadEnvironmentInfo(Il2CppString* environmentName, bool allDirections)
    {
        // declare result pointer ahead of time
        GlobalNamespace::EnvironmentInfoSO* result;
        
        // find an instance of environment list to question it about environments and their names
        GlobalNamespace::EnvironmentsListSO* environmentSceneInfoCollection = (GlobalNamespace::EnvironmentsListSO*)CRASH_UNLESS(Utils::Unity::GetFirstObjectOfType(il2cpp_utils::GetClassFromName("", "EnvironmentsListSO")));

        // get result from the name
        result = environmentSceneInfoCollection->GetEnviromentInfoBySerializedName(environmentName);

        return result;
    }

    bool Loader::BeatmapDifficultyFromSerializedName(std::string name, GlobalNamespace::BeatmapDifficulty &difficulty)
    {
        // Implementation of the base game method, done in cpp to make it easier or whatever

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
}
