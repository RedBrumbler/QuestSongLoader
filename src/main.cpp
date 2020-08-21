#include "../include/mod_interface.hpp"
#include <unordered_set>
#include "../extern/beatsaber-hook/shared/utils/utils.h"
#include "../extern/beatsaber-hook/shared/utils/logging.hpp"
#include "../extern/modloader/shared/modloader.hpp"
#include "../extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp" 
#include "../extern/beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "../extern/beatsaber-hook/shared/utils/typedefs.h"
#include "../extern/beatsaber-hook/shared/config/config-utils.hpp"
#include <chrono>
#include <thread>
#include "Utils/File.hpp"
#include "SongLoader.hpp"

#define SONG_PATH "/sdcard/BeatSaberSongs/%s"
#define FILE_PATH "/sdcard/BeatSaberSongs/%s/%s"

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
#include "GlobalNamespace/AdditionalContentModel.hpp"
#include "UnityEngine/ScriptableObject.hpp"
#include "System/Collections/Generic/HashSet_1.hpp"
#include "System/Uri.hpp"
#include "System/UriKind.hpp"
#include "GlobalNamespace/FileHelpers.hpp"
#include "UnityEngine/Networking/UnityWebRequest.hpp"
#include "LevelLoader.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "GlobalNamespace/HMCache_2.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "HMUI/ViewController.hpp"
#include "HMUI/ViewController_ActivationType.hpp"
#include "UnityEngine/UI/Toggle.hpp"
#include "HMUI/ToggleBinder.hpp"
#include "GlobalNamespace/PlayerData.hpp"
#include "GlobalNamespace/LevelParamsPanel.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "GlobalNamespace/BeatmapLevelDataExtensions.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSegmentedControlController.hpp"
#include "GlobalNamespace/BeatmapDifficultySegmentedControlController.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Object.hpp"
#include "GlobalNamespace/PlayerLevelStatsData.hpp"
#include "Polyglot/Localization.hpp"
#include "GlobalNamespace/RankModel.hpp"
#include "GlobalNamespace/LevelCollectionViewController.hpp"
#include "GlobalNamespace/StandardLevelDetailViewController.hpp"
#include "GlobalNamespace/PlayerDataModel.hpp"
#include <thread>

//#include "GlobalNamespace/"
static ModInfo modInfo;
bool customsExist = false;
static const Logger& getLogger() {
    static const Logger logger(modInfo);
    return logger;
}

const static inline std::string menuViewControllers = "MenuViewControllers"; 
const static inline std::string menuCore = "MenuCore"; 
const static inline std::string healthAndSafety = "HealthWarning";
bool firstMenuActivation = true;
static inline SongLoader::Loader* loader = nullptr;

bool hasupdated = false;
GlobalNamespace::BeatmapLevelPackCollectionSO* customPackCollection;
void makeFolder()
{
    std::string path = string_format(SONG_PATH, "");
    if (!direxists(path.c_str()))
    {
        getLogger().info("Song directory: \"" + path + "\" did not exist, attempting to create it");
        int makePath = mkpath(path.data(), 0700);
        if (makePath == -1)
        {
            getLogger().debug("Failed to make path!");
        }
    }
    else
    {
        getLogger().info("Song directory: \"" + path + "\" already exists!");
    }
}

bool isCustomSong(std::string_view levelID)
{
	if (levelID.rfind("custom_level_", 0) == 0) {
		getLogger().info("%s is custom song!", levelID.data());
		return true;
	}
    else return false;
}

MAKE_HOOK_OFFSETLESS(SceneManager_ActiveSceneChanged, void, Scene previousActiveScene, Scene nextActiveScene)
{
    Il2CppString* activeSceneName = CRASH_UNLESS(il2cpp_utils::RunMethod<Il2CppString*>(il2cpp_utils::GetClassFromName("UnityEngine.SceneManagement", "Scene"), "GetNameInternal", nextActiveScene.m_Handle));
    std::string activeSceneStr  = to_utf8(csstrtostr(activeSceneName));
    getLogger().info("Found active scene: " + activeSceneStr);
    
    //first menu load -> onload on loader
    if (activeSceneStr == menuViewControllers && !firstMenuActivation)
    {
        SongLoader::Loader::MenuLoaded();
    }

    if (firstMenuActivation && activeSceneStr == menuViewControllers)
    {
        loader = new SongLoader::Loader();
        firstMenuActivation = false;
    }
    
    SceneManager_ActiveSceneChanged(previousActiveScene, nextActiveScene);

    

}

