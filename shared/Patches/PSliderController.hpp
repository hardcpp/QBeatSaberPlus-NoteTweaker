#pragma once

#include <CP_SDK/Utils/Il2cpp.hpp>

namespace QBeatSaberPlus_NoteTweaker::Patches {

    /// @brief SliderController patch
    class PSliderController
    {
        DISABLE_CONSTRUCTORS(PSliderController);

        public:
            /// @brief Set from configuration
            /// @param p_OnSceneSwitch Reset on scene switch
            static void SetFromConfig(bool p_OnSceneSwitch);

    };

}   ///< namespace QBeatSaberPlus_NoteTweaker::Patches