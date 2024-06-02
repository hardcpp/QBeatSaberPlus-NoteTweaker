#include "UI/SettingsMainView.hpp"
#include "NoteTweaker.hpp"
#include "NTConfig.hpp"
#include "Logger.hpp"

#include <fstream>

#include <CP_SDK/Misc/Time.hpp>
#include <CP_SDK/UI/Data/TextListCell.hpp>
#include <CP_SDK/UI/ValueFormatters.hpp>
#include <CP_SDK/Unity/Extensions/ColorU.hpp>

#include <System/IO/Path.hpp>

using namespace CP_SDK::XUI;
using namespace UnityEngine;

namespace QBeatSaberPlus_NoteTweaker::UI {

    using namespace CP_SDK::Unity::Extensions;
    using namespace CP_SDK::XUI;
    using namespace UnityEngine;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    CP_SDK_IL2CPP_INHERIT_INIT(SettingsMainView);

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Constructor
    CP_SDK_IL2CPP_DECLARE_CTOR_IMPL(SettingsMainView)
    {
        m_PreventChanges = false;

        OnViewCreation      = {this, &SettingsMainView::OnViewCreation_Impl};
        OnViewDeactivation  = {this, &SettingsMainView::OnViewDeactivation_Impl};
    }
    /// @brief Destructor
    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_IMPL(SettingsMainView)
    {

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief On view creation
    void SettingsMainView::OnViewCreation_Impl()
    {
        Templates::FullRectLayoutMainView({
            Templates::TitleBar(u"Note Tweaker | Settings"),

            XUITabControl::Make({
                {u"<b><color=yellow>Profiles</color></b>",  BuildProfilesTab()},
                {u"Notes",                                  BuildNotesTab()},
                {u"Arrows",                                 BuildArrowsTab()},
                {u"Dots",                                   BuildDotsTab()},
                {u"Bombs",                                  BuildBombsTab()},
                {u"Arcs",                                   BuildArcsTab()},
                {u"BurstNotes",                             BuildBurstNotesTab()}
            })
            ->Bind(&m_TabControl)
            ->AsShared()
        })
        ->SetBackground(true, std::nullopt, true)
        ->BuildUI(get_transform());

        m_ProfileImportModal = CreateModal<Modals::ProfileImportModal>();

        ProfilesTab_Refresh();
        OnSettingChanged();
    }
    /// @brief On view deactivation
    void SettingsMainView::OnViewDeactivation_Impl()
    {
        NTConfig::Instance()->Save();
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Build profiles tab
    std::shared_ptr<IXUIElement> SettingsMainView::BuildProfilesTab()
    {
        return XUIVLayout::Make({
            XUIHLayout::Make({
                XUIVVList::Make()
                    ->SetListCellPrefab(CP_SDK::UI::Data::ListCellPrefabs<CP_SDK::UI::Data::TextListCell>::Get())
                    ->OnListItemSelected({this, &SettingsMainView::ProfilesTab_OnListItemSelect})
                    ->Bind(&m_ProfilesTab_List)
                    ->AsShared()
            })
            ->SetHeight(50)
            ->SetSpacing(0)
            ->SetPadding(0)
            ->SetBackground(true)
            ->OnReady([](CP_SDK::UI::Components::CHLayout* x) -> void {
                x->CSizeFitter()->set_horizontalFit(ContentSizeFitter::FitMode::Unconstrained);
                x->CSizeFitter()->set_verticalFit(ContentSizeFitter::FitMode::Unconstrained);
                x->HOrVLayoutGroup()->set_childForceExpandWidth(true);
                x->HOrVLayoutGroup()->set_childForceExpandHeight(true);
            })
            ->AsShared(),

            Templates::ExpandedButtonsLine({
                XUIPrimaryButton::Make(u"New")->OnClick({this, &SettingsMainView::ProfilesTab_OnNewButton})->AsShared(),
                XUIPrimaryButton::Make(u"Rename")->OnClick({this, &SettingsMainView::ProfilesTab_OnRenameButton})->AsShared(),
                XUIPrimaryButton::Make(u"Delete")->OnClick({this, &SettingsMainView::ProfilesTab_OnDeleteButton})->AsShared(),
                XUISecondaryButton::Make(u"Export")->OnClick({this, &SettingsMainView::ProfilesTab_OnExportButton})->AsShared(),
                XUISecondaryButton::Make(u"Import")->OnClick({this, &SettingsMainView::ProfilesTab_OnImportButton})->AsShared()
            })
        })
        ->OnReady([](CP_SDK::UI::Components::CVLayout* x) -> void {
            x->CSizeFitter()->set_enabled(false);
        })
        ->AsShared();
    }
    /// @brief Build notes tab
    std::shared_ptr<IXUIElement> SettingsMainView::BuildNotesTab()
    {
        return XUIVLayout::Make({
            XUIVLayout::Make({
                XUIText::Make(u"Note scale")->AsShared(),
                XUISlider::Make()
                    ->SetMinValue(0.4f)->SetMaxValue(1.2f)->SetIncrements(0.01f)->SetFormatter(CP_SDK::UI::ValueFormatters::Percentage)
                    ->OnValueChanged([this](float) { OnSettingChanged(); })
                    ->Bind(&m_NotesTab_Scale)
                    ->AsShared(),

                XUIText::Make(u"Show dot on directional notes")->AsShared(),
                XUIToggle::Make()
                    ->OnValueChanged([this](bool) { OnSettingChanged(); })
                    ->Bind(&m_NotesTab_ShowPrecisonDots)
                    ->AsShared(),

                XUIText::Make(u"Precision dot scale")->AsShared(),
                XUISlider::Make()
                    ->SetMinValue(0.2f)->SetMaxValue(1.5f)->SetIncrements(0.01f)->SetFormatter(CP_SDK::UI::ValueFormatters::Percentage)
                    ->OnValueChanged([this](float) { OnSettingChanged(); })
                    ->Bind(&m_NotesTab_PrecisionDotsScale)
                    ->AsShared()
            })
            ->SetWidth(80.0f)
            ->SetPadding(0)
            ->AsShared()
        })->AsShared();
    }
    /// @brief Build arrows tab
    std::shared_ptr<IXUIElement> SettingsMainView::BuildArrowsTab()
    {
        return XUIVLayout::Make({
            XUIText::Make(u"Arrow scale")->AsShared(),
            XUISlider::Make()
                ->SetMinValue(0.2f)->SetMaxValue(1.4f)->SetIncrements(0.01f)->SetFormatter(CP_SDK::UI::ValueFormatters::Percentage)
                ->OnValueChanged([this](float) { OnSettingChanged(); })
                ->Bind(&m_ArrowsTab_Scale)
                ->AsShared(),

            XUIText::Make(u"Arrow glow intensity")->AsShared(),
            XUISlider::Make()
                ->SetMinValue(0.0f)->SetMaxValue(1.0f)->SetIncrements(0.01f)->SetFormatter(CP_SDK::UI::ValueFormatters::Percentage)
                ->OnValueChanged([this](float) { OnSettingChanged(); })
                ->Bind(&m_ArrowsTab_Intensity)
                ->AsShared(),

            XUIText::Make(u"Override arrow colors")->AsShared(),
            XUIToggle::Make()
                ->OnValueChanged([this](bool) { OnSettingChanged(); })
                ->Bind(&m_ArrowsTab_OverrideColors)
                ->AsShared(),

            XUIHLayout::Make({
                XUIVLayout::Make({
                    XUIText::Make(u"Arrow left color")->AsShared(),
                    XUIColorInput::Make()
                        ->OnValueChanged([this](Color) { OnSettingChanged(); })
                        ->Bind(&m_ArrowsTab_LColor)
                        ->AsShared()
                })
                ->AsShared(),
                XUIVLayout::Make({
                    XUIText::Make(u"Arrow right color"),
                    XUIColorInput::Make()
                        ->OnValueChanged([this](Color) { OnSettingChanged(); })
                        ->Bind(&m_ArrowsTab_RColor)
                        ->AsShared()
                })
                ->AsShared()
            })
            ->SetWidth(80.0f)
            ->SetPadding(0)
            ->AsShared()
        })->AsShared();
    }
    /// @brief Build dots tab
    std::shared_ptr<IXUIElement> SettingsMainView::BuildDotsTab()
    {
        return XUIVLayout::Make({
            XUIText::Make(u"Dot scale")->AsShared(),
            XUISlider::Make()
                ->SetMinValue(0.2f)->SetMaxValue(1.5f)->SetIncrements(0.01f)->SetFormatter(CP_SDK::UI::ValueFormatters::Percentage)
                ->OnValueChanged([this](float) { OnSettingChanged(); })
                ->Bind(&m_DotsTab_Scale)
                ->AsShared(),

            XUIText::Make(u"Dot glow intensity")->AsShared(),
            XUISlider::Make()
                ->SetMinValue(0.0f)->SetMaxValue(1.0f)->SetIncrements(0.01f)->SetFormatter(CP_SDK::UI::ValueFormatters::Percentage)
                ->OnValueChanged([this](float) { OnSettingChanged(); })
                ->Bind(&m_DotsTab_Intensity)
                ->AsShared(),

            XUIText::Make(u"Override dot colors")->AsShared(),
            XUIToggle::Make()
                ->OnValueChanged([this](bool) { OnSettingChanged(); })
                ->Bind(&m_DotsTab_OverrideColors)
                ->AsShared(),

            XUIHLayout::Make({
                XUIVLayout::Make({
                    XUIText::Make(u"Dot left color")->AsShared(),
                    XUIColorInput::Make()
                        ->OnValueChanged([this](Color) { OnSettingChanged(); })
                        ->Bind(&m_DotsTab_LColor)
                        ->AsShared()
                })
                ->AsShared(),
                XUIVLayout::Make({
                    XUIText::Make(u"Dot right color")->AsShared(),
                    XUIColorInput::Make()
                        ->OnValueChanged([this](Color) { OnSettingChanged(); })
                        ->Bind(&m_DotsTab_RColor)
                        ->AsShared()
                })
                ->AsShared()
            })
            ->SetWidth(80.0f)
            ->SetPadding(0)
            ->AsShared()
        })->AsShared();
    }
    /// @brief Build bombs tabs
    std::shared_ptr<IXUIElement> SettingsMainView::BuildBombsTab()
    {
        return XUIVLayout::Make({
            XUIText::Make(u"Bomb scale")->AsShared(),
            XUISlider::Make()
                ->SetMinValue(0.4f)->SetMaxValue(1.2f)->SetIncrements(0.01f)->SetFormatter(CP_SDK::UI::ValueFormatters::Percentage)
                ->OnValueChanged([this](float) { OnSettingChanged(); })
                ->Bind(&m_BombsTab_Scale)
                ->AsShared(),

            XUIText::Make(u"Override bomb color")->AsShared(),
            XUIToggle::Make()
                ->OnValueChanged([this](bool) { OnSettingChanged(); })
                ->Bind(&m_BombsTab_OverrideColor)
                ->AsShared(),

            XUIText::Make(u"Bomb color")->AsShared(),
            XUIColorInput::Make()
                ->OnValueChanged([this](Color) { OnSettingChanged(); })
                ->Bind(&m_BombsTab_Color)
                ->AsShared()
        })
        ->SetWidth(80.0f)
        ->AsShared();
    }
    /// @brief Build arcs tab
    std::shared_ptr<IXUIElement> SettingsMainView::BuildArcsTab()
    {
        return XUIVLayout::Make({
            XUIText::Make(u"Arcs intensity")->AsShared(),
            XUISlider::Make()
                ->SetMinValue(0.0f)->SetMaxValue(1.0f)->SetIncrements(0.01f)->SetFormatter(CP_SDK::UI::ValueFormatters::Percentage)
                ->OnValueChanged([this](float) { OnSettingChanged(); })
                ->Bind(&m_ArcsTab_Intensity)
                ->AsShared(),

            XUIText::Make(u"Arcs haptics")->AsShared(),
            XUIToggle::Make()
                ->OnValueChanged([this](bool) { OnSettingChanged(); })
                ->Bind(&m_ArcsTab_Haptics)
                ->AsShared()
        })
        ->SetWidth(80.0f)
        ->AsShared();
    }
    /// @brief Build burst notes tab
    std::shared_ptr<IXUIElement> SettingsMainView::BuildBurstNotesTab()
    {
        return XUIVLayout::Make({
            XUIText::Make(u"Dot size")->AsShared(),
            XUISlider::Make()
                ->SetMinValue(0.0f)->SetMaxValue(3.0f)->SetIncrements(0.01f)->SetFormatter(CP_SDK::UI::ValueFormatters::Percentage)
                ->OnValueChanged([this](float) { OnSettingChanged(); })
                ->Bind(&m_BurstNotesTab_DotScale)
                ->AsShared()
        })
        ->SetWidth(80.0f)
        ->AsShared();
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief When a tab is selected
    /// @param p_TabIndex Tab index
    void SettingsMainView::OnTabSelected(int p_TabIndex)
    {
        if (NTConfig::Instance()->GetActiveProfile()->IsDefault() && p_TabIndex > 0)
        {
            ShowMessageModal(u"<color=yellow>No changes allowed on default config!</color>");
            m_TabControl->SetActiveTab(0);
        }
    }
    /// @brief When settings are changed
    void SettingsMainView::OnSettingChanged()
    {
        if (m_PreventChanges)
            return;

        auto& l_Profile = NTConfig::Instance()->GetActiveProfile();

        #pragma region Notes_Tab
        l_Profile->NotesScale                = m_NotesTab_Scale->Element()->GetValue();
        l_Profile->NotesShowPrecisonDots     = m_NotesTab_ShowPrecisonDots->Element()->GetValue();
        l_Profile->NotesPrecisonDotsScale    = m_NotesTab_PrecisionDotsScale->Element()->GetValue();
        #pragma endregion Notes_Tab

        #pragma region Arrows_Tab
        l_Profile->ArrowsScale           = m_ArrowsTab_Scale->Element()->GetValue();
        l_Profile->ArrowsIntensity       = m_ArrowsTab_Intensity->Element()->GetValue();
        l_Profile->ArrowsOverrideColors  = m_ArrowsTab_OverrideColors->Element()->GetValue();
        l_Profile->ArrowsLColor          = m_ArrowsTab_LColor->Element()->GetValue();
        l_Profile->ArrowsRColor          = m_ArrowsTab_RColor->Element()->GetValue();

        m_ArrowsTab_LColor->SetInteractable(l_Profile->ArrowsOverrideColors);
        m_ArrowsTab_RColor->SetInteractable(l_Profile->ArrowsOverrideColors);
        #pragma endregion Arrows_Tab

        #pragma region Dots_Tab
        l_Profile->DotsScale             = m_DotsTab_Scale->Element()->GetValue();
        l_Profile->DotsIntensity         = m_DotsTab_Intensity->Element()->GetValue();
        l_Profile->DotsOverrideColors    = m_DotsTab_OverrideColors->Element()->GetValue();
        l_Profile->DotsLColor            = m_DotsTab_LColor->Element()->GetValue();
        l_Profile->DotsRColor            = m_DotsTab_RColor->Element()->GetValue();

        m_DotsTab_LColor->SetInteractable(l_Profile->DotsOverrideColors);
        m_DotsTab_RColor->SetInteractable(l_Profile->DotsOverrideColors);
        #pragma endregion Dots_Tab

        #pragma region Bombs_Tab
        l_Profile->BombsScale         = m_BombsTab_Scale->Element()->GetValue();
        l_Profile->BombsOverrideColor = m_BombsTab_OverrideColor->Element()->GetValue();
        l_Profile->BombsColor         = m_BombsTab_Color->Element()->GetValue();

        m_BombsTab_Color->SetInteractable(l_Profile->BombsOverrideColor);
        #pragma endregion Bombs_Tab

        #pragma region Arcs_Tab
        l_Profile->ArcsIntensity = m_ArcsTab_Intensity->Element()->GetValue();
        l_Profile->ArcsHaptics   = m_ArcsTab_Haptics->Element()->GetValue();
        #pragma endregion Arcs_Tab

        #pragma region BurstNotes_Tab
        l_Profile->BurstNotesDotsScale = m_BurstNotesTab_DotScale->Element()->GetValue();
        #pragma endregion BurstNotes_Tab

        /// Refresh preview
        SettingsRightView::Instance()->RefreshSettings();
    }
    /// @brief Reset settings
    void SettingsMainView::RefreshSettings()
    {
        auto& l_Profile = NTConfig::Instance()->GetActiveProfile();

        m_PreventChanges = true;

        #pragma region Notes_Tab
        m_NotesTab_Scale                ->SetValue(l_Profile->NotesScale);
        m_NotesTab_ShowPrecisonDots     ->SetValue(l_Profile->NotesShowPrecisonDots);
        m_NotesTab_PrecisionDotsScale   ->SetValue(l_Profile->NotesPrecisonDotsScale);
        #pragma endregion Notes_Tab

        #pragma region Arrows_Tab
        m_ArrowsTab_Scale           ->SetValue(l_Profile->ArrowsScale);
        m_ArrowsTab_Intensity       ->SetValue(l_Profile->ArrowsIntensity);
        m_ArrowsTab_OverrideColors  ->SetValue(l_Profile->ArrowsOverrideColors);
        m_ArrowsTab_LColor          ->SetValue(ColorU::WithAlpha(l_Profile->ArrowsLColor, 1.00f));
        m_ArrowsTab_RColor          ->SetValue(ColorU::WithAlpha(l_Profile->ArrowsRColor, 1.00f));

        m_ArrowsTab_LColor->SetInteractable(l_Profile->ArrowsOverrideColors);
        m_ArrowsTab_RColor->SetInteractable(l_Profile->ArrowsOverrideColors);
        #pragma endregion Arrows_Tab

        #pragma region Dots_Tab
        m_DotsTab_Scale         ->SetValue(l_Profile->DotsScale);
        m_DotsTab_Intensity     ->SetValue(l_Profile->DotsIntensity);
        m_DotsTab_OverrideColors->SetValue(l_Profile->DotsOverrideColors);
        m_DotsTab_LColor        ->SetValue(ColorU::WithAlpha(l_Profile->DotsLColor, 1.00f));
        m_DotsTab_RColor        ->SetValue(ColorU::WithAlpha(l_Profile->DotsRColor, 1.00f));

        m_DotsTab_LColor->SetInteractable(l_Profile->DotsOverrideColors);
        m_DotsTab_RColor->SetInteractable(l_Profile->DotsOverrideColors);
        #pragma endregion Dots_Tab

        #pragma region Bombs_Tab
        m_BombsTab_Scale        ->SetValue(l_Profile->BombsScale);
        m_BombsTab_OverrideColor->SetValue(l_Profile->BombsOverrideColor);
        m_BombsTab_Color        ->SetValue(ColorU::WithAlpha(l_Profile->BombsColor, 1.00f));

        m_BombsTab_Color->SetInteractable(l_Profile->BombsOverrideColor);
        #pragma endregion Bombs_Tab

        #pragma region Arcs_Tab
        m_ArcsTab_Intensity ->SetValue(l_Profile->ArcsIntensity);
        m_ArcsTab_Haptics   ->SetValue(l_Profile->ArcsHaptics);
        #pragma endregion Arcs_Tab

        #pragma region BurstNotes_Tab
        m_BurstNotesTab_DotScale->SetValue(l_Profile->BurstNotesDotsScale);
        #pragma endregion BurstNotes_Tab

        m_PreventChanges = false;

        SettingsRightView::Instance()->RefreshSettings();
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Refresh list
    void SettingsMainView::ProfilesTab_Refresh()
    {
        m_Items.clear();
        for (auto l_I = 0; l_I < NTConfig::Instance()->Profiles.size(); ++l_I)
            m_Items.push_back(std::make_shared<CP_SDK::UI::Data::TextListItem>(NTConfig::Instance()->Profiles[l_I]->Name, u"", TMPro::TextAlignmentOptions::CaplineGeoAligned));

        m_ProfilesTab_List->SetListItems(m_Items);
        m_ProfilesTab_List->SetSelectedListItem(m_Items[NTConfig::Instance()->ActiveProfile]);
    }
    /// @brief On item selected
    /// @param p_ListItem Selected item
    void SettingsMainView::ProfilesTab_OnListItemSelect(const CP_SDK::UI::Data::IListItem::Ptr& p_ListItem)
    {
        m_SelectedItem = *reinterpret_cast<const std::shared_ptr<CP_SDK::UI::Data::TextListItem>*>(&p_ListItem);

        if (m_SelectedItem)
        {
            auto l_It = std::find_if(NTConfig::Instance()->Profiles.begin(), NTConfig::Instance()->Profiles.end(), [this](const std::shared_ptr<NTConfig::_Profile>& x) -> bool {
                return x->Name == m_SelectedItem->Text;
            });

            if (l_It != NTConfig::Instance()->Profiles.end())
                NTConfig::Instance()->ActiveProfile = l_It - NTConfig::Instance()->Profiles.begin();
        }

        RefreshSettings();
    }
    /// @brief New profile button
    void SettingsMainView::ProfilesTab_OnNewButton()
    {
        ShowKeyboardModal(u"", [this](std::u16string_view p_Name) -> void
        {
            auto l_ProfileName = p_Name.empty() ? u"No name..." : p_Name;
            auto l_NewProfile  = std::make_shared<NTConfig::_Profile>();

            l_NewProfile->Name = l_ProfileName;

            NTConfig::Instance()->Profiles.push_back(l_NewProfile);
            NTConfig::Instance()->ActiveProfile = NTConfig::Instance()->Profiles_IndexOf(l_NewProfile);

            ProfilesTab_Refresh();
            RefreshSettings();
        });
    }
    /// @brief Rename profile button
    void SettingsMainView::ProfilesTab_OnRenameButton()
    {
        if (NTConfig::Instance()->GetActiveProfile()->IsDefault())
        {
            ShowMessageModal(u"<color=yellow>No changes allowed on default config!</color>");
            return;
        }

        ShowKeyboardModal(NTConfig::Instance()->GetActiveProfile()->Name, [this](std::u16string_view p_NewName) -> void
        {
            NTConfig::Instance()->GetActiveProfile()->Name = p_NewName.empty() ? u"No name..." : p_NewName;
            ProfilesTab_Refresh();
            RefreshSettings();
        });
    }
    /// @brief Delete profile button
    void SettingsMainView::ProfilesTab_OnDeleteButton()
    {
        if (NTConfig::Instance()->GetActiveProfile()->IsDefault())
        {
            ShowMessageModal(u"<color=yellow>No changes allowed on default config!</color>");
            return;
        }

        ShowConfirmationModal(u"<color=red>Do you want to delete profile</color>\n\"" + NTConfig::Instance()->GetActiveProfile()->Name + u"\"?", [this](bool p_Confirm) -> void
        {
            if (!p_Confirm)
                return;

            auto l_It = std::find(NTConfig::Instance()->Profiles.begin(), NTConfig::Instance()->Profiles.end(), NTConfig::Instance()->GetActiveProfile());
            if (l_It != NTConfig::Instance()->Profiles.end())
                NTConfig::Instance()->Profiles.erase(l_It);

            NTConfig::Instance()->ActiveProfile = std::clamp<int>(NTConfig::Instance()->ActiveProfile - 1, 0, NTConfig::Instance()->Profiles.size());
            ProfilesTab_Refresh();
            RefreshSettings();
        });
    }
    /// @brief Export an profile
    void SettingsMainView::ProfilesTab_OnExportButton()
    {
        auto& l_Profile = NTConfig::Instance()->GetActiveProfile();
        if (l_Profile->IsDefault())
        {
            ShowMessageModal(u"<color=yellow>Cannot export default config!</color>");
            return;
        }

        std::ofstream l_Stream;
        try
        {
            StringW l_FileName = std::to_string(CP_SDK::Misc::Time::UnixTimeNow()) + "_" + CP_SDK::Utils::U16StrToStr(l_Profile->Name) + ".bspnt";
            l_FileName =  NoteTweaker::Instance()->ExportFolder() + u"/" + System::String::Join(u"", l_FileName->Split(System::IO::Path::GetInvalidPathChars()));

            l_Stream.open(l_FileName.operator std::__ndk1::string(), std::ios::trunc);

            CP_SDK::Utils::Json::U16Document l_Document;

            l_Document.SetObject();
            l_Profile->Serialize(l_Document, l_Document.GetAllocator());

            if (!CP_SDK::Utils::Json::TryToU8Stream(l_Document, true, l_Stream))
            {
                l_Stream.close();
                throw std::runtime_error("Failed to write json");
            }

            l_Stream.close();

            ShowMessageModal(u"Profile exported in\n" + NoteTweaker::Instance()->ExportFolder());
        }
        catch (const std::exception& l_Exception)
        {
            Logger::Instance->Error(u"");
            Logger::Instance->Error(l_Exception);
            ShowMessageModal(u"<color=yellow>Failed to export profile!</color>");
        }

        if (l_Stream.is_open())
            l_Stream.close();
    }
    /// @brief Import an profile
    void SettingsMainView::ProfilesTab_OnImportButton()
    {
        ShowModal(m_ProfileImportModal.Ptr());
        m_ProfileImportModal->Init({this, &SettingsMainView::ProfilesTab_Refresh});
    }

}   ///< namespace QBeatSaberPlus_NoteTweaker::UI