MAKE_HOOK_OFFSETLESS(LevelFilteringNavigationController_Setup, void, Il2CppObject *self, bool hideIfOneOrNoPacks, bool enableCustomLevels, Il2CppObject *annotatedBeatmapLevelCollectionToBeSelectedAfterPresent) {
    LevelFilteringNavigationController_Setup(self, hideIfOneOrNoPacks, true, annotatedBeatmapLevelCollectionToBeSelectedAfterPresent);
}

MAKE_HOOK_OFFSETLESS(BeatmapLevelsModel_Start, void, GlobalNamespace::BeatmapLevelsModel *self)
{
    //getLogger().info("BeatMaplevelsmodelstart called");
    if (!customsExist)
    {
        BeatmapLevelsModel_Start(self);
        return;
    }
    else BeatmapLevelsModel_Start(self);
}

MAKE_HOOK_OFFSETLESS(AnnotatedBeatmapLevelCollectionsViewController_SetData, void, Il2CppObject* self, Array<Il2CppObject*>* annotatedBeatmapLevelCollections, int selectedItemIndex, bool hideIfOneOrNoPacks)
{
    //getLogger().info("AnnotatedBeatmapLevelCollectionsViewControllerSetData called");
    AnnotatedBeatmapLevelCollectionsViewController_SetData(self, annotatedBeatmapLevelCollections, selectedItemIndex, hideIfOneOrNoPacks);
}

MAKE_HOOK_OFFSETLESS(LevelListTableCell_SetDataFromLevelAsync, void, Il2CppObject* self, GlobalNamespace::IPreviewBeatmapLevel* mainlevel, bool isFavorite)
{
    //getLogger().info("LevelListTableCellSetDataFromLevelAsync called on level: %s", to_utf8(csstrtostr(mainlevel->get_songName())).c_str());
    LevelListTableCell_SetDataFromLevelAsync(self, mainlevel, isFavorite);
}

MAKE_HOOK_OFFSETLESS(LevelFilteringNavigationController_InitializeIfNeeded, void, GlobalNamespace::LevelFilteringNavigationController* self)
{   
    //self->customSongsListNeedsReload = true;
    GlobalNamespace::TabBarViewController* tabBarViewController = self->tabBarViewController;
    LevelFilteringNavigationController_InitializeIfNeeded(self);
    if (!SongLoader::Loader::packAdded) return;
    
    GlobalNamespace::BeatmapLevelsModel* beatMapLevelsModel = (GlobalNamespace::BeatmapLevelsModel*)Utils::Unity::GetFirstObjectOfType(il2cpp_utils::GetClassFromName("", "BeatmapLevelsModel"));
    int numberOfPacks = beatMapLevelsModel->customLevelPackCollection->get_beatmapLevelPacks()->Length();
    Array<GlobalNamespace::IAnnotatedBeatmapLevelCollection*>* levelCollections = reinterpret_cast<Array<GlobalNamespace::IAnnotatedBeatmapLevelCollection*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "IAnnotatedBeatmapLevelCollection"), numberOfPacks));
    
    getLogger().info("found %d levelcollections", numberOfPacks);
    
    for (int i = 0; i < numberOfPacks; i++)//(auto var : customPackCollection->beatmapLevelPacks)
    {
        levelCollections->values[i] = beatMapLevelsModel->customLevelPackCollection->get_beatmapLevelPacks()->values[i];
    }

    self->customLevelsTabBarData->annotatedBeatmapLevelCollections = levelCollections;
}

MAKE_HOOK_OFFSETLESS(LevelFilteringNavigationController_TabBarDidSwitch, void, GlobalNamespace::LevelFilteringNavigationController* self)
{
    /*
        What it does base game:
        new local var tabbardata to store the selected cell numbers data in

        set data on annotatedbeatmaplevelcollectionsviewcontroller with the annotatedbeatmaplevelcollections, the selected item and a bool hide if one or no packs

        send event if needed to tabBarData.selectedAnnotatedBeatmapLevelCollections if beatmapcollection was selected
    */
   

   //getLogger().info("TabBarDidSwitch Called!");
   //getLogger().info("tabbardata null: %d", self->tabBarDatas == nullptr);
   //getLogger().info("tabbarviewcontroller null: %d", self->tabBarViewController == nullptr);
   //getLogger().info("annotatedbeatmaplevelcollectionviewcontroller null: %d", self->annotatedBeatmapLevelCollectionsViewController == nullptr);
    LevelFilteringNavigationController_TabBarDidSwitch(self);
}

MAKE_HOOK_OFFSETLESS(System_URI_CreateThis, void, System::Uri* self, Il2CppString* uri, bool dontEscape, System::UriKind uriKind)
{
    //getLogger().info("tried making uri out of: %s", to_utf8(csstrtostr(uri)).c_str());

    System_URI_CreateThis(self, uri, dontEscape, uriKind);
}

