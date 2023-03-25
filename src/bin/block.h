#pragma once
#include "PCH.h"
class Block // handles npc blocking
{
public:
	static Block* GetSingleton()
	{
		static Block singleton;
		return &singleton;
	}
	
	void init();

	void attempt_start_perfect_block(RE::Actor* a_actor);

	void attempt_end_perfect_block(RE::Actor* a_actor);
	
private:
	std::random_device rd;

	// uses VC graph variables
	static const inline RE::BSFixedString gv_bool_force_timed_blocking = "val_bForceTimedBlocking";
	static const inline RE::BSFixedString gv_bool_force_perfect_blocking = "val_bForcePerfectBlocking";
};
