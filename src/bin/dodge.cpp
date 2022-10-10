#include "dodge.h"
#include "include/Utils.h"
#include "include/lib/TrueHUDAPI.h"
#include <algorithm>
#define PI 3.1415926535f


bool dodge::attempt_passive_dodge(RE::TESObjectREFR& a_ref)
{
	if (a_ref.GetFormType() != RE::FormType::ActorCharacter) {
		return true;
	}
	RE::Actor* actor = a_ref.As<RE::Actor>();
	if (actor->IsPlayerRef() || actor->IsDead()) {
		return true;
	}

	auto skeleton = a_ref.As<RE::Actor>()->GetRace()->skeletonModels->GetModel();
	if (strcmp(skeleton, "Actors\Character\Character Assets\skeleton.nif")) {
		dodge::GetSingleton()->attempt_dodge(actor, dodge_directions_all);
	}

	return true;
}


/*Trigger reactive AI surrounding the attacker.*/
void dodge::react_to_attack(RE::Actor* a_attacker)
{
	if (!a_attacker->IsPlayerRef()) {
		return; // dodge player for now.
	}
	RE::TES::GetSingleton()->ForEachReferenceInRange(a_attacker, 200, &attempt_passive_dodge);
}

/*Attempt to dodge an incoming thread, choosing one of the directions from A_DIRECTIONS.*/
void dodge::attempt_dodge(RE::Actor* a_actor, std::vector<dodge_direction> a_directions)
{

	if (!able_dodge(a_actor)) {
		return;
	}

	std::shuffle(a_directions.begin(), a_directions.end(), rd);  //shuffle directions

	for (auto it = a_directions.begin(); it != a_directions.end(); ++it) {
		dodge_direction direction = *it;
		RE::NiPoint3 dodge_dest = inlineUtils::get_abs_pos(a_actor, get_dodge_vector(*it));
		if (can_goto(a_actor, dodge_dest)) {
			do_dodge(a_actor, direction);
			return;
		}
	}

}



/*Check if the actor is able to dodge.*/
bool dodge::able_dodge(RE::Actor* a_actor)
{
	return true;
}

#define MAX_DIST_DIFFERENCE 50
bool dodge::can_goto(RE::Actor* a_actor, RE::NiPoint3 a_dest)
{
	bool canNavigate = false;
	bool noObstacle = true;
	RE::NiPoint3 raycast_dest = a_dest;
	RE::NiPoint3 nav_dest = a_dest;

	bool gotoNavdest = false;
	float expect_dist = a_actor->GetPosition().GetDistance(a_dest);
	
	/*Use Skyrim's internal check*/
	if (a_actor->UpdateNavPos(a_actor->GetPosition(), nav_dest, 4.0f, a_actor->GetBoundRadius()) 
		&& abs(nav_dest.GetDistance(a_actor->GetPosition()) - expect_dist) < MAX_DIST_DIFFERENCE) {
		RE::NiPoint3 nav_dest_raycast = nav_dest;                   // make a copy of nav_dest to use for raycast
		if (DtryUtils::rayCast::object_exists(nav_dest_raycast)) {//check if the actor can stand on the nav dest
			gotoNavdest = true;
			canNavigate = true;
		}
	}
	
	/*Use our own pathing check, if skyrim's check fails.*/
	if (!gotoNavdest) {
		canNavigate = DtryUtils::rayCast::object_exists(raycast_dest, 75.f);//bigger range to account for slopes
	}
	
	if (canNavigate) {
		RE::NiPoint3 dest = gotoNavdest ? nav_dest : raycast_dest;
		debug::getsingleton()->debugAPI->DrawLine(a_actor->GetPosition(), dest, 1.f, 0xff00ff);  //green line
		
		/*Cast a ray from the centre of the actor to the destination. If the raycast gets nothing, nothing significant is on the way.*/
		dest.z += a_actor->GetHeight() * 1 / 4;
		noObstacle &= DtryUtils::rayCast::cast_ray(a_actor, dest, 0.25f) == nullptr;
		dest.z += a_actor->GetHeight() * 1 / 4;
		noObstacle &= DtryUtils::rayCast::cast_ray(a_actor, dest, 0.5f) == nullptr;
		dest.z += a_actor->GetHeight() * 1 / 4;
		noObstacle &= DtryUtils::rayCast::cast_ray(a_actor, dest, 0.75f) == nullptr;
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
	switch (a_direction) {
	case dodge_direction::forward:
		a_actor->NotifyAnimationGraph("TKDodgeForward");
		break;
	case dodge_direction::back:
		a_actor->NotifyAnimationGraph("TKDodgeBack");
		break;
	case dodge_direction::left:
		a_actor->NotifyAnimationGraph("TKDodgeLeft");
		break;
	case dodge_direction::right:
		a_actor->NotifyAnimationGraph("TKDodgeRight");
		break;
	}
}
#define DODGE_DIST 275
/*Get relative dodge vector*/
RE::NiPoint3 dodge::get_dodge_vector(dodge_direction a_direction)
{
	RE::NiPoint3 ret;
	switch (a_direction) {
	case forward:
		ret.x = 0;
		ret.y = DODGE_DIST;
		ret.z = 0;
		break;
	case back:
		ret.x = 0;
		ret.y = -DODGE_DIST;
		ret.z = 0;
		break;
	case left:
		ret.x = -DODGE_DIST;
		ret.y = 0;
		ret.z = 0;
		break;
	case right:
		ret.x = DODGE_DIST;
		ret.y = 0;
		ret.z = 0;
		break;
	}
	return ret;
}