MAKE_HOOK_OFFSETLESS(FileHelpers_GetEscapedURLForFilePath, Il2CppString*, Il2CppString* filePath)
{
    //getLogger().info("GetEscapedURLforFilePath Called, string is %d", filePath==nullptr);
    std::string file = "file:///";

    std::string path = to_utf8(csstrtostr(filePath));
    //getLogger().info("Getting escaped URL for %s", path.c_str());

    std::string escapedString = path;//to_utf8(csstrtostr(UnityEngine::Networking::UnityWebRequest::EscapeURL(filePath)));//FileHelpers_GetEscapedURLForFilePath(filePath);
    
    //getLogger().info("escaped URI was: %s", (file + escapedString).c_str());

    return il2cpp_utils::createcsstr(file + escapedString);
}

MAKE_HOOK_OFFSETLESS(LevelFilteringNavigationController_ReloadSongListIfNeeded, void, GlobalNamespace::LevelFilteringNavigationController* self)
{
    //getLogger().info("ReloadSongListIfNeeded Called, this.distartloadingevent is null: %d", self->didStartLoadingEvent == nullptr);
    //getLogger().info("this.annotatedbeatmaplevelcollectionsviewcontroller is null: %d", self->annotatedBeatmapLevelCollectionsViewController == nullptr);
    LevelFilteringNavigationController_ReloadSongListIfNeeded(self);
}

MAKE_HOOK_OFFSETLESS(LevelFilteringNavigationController_SwitchWithReloadIfNeeded, void, GlobalNamespace::LevelFilteringNavigationController* self)
{
    //getLogger().info("SwitchWithReload Called, this.distartloadingevent is null: %d", self->didStartLoadingEvent == nullptr);
    //getLogger().info("this.annotatedbeatmaplevelcollectionsviewcontroller is null: %d", self->annotatedBeatmapLevelCollectionsViewController == nullptr);
    LevelFilteringNavigationController_SwitchWithReloadIfNeeded(self);
}

