#include "Patches/PBombNoteController.hpp"
#include "NTConfig.hpp"

#include <CP_SDK/Unity/Operators.hpp>
#include <CP_SDK/Utils/MonoPtr.hpp>
#include <CP_SDK/ChatPlexSDK.hpp>
#include <CP_SDK_BS/Game/LevelData.hpp>
#include <CP_SDK_BS/Game/Logic.hpp>

#include <GlobalNamespace/BombNoteController.hpp>
#include <GlobalNamespace/CuttableBySaber.hpp>
#include <GlobalNamespace/GameplayModifiers.hpp>
#include <GlobalNamespace/MaterialPropertyBlockController.hpp>
#include <GlobalNamespace/NoteSpawnData.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/MaterialPropertyBlock.hpp>
#include <UnityEngine/Shader.hpp>
#include <UnityEngine/SphereCollider.hpp>
#include <UnityEngine/Transform.hpp>
#include <UnityEngine/Material.hpp>
#include <UnityEngine/Renderer.hpp>
#include <conditional-dependencies/shared/main.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace CP_SDK::Utils;

namespace QBeatSaberPlus_NoteTweaker::Patches {

    EXPOSE_API(Patches__PBombNoteController__SetTemp, void, bool __a, float __b)
    {
        PBombNoteController::SetTemp(__a, __b);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    static bool              PBombNoteController_Enabled          = false;
    static bool              PBombNoteController_ShouldRecolorize = false;
    static bool              PBombNoteController_TempEnabled      = false;
    static int32_t           PBombNoteController_ColorID          = 0;
    static Color             PBombNoteController_Color;
    static MonoPtr<Material> PBombNoteController_SharedMaterial   = nullptr;
    static Vector3           PBombNoteController_Scale            = Vector3(1.0f, 1.0f, 1.0f);
    static float             PBombNoteController_InvScale         = 1.0f;
    static Vector3           PBombNoteController_TempScale        = Vector3(1.0f, 1.0f, 1.0f);
    static float             PBombNoteController_TempInvScale     = 1.0f;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Set from configuration
    /// @param p_OnSceneSwitch Reset on scene switch
    void PBombNoteController::SetFromConfig(bool p_OnSceneSwitch)
    {
        Init();

        auto& l_Profile   = NTConfig::Instance()->GetActiveProfile();
        auto  l_BombScale = FilterScale(NTConfig::Instance()->Enabled ? l_Profile->BombsScale : 1.0f);

        PBombNoteController_Enabled          = IsScaleAllowed() ? NTConfig::Instance()->Enabled : false;
        PBombNoteController_ShouldRecolorize = true;
        PBombNoteController_Color            = (PBombNoteController_Enabled && l_Profile->BombsOverrideColor) ? l_Profile->BombsColor : Color(0.251f, 0.251f, 0.251f, 1.000f);
        PBombNoteController_Scale            = l_BombScale * Vector3::get_one();
        PBombNoteController_InvScale         =  1.0f / l_BombScale;

        if (p_OnSceneSwitch)
            PBombNoteController_Enabled = false;

        if (PBombNoteController_SharedMaterial)
        {
            if (CP_SDK_BS::Game::Logic::ActiveScene() == CP_SDK_BS::Game::Logic::ESceneType::Playing)
                PBombNoteController_SharedMaterial->SetColor(PBombNoteController_ColorID, PBombNoteController_Color);
            else
                PBombNoteController_SharedMaterial->SetColor(PBombNoteController_ColorID, Color(0.251f, 0.251f, 0.251f, 1.000f));
        }
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
        PBombNoteController_TempScale       = p_Scale * Vector3::get_one();
        PBombNoteController_TempInvScale    = 1.0f / (p_Scale);
    }
    /// @brief Set bomb color override
    /// @param p_Enabled  Is override enabled?
    /// @param p_NewColor New color
    void PBombNoteController::SetBombColorOverride(bool p_Enabled, Color p_NewColor)
    {
        PBombNoteController_ShouldRecolorize = true;

        if (p_Enabled)
            PBombNoteController_Color = p_NewColor;
        else
        {
            auto& l_Profile = NTConfig::Instance()->GetActiveProfile();
            PBombNoteController_Color = (PBombNoteController_Enabled && l_Profile->BombsOverrideColor) ? l_Profile->BombsColor : Color(0.251f, 0.251f, 0.251f, 1.000f);
        }
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
                if (l_GameplayModifiers->_proMode || l_GameplayModifiers->_smallCubes || l_GameplayModifiers->_strictAngles)
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
        if (PBombNoteController_ColorID == 0)
            PBombNoteController_ColorID = Shader::PropertyToID("_SimpleColor");
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    CP_SDK_IL2CPP_HOOK_MAKE_AUTO_HOOK_ORIG_MATCH(
        BombNoteController_Init, &BombNoteController::Init,
        void, BombNoteController* __Instance,

        NoteData* __a, ByRef<NoteSpawnData> __b)
    {
        BombNoteController_Init(__Instance, __a, __b);

        if (!PBombNoteController_SharedMaterial)
            PBombNoteController_SharedMaterial = __Instance->GetComponentInChildren<Renderer*>()->get_sharedMaterial();

        if (PBombNoteController_ShouldRecolorize && PBombNoteController_SharedMaterial)
        {
            PBombNoteController_SharedMaterial->SetColor(PBombNoteController_ColorID, PBombNoteController_Color);
            PBombNoteController_ShouldRecolorize = false;
        }

        if (!PBombNoteController_Enabled && !PBombNoteController_TempEnabled)
            return;

        __Instance->transform->set_localScale(PBombNoteController_TempEnabled ? PBombNoteController_TempScale : PBombNoteController_Scale);
        __Instance->GetComponent<SphereCollider*>()->set_radius(0.18f * (PBombNoteController_TempEnabled ? PBombNoteController_TempInvScale : PBombNoteController_InvScale));
    }

}   ///< namespace QBeatSaberPlus_NoteTweaker::Patches
