#pragma once
#include "PCH.h"
class dodge
{
public:
	
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

	
	
};
