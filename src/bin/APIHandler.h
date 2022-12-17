#pragma once
#include "include/lib/ValhallaCombatAPI.h"
#include "include/lib/TrueHUDAPI.h"
/// <summary>
/// Manage all plugin APIs
/// </summary>
namespace API
{
	inline bool ValhallaCombat_API_acquired = false;
	inline VAL_API::IVVAL2* _ValhallaCombat_API;

	inline bool TrueHUD_API_acquired = false;
	inline TRUEHUD_API::IVTrueHUD3* _TrueHud_API;
	
	void init();
};
