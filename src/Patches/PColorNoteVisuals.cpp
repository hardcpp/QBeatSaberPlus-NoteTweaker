#include "Patches/PColorNoteVisuals.hpp"
#include "CP_SDK/Utils/Il2cpp.hpp"
#include "NTConfig.hpp"

#include <CP_SDK/Unity/Extensions/ColorU.hpp>
#include <CP_SDK/Unity/Operators.hpp>
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
#include <System/Array.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"

using namespace CP_SDK::Unity::Extensions;
using namespace CP_SDK::Utils;
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
        using Ptr = std::shared_ptr<PColorNoteVisualsCache>;

        bool isBurstNote = false;

        MonoPtr<Array<Transform*>> arrowMeshRenderersTransforms;
        MonoPtr<Array<Transform*>> arrowGlowTransforms;
        MonoPtr<Array<MaterialPropertyBlockController*>> arrowGlowMaterialPropertyBlockControllers;

        MonoPtr<Array<Transform*>> circleMeshRenderersTransforms;
        MonoPtr<Array<MaterialPropertyBlockController*>> circleMaterialPropertyBlockControllers;
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

    static std::map<int, PColorNoteVisualsCache::Ptr>   PColorNoteVisuals_Cache;

    static MonoPtr<List_1<Component*>>                       PColorNoteVisuals_ComponentsCache;
    static MonoPtr<List_1<Transform*>>                       PColorNoteVisuals_TransformCacheA;
    static MonoPtr<List_1<Transform*>>                       PColorNoteVisuals_TransformCacheB;
    static MonoPtr<List_1<MaterialPropertyBlockController*>> PColorNoteVisuals_ArrowGlowMaterialPropertyBlockControllers;
    static MonoPtr<List_1<MaterialPropertyBlockController*>> PColorNoteVisuals_CircleMaterialPropertyBlockControllers;

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
        PColorNoteVisuals_ArrowScale        = (NTConfig::Instance()->Enabled ? p_Profile->ArrowsScale : 1.0f) * Vector3::get_one();
        PColorNoteVisuals_ArrowGlowScale    = (NTConfig::Instance()->Enabled ? p_Profile->ArrowsScale : 1.0f) * Vector3(0.6f, 0.3f, 0.6f);
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
        PColorNoteVisuals_CircleScale           = (NTConfig::Instance()->Enabled ? p_Profile->DotsScale              : 1.0f) * Vector3(0.5f, 0.5f, 0.5f);
        PColorNoteVisuals_BurstCircleScale      = (NTConfig::Instance()->Enabled ? p_Profile->BurstNotesDotsScale    : 1.0f) * Vector3(0.1f, 0.1f, 0.1f);
        PColorNoteVisuals_PrecisionCircleScale  = (NTConfig::Instance()->Enabled ? p_Profile->NotesPrecisonDotsScale : 1.0f) * Vector3(0.5f, 0.5f, 0.5f);
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
        if (!PColorNoteVisuals_ComponentsCache)
            PColorNoteVisuals_ComponentsCache = List_1<Component*>::New_ctor(10);

        if (!PColorNoteVisuals_TransformCacheA)
            PColorNoteVisuals_TransformCacheA = List_1<Transform*>::New_ctor(10);

        if (!PColorNoteVisuals_TransformCacheB)
            PColorNoteVisuals_TransformCacheB = List_1<Transform*>::New_ctor(10);

        if (!PColorNoteVisuals_ArrowGlowMaterialPropertyBlockControllers)
            PColorNoteVisuals_ArrowGlowMaterialPropertyBlockControllers = List_1<MaterialPropertyBlockController *>::New_ctor(10);

        if (!PColorNoteVisuals_CircleMaterialPropertyBlockControllers)
            PColorNoteVisuals_CircleMaterialPropertyBlockControllers = List_1<MaterialPropertyBlockController *>::New_ctor(10);

        if (PColorNoteVisuals_ColorID == 0)
            PColorNoteVisuals_ColorID = Shader::PropertyToID("_Color");

        if (!PColorNoteVisuals_WasInit)
        {
            CP_SDK::ChatPlexSDK::OnGenericSceneChange += [](CP_SDK::EGenericScene x)
            {
                if (x != CP_SDK::EGenericScene::Menu)
                    return;

                if (!PColorNoteVisuals_Cache.empty())
                    PColorNoteVisuals_Cache.clear();

                PColorNoteVisuals_ComponentsCache->Clear();
                PColorNoteVisuals_TransformCacheA->Clear();
                PColorNoteVisuals_TransformCacheB->Clear();
                PColorNoteVisuals_ArrowGlowMaterialPropertyBlockControllers->Clear();
                PColorNoteVisuals_CircleMaterialPropertyBlockControllers->Clear();
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

        if (!PColorNoteVisuals_WasInit)
            PColorNoteVisuals::Init();

        auto instanceID = __Instance->GetInstanceID();
        auto cached      = std::shared_ptr<PColorNoteVisualsCache>();

        if (PColorNoteVisuals_Cache.contains(instanceID))
            cached = PColorNoteVisuals_Cache[instanceID];

        if (!cached)
        {
            cached = std::make_shared<PColorNoteVisualsCache>();
            cached->isBurstNote = __Instance->get_gameObject()->GetComponent<BurstSliderGameNoteController*>() != nullptr;

            auto arrowMeshRenderers = __Instance->_arrowMeshRenderers;
            PColorNoteVisuals_TransformCacheA->Clear();
            PColorNoteVisuals_TransformCacheB->Clear();
            PColorNoteVisuals_ArrowGlowMaterialPropertyBlockControllers->Clear();

            for (int i = 0; i < arrowMeshRenderers.size(); ++i)
            {
                PColorNoteVisuals_TransformCacheA->Add(arrowMeshRenderers[i]->transform);

                auto glowTransform = arrowMeshRenderers[i]->transform->get_parent()->Find("NoteArrowGlow");
                if (glowTransform)
                {
                    PColorNoteVisuals_TransformCacheB->Add(glowTransform);

                    PColorNoteVisuals_ComponentsCache->Clear();
                    glowTransform->GetComponentsForListInternal(reinterpret_cast<System::Type*>(csTypeOf(MaterialPropertyBlockController*).convert()), PColorNoteVisuals_ComponentsCache.Ptr());

                    if (PColorNoteVisuals_ComponentsCache->Count > 0)
                    {
                        auto l_Count = PColorNoteVisuals_ComponentsCache->get_Count();
                        auto l_Items = PColorNoteVisuals_ComponentsCache->____items->_values;
                        for (auto l_I = 0; l_I < l_Count; ++l_I)
                        {
                            auto l_CurrentBlock = reinterpret_cast<MaterialPropertyBlockController*>(l_Items[l_I]);
                            if (!l_CurrentBlock->_materialPropertyBlock)
                                l_CurrentBlock->_materialPropertyBlock = MaterialPropertyBlock::New_ctor();

                            PColorNoteVisuals_ArrowGlowMaterialPropertyBlockControllers->Add(l_CurrentBlock);
                        }
                    }
                }
            }

            if (PColorNoteVisuals_TransformCacheA->Count > 0)
                cached->arrowMeshRenderersTransforms = static_cast<Array<Transform*>*>(PColorNoteVisuals_TransformCacheA->ToArray());
            if (PColorNoteVisuals_TransformCacheB->Count > 0)
                cached->arrowGlowTransforms = static_cast<Array<Transform*>*>(PColorNoteVisuals_TransformCacheB->ToArray());
            if (PColorNoteVisuals_ArrowGlowMaterialPropertyBlockControllers->Count > 0)
                cached->arrowGlowMaterialPropertyBlockControllers =static_cast<Array<MaterialPropertyBlockController*>*>(PColorNoteVisuals_ArrowGlowMaterialPropertyBlockControllers->ToArray());

            // =====

            auto circleMeshRenderers = __Instance->_circleMeshRenderers;
            PColorNoteVisuals_TransformCacheA->Clear();
            PColorNoteVisuals_CircleMaterialPropertyBlockControllers->Clear();

            for (int i = 0; i < circleMeshRenderers.size(); ++i)
            {
                PColorNoteVisuals_TransformCacheA->Add(circleMeshRenderers[i]->transform);

                PColorNoteVisuals_ComponentsCache->Clear();
                circleMeshRenderers[i]->GetComponentsForListInternal(reinterpret_cast<System::Type*>(csTypeOf(MaterialPropertyBlockController*).convert()), PColorNoteVisuals_ComponentsCache.Ptr());

                if (PColorNoteVisuals_ComponentsCache->Count > 0)
                {
                    auto l_Count = PColorNoteVisuals_ComponentsCache->get_Count();
                    auto l_Items = PColorNoteVisuals_ComponentsCache->____items->_values;
                    for (auto l_I = 0; l_I < l_Count; ++l_I)
                    {
                        auto l_CurrentBlock = reinterpret_cast<MaterialPropertyBlockController*>(l_Items[l_I]);
                        if (!l_CurrentBlock->_materialPropertyBlock)
                            l_CurrentBlock->_materialPropertyBlock = MaterialPropertyBlock::New_ctor();

                        PColorNoteVisuals_CircleMaterialPropertyBlockControllers->Add(l_CurrentBlock);
                    }
                }
            }

            if (PColorNoteVisuals_TransformCacheA->Count > 0)
                cached->circleMeshRenderersTransforms = static_cast<Array<Transform*>*>(PColorNoteVisuals_TransformCacheA->ToArray());
            if (PColorNoteVisuals_CircleMaterialPropertyBlockControllers->Count > 0)
                cached->circleMaterialPropertyBlockControllers = static_cast<Array<MaterialPropertyBlockController*>*>(PColorNoteVisuals_CircleMaterialPropertyBlockControllers->ToArray());

            // =====

            PColorNoteVisuals_Cache[instanceID] = cached;
        }

        auto colorType = __Instance->_noteController->get_noteData()->get_colorType();
        if (PColorNoteVisuals_BlockColorsEnabled)
        {
            auto blockColor = colorType == ColorType::ColorA ? PColorNoteVisuals_LeftBlockColor : PColorNoteVisuals_RightBlockColor;

            for (auto &block : __Instance->_materialPropertyBlockControllers)
            {
                block->_materialPropertyBlock->SetColor(PColorNoteVisuals_ColorID, blockColor);
                block->ApplyChanges();
            }


            if (!PColorNoteVisuals_Enabled && cached->arrowGlowMaterialPropertyBlockControllers)
            {
                auto newArrowGlowColor = ColorU::WithAlpha(blockColor, 0.6f);
                for (auto &currentBlock : *cached->arrowGlowMaterialPropertyBlockControllers.Ptr())
                {
                    currentBlock->materialPropertyBlock->SetColor(PColorNoteVisuals_ColorID, newArrowGlowColor);
                    currentBlock->ApplyChanges();
                }
            }
        }

        if (!PColorNoteVisuals_Enabled)
            return;

        auto cutDirection   = __Instance->_noteController->get_noteData()->get_cutDirection();
        auto dotEnabled     = cutDirection == NoteCutDirection::Any ? PColorNoteVisuals_CircleEnabled : (PColorNoteVisuals_CircleEnabled && PColorNoteVisuals_CircleForceEnabled);
        auto baseColor      = __Instance->_colorManager->ColorForType(colorType);
        auto isRight        = colorType == ColorType::ColorB;

        // =====

        auto arrowColor = ColorU::WithAlpha(PColorNoteVisuals_OverrideArrowColors ? (isRight ? PColorNoteVisuals_RightArrowColor : PColorNoteVisuals_LeftArrowColor) : baseColor, PColorNoteVisuals_ArrowAlpha);

        if (PColorNoteVisuals_BlockColorsEnabled)
            arrowColor = ColorU::WithAlpha(isRight ? PColorNoteVisuals_RightBlockColor : PColorNoteVisuals_LeftBlockColor, 0.6f);

        if (cached->arrowMeshRenderersTransforms)
        {
            for (auto currentMesh: *cached->arrowMeshRenderersTransforms.Ptr())
                currentMesh->localScale = PColorNoteVisuals_ArrowScale;
        }

        if (cached->arrowGlowTransforms)
        {
            for (auto currentTransform: *cached->arrowGlowTransforms.Ptr())
                currentTransform->localScale = PColorNoteVisuals_ArrowGlowScale;
        }

        if (cached->arrowGlowMaterialPropertyBlockControllers)
        {
            for (auto currentBlock : *cached->arrowGlowMaterialPropertyBlockControllers.Ptr())
            {
                currentBlock->materialPropertyBlock->SetColor(PColorNoteVisuals_ColorID, arrowColor);
                currentBlock->ApplyChanges();
            }
        }

        // =====

        auto dotColor = ColorU::WithAlpha(PColorNoteVisuals_OverrideDotColors ? (isRight ? PColorNoteVisuals_RightCircleColor : PColorNoteVisuals_LeftCircleColor) : baseColor, PColorNoteVisuals_DotAlpha);
        auto circleScale = cached->isBurstNote ? PColorNoteVisuals_BurstCircleScale : (cutDirection == NoteCutDirection::Any ? PColorNoteVisuals_CircleScale : PColorNoteVisuals_PrecisionCircleScale);

        if (cached->circleMeshRenderersTransforms) {
            for (auto currentMesh : *cached->circleMeshRenderersTransforms.Ptr())
                currentMesh->localScale = circleScale;
        }

        for (int i = 0; i < __Instance->_circleMeshRenderers.size(); ++i)
            __Instance->_circleMeshRenderers[i]->enabled = dotEnabled;

        if (cached->circleMaterialPropertyBlockControllers)
        {
            for (auto currentBlock : *cached->circleMaterialPropertyBlockControllers.Ptr())
            {
                currentBlock->materialPropertyBlock->SetColor(PColorNoteVisuals_ColorID, dotColor);
                currentBlock->ApplyChanges();
            }
        }
    }

}   ///< namespace QBeatSaberPlus_NoteTweaker::Patches