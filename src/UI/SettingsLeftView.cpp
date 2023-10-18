#include "UI/SettingsLeftView.hpp"
#include "NoteTweaker.hpp"

#include <UnityEngine/Application.hpp>

namespace QBeatSaberPlus_NoteTweaker::UI {

    using namespace CP_SDK::XUI;
    using namespace UnityEngine;

    constexpr const std::u16string_view s_InformationStr =
               u"<b>Note Tweaker</b>"
         u"\n" u"<i><color=#CCCCCCFF>This module allows you to customize the default notes</color></i>";

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    CP_SDK_IL2CPP_INHERIT_INIT(SettingsLeftView);

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Constructor
    CP_SDK_IL2CPP_DECLARE_CTOR_CHAIN_IMPL(SettingsLeftView, CP_SDK::UI::ViewController)
    {
        OnViewCreation = {this, &SettingsLeftView::OnViewCreation_Impl};
    }
    /// @brief Destructor
    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_CHAIN_IMPL(SettingsLeftView, CP_SDK::UI::ViewController)
    {

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief On view creation
    void SettingsLeftView::OnViewCreation_Impl()
    {
        Templates::FullRectLayout({
            Templates::TitleBar(u"Information"),

            Templates::ScrollableInfos(55.0f, {
                XUIText::Make(s_InformationStr)
                    ->SetAlign(TMPro::TextAlignmentOptions::Left)
                    ->AsShared()
            }),

            Templates::ExpandedButtonsLine({
                XUISecondaryButton::Make(u"Documentation", {this, &SettingsLeftView::OnDocumentationButton})->AsShared()
            })
        })
        ->SetBackground(true, std::nullopt, true)
        ->BuildUI(get_transform());
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Documentation button
    void SettingsLeftView::OnDocumentationButton()
    {
        ShowMessageModal(u"URL opened in your web browser.");
        Application::OpenURL(NoteTweaker::Instance()->DocumentationURL());
    }

}   ///< namespace QBeatSaberPlus_NoteTweaker::UI