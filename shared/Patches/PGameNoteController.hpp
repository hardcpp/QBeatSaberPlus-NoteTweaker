#pragma once

#include <CP_SDK/Utils/Il2cpp.hpp>

namespace QBeatSaberPlus_NoteTweaker::Patches {

    /// @brief GameNoteController Patch
    class PGameNoteController
    {
        DISABLE_CONSTRUCTORS(PGameNoteController);

        public:
            /// @brief Set from configuration
            /// @param p_OnSceneSwitch Reset on scene switch
            static void SetFromConfig(bool p_OnSceneSwitch);
            /// @brief Set temp config
            /// @param p_Enabled Is it enabled
            /// @param p_Scale   New scale
            static void SetTemp(bool p_Enabled, float p_Scale);

        private:
            /// @brief Is note scaling enabled
            static bool IsScaleAllowed();
            /// @brief Filter note scaling
            /// @param p_Scale Input scale
            static float FilterScale(float p_Scale);

    };

}   ///< namespace QBeatSaberPlus_NoteTweaker::Patches