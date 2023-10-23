#pragma once

#include <CP_SDK/UI/ViewController.hpp>
#include <CP_SDK/XUI/XUI.hpp>

#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/Color.hpp>

namespace QBeatSaberPlus_NoteTweaker::UI {

    namespace _u
    {
        using namespace UnityEngine;
    }

    /// @brief Settings right view controller
    class SettingsRightView : public CP_SDK::UI::ViewController
    {
        CP_SDK_IL2CPP_INHERIT("QBeatSaberPlus_NoteTweaker.UI", SettingsRightView, CP_SDK::UI::ViewController);
        CP_SDK_IL2CPP_DECLARE_CTOR_CHILD(SettingsRightView);
        CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_CHILD(SettingsRightView);
        CP_SDK_UI_VIEW_CONTROLLER_INSTANCE();

        private:
            CP_SDK::Utils::MonoPtr<_u::GameObject> m_Parent;
            CP_SDK::Utils::MonoPtr<_u::GameObject> m_NoteTemplate;
            CP_SDK::Utils::MonoPtr<_u::GameObject> m_BombTemplate;
            CP_SDK::Utils::MonoPtr<_u::GameObject> m_BurstSliderTemplate;

            CP_SDK::Utils::MonoPtr<_u::GameObject> m_CustomPreviewTL;
            CP_SDK::Utils::MonoPtr<_u::GameObject> m_CustomPreviewTR;
            CP_SDK::Utils::MonoPtr<_u::GameObject> m_CustomPreviewDL;
            CP_SDK::Utils::MonoPtr<_u::GameObject> m_CustomPreviewDR;
            CP_SDK::Utils::MonoPtr<_u::GameObject> m_CustomPreviewBomb;
            CP_SDK::Utils::MonoPtr<_u::GameObject> m_CustomPreviewSliderFill;

            CP_SDK::Utils::MonoPtr<_u::GameObject> m_DefaultPreviewTL;
            CP_SDK::Utils::MonoPtr<_u::GameObject> m_DefaultPreviewTR;
            CP_SDK::Utils::MonoPtr<_u::GameObject> m_DefaultPreviewDL;
            CP_SDK::Utils::MonoPtr<_u::GameObject> m_DefaultPreviewDR;
            CP_SDK::Utils::MonoPtr<_u::GameObject> m_DefaultPreviewBomb;
            CP_SDK::Utils::MonoPtr<_u::GameObject> m_DefaultPreviewSliderFill;

        private:
            /// @brief On view creation
            void OnViewCreation_Impl();
            /// @brief On view activation
            void OnViewActivation_Impl();
            /// @brief On view deactivation
            void OnViewDeactivation_Impl();
            /// @brief On view destruction
            void OnViewDestruction_Impl();

        public:
            /// @brief Refresh settings
            void RefreshSettings();

        private:
            /// @brief Create note preview
            /// @param p_NoteTemplate Note template
            void CreatePreview(_u::GameObject* p_NoteTemplate, _u::GameObject* p_BombTemplate, _u::GameObject* m_BurstFillTemplate);
            /// @brief Patch note
            /// @param p_Object Object
            /// @param p_Color  Color
            void PatchNote(_u::GameObject* p_Object, _u::Color p_Color);
            /// @brief Patch arrow
            /// @param p_Note  Instance
            /// @param p_Scale Scale
            /// @param p_Color Color
            /// @param p_Show  Show
            void PatchArrow(_u::GameObject* p_Note, float p_Scale, _u::Color p_Color, bool p_Show);
            /// @brief Patch circle
            /// @param p_Note  Instance
            /// @param p_Scale Scale
            /// @param p_Color Color
            /// @param p_Show  Show
            void PatchCircle(_u::GameObject* p_Note, float p_Scale, _u::Color p_Color, bool p_Show);
            /// @brief Patch bomb
            /// @param p_Object Object
            /// @param p_Color  Color
            void PatchBomb(_u::GameObject* p_Object, _u::Color p_Color);

    };

}   ///< namespace QBeatSaberPlus_NoteTweaker::UI

CP_SDK_IL2CPP_INHERIT_HELPERS(QBeatSaberPlus_NoteTweaker::UI::SettingsRightView);