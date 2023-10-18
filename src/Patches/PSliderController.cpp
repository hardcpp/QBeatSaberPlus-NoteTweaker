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

        SliderController::LengthType __a, SliderData* __b, float   __c, Vector3 __d,
        Vector3                      __e, Vector3     __f, Vector3 __g, float   __h,
        float                        __i, float       __j, float   __k)
    {
        SliderController_Init(__Instance, __a, __b, __c, __d, __e, __f, __g, __h, __i, __j, __k);

        if (PSliderController_Enabled)
            __Instance->initColor = ColorU::WithAlpha(__Instance->initColor, PSliderController_Opacity);
    }

}   ///< namespace QBeatSaberPlus_NoteTweaker::Patches