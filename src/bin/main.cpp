#pragma once
#include "hooks.h"
#include "perilous.h"
#include "dodge.h"
#include "APIHandler.h"
#include "include/Utils.h"
#include "settings.h"


void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		perilous::GetSingleton()->init();
		hooks::on_animation_event::install();
		hooks::on_melee_hit::install();
		hooks::on_set_rotation::install();
		if (settings::bDodgeAI_Passive_enable) {  //install hooks for passive dodge
			hooks::on_combatBehavior_backoff_createPath::install();
			hooks::on_combatBehavior_circle_createPath::install();
			hooks::on_combatBehavior_dodgethreat_createPath::install();
			hooks::on_combatBehavior_fallback_createPath::install();
		}
		AnimSpeedManager::init();
		settings::init();
		break;
	case SKSE::MessagingInterface::kPostLoad:
		API::init();
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
		perilous::GetSingleton()->clear();
		break;
	}
}

void onSKSEInit()
{
	settings::read();
	hooks::alloc();
	hooks::on_attack_action::install(); /*Install this hook prior to all other plugins(SCAR, VC) to ensure attack cancellation consistency.*/
}

namespace
{
	void InitializeLog()
	{
#ifndef NDEBUG
		auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
		auto path = logger::log_directory();
		if (!path) {
			util::report_and_fail("Failed to find standard logging directory"sv);
		}

		*path /= fmt::format("{}.log"sv, Plugin::NAME);
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

#ifndef NDEBUG
		const auto level = spdlog::level::trace;
#else
		const auto level = spdlog::level::info;
#endif

		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
		log->set_level(level);
		log->flush_on(level);

		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("%s(%#): [%^%l%$] %v"s);
	}
}

std::string wstring2string(const std::wstring& wstr, UINT CodePage)

{

	std::string ret;

	int len = WideCharToMultiByte(CodePage, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);

	ret.resize((size_t)len, 0);

	WideCharToMultiByte(CodePage, 0, wstr.c_str(), (int)wstr.size(), &ret[0], len, NULL, NULL);

	return ret;

}


extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Plugin::NAME.data();
	a_info->version = Plugin::VERSION[0];

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_SSE_1_5_39) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v;

	v.PluginVersion(Plugin::VERSION);
	v.PluginName(Plugin::NAME);

	v.UsesAddressLibrary(true);
	v.CompatibleVersions({ SKSE::RUNTIME_SSE_LATEST });
	v.HasNoStructUse(true);
	return v;
}();


extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
    #ifndef NDEBUG
	while (!IsDebuggerPresent()) { Sleep(100); }
#endif
	REL::Module::reset();
	InitializeLog();
	logger::info("{} v{}"sv, Plugin::NAME, Plugin::VERSION.string());

	SKSE::Init(a_skse);

	auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
		return false;
	}
	
	onSKSEInit();


	return true;
}


//extern "C" DLLEXPORT void* SKSEAPI RequestPluginAPI(const VAL_API::InterfaceVersion a_interfaceVersion)
//{
//	//auto api = Messaging::TrueHUDInterface::GetSingleton();
//	auto api = ModAPI::VALInterface::GetSingleton();
//	logger::info("ValhallaCombat::RequestPluginAPI called");
//
//	switch (a_interfaceVersion) {
//	case VAL_API::InterfaceVersion::V1:
//		logger::info("ValhallaCombat::RequestPluginAPI returned the API singleton");
//		return static_cast<void*>(api);
//	}
//	logger::info("ValhallaCombat::RequestPluginAPI requested the wrong interface version");
//	return nullptr;
//}
