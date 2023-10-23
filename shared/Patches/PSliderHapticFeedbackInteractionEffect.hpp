#pragma once

#include <CP_SDK/Utils/Il2cpp.hpp>

namespace QBeatSaberPlus_NoteTweaker::Patches {

    /// @brief SliderHapticFeedbackInteractionEffect patch
    class PSliderHapticFeedbackInteractionEffect
    {
        CP_SDK_NO_DEF_CTORS(PSliderHapticFeedbackInteractionEffect);

        public:
            /// @brief Set from configuration
            /// @param p_OnSceneSwitch Reset on scene switch
            static void SetFromConfig(bool p_OnSceneSwitch);

    };

}   ///< namespace QBeatSaberPlus_NoteTweaker::Patches