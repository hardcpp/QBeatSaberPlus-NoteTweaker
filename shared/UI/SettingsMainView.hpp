#pragma once

#include "Modals/ProfileImportModal.hpp"

#include <CP_SDK/UI/Data/TextListItem.hpp>
#include <CP_SDK/UI/ViewController.hpp>
#include <CP_SDK/XUI/XUI.hpp>

namespace QBeatSaberPlus_NoteTweaker::UI {

    namespace _u
    {
        using namespace CP_SDK::XUI;
        using namespace UnityEngine;
    }

    /// @brief Settings main view controller
    class SettingsMainView : public CP_SDK::UI::ViewController
    {
        CP_SDK_IL2CPP_INHERIT("QBeatSaberPlus_NoteTweaker.UI", SettingsMainView, CP_SDK::UI::ViewController);
        CP_SDK_IL2CPP_DECLARE_CTOR_CHILD(SettingsMainView);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_CHILD(SettingsMainView);
        CP_SDK_UI_VIEW_CONTROLLER_INSTANCE();

        private:
            _u::XUITabControl::Ptr  m_TabControl;
            _u::XUIVVList::Ptr      m_ProfilesTab_List;

            _u::XUISlider::Ptr      m_NotesTab_Scale;
            _u::XUIToggle::Ptr      m_NotesTab_ShowPrecisonDots;
            _u::XUISlider::Ptr      m_NotesTab_PrecisionDotsScale;

            _u::XUISlider::Ptr      m_ArrowsTab_Scale;
            _u::XUISlider::Ptr      m_ArrowsTab_Intensity;
            _u::XUIToggle::Ptr      m_ArrowsTab_OverrideColors;
            _u::XUIColorInput::Ptr  m_ArrowsTab_LColor;
            _u::XUIColorInput::Ptr  m_ArrowsTab_RColor;

            _u::XUISlider::Ptr      m_DotsTab_Scale;
            _u::XUISlider::Ptr      m_DotsTab_Intensity;
            _u::XUIToggle::Ptr      m_DotsTab_OverrideColors;
            _u::XUIColorInput::Ptr  m_DotsTab_LColor;
            _u::XUIColorInput::Ptr  m_DotsTab_RColor;

            _u::XUISlider::Ptr      m_BombsTab_Scale;
            _u::XUIToggle::Ptr      m_BombsTab_OverrideColor;
            _u::XUIColorInput::Ptr  m_BombsTab_Color;

            _u::XUISlider::Ptr      m_ArcsTab_Intensity;
            _u::XUIToggle::Ptr      m_ArcsTab_Haptics;

            _u::XUISlider::Ptr      m_BurstNotesTab_DotScale;

            CP_SDK::Utils::MonoPtr<Modals::ProfileImportModal> m_ProfileImportModal;

        private:
            std::vector<CP_SDK::UI::Data::TextListItem::Ptr>    m_Items;
            std::shared_ptr<CP_SDK::UI::Data::TextListItem>     m_SelectedItem;
            bool                                                m_PreventChanges;

        private:
            /// @brief On view creation
            void OnViewCreation_Impl();
            /// @brief On view deactivation
            void OnViewDeactivation_Impl();

        private:
            /// @brief Build profiles tab
            std::shared_ptr<_u::IXUIElement> BuildProfilesTab();
            /// @brief Build notes tab
            std::shared_ptr<_u::IXUIElement> BuildNotesTab();
            /// @brief Build arrows tab
            std::shared_ptr<_u::IXUIElement> BuildArrowsTab();
            /// @brief Build dots tab
            std::shared_ptr<_u::IXUIElement> BuildDotsTab();
            /// @brief Build bombs tabs
            std::shared_ptr<_u::IXUIElement> BuildBombsTab();
            /// @brief Build arcs tab
            std::shared_ptr<_u::IXUIElement> BuildArcsTab();
            /// @brief Build burst notes tab
            std::shared_ptr<_u::IXUIElement> BuildBurstNotesTab();

        private:
            /// @brief When a tab is selected
            /// @param p_TabIndex Tab index
            void OnTabSelected(int p_TabIndex);
            /// @brief When settings are changed
            void OnSettingChanged();
            /// @brief Reset settings
            void RefreshSettings();

        private:
            /// @brief Refresh list
            void ProfilesTab_Refresh();
            /// @brief On item selected
            /// @param p_ListItem Selected item
            void ProfilesTab_OnListItemSelect(const CP_SDK::UI::Data::IListItem::Ptr& p_ListItem);
            /// @brief New profile button
            void ProfilesTab_OnNewButton();
            /// @brief Rename profile button
            void ProfilesTab_OnRenameButton();
            /// @brief Delete profile button
            void ProfilesTab_OnDeleteButton();
            /// @brief Export an profile
            void ProfilesTab_OnExportButton();
            /// @brief Import an profile
            void ProfilesTab_OnImportButton();

    };

}   ///< namespace QBeatSaberPlus_NoteTweaker::UI

CP_SDK_IL2CPP_INHERIT_HELPERS(QBeatSaberPlus_NoteTweaker::UI::SettingsMainView);