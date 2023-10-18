#include "Logger.hpp"
#include "NoteTweaker.hpp"

#include <CP_SDK/ChatPlexSDK.hpp>
#include <CP_SDK/Logging/BMBFLogger.hpp>

#include <beatsaber-hook/shared/utils/il2cpp-functions.hpp>
#include <custom-types/shared/register.hpp>

static ModInfo s_ModInfo;

// Called at the early stages of game loading
extern "C" void setup(ModInfo & p_ModInfo)
{
    p_ModInfo.id        = "QBeatSaberPlus-NoteTweaker";
    p_ModInfo.version   = VERSION;

    s_ModInfo = p_ModInfo;

    auto l_Logger = new CP_SDK::Logging::BMBFLogger(new Logger(p_ModInfo));

    l_Logger->Error(u"QBeatSaberPlus-NoteTweaker Setuping!");

    QBeatSaberPlus_NoteTweaker::Logger::Instance = l_Logger;
    CP_SDK::ChatPlexSDK::RegisterModule(new QBeatSaberPlus_NoteTweaker::NoteTweaker());

    l_Logger->Error(u"QBeatSaberPlus-NoteTweaker Completed setup!");
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// Called later on in the game loading - a good time to install function hooks
extern "C" void load()
{
    il2cpp_functions::Init();

    QBeatSaberPlus_NoteTweaker::Logger::Instance->Error(u"Registering custom types.");
    custom_types::Register::AutoRegister();
}