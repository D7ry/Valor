#pragma once
#include "PCH.h"
#include "include/lib/TrueHUDAPI.h"
class dodge
{
public:
	TRUEHUD_API::IVTrueHUD3* debugAPI;
	
	static dodge* GetSingleton()
	{
		static dodge singleton;
		return &singleton;
	}
	
	void attempt_dodge(RE::Actor* a_actor, RE::Actor* a_attacker);

private:
	enum dodge_direction
	{
		forward,
		back,
		left,
		right
	};
	bool can_dodge(RE::Actor* a_actor);
	
	dodge_direction get_dodge_direction(RE::Actor* a_actor, RE::Actor* a_attacker);

	void do_dodge(RE::Actor* a_actor, dodge_direction a_direction);

	RE::NiPoint3 get_dodge_vector(dodge_direction a_direction);
	
	
};
