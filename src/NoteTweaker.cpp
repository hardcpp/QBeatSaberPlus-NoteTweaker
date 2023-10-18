#include "NoteTweaker.hpp"
#include "Patches/PBombNoteController.hpp"
#include "Patches/PBurstSliderGameNoteController.hpp"
#include "Patches/PColorNoteVisuals.hpp"
#include "Patches/PGameNoteController.hpp"
#include "Patches/PSliderController.hpp"
#include "Patches/PSliderHapticFeedbackInteractionEffect.hpp"
#include "Logger.hpp"

using namespace UnityEngine;

namespace QBeatSaberPlus_NoteTweaker {

    CP_SDK_MODULE_BASE_INSTANCE_IMPL(NoteTweaker);

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    std::string_view NoteTweaker::ImportFolder()
    {
        return m_ImportFolder;
    }
    std::string_view NoteTweaker::ExportFolder()
    {
        return m_ExportFolder;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// Constructor
    NoteTweaker::NoteTweaker()
    {

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Enable the Module
    void NoteTweaker::OnEnable()
    {
        CP_SDK_BS::Game::Logic::OnSceneChange += {this, &NoteTweaker::OnSceneChange};

        /// Trigger a Set config
        OnSceneChange(CP_SDK_BS::Game::Logic::ESceneType::Menu);

        m_ImportFolder = CP_SDK::ChatPlexSDK::BasePath() + "QBeatSaberPlus/NoteTweaker/Import";
        m_ExportFolder = CP_SDK::ChatPlexSDK::BasePath() + "QBeatSaberPlus/NoteTweaker/Export";

        try
        {
            if (!std::filesystem::exists(m_ImportFolder))
                std::filesystem::create_directories(m_ImportFolder);

            if (!std::filesystem::exists(m_ExportFolder))
                std::filesystem::create_directories(m_ExportFolder);
        }
        catch (const std::exception&)
        {

        }
    }
    /// @brief Disable the Module
    void NoteTweaker::OnDisable()
    {
        CP_SDK_BS::Game::Logic::OnSceneChange  -= {this, &NoteTweaker::OnSceneChange};

        CP_SDK::UI::UISystem::DestroyUI(&m_SettingsLeftView);
        CP_SDK::UI::UISystem::DestroyUI(&m_SettingsMainView);
        CP_SDK::UI::UISystem::DestroyUI(&m_SettingsRightView);

        /// Restore config
        Patches::PBombNoteController::SetFromConfig(true);
        Patches::PBombNoteController::SetTemp(false, 1.0f);
        Patches::PBurstSliderGameNoteController::SetFromConfig(true);
        Patches::PBurstSliderGameNoteController::SetTemp(false, 1.0f);
        Patches::PColorNoteVisuals::SetFromConfig(true);
        Patches::PColorNoteVisuals::SetBlockColorOverride(false, Color::get_black(), Color::get_black());
        Patches::PGameNoteController::SetFromConfig(true);
        Patches::PGameNoteController::SetTemp(false, 1.0f);
        Patches::PSliderController::SetFromConfig(true);
        Patches::PSliderHapticFeedbackInteractionEffect::SetFromConfig(true);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Get Module settings UI
    NoteTweaker::t_InitialViews NoteTweaker::GetSettingsViewControllers()
    {
        if (!m_SettingsLeftView)  m_SettingsLeftView  = CP_SDK::UI::UISystem::CreateViewController<UI::SettingsLeftView*>();
        if (!m_SettingsMainView)  m_SettingsMainView  = CP_SDK::UI::UISystem::CreateViewController<UI::SettingsMainView*>();
        if (!m_SettingsRightView) m_SettingsRightView = CP_SDK::UI::UISystem::CreateViewController<UI::SettingsRightView*>();

        return { m_SettingsMainView.Ptr(), m_SettingsLeftView.Ptr(), m_SettingsRightView.Ptr()};
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief On game scene change
    /// @param p_Scene New scene
    void NoteTweaker::OnSceneChange(CP_SDK_BS::Game::Logic::ESceneType p_Scene)
    {
        Patches::PBombNoteController::SetFromConfig(true);
        Patches::PBombNoteController::SetTemp(false, 1.0f);
        Patches::PBurstSliderGameNoteController::SetFromConfig(true);
        Patches::PBurstSliderGameNoteController::SetTemp(false, 1.0f);
        Patches::PColorNoteVisuals::SetFromConfig(true);
        Patches::PColorNoteVisuals::SetBlockColorOverride(false, Color::get_black(), Color::get_black());
        Patches::PGameNoteController::SetFromConfig(true);
        Patches::PGameNoteController::SetTemp(false, 1.0f);
        Patches::PSliderController::SetFromConfig(true);
        Patches::PSliderHapticFeedbackInteractionEffect::SetFromConfig(true);
    }

}   ///< namespace QBeatSaberPlus_NoteTweaker