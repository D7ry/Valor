#include "perilous.h"
#include "include/Utils.h"
bool perilous::should_perilous_attack(RE::Actor* a_actor)
{
	return true;  //TODO: implement a fair algorithm; allow users to tweak
}

void perilous::init()
{
	DtryUtils::formLoader loader = DtryUtils::formLoader("Anchor spell for MCO.esp");
	loader.load(perilousSpell, 0x8c7);
	loader.load(temp, 0x80a);
	//loader.load(perilousSound, )
	loader.log();
	DtryUtils::formLoader loader2("ValhallaCombat.esp");
	loader2.log();
	DtryUtils::formLoader loader3("Valgrind.esp");
	loader3.load(perilousHitEffectArt, 0xd69);
	loader3.load(perilousSound, 0xd6a);
	loader3.log();
}

void perilous::attempt_start_perilous_attack(RE::Actor* a_actor)
{
	if (a_actor->IsPlayerRef() || !should_perilous_attack(a_actor)) {
		return;
	}
	perform_perilous_attack(a_actor);
}

void perilous::attempt_end_perilous_attack(RE::Actor* a_actor) 
{
	if (a_actor->IsPlayerRef()) {
		return;
	}
	if (a_actor->HasSpell(perilousSpell)) {
		a_actor->RemoveSpell(perilousSpell);
	}
	
}

void perilous::perform_perilous_attack(RE::Actor* a_actor)
{
	logger::info("performing perilous attack!");
	
	auto weaponNode = a_actor->GetNodeByName("WEAPON");
	a_actor->InstantiateHitArt(perilousHitEffectArt, 1, a_actor, false, false, weaponNode ? weaponNode : a_actor->GetNodeByName("NPC Head"));
	if (settings::bPerilous_chargeTime_enable) {
		AnimSpeedManager::setAnimSpeed(a_actor->GetHandle(), settings::fPerilous_chargeTime_multiplier, settings::fPerilous_chargeTime_duration);
	}
	Utils::playSound(a_actor, perilousSound, 1.f);	

}
