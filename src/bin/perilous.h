#pragma once
#include "PCH.h"
#include <shared_mutex>

class perilous
{
public:
	enum PERILOUS_TYPE
	{
		none = 0,
		yellow = 1, // all heavy attacks, cannot be blocked, can be parried, stagger on hit.
		red = 2,     // cannot be blocked, cannot be parried, stagger on hit.
		blue = 3	// (vanilla bash)cannot be blocked, cannot be parried, can be bashed to lead to attacker stagger, stagger on hit.
	};
	
	static perilous* GetSingleton()
	{
		static perilous singleton;
		return &singleton;
	}

	void init();
	
	void play_perilous_attack_vfx(RE::Actor* a_actor, PERILOUS_TYPE a_type);
	void play_perilous_attack_sfx(RE::Actor* a_actor, PERILOUS_TYPE a_type);

	void attempt_start_perilous_attack(RE::Actor* a_actor, PERILOUS_TYPE a_type);

	void attempt_end_perilous_attack(RE::Actor* a_actor);

	bool is_perilous_attacking(RE::Actor* a_actor);
	bool is_perilous_bashing(RE::Actor* a_actor);
	bool is_perilous_attacking(RE::Actor* a_actor, RE::ActorHandle& r_target);

	void perform_perilous_attack(RE::Actor* a_actor, RE::ActorHandle a_target, PERILOUS_TYPE a_type);

	void clear();

private:
	static inline RE::BSFixedString gv_int_perilous_attack_type = "val_perilous_attack_type";
	inline void flag_perilous(RE::Actor* a_actor, perilous::PERILOUS_TYPE a_type);
	inline void unflag_perilous(RE::Actor* a_actor);

	std::random_device rd;
	
	RE::SpellItem* perilousSpell;
	RE::BGSSoundDescriptorForm* perilousSound;
	RE::BGSArtObject* perilousHitEffectArt_yellow;
	RE::BGSArtObject* perilousHitEffectArt_red;
	RE::BGSArtObject* perilousHitEffectArt_blue;
	RE::BGSArtObject* temp;
	
	#define MAX_PERILOUS_ATTACKER 2

	std::unordered_map<RE::ActorHandle, RE::ActorHandle> perilous_attacks; /*Map of Attacker->target to keep track of all ongoing perilous attacks.*/
	mutable std::shared_mutex perilous_attacks_lock;

	std::unordered_map<RE::ActorHandle, uint32_t> perilous_counter; /*Counter to keep track of an actor's current perilous attackers, to prevent being surrounded by too many perilous atks.*/
	mutable std::shared_mutex perilous_counter_lock;
};
