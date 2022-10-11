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
void perilous::addCharge(RE::ActorHandle a_refHandle, float a_chargeLength)
{
	activeChargesLock.lock();
	auto& charge = activeCharges[a_refHandle];
	charge += a_chargeLength;
	activeChargesLock.unlock();
}

float perilous::getCharge(RE::ActorHandle a_actorHandle, float a_deltaTime, bool a_bUpdate)
{
	if (a_deltaTime > 0.f) {
		activeChargesLock.lock();

		auto hitstop = activeCharges.find(a_actorHandle);
		if (hitstop != activeCharges.end()) {
			float newHitstopLength = hitstop->second - a_deltaTime;
			if (a_bUpdate) {
				hitstop->second = newHitstopLength;
			}

			float mult = 1.f;
			if (newHitstopLength <= 0.f) {
				mult = (a_deltaTime + newHitstopLength) / a_deltaTime;
				if (a_bUpdate) {
					activeCharges.erase(hitstop);
				}
			}

			a_deltaTime *= settings::fPerilous_chargeTime_multiplier + ((1.f - settings::fPerilous_chargeTime_multiplier) * (1.f - mult));
		}
		activeChargesLock.unlock();
			
	}
	return a_deltaTime;
}

void perilous::perform_perilous_attack(RE::Actor* a_actor)
{
	logger::info("performing perilous attack!");
	
	a_actor->InstantiateHitArt(perilousHitEffectArt, 1, a_actor, false, false, a_actor->GetNodeByName("WEAPON"));
	if (settings::bPerilous_chargeTime_enable) {
		addCharge(a_actor->GetHandle(), settings::fPerilous_chargeTime_duration);
	}
	//a_actor->InstantiateHitShader(temp, 0.5);
	

}
