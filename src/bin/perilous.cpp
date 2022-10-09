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
	loader2.load(perilousExplosionNode, 0x60c2f);
	loader2.log();
	DtryUtils::formLoader loader3("Valgrind.esp");
	loader3.load(perilousExplosion, 0xd67);
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

	auto node = RE::Offset::PlaceAtMe(a_actor, perilousExplosionNode, 1, false, false);
	node->MoveToNode(a_actor, "WEAPON");
	RE::Offset::PlaceAtMe(node, perilousExplosion, 1, false, false);
	node->SetDelete(true);
	//a_actor->InstantiateHitShader(temp, 0.5);
	

}
