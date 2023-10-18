#pragma once

#include <CP_SDK/Config/JsonConfig.hpp>

#include <UnityEngine/Color.hpp>

namespace QBeatSaberPlus_NoteTweaker {

    using namespace UnityEngine;

    class NTConfig : public CP_SDK::Config::JsonConfig
    {
        CP_SDK_CONFIG_JSONCONFIG_INSTANCE_DECL(NTConfig);

        public:
            struct _Profile
            {
                std::u16string Name             = u"New profile";

                float NotesScale                = 0.9f;
                bool  NotesShowPrecisonDots     = true;
                float NotesPrecisonDotsScale    = 0.40f;

                float ArrowsScale               = 1.0f;
                float ArrowsIntensity           = 1.0f;
                bool  ArrowsOverrideColors      = false;
                Color ArrowsLColor              = Color(0.12f, 0.75f, 1.00f, 1.00f);
                Color ArrowsRColor              = Color(0.12f, 0.75f, 1.00f, 1.00f);

                float DotsScale                 = 0.85f;
                float DotsIntensity             = 1.0f;
                bool  DotsOverrideColors        = false;
                Color DotsLColor                = Color(0.12f, 0.75f, 1.00f, 1.00f);
                Color DotsRColor                = Color(0.12f, 0.75f, 1.00f, 1.00f);

                float BombsScale                = 1.0f;
                bool  BombsOverrideColor        = false;
                Color BombsColor                = Color(1.0000f, 0.0000f, 0.6469f, 1.0f);

                float ArcsIntensity             = 1.00f;
                bool  ArcsHaptics               = true;

                float BurstNotesDotsScale       = 1.5f;

                static std::shared_ptr<_Profile> AsDefault();

                bool IsDefault();

                CP_SDK_JSON_SERIALIZE_DECL();
                CP_SDK_JSON_UNSERIALIZE_DECL();
            };

            bool Enabled = false;

            int ActiveProfile = 1;

            std::vector<std::shared_ptr<_Profile>> Profiles;
            int Profiles_IndexOf(const std::shared_ptr<_Profile>& p_Profile)
            {
                auto l_It = std::find(Profiles.begin(), Profiles.end(), p_Profile);
                return l_It == Profiles.end() ? -1 : (l_It - Profiles.begin());
            }

        public:
            /// @brief Get active profile
            std::shared_ptr<_Profile>& GetActiveProfile();

        protected:
            /// @brief Reset config to default
            void Reset_Impl() override;

        protected:
            /// @brief Write the document
            /// @param p_Document Target
            CP_SDK_JSON_SERIALIZE_DECL() override;
            /// @brief Read the document
            /// @param p_Document Source
            CP_SDK_JSON_UNSERIALIZE_DECL() override;

        protected:
            /// @brief Get relative config path
            std::filesystem::path GetRelativePath() override;

        protected:
            /// @brief On config init
            /// @param p_OnCreation On creation
            void OnInit(bool p_OnCreation) override;

    };

}   ///< namespace QBeatSaberPlus_NoteTweaker