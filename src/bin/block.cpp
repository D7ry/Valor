#include "block.h"
#include "settings.h"
#include "perilous.h"
float get_block_chance(RE::Actor* a_actor)
{
	if (a_actor->GetActorRuntimeData().combatController) {
		RE::TESCombatStyle* style = a_actor->GetActorRuntimeData().combatController->combatStyle;
		if (style) {
			return style->generalData.defensiveMult * settings::fPerfectBlock_chance_mult;
		}
	}
	return 0.f;
}

void Block::attempt_start_perfect_block(RE::Actor* a_actor)
{
	bool success = false;
	float chance = get_block_chance(a_actor);
	if (chance > 0.f) {
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0.f, 1.f);
		if (dis(gen) <= chance) {
			success = true;
		}
	}

	success = true; // true by default for testing
	
	if (success) { // flag gv, valhalla combat will handle the rest
		a_actor->SetGraphVariableBool(gv_bool_force_timed_blocking, true);
		a_actor->SetGraphVariableBool(gv_bool_force_perfect_blocking, true);
	}
	
	if (success) { // play effects
		// todo: use perfect block's own vfx
		perilous::GetSingleton()->play_perilous_attack_sfx(a_actor, perilous::PERILOUS_TYPE::blue);
		perilous::GetSingleton()->play_perilous_attack_vfx(a_actor, perilous::PERILOUS_TYPE::blue);
	}
}

void Block::attempt_end_perfect_block(RE::Actor* a_actor)
{
	a_actor->SetGraphVariableBool(gv_bool_force_timed_blocking, false);
	a_actor->SetGraphVariableBool(gv_bool_force_perfect_blocking, false);
}
