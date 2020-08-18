#pragma once
#include "../include/mod_interface.hpp"
#include <unordered_set>
#include "../extern/beatsaber-hook/shared/utils/utils.h"
#include "../extern/beatsaber-hook/shared/utils/logging.hpp"
#include "../extern/modloader/shared/modloader.hpp"
#include "../extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp" 
#include "../extern/beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "../extern/beatsaber-hook/shared/utils/typedefs.h"
#include "../extern/beatsaber-hook/shared/config/config-utils.hpp"

namespace Utils
{
    class Unity
    {
        public:
            /// @brief Returns last object of a given class
            /// @return Object of type klass
            /// @param klass unity il2cpp class to look for
            static Il2CppObject* GetFirstObjectOfType(Il2CppClass *klass);
    };
}