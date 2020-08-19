#include "LevelLoader.hpp"
ModInfo SongLoader::LevelLoader::modInfo;
namespace SongLoader
{

	const Logger& LevelLoader::getLogger() 
    {
        static const Logger logger(modInfo);
        return logger;
    }

    void LevelLoader::OnLoad(ModInfo modInfo)
    {
        LevelLoader::modInfo = modInfo;
        LevelLoader::modInfo.id += "::LevelLoader";
    }

    GlobalNamespace::CustomBeatmapLevel* LevelLoader::LoadCustomBeatmapLevel(GlobalNamespace::CustomPreviewBeatmapLevel* customPreviewBeatmapLevel)
    {
		getLogger().info("Begin LoadCustomBeatmapLevel");
        GlobalNamespace::StandardLevelInfoSaveData* standardLevelInfoSaveData = customPreviewBeatmapLevel->standardLevelInfoSaveData;
		Il2CppString* customLevelPath = customPreviewBeatmapLevel->customLevelPath;
		UnityEngine::Texture2D* coverImageTexture2D = customPreviewBeatmapLevel->coverImageTexture2D;
		std::string hash = "";
		hash += to_utf8(csstrtostr(customPreviewBeatmapLevel->levelID));
		hash.erase(0, 13);
		getLogger().info("Song Hash: %s", hash.c_str());
		

		if (customPreviewBeatmapLevel->previewAudioClip == nullptr) customPreviewBeatmapLevel->previewAudioClip = LoadAudioClip(customPreviewBeatmapLevel);//(UnityEngine::AudioClip*)SongLoader::Loader::soundLoader.find(hash)->second->getClip();
		UnityEngine::AudioClip* previewAudioClip = customPreviewBeatmapLevel->previewAudioClip;
		getLogger().info("Audio Clip should be loaded, isNull?: %d", previewAudioClip == nullptr);
		
		GlobalNamespace::CustomBeatmapLevel* customBeatmapLevel = GlobalNamespace::CustomBeatmapLevel::New_ctor(customPreviewBeatmapLevel, previewAudioClip, coverImageTexture2D);
		GlobalNamespace::BeatmapLevelData* beatmapLevelData = LevelLoader::LoadBeatmapLevelData(to_utf8(csstrtostr(customLevelPath)), customBeatmapLevel, standardLevelInfoSaveData, previewAudioClip);
		customBeatmapLevel->SetBeatmapLevelData(beatmapLevelData);
		getLogger().info("End LoadCustomBeatmapLevel");
		return customBeatmapLevel;
    }

