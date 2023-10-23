#include "UI/SettingsRightView.hpp"
#include "NoteTweaker.hpp"
#include "Logger.hpp"

#include <CP_SDK/Unity/Extensions/ColorU.hpp>

#include <GlobalNamespace/BeatmapObjectsInstaller.hpp>
#include <GlobalNamespace/BurstSliderGameNoteController.hpp>
#include <GlobalNamespace/BombNoteController.hpp>
#include <GlobalNamespace/ColorScheme.hpp>
#include <GlobalNamespace/ColorSchemesSettings.hpp>
#include <GlobalNamespace/GameNoteController.hpp>
#include <GlobalNamespace/MaterialPropertyBlockController.hpp>
#include <GlobalNamespace/MenuTransitionsHelper.hpp>
#include <GlobalNamespace/PlayerData.hpp>
#include <GlobalNamespace/PlayerDataModel.hpp>
#include <GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp>
#include <GlobalNamespace/SceneInfo.hpp>
#include <System/Action_1.hpp>
#include <UnityEngine/AsyncOperation.hpp>
#include <UnityEngine/MaterialPropertyBlock.hpp>
#include <UnityEngine/MeshRenderer.hpp>
#include <UnityEngine/Renderer.hpp>
#include <UnityEngine/Resources.hpp>
#include <UnityEngine/Shader.hpp>
#include <UnityEngine/Rendering/ShadowCastingMode.hpp>
#include <UnityEngine/SceneManagement/SceneManager.hpp>
#include <UnityEngine/SceneManagement/LoadSceneMode.hpp>

using namespace CP_SDK::Unity::Extensions;
using namespace CP_SDK::XUI;
using namespace GlobalNamespace;
using namespace UnityEngine;

namespace QBeatSaberPlus_NoteTweaker::UI {

