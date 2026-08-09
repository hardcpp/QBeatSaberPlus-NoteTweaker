#include "Patches/PGameNoteController.hpp"
#include "NTConfig.hpp"
#include "Logger.hpp"

#include <CP_SDK/Unity/Operators.hpp>
#include <CP_SDK/Utils/MonoPtr.hpp>
#include <CP_SDK_BS/Game/LevelData.hpp>
#include <CP_SDK_BS/Game/Logic.hpp>

#include <conditional-dependencies/shared/main.hpp>
#include <GlobalNamespace/BoxCuttableBySaber.hpp>
#include <GlobalNamespace/GameNoteController.hpp>
#include <GlobalNamespace/GameplayModifiers.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/Transform.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"

using namespace GlobalNamespace;
using namespace UnityEngine;

namespace QBeatSaberPlus_NoteTweaker::Patches {

    EXPOSE_API(Patches__PGameNoteController__SetTemp, void, bool __a, float __b)
    {
        PGameNoteController::SetTemp(__a, __b);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    static bool     PGameNoteController_Enabled         = false;
    static bool     PGameNoteController_TempEnabled     = false;
    static bool     PGameNoteController_WasTempEnabled  = false;
    static Vector3  PGameNoteController_NoteScale;
    static Vector3  PGameNoteController_NoteInvScale;
    static Vector3  PGameNoteController_TempNoteScale;
    static Vector3  PGameNoteController_TempNoteInvScale;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Set from configuration
    /// @param p_OnSceneSwitch Reset on scene switch
    void PGameNoteController::SetFromConfig(bool p_OnSceneSwitch)
    {
        auto& l_Profile           = NTConfig::Instance()->GetActiveProfile();
        auto  l_NoteScale         = FilterScale(NTConfig::Instance()->Enabled ? l_Profile->NotesScale : 1.0f);

        PGameNoteController_Enabled               = IsScaleAllowed() ? NTConfig::Instance()->Enabled : false;
        PGameNoteController_NoteScale             = (       l_NoteScale) * Vector3::get_one();
        PGameNoteController_NoteInvScale          = (1.0f / l_NoteScale) * Vector3::get_one();

        if (p_OnSceneSwitch)
        {
            PGameNoteController_TempEnabled     = false;
            PGameNoteController_WasTempEnabled  = false;
        }
    }
    /// @brief Set temp config
    /// @param p_Enabled Is it enabled
    /// @param p_Scale   New scale
    void PGameNoteController::SetTemp(bool p_Enabled, float p_Scale)
    {
        if (!IsScaleAllowed())
            return;

        p_Scale = FilterScale(p_Scale);

        PGameNoteController_TempEnabled           = p_Enabled;
        PGameNoteController_TempNoteScale         = (       p_Scale) * Vector3::get_one();
        PGameNoteController_TempNoteInvScale      = (1.0f / p_Scale) * Vector3::get_one();

        if (p_Enabled)
            PGameNoteController_WasTempEnabled = true;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Is note scaling enabled
    bool PGameNoteController::IsScaleAllowed()
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
    float PGameNoteController::FilterScale(float p_Scale)
    {
        return std::clamp<float>(p_Scale, 0.4f, 1.5f);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    CP_SDK_IL2CPP_HOOK_MAKE_AUTO_HOOK_ORIG_MATCH(
        GameNoteController_Init, &GameNoteController::Init,
        void, GameNoteController* __Instance,

        NoteData * __a, ByRef<NoteSpawnData> __b, NoteVisualModifierType __c , float __d, float __e)
    {
        GameNoteController_Init(__Instance, __a, __b, __c, __d, __e);

        if (!PGameNoteController_Enabled && !PGameNoteController_TempEnabled && !PGameNoteController_WasTempEnabled)
            return;

        auto l_NoteScale     = PGameNoteController_NoteScale;
        auto l_NoteInvScale  = PGameNoteController_NoteInvScale;

        if (PGameNoteController_TempEnabled)
        {
            l_NoteScale     = PGameNoteController_TempNoteScale;
            l_NoteInvScale  = PGameNoteController_TempNoteInvScale;
        }
        else if (PGameNoteController_WasTempEnabled)
        {
            l_NoteScale     = Vector3::get_one();
            l_NoteInvScale  = Vector3::get_one();
        }

        __Instance->get_transform()->set_localScale(l_NoteScale);

        for (auto l_Current : __Instance->_bigCuttableBySaberList)
            l_Current->get_transform()->set_localScale(l_NoteInvScale);

        for (auto l_Current : __Instance->_smallCuttableBySaberList)
            l_Current->get_transform()->set_localScale(l_NoteInvScale);
    }

}   ///< namespace QBeatSaberPlus_NoteTweaker::Patches