MAKE_HOOK_OFFSETLESS(LevelFilteringNavigationController_UpdateCustomSongs, void, GlobalNamespace::LevelFilteringNavigationController* self)
{
    /*
    getLogger().info("CancellationTokenSource null: %d", self->cancellationTokenSource == nullptr);
    getLogger().info("beatmaplevelsmodel null: %d", self->beatmapLevelsModel == nullptr);
    getLogger().info("customlevelstabbardata null: %d", self->customLevelsTabBarData == nullptr);
    getLogger().info("customlevelstabbardata->annotatedbeatmaplevelcollections null: %d", self->customLevelsTabBarData->annotatedBeatmapLevelCollections == nullptr);
    getLogger().info("tabbardatas null: %d", self->tabBarDatas == nullptr);
    getLogger().info("tabbarviewcontroller null: %d", self->tabBarViewController == nullptr);
    
    
        what it does base game:
        local var cancellationtokensource from this.canceltoken

        if source == null -> cancel

        make a new tokensource
        set local token var to the token of the new source
        try
        {
            get custom level pack collection (all custom maps) with cancel token
            if cancel was requested throw error so part of this is skipped

            if no error was thrown continue

            if the current tab bar data == the selected cell data -> execute tabbardidswitch

            else if current playlist tabbar data == selected cell -> execute tabbardidswitch

            if error was thrown, do nothing

            finally
                this.cancellationtokensource = null;
        }
    
    getLogger().info("End of UpdateCustomSongs called!");
    
    
    GlobalNamespace::BeatmapLevelsModel* beatMapLevelsModel = (GlobalNamespace::BeatmapLevelsModel*)Utils::Unity::GetFirstObjectOfType(il2cpp_utils::GetClassFromName("", "BeatmapLevelsModel"));
    GlobalNamespace::AlwaysOwnedContentContainerSO* contentContainer = (GlobalNamespace::AlwaysOwnedContentContainerSO*)Utils::Unity::GetFirstObjectOfType(il2cpp_utils::GetClassFromName("", "AlwaysOwnedContentContainerSO"));
    if (beatMapLevelsModel == nullptr) return;
    
    getLogger().info("Getting OST pack collection");
    GlobalNamespace::BeatmapLevelPackCollectionSO* ost = beatMapLevelsModel->get_ostAndExtrasPackCollection();

    auto levels = *SongLoader::Loader::GetLevels();
    getLogger().info("creating array out of loaded songs");
    Array<GlobalNamespace::CustomPreviewBeatmapLevel*>* customlevels = reinterpret_cast<Array<GlobalNamespace::CustomPreviewBeatmapLevel*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "CustomPreviewBeatmapLevel"), levels.size()));
    
    int i = 0;
    getLogger().info("adding levels to array");
    for (auto level : levels)
    {
        getLogger().info("%s", to_utf8(csstrtostr(level->get_songName())).c_str());
        customlevels->values[i] = level;
        contentContainer->alwaysOwnedBeatmapLevelIds->Add(level->get_levelID());

        getLogger().info((level->get_imageAsyncLoader() == nullptr) ? "ImageLoader was null" : "ImageLoader was defined");
        getLogger().info((level->get_audioClipAsyncLoader() == nullptr) ? "AudioLoader was null" : "AudioLoader was defined");
        //getLogger().info("Difficulty nullptr: %d", level->get_previewDifficultyBeatmapSets()->values[0]->get_beatmapDifficulties()->values[0].get_value() == nullptr);//.value);
        i++;
    }

    getLogger().info("Constructing new collection");
    GlobalNamespace::CustomBeatmapLevelCollection* constructedCollection = GlobalNamespace::CustomBeatmapLevelCollection::New_ctor(customlevels);
    
    getLogger().info("constructing new pack");
    GlobalNamespace::CustomBeatmapLevelPack* constructedPack = GlobalNamespace::CustomBeatmapLevelPack::New_ctor(
            il2cpp_utils::createcsstr("custom_levelpack_whatever"),
            il2cpp_utils::createcsstr("myCustomPack"),
            il2cpp_utils::createcsstr("custom"),
            ost->beatmapLevelPacks->values[0]->get_coverImage(),
            constructedCollection);

    //getLogger().info("Pack created, appending to ost packs");

    int length = ost->get_beatmapLevelPacks()->Length();
    
    Array<GlobalNamespace::IBeatmapLevelPack *>* packArray = reinterpret_cast<Array<GlobalNamespace::IBeatmapLevelPack*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "IBeatmapLevelPack"), length + 1));
    int j = 0;
    for (j = 0; j < length; j++)//auto pack : *ost->get_beatmapLevelPacks())
    {
        auto pack = ost->get_beatmapLevelPacks()->values[j];
        packArray->values[j] = pack;
    }

    packArray->values[length] = constructedPack;

    Il2CppObject* tempost = (Il2CppObject*)ost;
    il2cpp_utils::SetFieldValue(tempost, "_beatmapLevelPacks", packArray);
    ost = (GlobalNamespace::BeatmapLevelPackCollectionSO*)tempost;
    
    getLogger().info("Pack Created, Appending to beatmaplevelmodel");
    length = 0;
    GlobalNamespace::BeatmapLevelPackCollectionSO* tempCollection = UnityEngine::ScriptableObject::CreateInstance<GlobalNamespace::BeatmapLevelPackCollectionSO*>();//(System::Type*)il2cpp_utils::GetSystemType(il2cpp_utils::GetClassFromName("", "BeatmapLevelPackCollectionSO")));//GlobalNamespace::BeatmapLevelPackCollectionSO::New_ctor();
    tempCollection->beatmapLevelPacks = reinterpret_cast<Array<GlobalNamespace::BeatmapLevelPackSO*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "BeatmapLevelPackSO"), length + 1));
    tempCollection->previewBeatmapLevelPack = reinterpret_cast<Array<GlobalNamespace::PreviewBeatmapLevelPackSO*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "PreviewBeatmapLevelPackSO"), length + 1));
    tempCollection->allBeatmapLevelPacks = reinterpret_cast<Array<GlobalNamespace::IBeatmapLevelPack*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "IBeatmapLevelPack"), length + 1));
    
    int k = 0;
    for (k = 0; k < length ; k++)
    {
        auto pack = ost->beatmapLevelPacks->values[k];
        tempCollection->beatmapLevelPacks->values[k] = pack;
        tempCollection->previewBeatmapLevelPack->values[k] = (GlobalNamespace::PreviewBeatmapLevelPackSO*)pack;
        tempCollection->allBeatmapLevelPacks->values[k] = (GlobalNamespace::IBeatmapLevelPack*)pack;
    }
    
    
    length = 0;
    tempCollection->beatmapLevelPacks->values[length] = (GlobalNamespace::BeatmapLevelPackSO *)constructedPack;
    tempCollection->previewBeatmapLevelPack->values[length] = (GlobalNamespace::PreviewBeatmapLevelPackSO*)constructedPack;
    tempCollection->allBeatmapLevelPacks->values[length] = (GlobalNamespace::IBeatmapLevelPack*)constructedPack;
    
    contentContainer->alwaysOwnedPacksIds->Add(tempCollection->beatmapLevelPacks->values[0]->get_packID());

    
    beatMapLevelsModel->customLevelPackCollection = (GlobalNamespace::IBeatmapLevelPackCollection*)tempCollection;

    
    Il2CppObject* tempmodel = (Il2CppObject*) beatMapLevelsModel;
    //beatMapLevelsModel->allLoadedBeatmapLevelPackCollection = tempCollection;
    il2cpp_utils::SetFieldValue(tempmodel, "_customLevelPackCollection", (GlobalNamespace::IBeatmapLevelPackCollection*)tempCollection);
    beatMapLevelsModel = (GlobalNamespace::BeatmapLevelsModel*)tempmodel;
    
    getLogger().info("Added songs to beatmaplevelsmodel");

    beatMapLevelsModel->UpdateAllLoadedBeatmapLevelPacks();
    beatMapLevelsModel->UpdateLoadedPreviewLevels();
    customPackCollection = tempCollection;
    hasupdated = true;
    */
    //getLogger().info("End of UpdateCustomSongs");
    LevelFilteringNavigationController_UpdateCustomSongs(self);
    
}

