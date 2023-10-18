#include "Patches/PSliderHapticFeedbackInteractionEffect.hpp"
#include "NTConfig.hpp"

#include <CP_SDK/Unity/Extensions/ColorU.hpp>
#include <CP_SDK/Utils/MonoPtr.hpp>

#include <GlobalNamespace/SliderHapticFeedbackInteractionEffect.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/Transform.hpp>

using namespace CP_SDK::Unity::Extensions;
using namespace GlobalNamespace;
using namespace UnityEngine;

namespace QBeatSaberPlus_NoteTweaker::Patches {

    static bool PSliderHapticFeedbackInteractionEffect_Enabled   = false;
    static bool PSliderHapticFeedbackInteractionEffect_Haptic    = true;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Set from configuration
    /// @param p_OnSceneSwitch Reset on scene switch
    void PSliderHapticFeedbackInteractionEffect::SetFromConfig(bool p_OnSceneSwitch)
    {
        PSliderHapticFeedbackInteractionEffect_Enabled = NTConfig::Instance()->Enabled;
        PSliderHapticFeedbackInteractionEffect_Haptic  = NTConfig::Instance()->GetActiveProfile()->ArcsHaptics;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    CP_SDK_IL2CPP_HOOK_MAKE_AUTO_HOOK_ORIG_MATCH(
        SliderHapticFeedbackInteractionEffect_StartEffect, &SliderHapticFeedbackInteractionEffect::StartEffect,
        void, SliderHapticFeedbackInteractionEffect* __Instance,

        float __a)
    {
        if (!PSliderHapticFeedbackInteractionEffect_Enabled || PSliderHapticFeedbackInteractionEffect_Haptic)
        {
            /// Forward to base method
            SliderHapticFeedbackInteractionEffect_StartEffect(__Instance, __a);
            return;
        }

        /// Interrupt base method
        return;
    }

}   ///< namespace QBeatSaberPlus_NoteTweaker::Patches