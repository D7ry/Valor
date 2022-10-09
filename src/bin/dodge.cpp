#include "dodge.h"
#include "include/lib/TrueHUDAPI.h"
#define PI 3.1415926535f

/*Calculate the absolute position of an actor after performing a dodge.*/
inline RE::NiPoint3 get_dodge_destination(RE::Actor* a_actor, RE::NiPoint3 a_dodge_vec) {
	float dodge_len = sqrt(a_dodge_vec.x * a_dodge_vec.x + a_dodge_vec.y * a_dodge_vec.y);
	float actorAngleZ = a_actor->GetAngleZ();
	actorAngleZ -= 2 * actorAngleZ; /*Invert angle across axis*/

	float dodge_vecAngle = atan2(a_dodge_vec.y, a_dodge_vec.x);
	float dodge_vecAngleZ = actorAngleZ + dodge_vecAngle;
	
	float x_displacement = dodge_len * cos(dodge_vecAngleZ);
	float y_displacement = dodge_len * sin(dodge_vecAngleZ);

	RE::NiPoint3 dodge_dest = a_actor->GetPosition();
	dodge_dest.x += x_displacement;
	dodge_dest.y += y_displacement;
	return dodge_dest;
}

void dodge::attempt_dodge(RE::Actor* a_actor, RE::Actor* a_attacker)
{
	if (!can_dodge(a_actor)) {
		return;
	}
	
	RE::NiPoint3 dodge_dest = get_dodge_destination(a_actor, get_dodge_vector(get_dodge_direction(a_actor, a_attacker)));
	

	debugAPI->DrawLine(a_actor->GetPosition(), dodge_dest, 1);
	
	//if (a_attacker->CanNavigateToPosition(a_actor->GetPosition(), new_pos)) {
		//do_dodge(a_actor, direction);
	//}
	
}

/*Check if the able is able to dodge.*/
bool dodge::can_dodge(RE::Actor* a_actor)
{
	return true;
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
		ret.y = 100;
		ret.z = 0;
		break;
	case back:
		ret.x = 0;
		ret.y = -100;
		ret.z = 0;
		break;
	case left:
		ret.x = -100;
		ret.y = 0;
		ret.z = 0;
		break;
	case right:
		ret.x = 100;
		ret.y = 0;
		ret.z = 0;
		break;
	}
	return ret;
}
