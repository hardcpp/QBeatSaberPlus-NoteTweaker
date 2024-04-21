#include "Patches/PColorNoteVisuals.hpp"
#include "NTConfig.hpp"
#include "Logger.hpp"

#include <CP_SDK/Unity/Extensions/ColorU.hpp>
#include <CP_SDK/Utils/MonoPtr.hpp>
#include <CP_SDK/ChatPlexSDK.hpp>
#include <CP_SDK_BS/Game/LevelData.hpp>
#include <CP_SDK_BS/Game/Logic.hpp>

#include <conditional-dependencies/shared/main.hpp>
#include <GlobalNamespace/BurstSliderGameNoteController.hpp>
#include <GlobalNamespace/ColorManager.hpp>
#include <GlobalNamespace/ColorNoteVisuals.hpp>
#include <GlobalNamespace/MaterialPropertyBlockController.hpp>
#include <GlobalNamespace/NoteControllerBase.hpp>
#include <GlobalNamespace/NoteData.hpp>
#include <System/Collections/Generic/List_1.hpp>
#include <UnityEngine/GameObject.hpp>
#include <UnityEngine/MaterialPropertyBlock.hpp>
#include <UnityEngine/Material.hpp>
#include <UnityEngine/MeshRenderer.hpp>
#include <UnityEngine/Shader.hpp>
#include <UnityEngine/Transform.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"

using namespace CP_SDK::Unity::Extensions;
using namespace GlobalNamespace;
using namespace System::Collections::Generic;
using namespace UnityEngine;

namespace QBeatSaberPlus_NoteTweaker::Patches {

