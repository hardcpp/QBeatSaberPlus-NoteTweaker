#pragma once

#include <CP_SDK/Utils/Il2cpp.hpp>

#include <UnityEngine/MonoBehaviour.hpp>
#include <UnityEngine/Color.hpp>

namespace QBeatSaberPlus_NoteTweaker::Patches {

    namespace _v
    {
        using namespace UnityEngine;
    }

    /// @brief BombNoteController patch
    class PBombNoteController
    {
        CP_SDK_NO_DEF_CTORS(PBombNoteController);

        public:
            /// @brief Set from configuration
            /// @param p_OnSceneSwitch Reset on scene switch
            static void SetFromConfig(bool p_OnSceneSwitch);
            /// @brief Set temp config
            /// @param p_Enabled Is it enabled
            /// @param p_Scale   New scale
            static void SetTemp(bool p_Enabled, float p_Scale);
            /// @brief Set bomb color override
            /// @param p_Enabled  Is override enabled?
            /// @param p_NewColor New color
            static void SetBombColorOverride(bool p_Enabled, _v::Color p_NewColor);

        private:
            /// @brief Is note scaling enabled
            static bool IsScaleAllowed();
            /// @brief Filter note scaling
            /// @param p_Scale Input scale
            static float FilterScale(float p_Scale);

        private:
            /// @brief Init specific data and register events
            static void Init();

    };

}   ///< namespace QBeatSaberPlus_NoteTweaker::Patches