#pragma once

#include "../NTConfig.hpp"

#include <CP_SDK/Utils/Il2cpp.hpp>

#include <UnityEngine/Vector3.hpp>

namespace QBeatSaberPlus_NoteTweaker::Patches {

    namespace _u
    {
        using namespace UnityEngine;
    }

    /// @brief ColorNoteVisuals patch
    class PColorNoteVisuals
    {
        CP_SDK_NO_DEF_CTORS(PColorNoteVisuals);

        public:
            /// @brief Set from configuration
            static void SetFromConfig(bool p_OnSceneSwitch);
            static void SetBlockColorOverride(bool p_Enabled, _u::Color p_Left, _u::Color p_Right);

        private:
            static void SetArrowScaleFromConfig(std::shared_ptr<NTConfig::_Profile>& p_Profile);
            static void SetArrowColorsFromConfig(std::shared_ptr<NTConfig::_Profile>& p_Profile);

            static void SetDotScaleFromConfig(std::shared_ptr<NTConfig::_Profile>& p_Profile);
            static void SetDotColorsFromConfig(std::shared_ptr<NTConfig::_Profile>& p_Profile);

        private:
            /// @brief Init specific data and register events
            static void Init();

    };

}   ///< namespace QBeatSaberPlus_NoteTweaker::Patches