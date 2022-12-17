#include "perilous.h"
#include "include/Utils.h"
#include "APIHandler.h"
using writeLock = std::unique_lock<std::shared_mutex>;
using readLock = std::shared_lock<std::shared_mutex>;

void perilous::init()
{
	DtryUtils::formLoader loader2("ValhallaCombat.esp");
	loader2.log();
	DtryUtils::formLoader loader3("Valor.esp");
	loader3.load(perilousHitEffectArt_yellow, 0xd6f);
	loader3.load(perilousHitEffectArt_red, 0xd69);
	loader3.load(perilousHitEffectArt_blue, 0xd70);
	loader3.load(perilousSound, 0xd6a);
	loader3.load(perilousSpell, 0xD6B);
	loader3.log();
}

float get_perilous_chance(RE::Actor* a_actor) {
	if (a_actor->GetActorRuntimeData().combatController) {
		RE::TESCombatStyle* style = a_actor->GetActorRuntimeData().combatController->combatStyle;
		if (style) {
			return style->generalData.offensiveMult * settings::fPerilous_chance_multiplier;
		}
	}
	return 0.f;
}

/* Attempt to initiate a perilous attack for A_ACTOR based on many variables and conditions. 
* Actor MUST be doing a power attack already.
*/
void perilous::attempt_start_perilous_attack(RE::Actor* a_actor)
{
	if (!settings::bPerilous_enable) {
		return;
	}
	if (a_actor->IsPlayerRef() || !a_actor->IsInCombat()) {
		return;
	}
	
	auto target = a_actor->GetActorRuntimeData().currentCombatTarget;
	if (!target) {
		return;
	}

	if (API::ValhallaCombat_API_acquired) {
		if (API::_ValhallaCombat_API->isActorExhausted(a_actor) || API::_ValhallaCombat_API->isActorStunned(a_actor)) {
			return;
		}
	}
	attempt_end_perilous_attack(a_actor);  //End the previous perilous attack if any.

	bool success = false;
	
	float chance = get_perilous_chance(a_actor);
	if (chance > 0.f) {
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0.f, 1.f);
		if (dis(gen) <= chance) {
			success = true;  // trigger special perilous attack
		}
	}
	
	if (success) {
		{
			readLock lock(perilous_counter_lock);
			auto it = perilous_counter.find(target);
			if (it != perilous_counter.end()) {
				success = it->second < MAX_PERILOUS_ATTACKER;  // limit max # of actors doing perilous attacks to a specific actor to avoid chaos
			}
		}
	}

	if (success) {
		perform_perilous_attack(a_actor, target);
	}
}

/*Flag the actor as perilous attacker for other mods&plugins.*/
inline void perilous::flag_perilous(RE::Actor* a_actor, perilous::PERILOUS_TYPE a_type)
{
	a_actor->SetGraphVariableInt(gv_int_perilous_attack_type, static_cast<int>(a_type));
}
inline void perilous::unflag_perilous(RE::Actor* a_actor)
{
	a_actor->SetGraphVariableInt(gv_int_perilous_attack_type, 0);
}


void perilous::attempt_end_perilous_attack(RE::Actor* a_actor)
{
	if (a_actor->IsPlayerRef()) {
		return;
	}
	RE::ActorHandle target;
	if (!is_perilous_attacking(a_actor, target)) {
		return;
	}
	unflag_perilous(a_actor);
	/*Remove the actor from the perilous_attacks map*/
	{
		writeLock lock(perilous_attacks_lock);
		perilous_attacks.erase(a_actor->GetHandle());
	}
	/* Decrement counter for target*/
	{
		writeLock lock(perilous_counter_lock);
		auto it = perilous_counter.find(target);
		if (it != perilous_counter.end()) {
			it->second--;
			if (it->second <= 0) {
				perilous_counter.erase(it);
			}
		}
	}
}

bool perilous::is_perilous_attacking(RE::Actor* a_actor, RE::ActorHandle& r_target)
{
	int type;
	if (!a_actor->GetGraphVariableInt("gv_int_perilous_attack_type", type)) {
		return false;
	}
	if (type == static_cast<int>(PERILOUS_TYPE::none)) {
		return false;
	}
	readLock lock(perilous_attacks_lock);
	auto it = perilous_attacks.find(a_actor->GetHandle());
	if (it != perilous_attacks.end()) {
		r_target = it->second;
		return true;
	}
	return false;
}


void perilous::play_perilous_attack_vfx(RE::Actor* a_actor, PERILOUS_TYPE a_type)
{
	RE::NiAVObject* fxNode = nullptr;
	Utils::Actor::getHeadPos(a_actor, fxNode);
	switch (a_type) {
	case PERILOUS_TYPE::yellow:
		a_actor->InstantiateHitArt(perilousHitEffectArt_yellow, 1.5, a_actor, false, false, fxNode);
		break;
	case PERILOUS_TYPE::red:
		a_actor->InstantiateHitArt(perilousHitEffectArt_red, 1.5, a_actor, false, false, fxNode);
		break;
	}
}


void perilous::perform_perilous_attack(RE::Actor* a_actor, RE::ActorHandle a_target)//TODO: add blue perilous attack
{
	/*increment counter*/
	{
		writeLock lock(perilous_counter_lock);
		auto it = perilous_counter.find(a_target);
		if (it != perilous_counter.end()) {
			it->second++;
		} else {
			perilous_counter.emplace(a_target, 1);
		}
	}
	/*add to map*/
	{
		writeLock lock(perilous_attacks_lock);
		perilous_attacks.emplace(a_actor->GetHandle(), a_target);
	}

	flag_perilous(a_actor, PERILOUS_TYPE::red);

	/*VFX*/
	play_perilous_attack_vfx(a_actor, PERILOUS_TYPE::red);
	/*SFX*/
	Utils::playSound(a_actor, perilousSound, 1.f);  //TODO: increase the sound mult if the target is player.

	if (settings::bPerilous_chargeTime_enable) {
		AnimSpeedManager::setAnimSpeed(a_actor->GetHandle(), settings::fPerilous_chargeTime_multiplier, settings::fPerilous_chargeTime_duration);
	}
}

bool perilous::is_perilous_attacking(RE::Actor* a_actor)
{
	readLock lock(perilous_attacks_lock);
	return perilous_attacks.contains(a_actor->GetHandle());
}