    GlobalNamespace::BeatmapLevelData* LevelLoader::LoadBeatmapLevelData(std::string customLevelPath, GlobalNamespace::CustomBeatmapLevel* customBeatmapLevel, GlobalNamespace::StandardLevelInfoSaveData* standardLevelInfoSaveData, UnityEngine::AudioClip* previewAudioClip)
    {
		getLogger().info("Begin LoadBeatmapLevelData");
        Array<GlobalNamespace::IDifficultyBeatmapSet*>* array = LevelLoader::LoadDifficultyBeatmapSets(customLevelPath, customBeatmapLevel, standardLevelInfoSaveData);
		
        
        Array<GlobalNamespace::IDifficultyBeatmapSet*>* difficultyBeatmapSets = reinterpret_cast<Array<GlobalNamespace::IDifficultyBeatmapSet*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "IDifficultyBeatmapSet"), array->Length()));
		difficultyBeatmapSets = array;
        UnityEngine::AudioClip* audioClip = previewAudioClip;//(previewAudioClip == nullptr) ? (UnityEngine::AudioClip*)audioLoader->getClip() : previewAudioClip;
		GlobalNamespace::BeatmapLevelData* result;
		if (audioClip == nullptr)
		{
			getLogger().info("AudioClip was nullptr, returning nullptr");
			result = nullptr;
		}
		else
		{
			result = GlobalNamespace::BeatmapLevelData::New_ctor(audioClip, difficultyBeatmapSets);
		}
		getLogger().info("End LoadBeatmapLevelData");
		return result;
    }

    Array<GlobalNamespace::IDifficultyBeatmapSet*>* LevelLoader::LoadDifficultyBeatmapSets(std::string customLevelPath, GlobalNamespace::CustomBeatmapLevel* customBeatmapLevel, GlobalNamespace::StandardLevelInfoSaveData* standardLevelInfoSaveData)
    {
		getLogger().info("Begin LoadDifficultyBeatmapSetS");
        Array<GlobalNamespace::IDifficultyBeatmapSet*>* difficultyBeatmapSets = reinterpret_cast<Array<GlobalNamespace::IDifficultyBeatmapSet*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "IDifficultyBeatmapSet"), standardLevelInfoSaveData->difficultyBeatmapSets->Length()));
		int num;
		for (int i = 0; i < difficultyBeatmapSets->Length(); i++)
		{
			GlobalNamespace::IDifficultyBeatmapSet* difficultyBeatmapSet = LevelLoader::LoadDifficultyBeatmapSet(customLevelPath, customBeatmapLevel, standardLevelInfoSaveData, standardLevelInfoSaveData->difficultyBeatmapSets->values[i]);
			difficultyBeatmapSets->values[i] = difficultyBeatmapSet;
		}
		getLogger().info("End LoadDifficultyBeatmapSetS");
		return difficultyBeatmapSets;
    }

    GlobalNamespace::IDifficultyBeatmapSet* LevelLoader::LoadDifficultyBeatmapSet(std::string customLevelPath, GlobalNamespace::CustomBeatmapLevel* customBeatmapLevel, GlobalNamespace::StandardLevelInfoSaveData* standardLevelInfoSaveData, GlobalNamespace::StandardLevelInfoSaveData::DifficultyBeatmapSet* difficultyBeatmapSetSaveData)
    {
		getLogger().info("Begin LoadDifficultyBeatmapSet");
        GlobalNamespace::BeatmapCharacteristicCollectionSO* beatmapCharacteristicCollection = (GlobalNamespace::BeatmapCharacteristicCollectionSO*)CRASH_UNLESS(Utils::Unity::GetFirstObjectOfType(il2cpp_utils::GetClassFromName("", "BeatmapCharacteristicCollectionSO")));
        GlobalNamespace::BeatmapCharacteristicSO* beatmapCharacteristicBySerializedName = beatmapCharacteristicCollection->GetBeatmapCharacteristicBySerializedName(difficultyBeatmapSetSaveData->beatmapCharacteristicName);
		
        Array<GlobalNamespace::CustomDifficultyBeatmap*>* difficultyBeatmaps = reinterpret_cast<Array<GlobalNamespace::CustomDifficultyBeatmap*>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("", "CustomDifficultyBeatmap"), difficultyBeatmapSetSaveData->difficultyBeatmaps->Length()));
		GlobalNamespace::CustomDifficultyBeatmapSet* difficultyBeatmapSet = GlobalNamespace::CustomDifficultyBeatmapSet::New_ctor(beatmapCharacteristicBySerializedName);

		for (int i = 0; i < difficultyBeatmapSetSaveData->difficultyBeatmaps->Length(); i++)
		{
			GlobalNamespace::CustomDifficultyBeatmap* customDifficultyBeatmap = LevelLoader::LoadDifficultyBeatmap(customLevelPath, customBeatmapLevel, difficultyBeatmapSet, standardLevelInfoSaveData, difficultyBeatmapSetSaveData->difficultyBeatmaps->values[i]);
			difficultyBeatmaps->values[i] = customDifficultyBeatmap;
		}
		difficultyBeatmapSet->SetCustomDifficultyBeatmaps(difficultyBeatmaps);
		getLogger().info("End LoadDifficultyBeatmapSet");
		return difficultyBeatmapSet;
    }
            
    GlobalNamespace::CustomDifficultyBeatmap* LevelLoader::LoadDifficultyBeatmap(std::string customLevelPath, GlobalNamespace::CustomBeatmapLevel* parentCustomBeatmapLevel, GlobalNamespace::CustomDifficultyBeatmapSet* parentDifficultyBeatmapSet, GlobalNamespace::StandardLevelInfoSaveData* standardLevelInfoSaveData, GlobalNamespace::StandardLevelInfoSaveData::DifficultyBeatmap* difficultyBeatmapSaveData)
    {
		getLogger().info("Begin LoadDifficultyBeatmap");
        GlobalNamespace::BeatmapData* beatmapData = LevelLoader::LoadBeatmapData(customLevelPath, to_utf8(csstrtostr(difficultyBeatmapSaveData->beatmapFilename)), standardLevelInfoSaveData);
		GlobalNamespace::BeatmapDifficulty difficulty;

		Loader::BeatmapDifficultyFromSerializedName(to_utf8(csstrtostr(difficultyBeatmapSaveData->difficulty)), difficulty);

		getLogger().info("Constructing diffbeatmap");
		
		GlobalNamespace::CustomDifficultyBeatmap* result = GlobalNamespace::CustomDifficultyBeatmap::New_ctor(reinterpret_cast<GlobalNamespace::IBeatmapLevel*>(parentCustomBeatmapLevel),
		 																									  reinterpret_cast<GlobalNamespace::IDifficultyBeatmapSet*>(parentDifficultyBeatmapSet),
		  																									  difficulty,
																											  difficultyBeatmapSaveData->difficultyRank,
																											  difficultyBeatmapSaveData->noteJumpMovementSpeed,
																											  difficultyBeatmapSaveData->noteJumpStartBeatOffset,
																											  beatmapData);
		
		getLogger().info("End LoadDifficultyBeatmap");
		return result;
	}

    GlobalNamespace::BeatmapData* LevelLoader::LoadBeatmapData(std::string customLevelPath, std::string difficultyFileName, GlobalNamespace::StandardLevelInfoSaveData* standardLevelInfoSaveData)
    {
		getLogger().info("Begin LoadBeatmapData");
        GlobalNamespace::BeatmapData* beatmapData = nullptr;
		std::string path = customLevelPath + "/" + difficultyFileName;
		bool fileExists = fileexists(path);
		getLogger().info("loading beatmapdata for %s", path.c_str());
		if (fileExists)
		{
		    std::string json = readfile(path);
			GlobalNamespace::BeatmapDataLoader* beatmapLevelLoader = GlobalNamespace::BeatmapDataLoader::New_ctor();
			beatmapData = beatmapLevelLoader->GetBeatmapDataFromJson(il2cpp_utils::createcsstr(json), standardLevelInfoSaveData->beatsPerMinute, standardLevelInfoSaveData->shuffle, standardLevelInfoSaveData->shufflePeriod);
			getLogger().info("End LoadBeatmapData");
			return beatmapData;
		}
		getLogger().info("End LoadBeatmapData, was null");
		return nullptr;
    }

	UnityEngine::AudioClip* LevelLoader::LoadAudioClip(GlobalNamespace::CustomPreviewBeatmapLevel* customPreviewBeatmapLevel)
	{
		getLogger().info("Attempting to decode audio file");
		short* data;
		int sampleRate;
		int channels;
		std::string filename = to_utf8(csstrtostr(customPreviewBeatmapLevel->customLevelPath)) + "/" + to_utf8(csstrtostr(customPreviewBeatmapLevel->standardLevelInfoSaveData->songFilename));

		getLogger().info("Sound File filename: %s", filename.c_str());
		int dataLength = stb_vorbis_decode_filename(filename.c_str(), &channels, &sampleRate, &data);
		if(dataLength > 0)
		{
			getLogger().info("Sound file decoded into data");
			int trueLength = dataLength * channels;
			
			Array<float>* samples = reinterpret_cast<Array<float>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("System", "Single"), trueLength));
			
			for (int i = 0; i < trueLength; i++)
			{
				short sample = data[i];
				samples->values[i] = (float) ((double) sample) / SHRT_MAX;
			}

			free(data);
			getLogger().info("channels: %d", channels);
			getLogger().info("sampleRate: %d", sampleRate);
			getLogger().info("amount of samples: %d, true data length: %d", dataLength, trueLength);

			getLogger().info("Sound file decoded into C# float array");
			UnityEngine::AudioClip* clip = UnityEngine::AudioClip::Create(customPreviewBeatmapLevel->customLevelPath, dataLength, channels, sampleRate, false);
			getLogger().info("clip created");
			clip->SetData(samples, 0);
			getLogger().info("Data set on clip");
			return clip;
		}
		else
		{
			getLogger().error("Error loading audio file with error code %d, returning nullptr", dataLength);
			return nullptr;
		}
	}
}