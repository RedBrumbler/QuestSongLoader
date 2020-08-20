#include "Utils/Hashing.hpp"

ModInfo Utils::Hashing::modInfo;

namespace Utils
{

    const Logger& Hashing::getLogger() 
    {
        static const Logger logger(modInfo);
        return logger;
    }

    void Hashing::OnLoad(ModInfo modInfo)
    {
        Hashing::modInfo = modInfo;
        Hashing::modInfo.id += "::Hashing";
    }

    std::string Hashing::GetCustomLevelHash(GlobalNamespace::StandardLevelInfoSaveData* level, std::string customLevelPath)
    {
        getLogger().info("GetCustomLevelHash Called");
        std::string actualPath = customLevelPath + "/Info.dat";
        if (!fileexists(actualPath)) actualPath = customLevelPath + "/info.dat";
        
        std::string hash = "";

        getLogger().info("Reading all bytes from %s", actualPath.c_str());

        std::vector<char> bytesAsChar = readbytes(actualPath);
        getLogger().info("Starting reading beatmaps");
        for (int i = 0; i < level->difficultyBeatmapSets->Length(); i++)
        {
            getLogger().info("diffBeatmapSet %d", i);
            for (int j = 0; j < level->difficultyBeatmapSets->values[i]->difficultyBeatmaps->Length(); j++)
            {
                getLogger().info("diffBeatmap %d", j);
                std::string diffFile = to_utf8(csstrtostr(level->difficultyBeatmapSets->values[i]->difficultyBeatmaps->values[j]->beatmapFilename));
                if (!fileexists(customLevelPath + "/" + diffFile))
                {
                    getLogger().error("File %s did not exist", (customLevelPath + "/" + diffFile).c_str());
                    continue;
                } 

                std::vector<char> currentDiff = readbytes(customLevelPath + "/" + diffFile);

                for (auto c : currentDiff)
                {
                    bytesAsChar.push_back(c);
                }
            }
        }
        getLogger().info("getting bytes vector");
        std::vector<uint8_t> bytesVector;
        getLogger().info("Setting values");
        for (auto c : bytesAsChar)
        {
            bytesVector.push_back(c);
        }

        getLogger().info("converting to array");
        Array<uint8_t>* bytes = il2cpp_utils::vectorToArray(bytesVector);//Utils::File::ReadAllBytes(customLevelPath);

        getLogger().info("Got Bytes: %d", bytes == nullptr);

        getLogger().info("IEnumerable: %d", sizeof(System::Collections::Generic::IEnumerable_1<uint8_t>));
        getLogger().info("Il2CppArray: %d", sizeof(Il2CppArray));
        getLogger().info("Array<uint8_t>: %d", sizeof(Array<uint8_t>));
        getLogger().info("System::Array: %d", sizeof(System::Array));
        //combinedBytes.insert(0);
        //TODO cache hashes and read from that instead of making a hash each time
        int bytelength = bytes->Length();
        getLogger().info("Turning %d Bytes into hash", bytelength);
        hash = CreateSha1FromBytes(bytes);
        
        //cachedSongHashData[customLevelPath] = *new SongHashData(directoryHash, hash);
        getLogger().info("End of getHash");
        return hash;
    }

    std::string Hashing::CreateSha1FromBytes(Array<uint8_t>* input)
    {
        std::string hash = "";
        getLogger().info("Creating sha1 instance");
        auto sha1_algorithm = System::Security::Cryptography::SHA1::Create();

        getLogger().info("computing Hash, found %d bytes", input->Length());
        hash = to_utf8(csstrtostr(System::BitConverter::ToString(sha1_algorithm->ComputeHash(input))));
        //remove all -

        

        getLogger().info("removing all '-' ");
        hash.erase(std::remove(hash.begin(), hash.end(), '-'), hash.end());
        
        getLogger().info("end of create from bytes");
        return hash;
    }
}