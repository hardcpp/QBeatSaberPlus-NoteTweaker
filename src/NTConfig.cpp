#include "NTConfig.hpp"

#include <CP_SDK/ChatPlexSDK.hpp>

using namespace UnityEngine;

namespace QBeatSaberPlus_NoteTweaker {

    CP_SDK_CONFIG_JSONCONFIG_INSTANCE_IMPL(NTConfig);

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Reset config to default
    void NTConfig::Reset_Impl()
    {

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Write the document
    /// @param p_Document Target
    CP_SDK_JSON_SERIALIZE_IMPL(NTConfig)
    {
        CP_SDK_JSON_SERIALIZE_BOOL(Enabled);

        CP_SDK_JSON_SERIALIZE_INT32(ActiveProfile);

        CP_SDK_JSON_SERIALIZE_OBJECT_ARRAY(Profiles);
    }
    /// @brief Read the document
    /// @param p_Document Source
    CP_SDK_JSON_UNSERIALIZE_IMPL(NTConfig)
    {
        CP_SDK_JSON_UNSERIALIZE_BOOL(Enabled);

        CP_SDK_JSON_UNSERIALIZE_INT32(ActiveProfile);

        CP_SDK_JSON_UNSERIALIZE_OBJECT_ARRAY(Profiles);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<NTConfig::_Profile> NTConfig::_Profile::AsDefault()
    {
        auto l_Result = std::make_shared<_Profile>();
        l_Result->Name                    = u"<i>============ Default ============</i>";
        l_Result->NotesScale              = 1.0f;
        l_Result->NotesShowPrecisonDots   = false;
        l_Result->DotsScale               = 1.0f;
        l_Result->BombsColor              = Color(0.251f, 0.251f, 0.251f, 1.000f);

        return l_Result;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    bool NTConfig::_Profile::IsDefault()
    {
        return Name == u"<i>============ Default ============</i>";
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    CP_SDK_JSON_SERIALIZE_IMPL(NTConfig::_Profile)
    {
        CP_SDK_JSON_SERIALIZE_STRING(Name);

        CP_SDK_JSON_SERIALIZE_FLOAT(NotesScale);
        CP_SDK_JSON_SERIALIZE_BOOL(NotesShowPrecisonDots);
        CP_SDK_JSON_SERIALIZE_FLOAT(NotesPrecisonDotsScale);

        CP_SDK_JSON_SERIALIZE_FLOAT(ArrowsScale);
        CP_SDK_JSON_SERIALIZE_FLOAT(ArrowsIntensity);
        CP_SDK_JSON_SERIALIZE_BOOL(ArrowsOverrideColors);
        CP_SDK_JSON_SERIALIZE_COLOR(ArrowsLColor);
        CP_SDK_JSON_SERIALIZE_COLOR(ArrowsRColor);

        CP_SDK_JSON_SERIALIZE_FLOAT(DotsScale);
        CP_SDK_JSON_SERIALIZE_FLOAT(DotsIntensity);
        CP_SDK_JSON_SERIALIZE_BOOL(DotsOverrideColors);
        CP_SDK_JSON_SERIALIZE_COLOR(DotsLColor);
        CP_SDK_JSON_SERIALIZE_COLOR(DotsRColor);

        CP_SDK_JSON_SERIALIZE_FLOAT(BombsScale);
        CP_SDK_JSON_SERIALIZE_BOOL(BombsOverrideColor);
        CP_SDK_JSON_SERIALIZE_COLOR(BombsColor);

        CP_SDK_JSON_SERIALIZE_FLOAT(ArcsIntensity);
        CP_SDK_JSON_SERIALIZE_BOOL(ArcsHaptics);

        CP_SDK_JSON_SERIALIZE_FLOAT(BurstNotesDotsScale);
    }
    CP_SDK_JSON_UNSERIALIZE_IMPL(NTConfig::_Profile)
    {
        CP_SDK_JSON_UNSERIALIZE_STRING(Name);

        CP_SDK_JSON_UNSERIALIZE_FLOAT(NotesScale);
        CP_SDK_JSON_UNSERIALIZE_BOOL(NotesShowPrecisonDots);
        CP_SDK_JSON_UNSERIALIZE_FLOAT(NotesPrecisonDotsScale);

        CP_SDK_JSON_UNSERIALIZE_FLOAT(ArrowsScale);
        CP_SDK_JSON_UNSERIALIZE_FLOAT(ArrowsIntensity);
        CP_SDK_JSON_UNSERIALIZE_BOOL(ArrowsOverrideColors);
        CP_SDK_JSON_UNSERIALIZE_COLOR(ArrowsLColor);
        CP_SDK_JSON_UNSERIALIZE_COLOR(ArrowsRColor);

        CP_SDK_JSON_UNSERIALIZE_FLOAT(DotsScale);
        CP_SDK_JSON_UNSERIALIZE_FLOAT(DotsIntensity);
        CP_SDK_JSON_UNSERIALIZE_BOOL(DotsOverrideColors);
        CP_SDK_JSON_UNSERIALIZE_COLOR(DotsLColor);
        CP_SDK_JSON_UNSERIALIZE_COLOR(DotsRColor);

        CP_SDK_JSON_UNSERIALIZE_FLOAT(BombsScale);
        CP_SDK_JSON_UNSERIALIZE_BOOL(BombsOverrideColor);
        CP_SDK_JSON_UNSERIALIZE_COLOR(BombsColor);

        CP_SDK_JSON_UNSERIALIZE_FLOAT(ArcsIntensity);
        CP_SDK_JSON_UNSERIALIZE_BOOL(ArcsHaptics);

        CP_SDK_JSON_UNSERIALIZE_FLOAT(BurstNotesDotsScale);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Get active profile
    std::shared_ptr<NTConfig::_Profile>& NTConfig::GetActiveProfile()
    {
        if (Profiles.size() == 0)
        {
            Profiles.push_back(_Profile::AsDefault());
            Profiles.push_back(std::make_shared<_Profile>());
            Save();
        }

        if (ActiveProfile < 0 || ActiveProfile >= Profiles.size())
        {
            ActiveProfile %= Profiles.size();
            Save();
        }

        return Profiles[ActiveProfile];
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Get relative config path
    std::filesystem::path NTConfig::GetRelativePath()
    {
        return std::filesystem::path("QBeatSaberPlus") / "NoteTweaker" / "Config";
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief On config init
    /// @param p_OnCreation On creation
    void NTConfig::OnInit(bool p_OnCreation)
    {
        if (Profiles.size() == 0)
        {
            Profiles.push_back(_Profile::AsDefault());
            Profiles.push_back(std::make_shared<_Profile>());
        }
        else
        {
            Profiles.erase(std::remove_if(Profiles.begin(), Profiles.end(), [](const std::shared_ptr<_Profile>& p_Profile) { return p_Profile->IsDefault(); }), Profiles.end());
            Profiles.insert(Profiles.begin(), _Profile::AsDefault());
        }
    }

}   ///< namespace QBeatSaberPlus_NoteTweaker