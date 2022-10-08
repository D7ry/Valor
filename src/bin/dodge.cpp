#include "dodge.h"

void dodge::attempt_dodge(RE::Actor* a_actor, RE::Actor* a_attacker)
{
	if (!can_dodge(a_actor)) {
		return;
	}

	
}

/*Check if the able is able to dodge.*/
bool dodge::can_dodge(RE::Actor* a_actor)
{
	return true;
}
