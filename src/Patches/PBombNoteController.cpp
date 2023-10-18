#include "Patches/PBombNoteController.hpp"
#include "NTConfig.hpp"
#include "Logger.hpp"

#include <vector>

#include <CP_SDK/Utils/MonoPtr.hpp>
#include <CP_SDK/ChatPlexSDK.hpp>
#include <CP_SDK_BS/Game/LevelData.hpp>
#include <CP_SDK_BS/Game/Logic.hpp>

#include <conditional-dependencies/shared/main.hpp>
#include <GlobalNamespace/BombNoteController.hpp>
#include <GlobalNamespace/CuttableBySaber.hpp>
#include <GlobalNamespace/GameplayModifiers.hpp>
#include <GlobalNamespace/MaterialPropertyBlockController.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/MaterialPropertyBlock.hpp>
#include <UnityEngine/Shader.hpp>
#include <UnityEngine/SphereCollider.hpp>
#include <UnityEngine/Transform.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"

using namespace GlobalNamespace;
using namespace UnityEngine;

namespace QBeatSaberPlus_NoteTweaker::Patches {

    EXPOSE_API(Patches__PBombNoteController__SetTemp, void, bool __a, float __b)
    {
        PBombNoteController::SetTemp(__a, __b);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    struct PBombNoteControllerCache
    {
        BombNoteController*              bombNoteController;
        MaterialPropertyBlockController* materialPropertyBlockController;
        SphereCollider*                  sphereCollider;
        Transform*                       transform;
    };

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    static bool     PBombNoteController_Enabled     = false;
    static bool     PBombNoteController_TempEnabled = false;
    static int32_t  PBombNoteController_ColorID     = 0;
    static Color    PBombNoteController_Color;
    static Vector3  PBombNoteController_Scale;
    static float    PBombNoteController_InvScale;
    static Vector3  PBombNoteController_TempScale;
    static float    PBombNoteController_TempInvScale;

    static bool                                     PBombNoteController_WasInit = false;
    static std::vector<PBombNoteControllerCache>    PBombNoteController_Cache;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Set from configuration
    /// @param p_OnSceneSwitch Reset on scene switch
    void PBombNoteController::SetFromConfig(bool p_OnSceneSwitch)
    {
        Init();

        auto& l_Profile   = NTConfig::Instance()->GetActiveProfile();
        auto  l_BombScale = FilterScale(NTConfig::Instance()->Enabled ? l_Profile->BombsScale : 1.0f);

        PBombNoteController_Enabled      = IsScaleAllowed() ? NTConfig::Instance()->Enabled : false;
        PBombNoteController_Color        = (PBombNoteController_Enabled && l_Profile->BombsOverrideColor) ? l_Profile->BombsColor : Color(0.251f, 0.251f, 0.251f, 1.000f);
        PBombNoteController_Scale        = (       l_BombScale) * Vector3::get_one();
        PBombNoteController_InvScale     =  1.0f / l_BombScale;

        if (p_OnSceneSwitch)
            PBombNoteController_Enabled = false;
    }
    /// @brief Set temp config
    /// @param p_Enabled Is it enabled
    /// @param p_Scale   New scale
    void PBombNoteController::SetTemp(bool p_Enabled, float p_Scale)
    {
        Init();

        if (!IsScaleAllowed())
            return;

        p_Scale = FilterScale(p_Scale);

        PBombNoteController_TempEnabled     = p_Enabled;
        PBombNoteController_TempScale       =        (p_Scale) * Vector3::get_one();
        PBombNoteController_TempInvScale    = 1.0f / (p_Scale);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Is note scaling enabled
    bool PBombNoteController::IsScaleAllowed()
    {
        auto& l_LevelData = CP_SDK_BS::Game::Logic::LevelData();
        if (l_LevelData)
        {
            auto l_GameplayModifiers = l_LevelData->Data ? l_LevelData->Data->gameplayModifiers : nullptr;
            if (l_GameplayModifiers)
            {
                if (l_GameplayModifiers->proMode || l_GameplayModifiers->smallCubes || l_GameplayModifiers->strictAngles)
                    return false;
            }
        }

        return true;
    }
    /// @brief Filter note scaling
    /// @param p_Scale Input scale
    float PBombNoteController::FilterScale(float p_Scale)
    {
        return std::clamp<float>(p_Scale, 0.4f, 1.5f);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Init specific data and register events
    void PBombNoteController::Init()
    {
        if (PBombNoteController_Cache.capacity() < 100)
            PBombNoteController_Cache.reserve(100);

        if (PBombNoteController_ColorID == 0)
            PBombNoteController_ColorID = Shader::PropertyToID("_SimpleColor");

        if (!PBombNoteController_WasInit)
        {
            CP_SDK::ChatPlexSDK::OnGenericSceneChange += [](CP_SDK::ChatPlexSDK::EGenericScene x) { if (x == CP_SDK::ChatPlexSDK::EGenericScene::Menu) PBombNoteController_Cache.clear(); };
            PBombNoteController_WasInit = true;
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    CP_SDK_IL2CPP_HOOK_MAKE_AUTO_HOOK_ORIG_MATCH(
        BombNoteController_Init, &BombNoteController::Init,
        void, BombNoteController* __Instance,

        NoteData*               __a, float __b, Vector3 __c, Vector3 __d,
        Vector3                 __e, float __f, float   __g, float   __h)
    {
        BombNoteController_Init(__Instance, __a, __b, __c, __d, __e, __f, __g, __h);

        auto l_Cache      = (PBombNoteControllerCache*)nullptr;
        auto l_CacheCount = PBombNoteController_Cache.size();
        for (auto l_I = 0; l_I < l_CacheCount; ++l_I)
        {
            l_Cache = &PBombNoteController_Cache[l_I];
            if (l_Cache->bombNoteController == __Instance)
                break;
        }

        if (!l_Cache || l_Cache->bombNoteController != __Instance)
        {
            PBombNoteController_Cache.push_back({
                __Instance,
                __Instance->GetComponentInChildren<MaterialPropertyBlockController*>(),
                __Instance->cuttableBySaber->GetComponent<SphereCollider*>(),
                __Instance->get_transform()
            });
            l_Cache = &PBombNoteController_Cache.back();

            if (!l_Cache->materialPropertyBlockController->materialPropertyBlock)
                l_Cache->materialPropertyBlockController->materialPropertyBlock = MaterialPropertyBlock::New_ctor();
        }

        l_Cache->materialPropertyBlockController->materialPropertyBlock->SetColor(PBombNoteController_ColorID, PBombNoteController_Color);
        l_Cache->materialPropertyBlockController->ApplyChanges();
        l_Cache->sphereCollider->set_radius(0.18f * (PBombNoteController_TempEnabled ? PBombNoteController_TempInvScale : PBombNoteController_InvScale));
        l_Cache->transform->set_localScale(PBombNoteController_TempEnabled ? PBombNoteController_TempScale : PBombNoteController_Scale);
    }

}   ///< namespace QBeatSaberPlus_NoteTweaker::Patches