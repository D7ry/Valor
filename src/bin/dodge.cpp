#include "dodge.h"
#include "include/Utils.h"
#include "include/lib/TrueHUDAPI.h"
#define PI 3.1415926535f


void dodge::attempt_dodge(RE::Actor* a_actor, RE::Actor* a_attacker)
{
	if (!a_actor->IsPlayerRef()) {//TODO:get rid of this temporary check
		return;
	}
	if (!able_dodge(a_actor)) {
		return;
	}
	
	dodge_direction dir = get_dodge_direction(a_actor, a_attacker);
	RE::NiPoint3 dodge_dest = inlineUtils::get_abs_pos(a_actor, get_dodge_vector(dir));

	if (can_dodge_do_position(a_actor, dodge_dest)) {
		//do dodge
	}
	
	DtryUtils::rayCast::cast_ray(a_actor, 1, 500);
}

/*Check if the actor is able to dodge.*/
bool dodge::able_dodge(RE::Actor* a_actor)
{
	return true;
}


bool can_dodge_do_position(RE::Actor* a_actor, RE::NiPoint3 a_pos) 
{
	bool canNavigate = false;
	bool noObstacle = false;
	if (a_actor->CanNavigateToPosition(a_actor->GetPosition(), a_pos, 4.0f, a_actor->GetBoundRadius())) 
	{
		debug::getsingleton()->debugAPI->DrawLine(a_actor->GetPosition(), a_pos, 1.f, 0xff00ff);//green line
		canNavigate = true;
	} else {
		debug::getsingleton()->debugAPI->DrawLine(a_actor->GetPosition(), a_pos, 1.f);
	}

	

	return canNavigate && noObstacle;
	
}


/*Get the direction the actor should dodge in.*/
dodge::dodge_direction dodge::get_dodge_direction(RE::Actor* a_actor, RE::Actor* a_attacker)
{
	return dodge_direction::back; /*defaults to backward dodging for now*/
}

void dodge::do_dodge(RE::Actor* a_actor, dodge_direction a_direction)
{
}

/*Get relative dodge vector*/
RE::NiPoint3 dodge::get_dodge_vector(dodge_direction a_direction)
{
	RE::NiPoint3 ret;
	switch (a_direction) {
	case forward:
		ret.x = 0;
		ret.y = 150;
		ret.z = 0;
		break;
	case back:
		ret.x = 0;
		ret.y = -150;
		ret.z = 0;
		break;
	case left:
		ret.x = -150;
		ret.y = 0;
		ret.z = 0;
		break;
	case right:
		ret.x = 150;
		ret.y = 0;
		ret.z = 0;
		break;
	}
	return ret;
}