MAKE_HOOK_OFFSETLESS(AdditionalContentModel_GetLevelEntitlementStatusAsync, Il2CppObject*, GlobalNamespace::AdditionalContentModel* self, Il2CppString* levelID ,System::Threading::CancellationToken* token)
{
    
    //getLogger().info("Trying to get status of %s", to_utf8(csstrtostr(levelID)).c_str());

    //if (self->alwaysOwnedContentContainer->alwaysOwnedBeatmapLevelIds->Contains(levelID)) getLogger().info("This levelID was in the list!");
    //else getLogger().info("This levelID was not in the list!");

    return AdditionalContentModel_GetLevelEntitlementStatusAsync(self, levelID, token);
}

MAKE_HOOK_OFFSETLESS(BeatmapLevelsModel_GetBeatmapLevelAsync, Il2CppObject*, GlobalNamespace::BeatmapLevelsModel* self, Il2CppString* levelID, System::Threading::CancellationToken* token)
{
    getLogger().info("Called getbeatmaplevelAsync!");
    if (isCustomSong(to_utf8(csstrtostr(levelID))) && !self->loadedBeatmapLevels->IsInCache(levelID))
    {
        getLogger().info("LevelID before: %s", to_utf8(csstrtostr(levelID)).c_str());
        
        GlobalNamespace::StandardLevelDetailViewController* detailView = (GlobalNamespace::StandardLevelDetailViewController*)Utils::Unity::GetFirstObjectOfType(il2cpp_utils::GetClassFromName("", "StandardLevelDetailViewController"));

        //auto function = [](Il2CppString* levelID, GlobalNamespace::BeatmapLevelsModel* beatmapLevelsModel, GlobalNamespace::StandardLevelDetailViewController* detailView){//if not in cache, add to cache
        
        //detailView->ShowContent(GlobalNamespace::StandardLevelDetailViewController::ContentType::_get_Loading(), il2cpp_utils::createcsstr(""), 0.0, il2cpp_utils::createcsstr(""));
        
        getLogger().info("Getting custom level from loadedpreviewbeatmaplevels");
        
        std::string customLevelHash = to_utf8(csstrtostr(levelID));
        customLevelHash.erase(0, 13);

        GlobalNamespace::CustomPreviewBeatmapLevel* customLevel = reinterpret_cast<GlobalNamespace::CustomPreviewBeatmapLevel*>(SongLoader::Loader::previewCache.find(customLevelHash)->second);//self->loadedPreviewBeatmapLevels->get_Item(levelID));
        getLogger().info("Level %s was not in cache, testing if custom", to_utf8(csstrtostr(customLevel->songName)).c_str());
        GlobalNamespace::CustomBeatmapLevel* level = SongLoader::LevelLoader::LoadCustomBeatmapLevel(customLevel);
        
        if (level->beatmapLevelData != nullptr) self->loadedBeatmapLevels->PutToCache(levelID, reinterpret_cast<GlobalNamespace::IBeatmapLevel*>(level));
        getLogger().info("isInCache now: %d", self->loadedBeatmapLevels->IsInCache(levelID));
        //detailView->ShowContent(GlobalNamespace::StandardLevelDetailViewController::ContentType::_get_OwnedAndReady(), il2cpp_utils::createcsstr(""), 0.0, il2cpp_utils::createcsstr(""));
        /*
        if (to_utf8(csstrtostr(level->levelID)) == to_utf8(csstrtostr(detailView->previewBeatmapLevel->get_levelID())))
        {
            getLogger().info("Level Set");
            detailView->beatmapLevel = level;
        }
        getLogger().info(string_format("detailView->beatmapLevel is null: %d", detailView->beatmapLevel == nullptr));
        getLogger().info(string_format("detailView->playerDataModel->playerData->lastSelectedBeatmapDifficulty is: %d",  detailView->playerDataModel->playerData->lastSelectedBeatmapDifficulty));
        getLogger().info(string_format("detailView->playerDataModel->playerData->lastSelectedBeatmapCharacteristic is null: %d", detailView->playerDataModel->playerData->lastSelectedBeatmapCharacteristic == nullptr));
        getLogger().info(string_format("detailView->playerDataModel->playerData is null: %d",detailView->playerDataModel->playerData == nullptr));
        getLogger().info(string_format("detailView->showPlayerStats is: %d", detailView->showPlayerStats));
        //getLogger().info("is null: %d", == nullptr);

        getLogger().info("Setcontent from async");
        detailView->standardLevelDetailView->SetContent(reinterpret_cast<GlobalNamespace::IBeatmapLevel*>(detailView->beatmapLevel), detailView->playerDataModel->playerData->lastSelectedBeatmapDifficulty, detailView->playerDataModel->playerData->lastSelectedBeatmapCharacteristic, 
        detailView->playerDataModel->playerData, detailView->showPlayerStats);
        getLogger().info("ShowContent from async");
        detailView->ShowContent(GlobalNamespace::StandardLevelDetailViewController::ContentType::_get_OwnedAndReady(), il2cpp_utils::createcsstr(""), 0.0, il2cpp_utils::createcsstr(""));
        */
        
        
        //detailView->RefreshAvailabilityAsync();
        //};

        //std::thread addToCache(function, levelID, self, detailView);
        //addToCache.detach();
        detailView->RefreshAvailabilityAsync();
    }
    
    return BeatmapLevelsModel_GetBeatmapLevelAsync(self, levelID, token);
}

