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
	
	if (can_goto(a_actor, dir)) {
		//do_dodge(a_actor, dir);
	}
	

	
}

/*Check if the actor is able to dodge.*/
bool dodge::able_dodge(RE::Actor* a_actor)
{
	return true;
}


bool dodge::can_goto(RE::Actor* a_actor, RE::NiPoint3 a_dest)
{
	bool canNavigate = false;
	bool noObstacle = false;
	RE::NiPoint3 dodge_dest = inlineUtils::get_abs_pos(a_actor, get_dodge_vector(a_direction));//TODO: do this calculation only once
	if (a_actor->CanNavigateToPosition(a_actor->GetPosition(), dodge_dest, 4.0f, a_actor->GetBoundRadius())) 
	{
		debug::getsingleton()->debugAPI->DrawLine(a_actor->GetPosition(), dodge_dest, 1.f, 0xff00ff);  //green line
		canNavigate = true;
	} else {
		debug::getsingleton()->debugAPI->DrawLine(a_actor->GetPosition(), dodge_dest, 1.f);
	}

	if (DtryUtils::rayCast::cast_ray(a_actor, get_dodge_angle(a_direction), 150) == nullptr) {
		noObstacle = true;
	}
	
	return canNavigate && noObstacle;
	
}


/*Get the direction the actor should dodge in.*/
dodge::dodge_direction dodge::get_dodge_direction(RE::Actor* a_actor, RE::Actor* a_attacker)
{
	return dodge_direction::forward; /*defaults to backward dodging for now*/
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

/*Return dodge angle in form of multiples of pi.*/
int dodge::get_dodge_angle(dodge_direction a_direction) {
	int ret;
	switch (a_direction) {
	case forward:
		ret = 0;
		break;
	case back:
		ret = 1;
		break;
	case left:
		ret = 0.5;
		break;
	case right:
		ret = 1.5;
		break;
	}
	return ret;
}
