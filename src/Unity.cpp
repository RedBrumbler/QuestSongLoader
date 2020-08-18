#include "Utils/Unity.hpp"

namespace Utils
{
        Il2CppObject* Unity::GetFirstObjectOfType(Il2CppClass *klass) 
        {
            Il2CppReflectionType* klassType = il2cpp_utils::GetSystemType(klass);
            Array<Il2CppObject*>* objects = CRASH_UNLESS(il2cpp_utils::RunMethod<Array<Il2CppObject*>*>(il2cpp_utils::GetClassFromName("UnityEngine", "Resources"), "FindObjectsOfTypeAll", klassType));
            if(objects == nullptr) return nullptr;
            return objects->values[0];
        }
}