MAKE_HOOK_OFFSETLESS(StandardLevelDetailView_SetContent, void, GlobalNamespace::StandardLevelDetailView* self, GlobalNamespace::IBeatmapLevel* level, GlobalNamespace::BeatmapDifficulty defaultDifficulty, GlobalNamespace::BeatmapCharacteristicSO* defaultBeatmapCharacteristic, GlobalNamespace::PlayerData* playerdata, bool showPlayerStats)
{
    getLogger().info("SetContent Called");
    /*
    getLogger().info("level: %s", to_utf8(csstrtostr(level->get_songName())).c_str());
    getLogger().info("beatmapleveldata is null: %d", level->get_beatmapLevelData() == nullptr);
    getLogger().info("defaultDiff: %d", defaultDifficulty.value);
    getLogger().info("defaultChar: %s", to_utf8(csstrtostr(defaultBeatmapCharacteristic->get_name())).c_str());
    getLogger().info("playerdata null: %d", playerdata == nullptr);
    */
    StandardLevelDetailView_SetContent(self, level, defaultDifficulty, defaultBeatmapCharacteristic, playerdata, showPlayerStats);
}

MAKE_HOOK_OFFSETLESS(StandardLevelDetailViewController_ShowContent, void, GlobalNamespace::StandardLevelDetailViewController* self, GlobalNamespace::StandardLevelDetailViewController::ContentType contentType, Il2CppString* errorText, float downloadingProgress, Il2CppString* downloadingText)
{
    //if (contentType.value == 5) contentType.value = 0;
    getLogger().info("ShowContent called");
    StandardLevelDetailViewController_ShowContent(self, contentType, errorText, downloadingProgress, downloadingText);
}

