#pragma once

#include <CP_SDK/Logging/ILogger.hpp>

namespace QBeatSaberPlus_NoteTweaker {

    /// @brief Logger instance holder
    class Logger
    {
        public:
            /// @brief Logger instance
            static CP_SDK::Logging::ILogger* Instance;

    };

}   ///< namespace QBeatSaberPlus_NoteTweaker