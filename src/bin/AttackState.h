#pragma once
#include "PCH.h"
#include <shared_mutex>
class AttackState
{
public:
	enum atk_state
	{
		kNone,
		kStart,
		kMid,
		kEnd
	};
	
	static AttackState* GetSingleton() {
		static AttackState singleton;
		return &singleton;
	}

	void clean_atk_state(RE::ActorHandle a_handle);

	void set_atk_state(RE::ActorHandle a_handle, atk_state a_state);
	
	atk_state get_atk_state(RE::ActorHandle a_handle);
	
private:
	std::unordered_map<RE::ActorHandle, atk_state> _atk_state_map;

	mutable std::shared_mutex _atk_state_map_lock;
	
	
};
