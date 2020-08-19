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
        std::vector<uint8_t> bytesVector;
        
        for (auto c : bytesAsChar)
        {
            bytesVector.push_back(c);
        }

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