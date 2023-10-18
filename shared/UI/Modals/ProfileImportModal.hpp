#pragma once

#include <CP_SDK/UI/IModal.hpp>
#include <CP_SDK/XUI/XUI.hpp>

namespace QBeatSaberPlus_NoteTweaker::UI::Modals {

    namespace _u
    {
        using namespace CP_SDK::XUI;
        using namespace UnityEngine;
    }

    /// @brief Profile import modal
    class ProfileImportModal : public CP_SDK::UI::IModal
    {
        CP_SDK_IL2CPP_INHERIT_CUSTOM("QBeatSaberPlus_NoteTweaker.Modals", ProfileImportModal, CP_SDK::UI::IModal);
        CP_SDK_IL2CPP_DECLARE_CTOR_CHAIN(ProfileImportModal, CP_SDK::UI::IModal);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_CHAIN(ProfileImportModal, CP_SDK::UI::IModal);

        private:
            _u::XUIDropdown::Ptr m_Dropdown;

            CP_SDK::Utils::Delegate<void()> m_Callback;
            std::u16string                  m_Selected;

        private:
            /// @brief On modal show
            void Impl_OnShow();
            /// @brief On modal close
            void Impl_OnClose();

        public:
            /// @brief Init
            /// @param p_Callback Callback
            void Init(CP_SDK::Utils::CActionRef<> p_Callback);

        private:
            /// @brief On cancel button
            void OnCancelButton();
            /// @brief On import button
            void OnImportButton();

    };

}   ///< namespace QBeatSaberPlus_NoteTweaker::UI::Modals

CP_SDK_IL2CPP_INHERIT_HELPERS(QBeatSaberPlus_NoteTweaker::UI::Modals::ProfileImportModal);