#include "perilous.h"

bool perilous::should_perilous_attack(RE::Actor* a_actor)
{
	return true;  //TODO: implement
}

void perilous::attempt_start_perilous_attack(RE::Actor* a_actor)
{
	if (a_actor->IsPlayerRef() || !should_perilous_attack(a_actor)) {
		return;
	}
	perform_perilous_attack(a_actor);
	a_actor->AddSpell(perilousSpell);
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
	
}