    CP_SDK_IL2CPP_INHERIT_INIT(SettingsRightView);

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Constructor
    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(SettingsRightView)
    {
        OnViewCreation      = {this, &SettingsRightView::OnViewCreation_Impl};
        OnViewActivation    = {this, &SettingsRightView::OnViewActivation_Impl};
        OnViewDeactivation  = {this, &SettingsRightView::OnViewDeactivation_Impl};
        OnViewDestruction   = {this, &SettingsRightView::OnViewDestruction_Impl};
    }
    /// @brief Destructor
    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(SettingsRightView)
    {

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief On view creation
    void SettingsRightView::OnViewCreation_Impl()
    {
        Templates::FullRectLayout({
            Templates::TitleBar(u"Custom / Default"),

            XUIVSpacer::Make(65.0f)->AsShared()
        })
        ->SetBackground(true, std::nullopt, true)
        ->BuildUI(get_transform());

        m_CustomPreviewTL           = nullptr;
        m_CustomPreviewTR           = nullptr;
        m_CustomPreviewDL           = nullptr;
        m_CustomPreviewDR           = nullptr;
        m_CustomPreviewBomb         = nullptr;
        m_CustomPreviewSliderFill   = nullptr;

        m_DefaultPreviewTL          = nullptr;
        m_DefaultPreviewTR          = nullptr;
        m_DefaultPreviewDL          = nullptr;
        m_DefaultPreviewDR          = nullptr;
        m_DefaultPreviewBomb        = nullptr;
        m_DefaultPreviewSliderFill  = nullptr;

        m_Parent = GameObject::New_ctor();
        m_Parent->get_transform()->set_position(Vector3(3.25f, 1.15f, 2.28f));
        m_Parent->get_transform()->set_rotation(Quaternion::Euler(0.0f, 145.30f, 0.0f));

        GameObject::DontDestroyOnLoad(m_Parent.Ptr());

        auto l_MenuTransitionsHelper                  = Resources::FindObjectsOfTypeAll<MenuTransitionsHelper*>().FirstOrDefault();
        auto l_StandardLevelScenesTransitionSetupData = l_MenuTransitionsHelper->standardLevelScenesTransitionSetupData;
        auto l_StandardGameplaySceneInfo              = l_StandardLevelScenesTransitionSetupData->standardGameplaySceneInfo;
        auto l_GameCoreSceneInfo                      = l_StandardLevelScenesTransitionSetupData->gameCoreSceneInfo;

        using t_Delegate1 = System::Action_1<AsyncOperation*>*;
        SceneManagement::SceneManager::LoadSceneAsync(l_GameCoreSceneInfo->sceneName, SceneManagement::LoadSceneMode::Additive)->add_completed(custom_types::MakeDelegate<t_Delegate1>(std::function([=, this](AsyncOperation*)
        {
            SceneManagement::SceneManager::LoadSceneAsync(l_StandardGameplaySceneInfo->sceneName, SceneManagement::LoadSceneMode::Additive)->add_completed(custom_types::MakeDelegate<t_Delegate1>(std::function([=, this](AsyncOperation*)
            {
                auto l_BeatmapObjectsInstaller   = Resources::FindObjectsOfTypeAll<BeatmapObjectsInstaller*>().FirstOrDefault();
                auto l_OriginalNotePrefab        = l_BeatmapObjectsInstaller->normalBasicNotePrefab;

                m_NoteTemplate = GameObject::Instantiate(l_OriginalNotePrefab->get_transform()->GetChild(0)->get_gameObject());
                m_NoteTemplate->get_gameObject()->SetActive(false);

                GameObject::DontDestroyOnLoad(m_NoteTemplate.Ptr());

                auto l_OriginalBombPrefab = l_BeatmapObjectsInstaller->bombNotePrefab;
                m_BombTemplate = GameObject::Instantiate(l_OriginalBombPrefab->get_transform()->GetChild(0)->get_gameObject());
                m_BombTemplate->get_gameObject()->SetActive(false);

                GameObject::DontDestroyOnLoad(m_BombTemplate.Ptr());

                auto l_OriginalBurstSliderPrefab = l_BeatmapObjectsInstaller->burstSliderNotePrefab;
                m_BurstSliderTemplate = GameObject::Instantiate(l_OriginalBurstSliderPrefab->get_transform()->GetChild(0)->get_gameObject());
                m_BurstSliderTemplate->get_gameObject()->SetActive(false);

                GameObject::DontDestroyOnLoad(m_BurstSliderTemplate.Ptr());

                try
                {
                    CreatePreview(m_NoteTemplate.Ptr(), m_BombTemplate.Ptr(), m_BurstSliderTemplate.Ptr());
                }
                catch (const std::exception& ex)
                {
                    Logger::Instance->Error(u"[UI][SettingsRightView.OnViewCreation_Impl] Error:");
                    Logger::Instance->Error(ex);
                }

                SceneManagement::SceneManager::UnloadSceneAsync(l_StandardGameplaySceneInfo->sceneName);
                SceneManagement::SceneManager::UnloadSceneAsync(l_GameCoreSceneInfo->sceneName);
            })));
        })));
    }
    /// @brief On view activation
    void SettingsRightView::OnViewActivation_Impl()
    {
        if (   !m_CustomPreviewTL
            || !m_CustomPreviewTR)
        {
            if (m_NoteTemplate)
                CreatePreview(m_NoteTemplate.Ptr(), m_BombTemplate.Ptr(), m_BurstSliderTemplate.Ptr());
        }
    }
    /// @brief On view deactivation
    void SettingsRightView::OnViewDeactivation_Impl()
    {
        if (m_CustomPreviewTL)     GameObject::Destroy(m_CustomPreviewTL.Ptr());
        if (m_CustomPreviewTR)     GameObject::Destroy(m_CustomPreviewTR.Ptr());
        if (m_CustomPreviewDL)     GameObject::Destroy(m_CustomPreviewDL.Ptr());
        if (m_CustomPreviewDR)     GameObject::Destroy(m_CustomPreviewDR.Ptr());

        m_CustomPreviewTL = nullptr;
        m_CustomPreviewTR = nullptr;
        m_CustomPreviewDL = nullptr;
        m_CustomPreviewDR = nullptr;

        if (m_CustomPreviewBomb)         GameObject::Destroy(m_CustomPreviewBomb.Ptr());
        if (m_CustomPreviewSliderFill)   GameObject::Destroy(m_CustomPreviewSliderFill.Ptr());

        m_CustomPreviewBomb = nullptr;

        if (m_DefaultPreviewTL)   GameObject::Destroy(m_DefaultPreviewTL.Ptr());
        if (m_DefaultPreviewTR)   GameObject::Destroy(m_DefaultPreviewTR.Ptr());
        if (m_DefaultPreviewDL)   GameObject::Destroy(m_DefaultPreviewDL.Ptr());
        if (m_DefaultPreviewDR)   GameObject::Destroy(m_DefaultPreviewDR.Ptr());

        m_DefaultPreviewTL  = nullptr;
        m_DefaultPreviewTR  = nullptr;
        m_DefaultPreviewDL  = nullptr;
        m_DefaultPreviewDR  = nullptr;

        if (m_DefaultPreviewBomb)        GameObject::Destroy(m_DefaultPreviewBomb.Ptr());
        if (m_DefaultPreviewSliderFill)  GameObject::Destroy(m_DefaultPreviewSliderFill.Ptr());

        m_DefaultPreviewBomb = nullptr;
    }
    /// @brief On view destruction
    void SettingsRightView::OnViewDestruction_Impl()
    {
        if (m_Parent)
            GameObject::Destroy(m_Parent.Ptr());
        if (m_NoteTemplate)
            GameObject::Destroy(m_NoteTemplate.Ptr());

        m_Parent        = nullptr;
        m_NoteTemplate  = nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Refresh settings
    void SettingsRightView::RefreshSettings()
    {
        if (!m_CustomPreviewTL
            || !m_CustomPreviewTR
            || !m_CustomPreviewDR
            || !m_CustomPreviewDR)
            return;

        auto& l_Profile = NTConfig::Instance()->GetActiveProfile();

        m_CustomPreviewTL->get_transform()->set_localScale(Vector3::get_one() * l_Profile->NotesScale);
        m_CustomPreviewTR->get_transform()->set_localScale(Vector3::get_one() * l_Profile->NotesScale);
        m_CustomPreviewDL->get_transform()->set_localScale(Vector3::get_one() * l_Profile->NotesScale);
        m_CustomPreviewDR->get_transform()->set_localScale(Vector3::get_one() * l_Profile->NotesScale);

        auto l_PlayerData    = Resources::FindObjectsOfTypeAll<PlayerDataModel*>().First()->playerData;
        auto l_ColorScheme   = l_PlayerData->colorSchemesSettings->overrideDefaultColors ? l_PlayerData->colorSchemesSettings->GetSelectedColorScheme() : nullptr;
        auto l_LeftColor     = l_ColorScheme != nullptr ? l_ColorScheme->saberAColor : Color(0.658823549747467f, 0.125490203499794f,  0.125490203499794f, 1.0f);
        auto l_RightColor    = l_ColorScheme != nullptr ? l_ColorScheme->saberBColor : Color(0.125490203499794f, 0.3921568691730499f, 0.658823549747467f, 1.0f);

        auto l_ArrowLColor = l_Profile->ArrowsOverrideColors ? l_Profile->ArrowsLColor : ColorU::WithAlpha(l_LeftColor,  l_Profile->ArrowsLColor.a);
        auto l_ArrowRColor = l_Profile->ArrowsOverrideColors ? l_Profile->ArrowsRColor : ColorU::WithAlpha(l_RightColor, l_Profile->ArrowsRColor.a);

        PatchArrow(m_CustomPreviewTL.Ptr(),  l_Profile->ArrowsScale,        ColorU::WithAlpha(l_ArrowLColor, l_Profile->ArrowsIntensity), true);
        PatchArrow(m_CustomPreviewTR.Ptr(),  l_Profile->ArrowsScale,        ColorU::WithAlpha(l_ArrowRColor, l_Profile->ArrowsIntensity), true);
        PatchArrow(m_CustomPreviewDL.Ptr(),  l_Profile->ArrowsScale,        ColorU::WithAlpha(l_ArrowLColor, l_Profile->ArrowsIntensity), false);
        PatchArrow(m_CustomPreviewDR.Ptr(),  l_Profile->ArrowsScale,        ColorU::WithAlpha(l_ArrowRColor, l_Profile->ArrowsIntensity), false);

        auto l_DotLColor = l_Profile->DotsOverrideColors ? l_Profile->DotsLColor : ColorU::WithAlpha(l_LeftColor,  l_Profile->DotsLColor.a);
        auto l_DotRColor = l_Profile->DotsOverrideColors ? l_Profile->DotsRColor : ColorU::WithAlpha(l_RightColor, l_Profile->DotsRColor.a);

        PatchCircle(m_CustomPreviewTL.Ptr(), l_Profile->NotesPrecisonDotsScale,    ColorU::WithAlpha(l_DotLColor, l_Profile->DotsIntensity),   l_Profile->NotesShowPrecisonDots);
        PatchCircle(m_CustomPreviewTR.Ptr(), l_Profile->NotesPrecisonDotsScale,    ColorU::WithAlpha(l_DotRColor, l_Profile->DotsIntensity),   l_Profile->NotesShowPrecisonDots);
        PatchCircle(m_CustomPreviewDL.Ptr(), l_Profile->DotsScale,                 ColorU::WithAlpha(l_DotLColor, l_Profile->DotsIntensity),   true);
        PatchCircle(m_CustomPreviewDR.Ptr(), l_Profile->DotsScale,                 ColorU::WithAlpha(l_DotRColor, l_Profile->DotsIntensity),   true);

        PatchBomb(m_CustomPreviewBomb.Ptr(), l_Profile->BombsOverrideColor ? l_Profile->BombsColor : Color(0.251f, 0.251f, 0.251f, 1.0f));

        m_CustomPreviewBomb->get_transform()->set_localScale(Vector3::get_one() * l_Profile->BombsScale);

        PatchCircle(m_CustomPreviewSliderFill.Ptr(), l_Profile->BurstNotesDotsScale, ColorU::WithAlpha(l_DotLColor, l_Profile->DotsIntensity), true);

        m_CustomPreviewSliderFill->get_transform()->set_localScale(Vector3::get_one() * l_Profile->NotesScale);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Create note preview
    /// @param p_NoteTemplate Note template
    void SettingsRightView::CreatePreview(GameObject* p_NoteTemplate, GameObject* p_BombTemplate, GameObject* m_BurstFillTemplate)
    {
        auto& l_Profile       = NTConfig::Instance()->GetActiveProfile();
        auto  l_PlayerData    = Resources::FindObjectsOfTypeAll<PlayerDataModel*>().First()->playerData;
        auto  l_ColorScheme   = l_PlayerData->colorSchemesSettings->overrideDefaultColors ? l_PlayerData->colorSchemesSettings->GetSelectedColorScheme() : nullptr;
        auto  l_LeftColor     = l_ColorScheme != nullptr ? l_ColorScheme->saberAColor : Color(0.658823549747467f, 0.125490203499794f, 0.125490203499794f, 1.0f);
        auto  l_RightColor    = l_ColorScheme != nullptr ? l_ColorScheme->saberBColor : Color(0.125490203499794f, 0.3921568691730499f, 0.658823549747467f, 1.0f);

        /// ==============

        m_CustomPreviewTL   = GameObject::Instantiate(p_NoteTemplate);
        m_CustomPreviewTR   = GameObject::Instantiate(p_NoteTemplate);
        m_CustomPreviewDL   = GameObject::Instantiate(p_NoteTemplate);
        m_CustomPreviewDR   = GameObject::Instantiate(p_NoteTemplate);

        m_CustomPreviewTL->get_transform()->SetParent(m_Parent->get_transform(), false);
        m_CustomPreviewTR->get_transform()->SetParent(m_Parent->get_transform(), false);
        m_CustomPreviewDL->get_transform()->SetParent(m_Parent->get_transform(), false);
        m_CustomPreviewDR->get_transform()->SetParent(m_Parent->get_transform(), false);

        m_CustomPreviewTL->set_name("BSP_NOTE_TWEAKER_CUSTOM");
        m_CustomPreviewTR->set_name("BSP_NOTE_TWEAKER_CUSTOM");
        m_CustomPreviewDL->set_name("BSP_NOTE_TWEAKER_CUSTOM");
        m_CustomPreviewDR->set_name("BSP_NOTE_TWEAKER_CUSTOM");

        m_CustomPreviewTL->get_transform()->set_localPosition(Vector3::get_zero() - Vector3(0, -0.25f, 1.0f));
        m_CustomPreviewTR->get_transform()->set_localPosition(Vector3::get_zero() - Vector3(0, -0.25f, 0.5f));
        m_CustomPreviewDL->get_transform()->set_localPosition(Vector3::get_zero() - Vector3(0,  0.25f, 1.0f));
        m_CustomPreviewDR->get_transform()->set_localPosition(Vector3::get_zero() - Vector3(0,  0.25f, 0.5f));

        m_CustomPreviewTL->get_transform()->set_localRotation(Quaternion::Euler(0, 272, 0));
        m_CustomPreviewTR->get_transform()->set_localRotation(Quaternion::Euler(0, 272, 0));
        m_CustomPreviewDL->get_transform()->set_localRotation(Quaternion::Euler(0, 272, 0));
        m_CustomPreviewDR->get_transform()->set_localRotation(Quaternion::Euler(0, 272, 0));

        PatchNote(m_CustomPreviewTL.Ptr(), l_LeftColor);
        PatchNote(m_CustomPreviewTR.Ptr(), l_RightColor);
        PatchNote(m_CustomPreviewDL.Ptr(), l_LeftColor);
        PatchNote(m_CustomPreviewDR.Ptr(), l_RightColor);

        /// ==============

        m_CustomPreviewBomb = GameObject::Instantiate(p_BombTemplate);
        m_CustomPreviewBomb->get_transform()->SetParent(m_Parent->get_transform(), false);
        m_CustomPreviewBomb->set_name("BSP_NOTE_TWEAKER_CUSTOM");
        m_CustomPreviewBomb->get_transform()->set_localPosition(Vector3::get_zero() - Vector3(0, 0.75f, 0.75f));
        m_CustomPreviewBomb->SetActive(true);

        PatchBomb(m_CustomPreviewBomb.Ptr(), l_Profile->BombsOverrideColor ? l_Profile->BombsColor : Color(0.251f, 0.251f, 0.251f, 1.0f));

        /// ==============

        m_CustomPreviewSliderFill = GameObject::Instantiate(m_BurstFillTemplate);
        m_CustomPreviewSliderFill->get_transform()->SetParent(m_Parent->get_transform(), false);
        m_CustomPreviewSliderFill->set_name("BSP_NOTE_TWEAKER_CUSTOM");
        m_CustomPreviewSliderFill->get_transform()->set_localPosition(Vector3::get_zero() - Vector3(0, -0.75f, 0.75f));
        m_CustomPreviewSliderFill->get_transform()->set_localRotation(Quaternion::Euler(0, 272, 0));
        m_CustomPreviewSliderFill->SetActive(true);

        PatchNote(m_CustomPreviewSliderFill.Ptr(), l_LeftColor);
        PatchCircle(m_CustomPreviewSliderFill.Ptr(), l_Profile->BurstNotesDotsScale, l_LeftColor, true);

        /// ========================================================

        m_DefaultPreviewTL = GameObject::Instantiate(p_NoteTemplate);
        m_DefaultPreviewTR = GameObject::Instantiate(p_NoteTemplate);
        m_DefaultPreviewDL = GameObject::Instantiate(p_NoteTemplate);
        m_DefaultPreviewDR = GameObject::Instantiate(p_NoteTemplate);
        m_DefaultPreviewTL->get_transform()->SetParent(m_Parent->get_transform(), false);
        m_DefaultPreviewTR->get_transform()->SetParent(m_Parent->get_transform(), false);
        m_DefaultPreviewDL->get_transform()->SetParent(m_Parent->get_transform(), false);
        m_DefaultPreviewDR->get_transform()->SetParent(m_Parent->get_transform(), false);
        m_DefaultPreviewTL->set_name("BSP_NOTE_TWEAKER_DEFAULT");
        m_DefaultPreviewTR->set_name("BSP_NOTE_TWEAKER_DEFAULT");
        m_DefaultPreviewDL->set_name("BSP_NOTE_TWEAKER_DEFAULT");
        m_DefaultPreviewDR->set_name("BSP_NOTE_TWEAKER_DEFAULT");
        m_DefaultPreviewTL->get_transform()->set_localPosition(Vector3::get_zero() + Vector3(0,  0.25f, 0.5f));
        m_DefaultPreviewTR->get_transform()->set_localPosition(Vector3::get_zero() + Vector3(0,  0.25f, 1.0f));
        m_DefaultPreviewDL->get_transform()->set_localPosition(Vector3::get_zero() + Vector3(0, -0.25f, 0.5f));
        m_DefaultPreviewDR->get_transform()->set_localPosition(Vector3::get_zero() + Vector3(0, -0.25f, 1.0f));
        m_DefaultPreviewTL->get_transform()->set_localRotation(Quaternion::Euler(0, 272, 0));
        m_DefaultPreviewTR->get_transform()->set_localRotation(Quaternion::Euler(0, 272, 0));
        m_DefaultPreviewDL->get_transform()->set_localRotation(Quaternion::Euler(0, 272, 0));
        m_DefaultPreviewDR->get_transform()->set_localRotation(Quaternion::Euler(0, 272, 0));

        PatchNote(m_DefaultPreviewTL.Ptr(), l_LeftColor);
        PatchNote(m_DefaultPreviewTR.Ptr(), l_RightColor);
        PatchNote(m_DefaultPreviewDL.Ptr(), l_LeftColor);
        PatchNote(m_DefaultPreviewDR.Ptr(), l_RightColor);

        PatchArrow(m_DefaultPreviewTL.Ptr(), 1.f, ColorU::WithAlpha(l_LeftColor,  0.6f), true);
        PatchArrow(m_DefaultPreviewTR.Ptr(), 1.f, ColorU::WithAlpha(l_RightColor, 0.6f), true);
        PatchArrow(m_DefaultPreviewDL.Ptr(), 0.f, Color::get_white(), false);
        PatchArrow(m_DefaultPreviewDR.Ptr(), 0.f, Color::get_white(), false);

        PatchCircle(m_DefaultPreviewDL.Ptr(), 1.0f, l_LeftColor,  true);
        PatchCircle(m_DefaultPreviewDR.Ptr(), 1.0f, l_RightColor, true);

        /// ==============

        m_DefaultPreviewBomb = GameObject::Instantiate(p_BombTemplate);
        m_DefaultPreviewBomb->get_transform()->SetParent(m_Parent->get_transform(), false);
        m_DefaultPreviewBomb->set_name("BSP_NOTE_TWEAKER_DEFAULT");
        m_DefaultPreviewBomb->get_transform()->set_localPosition(Vector3::get_zero() + Vector3(0, -0.75f, 0.75f));
        m_DefaultPreviewBomb->SetActive(true);

        /// ==============

        m_DefaultPreviewSliderFill = GameObject::Instantiate(m_BurstFillTemplate);
        m_DefaultPreviewSliderFill->get_transform()->SetParent(m_Parent->get_transform(), false);
        m_DefaultPreviewSliderFill->set_name("BSP_NOTE_TWEAKER_DEFAULT");
        m_DefaultPreviewSliderFill->get_transform()->set_localPosition(Vector3::get_zero() + Vector3(0, 0.75f, 0.75f));
        m_DefaultPreviewSliderFill->get_transform()->set_localRotation(Quaternion::Euler(0, 272, 0));
        m_DefaultPreviewSliderFill->SetActive(true);

        PatchNote(m_DefaultPreviewSliderFill.Ptr(), l_LeftColor);
        PatchCircle(m_DefaultPreviewSliderFill.Ptr(), 1.0f, l_LeftColor, true);

        RefreshSettings();
    }
    /// @brief Patch note
    /// @param p_Object Object
    /// @param p_Color  Color
    void SettingsRightView::PatchNote(GameObject* p_Object, Color p_Color)
    {
        p_Object->SetActive(true);

        for (auto l_I = 0; l_I < p_Object->get_transform()->get_childCount(); ++l_I)
            p_Object->get_transform()->GetChild(l_I)->get_gameObject()->set_active(true);

        auto l_Renderer = p_Object->GetComponent<MeshRenderer*>();
        if (l_Renderer)
        {
            /// _Smoothness                 Float                       0,95
            /// _NoteSize                   Float                       0,25
            /// _EnableColorInstancing      Float                       1
            /// _SimpleColor                Color                       RGBA(0.000, 0.000, 0.000, 0.000)
            /// _FinalColorMul              Float                       1
            /// _EnvironmentReflectionCube  Texture
            /// _EnableFog                  Float                       1
            /// _FogStartOffset             Float                       100
            /// _FogScale                   Float                       0,5
            /// _EnableCutout               Float                       1
            /// _CutoutTexScale             Float                       0,5
            /// _EnableCloseToCameraCutout  Float                       0
            /// _CloseToCameraCutoutOffset  Float                       0,5
            /// _CloseToCameraCutoutScale   Float                       0,5
            /// _EnablePlaneCut             Float                       0
            /// _CutPlaneEdgeGlowWidth      Float                       0,01
            /// _CutPlane                   Vector                      (1.0, 0.0, 0.0, 0.0)
            /// _CullMode                   Float                       2
            /// _EnableRimDim               Float                       1
            /// _RimScale                   Float                       2
            /// _RimOffset                  Float                       -0,1
            /// _RimCameraDistanceOffset    Float                       5
            /// _RimCameraDistanceScale     Float                       0,03
            /// _RimDarkenning              Float                       0,5

            ///for (var l_PropertyIndex = 0; l_PropertyIndex < l_Renderer.material.shader.GetPropertyCount(); ++l_PropertyIndex)
            ///{
            ///    var l_SpaceBuffer = "                            ";
            ///    var l_Name = l_Renderer.material.shader.GetPropertyName(l_PropertyIndex);
            ///    var l_Type = l_Renderer.material.shader.GetPropertyType(l_PropertyIndex);
            ///    var l_Line = "";
            ///
            ///    l_Line += l_Name + l_SpaceBuffer.Substring(l_Name.Length);
            ///    l_Line += l_Type + l_SpaceBuffer.Substring(l_Type.ToString().Length);
            ///
            ///    switch (l_Type)
            ///    {
            ///        case UnityEngine.Rendering.ShaderPropertyType.Color:
            ///            l_Line += l_Renderer.material.GetColor(l_Name);
            ///            break;
            ///
            ///        case UnityEngine.Rendering.ShaderPropertyType.Float:
            ///            l_Line += l_Renderer.material.GetFloat(l_Name);
            ///            break;
            ///
            ///        case UnityEngine.Rendering.ShaderPropertyType.Range:
            ///            l_Line += l_Renderer.material.GetFloatArray(l_Name);
            ///            break;
            ///
            ///        case UnityEngine.Rendering.ShaderPropertyType.Vector:
            ///            l_Line += l_Renderer.material.GetVector(l_Name);
            ///            break;
            ///    }
            ///    Logger.Instance.Error(l_Line);
            ///}

            for (auto l_PropertyBlockController : p_Object->GetComponents<MaterialPropertyBlockController*>())
            {
                if (!l_PropertyBlockController->materialPropertyBlock)
                    l_PropertyBlockController->materialPropertyBlock = MaterialPropertyBlock::New_ctor();

                l_PropertyBlockController->materialPropertyBlock->SetColor(Shader::PropertyToID("_Color"),           ColorU::WithAlpha(p_Color, 1.0f));
                l_PropertyBlockController->materialPropertyBlock->SetFloat(Shader::PropertyToID("_EnableRimDim"),    0.0f);
                l_PropertyBlockController->materialPropertyBlock->SetFloat(Shader::PropertyToID("_EnableFog"),       0.0f);
                l_PropertyBlockController->materialPropertyBlock->SetFloat(Shader::PropertyToID("_RimDarkenning"),   0.0f);

                l_PropertyBlockController->ApplyChanges();
            }

            l_Renderer->set_receiveShadows   (false);
            l_Renderer->set_shadowCastingMode(UnityEngine::Rendering::ShadowCastingMode::Off);
        }
    }
    /// @brief Patch arrow
    /// @param p_Note  Instance
    /// @param p_Scale Scale
    /// @param p_Color Color
    /// @param p_Show  Show
    void SettingsRightView::PatchArrow(GameObject* p_Note, float p_Scale, Color p_Color, bool p_Show)
    {
        p_Note->get_transform()->Find("NoteArrow")->get_transform()->set_localScale(Vector3::get_one() * p_Scale);
        p_Note->get_transform()->Find("NoteArrow")->get_gameObject()->SetActive(p_Show);

        auto l_Glow = p_Note->get_transform()->Find("NoteArrowGlow");
        if (l_Glow)
        {
            l_Glow->get_transform()->set_localScale(Vector3(0.6f, 0.3f, 0.6f) * p_Scale);

            for (auto l_PropertyBlockController : l_Glow->GetComponents<MaterialPropertyBlockController*>())
            {
                if (!l_PropertyBlockController->materialPropertyBlock)
                    l_PropertyBlockController->materialPropertyBlock = MaterialPropertyBlock::New_ctor();

                l_PropertyBlockController->materialPropertyBlock->SetColor(Shader::PropertyToID("_Color"), p_Color);
                l_PropertyBlockController->ApplyChanges();
            }

            l_Glow->GetComponent<Renderer*>()->set_enabled(p_Show);
        }
    }
    /// @brief Patch circle
    /// @param p_Note  Instance
    /// @param p_Scale Scale
    /// @param p_Color Color
    /// @param p_Show  Show
    void SettingsRightView::PatchCircle(GameObject* p_Note, float p_Scale, Color p_Color, bool p_Show)
    {
        auto l_CircleGlow    = p_Note->get_transform()->Find("NoteCircleGlow");
        auto l_Circle        = p_Note->get_transform()->Find("Circle");
        if (l_CircleGlow)
        {
            l_CircleGlow->get_transform()->set_localScale(Vector3(0.5f, 0.5f, 0.5f) * p_Scale);

            for (auto l_PropertyBlockController : l_CircleGlow->GetComponents<MaterialPropertyBlockController*>())
            {
                if (!l_PropertyBlockController->materialPropertyBlock)
                    l_PropertyBlockController->materialPropertyBlock = MaterialPropertyBlock::New_ctor();

                l_PropertyBlockController->materialPropertyBlock->SetColor(Shader::PropertyToID("_Color"), p_Color);
                l_PropertyBlockController->ApplyChanges();
            }

            l_CircleGlow->GetComponent<Renderer*>()->set_enabled(p_Show);
        }
        else if (l_Circle)
        {
            l_Circle->get_transform()->set_localScale(Vector3(0.1f, 0.1f, 0.1f) * p_Scale);

            for (auto l_PropertyBlockController : l_Circle->GetComponents<MaterialPropertyBlockController*>())
            {
                if (!l_PropertyBlockController->materialPropertyBlock)
                    l_PropertyBlockController->materialPropertyBlock = MaterialPropertyBlock::New_ctor();

                l_PropertyBlockController->materialPropertyBlock->SetColor(Shader::PropertyToID("_Color"), p_Color);
                l_PropertyBlockController->ApplyChanges();
            }

            l_Circle->GetComponent<Renderer*>()->set_enabled(p_Show);
        }
    }
    /// @brief Patch bomb
    /// @param p_Object Object
    /// @param p_Color  Color
    void SettingsRightView::PatchBomb(GameObject* p_Object, Color p_Color)
    {
        p_Object->GetComponent<Renderer*>()->get_material()->SetColor("_SimpleColor", p_Color);
    }

}   ///< namespace QBeatSaberPlus_NoteTweaker::UI