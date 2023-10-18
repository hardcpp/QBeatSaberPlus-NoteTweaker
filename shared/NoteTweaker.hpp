#pragma once

#include "UI/SettingsMainView.hpp"
#include "UI/SettingsLeftView.hpp"
#include "UI/SettingsRightView.hpp"
#include "NTConfig.hpp"

#include <CP_SDK/ModuleBase.hpp>
#include <CP_SDK/ChatPlexSDK.hpp>
#include <CP_SDK_BS/Game/Logic.hpp>

namespace QBeatSaberPlus_NoteTweaker {

    /// @brief NoteTweaker Module
    class NoteTweaker : public CP_SDK::ModuleBase<NoteTweaker>
    {
        CP_SDK_MODULE_BASE_INSTANCE_DECL(NoteTweaker);

        public:
            CP_SDK::EIModuleBaseType            Type()                      const override { return CP_SDK::EIModuleBaseType::Integrated;                               }
            std::u16string_view                 Name()                      const override { return u"Note Tweaker";                                                    }
            std::u16string_view                 FancyName()                 const override { return u"Note Tweaker";                                                    }
            std::u16string_view                 Description()               const override { return u"Customize your game play & menu experience!";                     }
            std::u16string_view                 DocumentationURL()          const override { return u"https://github.com/hardcpp/BeatSaberPlus/wiki#note-tweaker";      }
            bool                                UseChatFeatures()           const override { return false;                                                              }
            bool                                IsEnabled()                 const override { return NTConfig::Instance()->Enabled;                                      }
            void                                IsEnabled(bool p_Enabled)         override { NTConfig::Instance()->Enabled = p_Enabled; NTConfig::Instance()->Save();   }
            CP_SDK::EIModuleBaseActivationType  ActivationType()            const override { return CP_SDK::EIModuleBaseActivationType::OnMenuSceneLoaded;              }

        private:
            CP_SDK::Utils::MonoPtr<UI::SettingsLeftView>    m_SettingsLeftView;
            CP_SDK::Utils::MonoPtr<UI::SettingsMainView>    m_SettingsMainView;
            CP_SDK::Utils::MonoPtr<UI::SettingsRightView>   m_SettingsRightView;

        private:
            std::string m_ImportFolder;
            std::string m_ExportFolder;

        public:
            std::string_view ImportFolder();
            std::string_view ExportFolder();

        public:
            /// @brief Constructor
            NoteTweaker();

        protected:
            /// @brief Enable the Module
            void OnEnable() override;
            /// @brief Disable the Module
            void OnDisable() override;

        public:
            /// @brief Get Module settings UI
            t_InitialViews GetSettingsViewControllers() override;

        private:
            /// @brief On game scene change
            /// @param p_Scene New scene
            void OnSceneChange(CP_SDK_BS::Game::Logic::ESceneType p_Scene);

    };

}   ///< namespace QBeatSaberPlus_NoteTweaker