MAKE_HOOK_OFFSETLESS(StandardLevelDetailView_RefreshContent, void, GlobalNamespace::StandardLevelDetailView* self)
{
        getLogger().info("RefreshContent called");
        if (self->level == nullptr || self->level->get_beatmapLevelData() == nullptr)
		{
			return;
		}
        //getLogger().info("audio clip is null: %d", self->level->get_beatmapLevelData()->get_audioClip() == nullptr);
        float songLength = (self->level->get_beatmapLevelData()->get_audioClip() == nullptr) ? 260.0f : self->level->get_beatmapLevelData()->get_audioClip()->get_length();
		self->toggleBinder->Disable();
		self->favoriteToggle->set_isOn(self->playerData->IsLevelUserFavorite(self->level));
		self->toggleBinder->Enable();
		self->selectedDifficultyBeatmap = GlobalNamespace::BeatmapLevelDataExtensions::GetDifficultyBeatmap(self->level->get_beatmapLevelData(), self->beatmapCharacteristicSegmentedControlController->selectedBeatmapCharacteristic, self->beatmapDifficultySegmentedControlController->selectedDifficulty);
		self->songNameText->set_text(self->level->get_songName());
		self->SetTextureAsync(self->level);
		self->levelParamsPanel->set_duration(songLength);//);
		self->levelParamsPanel->set_bpm(self->level->get_beatsPerMinute());
		self->levelParamsPanel->set_notesPerSecond((float)self->selectedDifficultyBeatmap->get_beatmapData()->notesCount / songLength);//self->level->get_beatmapLevelData()->get_audioClip()->get_length());
		self->levelParamsPanel->set_notesCount(self->selectedDifficultyBeatmap->get_beatmapData()->notesCount);
		self->levelParamsPanel->set_obstaclesCount(self->selectedDifficultyBeatmap->get_beatmapData()->obstaclesCount);
		self->levelParamsPanel->set_bombsCount(self->selectedDifficultyBeatmap->get_beatmapData()->bombsCount);
		if (self->playerStatsContainer)
		{
			if (self->showPlayerStats && self->playerData != nullptr)
			{
				self->playerStatsContainer->SetActive(true);
				GlobalNamespace::PlayerLevelStatsData* playerLevelStatsData = self->playerData->GetPlayerLevelStatsData(self->level->get_levelID(), self->selectedDifficultyBeatmap->get_difficulty(), self->selectedDifficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic());
				Il2CppString* highscore = il2cpp_utils::createcsstr("-");
                if (playerLevelStatsData->get_validScore())
                {
                    highscore = il2cpp_utils::createcsstr(string_format("%d", playerLevelStatsData->highScore));
                }

                self->highScoreText->set_text(highscore);
				Il2CppString* text =  playerLevelStatsData->fullCombo ? Polyglot::Localization::Get(il2cpp_utils::createcsstr("STATS_FULL_COMBO"))->ToUpper() : il2cpp_utils::createcsstr(string_format("%d", playerLevelStatsData->maxCombo));
				self->maxComboText->set_text(playerLevelStatsData->validScore ? text : il2cpp_utils::createcsstr("-"));
				self->maxRankText->set_text(playerLevelStatsData->validScore ? GlobalNamespace::RankModel::GetRankName(playerLevelStatsData->maxRank) : il2cpp_utils::createcsstr("-"));
			}
			else
			{
				self->playerStatsContainer->SetActive(false);
			}
		}
		self->separator->SetActive(!self->playerStatsContainer->get_activeSelf());
}

MAKE_HOOK_OFFSETLESS(LevelCollectionViewController_SongPlayerCrossfadeToLevelAsync, void, GlobalNamespace::LevelCollectionViewController* self, GlobalNamespace::IPreviewBeatmapLevel* level)
{
    //getLogger().info("Song Crossfade called");
    LevelCollectionViewController_SongPlayerCrossfadeToLevelAsync(self, level);
}
    /*
MAKE_HOOK_OFFSETLESS(StandardLevelDetailView_DidActivate, void, GlobalNamespace::StandardLevelDetailView* self, bool firstActivation, HMUI::ViewController::ActivationType activationType)
{
    getLogger().info("StandardLevelDetailView Activated!");
    
    StandardLevelDetailView_DidActivate(self, firstActivation, activationType);
}
*/
/*
MAKE_HOOK_OFFSETLESS(CustomDifficultyBeatmap_New_ctor, void, GlobalNamespace::CustomDifficultyBeatmap* self, GlobalNamespace::IBeatmapLevel* level, GlobalNamespace::IDifficultyBeatmapSet* parentDifficultyBeatmapSet, GlobalNamespace::BeatmapDifficulty difficulty, int difficultyRank, float noteJumpMovementSpeed, float noteJumpStartBeatOffset, GlobalNamespace::BeatmapData* beatmapData)
{
    getLogger().info("Constructor called!");
    getLogger().info("parentlevel: %d", level == nullptr);
	getLogger().info("parentdiff: %d", parentDifficultyBeatmapSet == nullptr);
	getLogger().info("diff: %d", difficulty.value);
	getLogger().info("diffRank: %d", difficultyRank);
	getLogger().info("NJS: %f", noteJumpMovementSpeed);
	getLogger().info("N offset: %f", noteJumpStartBeatOffset);
	getLogger().info("beatdata: %d", beatmapData == nullptr);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    CustomDifficultyBeatmap_New_ctor(self, level, parentDifficultyBeatmapSet, difficulty, difficultyRank, noteJumpMovementSpeed, noteJumpStartBeatOffset, beatmapData);
}
*/

extern "C" void setup(ModInfo& info) {

    info.id = "SongLoader";
    info.version = "0.1.0";
    modInfo = info;
    SongLoader::Loader::OnLoad(modInfo);
    SongLoader::LevelLoader::OnLoad(modInfo);
    Utils::Hashing::OnLoad(modInfo);
    Utils::File::OnLoad(modInfo);
    Utils::Audio::OnLoad(modInfo);
    Utils::Texture::OnLoad(modInfo);
    getLogger().info("Modloader name: %s", Modloader::getInfo().name.c_str());
}

