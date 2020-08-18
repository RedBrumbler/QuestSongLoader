#include "Utils/File.hpp"

ModInfo Utils::File::modInfo;

namespace Utils
{
    const Logger& File::getLogger() 
    {
        static const Logger logger(modInfo);
        return logger;
    }

    void File::OnLoad(ModInfo modInfo)
    {
        File::modInfo = modInfo;
        File::modInfo.id += "::File";
    }

    bool File::GetDirectoriesInDir(std::string dir, std::vector<std::string> &out)
    {
        bool foundDir = false;
    	
        DIR* fileDir = opendir(dir.data());
        struct dirent *entry;

        while((entry = readdir(fileDir)) != NULL)
        {
            if (entry->d_type != DT_DIR) continue;
            
            std::string dirName = entry->d_name;

            if (dirName == "." || dirName == "..") continue;
                
            out.push_back(dirName);

            if (!foundDir) foundDir = true;
        }

        closedir(fileDir);
        
        return foundDir;
    }

    std::string File::GetInfoFromDirectory(std::string dir)
    {
        std::string infoPath = dir + "info.dat";

        if (!fileexists(infoPath)) return NULL;

        std::string info = "";
        info = readfile(infoPath);

        if (info == "") return NULL;
        return info;
    }

    Array<uint8_t>* File::ReadAllBytes(std::string path)
    {
        if (!fileexists(path))
        {
            getLogger().error("ReadAllBytes could not read file because file did not exist, returning nullptr");
            return nullptr;
        }
        getLogger().info("File Found, reading info");
        std::string info = readfile(path);
        //getLogger().info("ReadAllBytes found info: %s", info.c_str());
        getLogger().info("info read, generating result array");
        Array<uint8_t>* result = reinterpret_cast<Array<uint8_t>*>(il2cpp_functions::array_new(il2cpp_utils::GetClassFromName("System", "Byte"), info.length() + 1));
        
        getLogger().info("Setting info contents into given array");
        
        int i = 0;
        for (char b : info)
        {
            result->values[i] = (uint8_t)b;
            i++;
        }

        getLogger().info("returning result");
        return result;
    }
}