#pragma once

#include <CP_SDK/Utils/Il2cpp.hpp>

namespace QBeatSaberPlus_NoteTweaker::Patches {

    /// @brief SliderController patch
    class PSliderController
    {
        CP_SDK_NO_DEF_CTORS(PSliderController);

        public:
            /// @brief Set from configuration
            /// @param p_OnSceneSwitch Reset on scene switch
            static void SetFromConfig(bool p_OnSceneSwitch);

    };

}   ///< namespace QBeatSaberPlus_NoteTweaker::Patches