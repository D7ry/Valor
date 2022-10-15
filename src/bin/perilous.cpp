#include "perilous.h"
#include "include/Utils.h"
using writeLock = std::unique_lock<std::shared_mutex>;
using readLock = std::shared_lock<std::shared_mutex>;

void perilous::init()
{
	DtryUtils::formLoader loader2("ValhallaCombat.esp");
	loader2.log();
	DtryUtils::formLoader loader3("Valgrind.esp");
	loader3.load(perilousHitEffectArt, 0xd69);
	loader3.load(perilousSound, 0xd6a);
	loader3.load(perilousSpell, 0xD6B);
	loader3.log();
}

float get_perilous_chance(RE::Actor* a_actor) {
	if (a_actor->GetActorRuntimeData().combatController) {
		RE::TESCombatStyle* style = a_actor->GetActorRuntimeData().combatController->combatStyle;
		if (style) {
			return style->generalData.offensiveMult;
		}
	}
	return 0.f;
}

void perilous::attempt_start_perilous_attack(RE::Actor* a_actor)
{
	if (a_actor->IsPlayerRef()) {
		return;
	}
	
	auto target = a_actor->GetActorRuntimeData().currentCombatTarget;
	if (!target) {
		return;
	}

	float chance = get_perilous_chance(a_actor);
	if (chance <= 0.f) {
		return;
	}
	
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.f, 1.f);
	if (dis(gen) > chance) {
		return;
	}
	
	attempt_end_perilous_attack(a_actor);  //End the previous perilous attack if any.
	
	{
		readLock lock(perilous_counter_lock);
		auto it = perilous_counter.find(target);
		if (it != perilous_counter.end()) {
			if (it->second >= MAX_PERILOUS_ATTACKER) {
				return;
			}
		}
	}

	//if (Utils::Actor::isPowerAttacking(a_actor)) { /*Power Perilous*/
		perform_perilous_attack(a_actor, target); 
	//} else {/*Light perilous*/

	//}

}

/*Flag the actor as perilous attacker for other mods&plugins.*/
void perilous::flag_perilous(RE::Actor* a_actor)
{
	if (!a_actor->HasSpell(perilousSpell)) {
		a_actor->AddSpell(perilousSpell);
	}
}
void perilous::unflag_perilous(RE::Actor* a_actor)
{
	if (a_actor->HasSpell(perilousSpell)) {
		a_actor->RemoveSpell(perilousSpell);
	}
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
	/*Decrement the counter for the target*/
	{
		writeLock lock(perilous_counter_lock);
		auto it = perilous_counter.find(target);
		if (it != perilous_counter.end()) {
			it->second--;
			if (it->second == 0) {
				perilous_counter.erase(it);
			}
		}
	}
	
}

void perilous::perform_perilous_attack(RE::Actor* a_actor, RE::ActorHandle a_target)
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

	flag_perilous(a_actor);

	/*VFX*/
	RE::NiAVObject* fxNode = nullptr;
	if (Utils::Actor::isHumanoid(a_actor)) {
		fxNode = a_actor->GetNodeByName("WEAPON");
	} else {
		fxNode = a_actor->GetNodeByName("NPC Head");
	}
	a_actor->InstantiateHitArt(perilousHitEffectArt, 1, a_actor, false, false, fxNode);
	/*SFX*/
	Utils::playSound(a_actor, perilousSound, 1.f);  //TODO: increase the sound mult if the target is player.
	
	
	if (settings::bPerilous_chargeTime_enable) {
		AnimSpeedManager::setAnimSpeed(a_actor->GetHandle(), settings::fPerilous_chargeTime_multiplier, settings::fPerilous_chargeTime_duration);
	}

}

bool perilous::is_perilous_attacking(RE::Actor* a_actor, RE::ActorHandle& r_target)
{
	readLock lock(perilous_attacks_lock);
	auto it = perilous_attacks.find(a_actor->GetHandle());
	if (it != perilous_attacks.end()) {
		r_target = it->second;
		return true;
	}
	return false;
}

