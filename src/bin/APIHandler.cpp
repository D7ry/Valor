#include "APIHandler.h"

void API::init()
{
	logger::info("Obtaining ValhallaCombat API...");
	_ValhallaCombat_API = reinterpret_cast<VAL_API::IVVAL2*>(VAL_API::RequestPluginAPI());
	if (_ValhallaCombat_API) {
		ValhallaCombat_API_acquired = true;
		logger::info("Obtained ValhallaCombat API - {0:x}", (uintptr_t)_ValhallaCombat_API);
	} else {
		logger::info("ValhallaCombat API not found.");
	}

	_TrueHud_API = reinterpret_cast<TRUEHUD_API::IVTrueHUD3*>(TRUEHUD_API::RequestPluginAPI(TRUEHUD_API::InterfaceVersion::V3));
	if (_TrueHud_API) {
		TrueHUD_API_acquired = true;
		logger::info("Obtained Truehud API - {0:x}", (uintptr_t)_TrueHud_API);
	} else {
		logger::info("TrueHUD API not found.");
	}
}
