#include "Patches/PSliderController.hpp"
#include "NTConfig.hpp"

#include <CP_SDK/Unity/Extensions/ColorU.hpp>
#include <CP_SDK/Utils/MonoPtr.hpp>

#include <GlobalNamespace/SliderController.hpp>
#include <GlobalNamespace/SliderData.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/Transform.hpp>

using namespace CP_SDK::Unity::Extensions;
using namespace GlobalNamespace;
using namespace UnityEngine;

namespace QBeatSaberPlus_NoteTweaker::Patches {

    static bool     PSliderController_Enabled   = false;
    static float    PSliderController_Opacity   = 1.0f;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Set from configuration
    /// @param p_OnSceneSwitch Reset on scene switch
    void PSliderController::SetFromConfig(bool p_OnSceneSwitch)
    {
        PSliderController_Enabled = NTConfig::Instance()->Enabled;
        PSliderController_Opacity = NTConfig::Instance()->GetActiveProfile()->ArcsIntensity;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    CP_SDK_IL2CPP_HOOK_MAKE_AUTO_HOOK_ORIG_MATCH(
        SliderController_Init, &SliderController::Init,
        void, SliderController* __Instance,

        SliderController_LengthType __a, SliderData* __b, ByRef<SliderSpawnData> __c, float __d, float __e)
    {
        SliderController_Init(__Instance, __a, __b, __c, __d, __e);

        if (PSliderController_Enabled)
            __Instance->____initColor = ColorU::WithAlpha(__Instance->____initColor, PSliderController_Opacity);
    }

}   ///< namespace QBeatSaberPlus_NoteTweaker::Patches