    EXPOSE_API(Patches__PColorNoteVisuals__SetBlockColorOverride, void, bool __a, Color __b, Color __c)
    {
        PColorNoteVisuals::SetBlockColorOverride(__a, __b, __c);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    constexpr int PColorNoteVisualsCache_MaxSubArray = 10;
    struct PColorNoteVisualsCache
    {
        ColorNoteVisuals*                   colorNoteVisuals;
        BurstSliderGameNoteController*      burstSliderGameNoteController;
        MaterialPropertyBlockController*    materialPropertyBlockControllers[PColorNoteVisualsCache_MaxSubArray];
        Transform*                          noteArrows[PColorNoteVisualsCache_MaxSubArray];
        Transform*                          noteArrowGlows[PColorNoteVisualsCache_MaxSubArray];
        MaterialPropertyBlockController*    noteArrowGlowsMaterialPropertyBlockControllers[PColorNoteVisualsCache_MaxSubArray];
    };

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    static bool     PColorNoteVisuals_Enabled               = false;
    static bool     PColorNoteVisuals_BlockColorsEnabled    = false;
    static int32_t  PColorNoteVisuals_ColorID               = 0;
    static Vector3  PColorNoteVisuals_ArrowScale;
    static Vector3  PColorNoteVisuals_ArrowGlowScale;
    static bool     PColorNoteVisuals_OverrideArrowColors;
    static float    PColorNoteVisuals_ArrowAlpha;
    static Color    PColorNoteVisuals_LeftArrowColor;
    static Color    PColorNoteVisuals_RightArrowColor;
    static bool     PColorNoteVisuals_CircleEnabled;
    static bool     PColorNoteVisuals_CircleForceEnabled;
    static Vector3  PColorNoteVisuals_CircleScale;
    static Vector3  PColorNoteVisuals_BurstCircleScale;
    static Vector3  PColorNoteVisuals_PrecisionCircleScale;
    static bool     PColorNoteVisuals_OverrideDotColors;
    static float    PColorNoteVisuals_DotAlpha;
    static Color    PColorNoteVisuals_LeftCircleColor;
    static Color    PColorNoteVisuals_RightCircleColor;
    static Color    PColorNoteVisuals_LeftBlockColor;
    static Color    PColorNoteVisuals_RightBlockColor;

    static bool                                         PColorNoteVisuals_WasInit = false;
    static std::vector<PColorNoteVisualsCache>          PColorNoteVisuals_Cache;
    static CP_SDK::Utils::MonoPtr<List_1<Component*>>   PColorNoteVisuals_ComponentsCache;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Set from configuration
    void PColorNoteVisuals::SetFromConfig(bool p_OnSceneSwitch)
    {
        Init();

        auto& l_Profile = NTConfig::Instance()->GetActiveProfile();

        PColorNoteVisuals_Enabled               = NTConfig::Instance()->Enabled;
        SetArrowScaleFromConfig(l_Profile);
        SetArrowColorsFromConfig(l_Profile);
        PColorNoteVisuals_CircleEnabled         = NTConfig::Instance()->Enabled ? l_Profile->DotsIntensity != 0.0f   : true;
        PColorNoteVisuals_CircleForceEnabled    = NTConfig::Instance()->Enabled ? l_Profile->NotesShowPrecisonDots   : false;
        SetDotScaleFromConfig(l_Profile);
        SetDotColorsFromConfig(l_Profile);
    }
    void PColorNoteVisuals::SetBlockColorOverride(bool p_Enabled, Color p_Left, Color p_Right)
    {
        Init();

        PColorNoteVisuals_BlockColorsEnabled    = p_Enabled;
        PColorNoteVisuals_LeftBlockColor        = ColorU::WithAlpha(p_Left,  1.0f);
        PColorNoteVisuals_RightBlockColor       = ColorU::WithAlpha(p_Right, 1.0f);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    void PColorNoteVisuals::SetArrowScaleFromConfig(std::shared_ptr<NTConfig::_Profile>& p_Profile)
    {
        PColorNoteVisuals_ArrowScale        = Vector3::op_Multiply((NTConfig::Instance()->Enabled ? p_Profile->ArrowsScale : 1.0f), Vector3::get_one());
        PColorNoteVisuals_ArrowGlowScale    = Vector3::op_Multiply((NTConfig::Instance()->Enabled ? p_Profile->ArrowsScale : 1.0f), Vector3(0.6f, 0.3f, 0.6f));
    }
    void PColorNoteVisuals::SetArrowColorsFromConfig(std::shared_ptr<NTConfig::_Profile>& p_Profile)
    {
        PColorNoteVisuals_OverrideArrowColors   = NTConfig::Instance()->Enabled ? p_Profile->ArrowsOverrideColors    : false;
        PColorNoteVisuals_ArrowAlpha            = NTConfig::Instance()->Enabled ? p_Profile->ArrowsIntensity         : 0.6f;
        PColorNoteVisuals_LeftArrowColor        = NTConfig::Instance()->Enabled ? p_Profile->ArrowsLColor            : Color(0.659f, 0.125f, 0.125f, 1.000f);
        PColorNoteVisuals_RightArrowColor       = NTConfig::Instance()->Enabled ? p_Profile->ArrowsRColor            : Color(0.125f, 0.392f, 0.659f, 1.000f);
    }

    void PColorNoteVisuals::SetDotScaleFromConfig(std::shared_ptr<NTConfig::_Profile>& p_Profile)
    {
        PColorNoteVisuals_CircleScale           = Vector3::op_Multiply((NTConfig::Instance()->Enabled ? p_Profile->DotsScale              : 1.0f), Vector3(0.5f, 0.5f, 0.5f));
        PColorNoteVisuals_BurstCircleScale      = Vector3::op_Multiply((NTConfig::Instance()->Enabled ? p_Profile->BurstNotesDotsScale    : 1.0f), Vector3(0.1f, 0.1f, 0.1f));
        PColorNoteVisuals_PrecisionCircleScale  = Vector3::op_Multiply((NTConfig::Instance()->Enabled ? p_Profile->NotesPrecisonDotsScale : 1.0f), Vector3(0.5f, 0.5f, 0.5f));
    }
    void PColorNoteVisuals::SetDotColorsFromConfig(std::shared_ptr<NTConfig::_Profile>& p_Profile)
    {
        PColorNoteVisuals_OverrideDotColors = NTConfig::Instance()->Enabled ? p_Profile->DotsOverrideColors  : false;
        PColorNoteVisuals_DotAlpha          = NTConfig::Instance()->Enabled ? p_Profile->DotsIntensity       : 1.0f;
        PColorNoteVisuals_LeftCircleColor   = NTConfig::Instance()->Enabled ? p_Profile->DotsLColor          : Color(0.659f, 0.125f, 0.125f, 1.000f);
        PColorNoteVisuals_RightCircleColor  = NTConfig::Instance()->Enabled ? p_Profile->DotsRColor          : Color(0.125f, 0.392f, 0.659f, 1.000f);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Init specific data and register events
    void PColorNoteVisuals::Init()
    {
        if (PColorNoteVisuals_Cache.capacity() < 100)
            PColorNoteVisuals_Cache.reserve(100);

        if (!PColorNoteVisuals_ComponentsCache)
            PColorNoteVisuals_ComponentsCache = List_1<Component*>::New_ctor(10);

        if (PColorNoteVisuals_ColorID == 0)
            PColorNoteVisuals_ColorID = Shader::PropertyToID("_SimpleColor");

        if (!PColorNoteVisuals_WasInit)
        {
            CP_SDK::ChatPlexSDK::OnGenericSceneChange += [](CP_SDK::EGenericScene x)
            {
                if (x != CP_SDK::EGenericScene::Menu)
                    return;

                if (!PColorNoteVisuals_Cache.empty())
                    PColorNoteVisuals_Cache.clear();
            };

            PColorNoteVisuals_WasInit = true;
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    CP_SDK_IL2CPP_HOOK_MAKE_AUTO_HOOK_ORIG_MATCH(
        ColorNoteVisuals_HandleNoteControllerDidInit, &ColorNoteVisuals::HandleNoteControllerDidInit,
        void, ColorNoteVisuals* __Instance,

        NoteControllerBase* __a)
    {
        ColorNoteVisuals_HandleNoteControllerDidInit(__Instance, __a);

        auto l_Cache      = (PColorNoteVisualsCache*)nullptr;
        auto l_CacheCount = PColorNoteVisuals_Cache.size();
        for (auto l_I = 0; l_I < l_CacheCount; ++l_I)
        {
            l_Cache = &PColorNoteVisuals_Cache[l_I];
            if (l_Cache->colorNoteVisuals == __Instance)
                break;
        }

        if (!PColorNoteVisuals_WasInit)
            PColorNoteVisuals::Init();

        if (!l_Cache || l_Cache->colorNoteVisuals != __Instance)
        {
            PColorNoteVisuals_Cache.push_back({
                __Instance,
                __Instance->GetComponent<BurstSliderGameNoteController*>(),
                {},
                {},
                {}
            });
            l_Cache = &PColorNoteVisuals_Cache.back();

            auto l_MaterialPropertyBlockControllersIt               = 0;
            auto l_NoteArrowsIt                                     = 0;
            auto l_NoteArrowGlowsIt                                 = 0;
            auto l_NoteArrowGlowsMaterialPropertyBlockControllersIt = 0;

            for (auto l_CurrentBlock : __Instance->____materialPropertyBlockControllers)
            {
                if (!l_CurrentBlock->____materialPropertyBlock)
                    l_CurrentBlock->____materialPropertyBlock = MaterialPropertyBlock::New_ctor();

                l_Cache->materialPropertyBlockControllers[l_MaterialPropertyBlockControllersIt++] = l_CurrentBlock;

                if (l_MaterialPropertyBlockControllersIt >= PColorNoteVisualsCache_MaxSubArray)
                    Logger::Instance->Error(u"[Patches][ColorNoteVisuals_HandleNoteControllerDidInit] Limit reached for l_MaterialPropertyBlockControllersIt");
            }

            for (auto l_CurrentArrow : __Instance->____arrowMeshRenderers)
            {
                l_Cache->noteArrows[l_NoteArrowsIt++] = l_CurrentArrow->get_transform();
                if (l_NoteArrowsIt >= PColorNoteVisualsCache_MaxSubArray)
                    Logger::Instance->Error(u"[Patches][ColorNoteVisuals_HandleNoteControllerDidInit] Limit reached for l_NoteArrowsIt");

                auto l_Glow = l_CurrentArrow->get_transform()->get_parent()->Find("NoteArrowGlow");
                if (l_Glow)
                {
                    PColorNoteVisuals_ComponentsCache->Clear();

                    l_Cache->noteArrowGlows[l_NoteArrowGlowsIt++] = l_Glow;
                    l_Glow->GetComponentsForListInternal(reinterpret_cast<System::Type*>(csTypeOf(MaterialPropertyBlockController*).convert()), PColorNoteVisuals_ComponentsCache.Ptr());

                    if (l_NoteArrowGlowsIt >= PColorNoteVisualsCache_MaxSubArray)
                        Logger::Instance->Error(u"[Patches][ColorNoteVisuals_HandleNoteControllerDidInit] Limit reached for l_NoteArrowGlowsIt");

                    auto l_Count = PColorNoteVisuals_ComponentsCache->get_Count();
                    auto l_Items = PColorNoteVisuals_ComponentsCache->____items->_values;
                    for (auto l_I = 0; l_I < l_Count; ++l_I)
                    {
                        auto l_CurrentBlock = reinterpret_cast<MaterialPropertyBlockController*>(l_Items[l_I]);
                        if (!l_CurrentBlock->____materialPropertyBlock)
                            l_CurrentBlock->____materialPropertyBlock = MaterialPropertyBlock::New_ctor();

                        l_Cache->noteArrowGlowsMaterialPropertyBlockControllers[l_NoteArrowGlowsMaterialPropertyBlockControllersIt++] = l_CurrentBlock;

                        if (l_NoteArrowGlowsMaterialPropertyBlockControllersIt >= PColorNoteVisualsCache_MaxSubArray)
                            Logger::Instance->Error(u"[Patches][ColorNoteVisuals_HandleNoteControllerDidInit] Limit reached for l_NoteArrowGlowsMaterialPropertyBlockControllersIt");
                    }
                }
            }

            l_Cache->materialPropertyBlockControllers              [l_MaterialPropertyBlockControllersIt++              ] = nullptr;
            l_Cache->noteArrows                                    [l_NoteArrowsIt++                                    ] = nullptr;
            l_Cache->noteArrowGlows                                [l_NoteArrowGlowsIt++                                ] = nullptr;
            l_Cache->noteArrowGlowsMaterialPropertyBlockControllers[l_NoteArrowGlowsMaterialPropertyBlockControllersIt++] = nullptr;
        }

        ////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////

        auto l_NoteData         = __Instance->____noteController->get_noteData();
        auto l_ColorType        = l_NoteData->get_colorType();
        auto l_CutDirection     = l_NoteData->get_cutDirection();
        auto l_OverrideColor    = l_ColorType == ColorType::ColorA ? PColorNoteVisuals_LeftBlockColor : PColorNoteVisuals_RightBlockColor;

        if (PColorNoteVisuals_BlockColorsEnabled)
        {
            for (auto l_Current : l_Cache->materialPropertyBlockControllers)
            {
                if (!l_Current) break;
                l_Current->____materialPropertyBlock->SetColor(PColorNoteVisuals_ColorID, l_OverrideColor);
                l_Current->ApplyChanges();
            }

            if (!PColorNoteVisuals_Enabled)
            {
                auto l_NoteArrowGlowColor = ColorU::WithAlpha(l_OverrideColor, 0.6f);
                for (auto l_Current : l_Cache->noteArrowGlowsMaterialPropertyBlockControllers)
                {
                    if (!l_Current) break;
                    l_Current->____materialPropertyBlock->SetColor(PColorNoteVisuals_ColorID, l_NoteArrowGlowColor);
                    l_Current->ApplyChanges();
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////

        if (!PColorNoteVisuals_Enabled)
            return;

        auto l_BaseColor    = __Instance->____colorManager->ColorForType(l_ColorType);
        auto l_ArrowColor   = ColorU::WithAlpha(PColorNoteVisuals_OverrideArrowColors ? (l_ColorType == ColorType::ColorB ? PColorNoteVisuals_RightArrowColor  : PColorNoteVisuals_LeftArrowColor) : l_BaseColor, PColorNoteVisuals_ArrowAlpha);

        if (PColorNoteVisuals_BlockColorsEnabled)
            l_ArrowColor = ColorU::WithAlpha(l_OverrideColor, 0.6f);

        for (auto l_Current : l_Cache->noteArrows)
        {
            if (!l_Current) break;
            l_Current->set_localScale(PColorNoteVisuals_ArrowScale);
        }
        for (auto l_Current : l_Cache->noteArrowGlows)
        {
            if (!l_Current) break;
            l_Current->set_localScale(PColorNoteVisuals_ArrowGlowScale);
        }
        for (auto l_Current : l_Cache->noteArrowGlowsMaterialPropertyBlockControllers)
        {
            if (!l_Current) break;
            l_Current->____materialPropertyBlock->SetColor(PColorNoteVisuals_ColorID, l_ArrowColor);
            l_Current->ApplyChanges();
        }

        auto l_IsBurstNote  = l_Cache->burstSliderGameNoteController != nullptr;
        auto l_CircleScale  = l_IsBurstNote ? PColorNoteVisuals_BurstCircleScale : (l_CutDirection == NoteCutDirection::Any ? PColorNoteVisuals_CircleScale : PColorNoteVisuals_PrecisionCircleScale);
        auto l_DotEnabled   = l_CutDirection == NoteCutDirection::Any ? PColorNoteVisuals_CircleEnabled : (PColorNoteVisuals_CircleEnabled && PColorNoteVisuals_CircleForceEnabled);
        auto l_DotColor     = ColorU::WithAlpha(PColorNoteVisuals_OverrideDotColors ? (l_ColorType == ColorType::ColorB ? PColorNoteVisuals_RightCircleColor : PColorNoteVisuals_LeftCircleColor) : l_BaseColor, PColorNoteVisuals_DotAlpha);

        for (auto l_CurrentRenderer : __Instance->____circleMeshRenderers)
        {
            l_CurrentRenderer->set_enabled                    (l_DotEnabled);
            l_CurrentRenderer->get_transform()->set_localScale(l_CircleScale);
            l_CurrentRenderer->get_material()->set_color      (l_DotColor);
        }
    }

}   ///< namespace QBeatSaberPlus_NoteTweaker::Patches