#include "UI/Modals/ProfileImportModal.hpp"
#include "NoteTweaker.hpp"
#include "Logger.hpp"

#include <filesystem>
#include <fstream>

using namespace CP_SDK::XUI;

namespace QBeatSaberPlus_NoteTweaker::UI::Modals {

    CP_SDK_IL2CPP_INHERIT_INIT(ProfileImportModal);

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Constructor
    CP_SDK_IL2CPP_DECLARE_CTOR_CHAIN_IMPL(ProfileImportModal, CP_SDK::UI::IModal)
    {
        OnShow  = {this, &ProfileImportModal::Impl_OnShow};
        OnClose = {this, &ProfileImportModal::Impl_OnClose};
    }
    /// @brief Destructor
    CP_SDK_IL2CPP_DECLARE_DTOR_MONOBEHAVIOUR_CHAIN_IMPL(ProfileImportModal, CP_SDK::UI::IModal)
    {

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief On modal show
    void ProfileImportModal::Impl_OnShow()
    {
        if (m_Dropdown)
            return;

        Templates::ModalRectLayout({
            XUIText::Make(u"What profile do you want to import?")->AsShared(),

            XUIDropdown::Make()
                ->OnValueChanged([this](int, std::u16string_view p_Selected) -> void { m_Selected = p_Selected; })
                ->Bind(&m_Dropdown)
                ->AsShared(),

            XUIHLayout::Make({
                XUISecondaryButton::Make(u"Cancel", {this, &ProfileImportModal::OnCancelButton})->SetWidth(30.0f)->AsShared(),
                XUIPrimaryButton::Make(u"Import", {this, &ProfileImportModal::OnImportButton})->SetWidth(30.0f)->AsShared()
            })
            ->SetPadding(0)
            ->AsShared()
        })
        ->SetWidth(90.0f)
        ->BuildUI(get_transform());
    }
    /// @brief On modal close
    void ProfileImportModal::Impl_OnClose()
    {

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Init
    /// @param p_Callback Callback
    void ProfileImportModal::Init(CP_SDK::Utils::CActionRef<> p_Callback)
    {
        m_Callback = nullptr;
        m_Selected = u"";

        std::vector<std::u16string> l_Files;
        for (const auto& l_Entry : std::filesystem::directory_iterator(NoteTweaker::Instance()->ImportFolder()))
        {
            auto& l_Path = l_Entry.path();

            if (!l_Entry.is_regular_file() || (l_Path.extension() != ".bspnt" && l_Path.extension() != ".BSPNT"))
                continue;

            l_Files.push_back(CP_SDK::Utils::StrToU16Str(l_Path.stem().string()));
        }
        m_Dropdown->SetOptions(l_Files);

        m_Callback = p_Callback;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    /// @brief On cancel button
    void ProfileImportModal::OnCancelButton()
    {
        VController->CloseModal(this);
    }
    /// @brief On import button
    void ProfileImportModal::OnImportButton()
    {
        auto l_Path = NoteTweaker::Instance()->ImportFolder() + "/" + m_Selected + ".bspnt";

        if (std::filesystem::exists(l_Path))
        {
            std::ifstream l_Stream;

            try
            {
                l_Stream.open(l_Path);

                CP_SDK::Utils::Json::U16Document l_Document;
                if (!CP_SDK::Utils::Json::TryFromU8Stream(l_Stream, l_Document))
                {
                    l_Stream.close();
                    throw std::runtime_error("Unable to parse document");
                }
                l_Stream.close();

                auto l_NewProfile = std::make_shared<NTConfig::_Profile>();
                l_NewProfile->Unserialize(l_Document);
                l_NewProfile->Name += u" (Imported)";

                NTConfig::Instance()->Profiles.push_back(l_NewProfile);

                VController->CloseModal(this);

                try { m_Callback(); }
                catch (const std::exception& l_Exception)
                {
                    Logger::Instance->Error(u"[UI][ProfileImportModal.OnImportButton] Error:");
                    Logger::Instance->Error(l_Exception);
                }
            }
            catch (const std::exception&)
            {
                VController->CloseModal(this);
                VController->ShowMessageModal(u"Invalid file!");
            }

            if (l_Stream.is_open())
                l_Stream.close();
        }
        else
        {
            VController->CloseModal(this);
            VController->ShowMessageModal(u"File not found!");
        }
    }

}   ///< namespace QBeatSaberPlus_NoteTweaker::UI::Modals