extern "C" void load() {
    getLogger().debug("Hello from il2cpp_init!");
    getLogger().debug("Installing hooks...");
    makeFolder();
    il2cpp_functions::Init();

    INSTALL_HOOK_OFFSETLESS(SceneManager_ActiveSceneChanged, il2cpp_utils::FindMethodUnsafe("UnityEngine.SceneManagement", "SceneManager", "Internal_ActiveSceneChanged", 2));
    INSTALL_HOOK_OFFSETLESS(LevelFilteringNavigationController_Setup, il2cpp_utils::FindMethodUnsafe("", "LevelFilteringNavigationController", "Setup", 3));
    INSTALL_HOOK_OFFSETLESS(BeatmapLevelsModel_Start, il2cpp_utils::FindMethodUnsafe("", "BeatmapLevelsModel", "Start", 0));
    INSTALL_HOOK_OFFSETLESS(AnnotatedBeatmapLevelCollectionsViewController_SetData, il2cpp_utils::FindMethodUnsafe("", "AnnotatedBeatmapLevelCollectionsViewController", "SetData", 3));
    INSTALL_HOOK_OFFSETLESS(LevelListTableCell_SetDataFromLevelAsync, il2cpp_utils::FindMethodUnsafe("", "LevelListTableCell", "SetDataFromLevelAsync", 2));
    //INSTALL_HOOK_OFFSETLESS(BeatMapLevelsModel_GetCustomLevelPackCollectionAsync, il2cpp_utils::FindMethodUnsafe("", "BeatMapLevelsModel", "GetCustomLevelPackCollectionAsync", 1));
    INSTALL_HOOK_OFFSETLESS(LevelFilteringNavigationController_UpdateCustomSongs, il2cpp_utils::FindMethodUnsafe("", "LevelFilteringNavigationController", "UpdateCustomSongs", 0));
    INSTALL_HOOK_OFFSETLESS(LevelFilteringNavigationController_TabBarDidSwitch, il2cpp_utils::FindMethodUnsafe("", "LevelFilteringNavigationController", "TabBarDidSwitch", 0));
    INSTALL_HOOK_OFFSETLESS(LevelFilteringNavigationController_InitializeIfNeeded, il2cpp_utils::FindMethodUnsafe("", "LevelFilteringNavigationController", "InitializeIfNeeded", 0));
    INSTALL_HOOK_OFFSETLESS(LevelFilteringNavigationController_ReloadSongListIfNeeded, il2cpp_utils::FindMethodUnsafe("", "LevelFilteringNavigationController", "ReloadSongListIfNeeded", 0));
    INSTALL_HOOK_OFFSETLESS(LevelFilteringNavigationController_SwitchWithReloadIfNeeded, il2cpp_utils::FindMethodUnsafe("", "LevelFilteringNavigationController", "SwitchWithReloadIfNeeded", 0));
    INSTALL_HOOK_OFFSETLESS(System_URI_CreateThis, il2cpp_utils::FindMethodUnsafe("System", "Uri", "CreateThis", 3));
    INSTALL_HOOK_OFFSETLESS(FileHelpers_GetEscapedURLForFilePath, il2cpp_utils::FindMethodUnsafe("", "FileHelpers", "GetEscapedURLForFilePath", 1));
    INSTALL_HOOK_OFFSETLESS(AdditionalContentModel_GetLevelEntitlementStatusAsync, il2cpp_utils::FindMethodUnsafe("", "AdditionalContentModel", "GetLevelEntitlementStatusAsync", 2));
    INSTALL_HOOK_OFFSETLESS(BeatmapLevelsModel_GetBeatmapLevelAsync, il2cpp_utils::FindMethodUnsafe("", "BeatmapLevelsModel", "GetBeatmapLevelAsync", 2));
    //INSTALL_HOOK_OFFSETLESS(CustomDifficultyBeatmap_New_ctor, il2cpp_utils::FindMethodUnsafe("", "CustomDifficultyBeatmap", ".ctor", 7));
    INSTALL_HOOK_OFFSETLESS(StandardLevelDetailView_SetContent, il2cpp_utils::FindMethodUnsafe("", "StandardLevelDetailView", "SetContent", 5));
    INSTALL_HOOK_OFFSETLESS(StandardLevelDetailViewController_ShowContent, il2cpp_utils::FindMethodUnsafe("", "StandardLevelDetailViewController", "ShowContent", 4));
    INSTALL_HOOK_OFFSETLESS(StandardLevelDetailView_RefreshContent, il2cpp_utils::FindMethodUnsafe("", "StandardLevelDetailView", "RefreshContent", 0));
    INSTALL_HOOK_OFFSETLESS(LevelCollectionViewController_SongPlayerCrossfadeToLevelAsync, il2cpp_utils::FindMethodUnsafe("", "LevelCollectionViewController", "SongPlayerCrossfadeToLevelAsync", 1));
    getLogger().debug("Installed all hooks!");
}