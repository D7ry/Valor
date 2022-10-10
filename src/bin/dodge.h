#pragma once
#include "PCH.h"
#include "include/lib/TrueHUDAPI.h"
class dodge
{
private:
	std::random_device rd;

public:
	enum dodge_direction
	{
		forward,
		back,
		left,
		right
	};
	
	dodge()
	{
		std::mt19937(rd);  // seed PRNG
	}
	
	static dodge* GetSingleton()
	{
		static dodge singleton;
		return &singleton;
	}
	
	static bool attempt_passive_dodge(RE::TESObjectREFR& a_ref);
	
	void attempt_dodge(RE::Actor* a_actor, std::vector<dodge_direction> a_directions);

	void react_to_attack(RE::Actor* a_attacker);

private:
	
	bool able_dodge(RE::Actor* a_actor);

	bool can_goto(RE::Actor* a_actor, RE::NiPoint3 a_dest);
	
	dodge_direction get_dodge_direction(RE::Actor* a_actor, RE::Actor* a_attacker);

	void do_dodge(RE::Actor* a_actor, dodge_direction a_direction);

	RE::NiPoint3 get_dodge_vector(dodge_direction a_direction);
	

	static inline const std::vector<dodge_direction> dodge_directions_horizontal = { dodge_direction::left, dodge_direction::right };
	static inline const std::vector<dodge_direction> dodge_directions_vertical = { dodge_direction::forward, dodge_direction::back };
	static inline const std::vector<dodge_direction> dodge_directions_all = { dodge_direction::forward, dodge_direction::back, dodge_direction::left, dodge_direction::right };

	
};
