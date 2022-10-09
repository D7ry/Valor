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
	if (actor->IsPlayerRef()) {
		return true;
	}
	
	auto skeleton = a_ref.As<RE::Actor>()->GetRace()->skeletonModels->GetModel();
	if (strcmp(skeleton, "Actors\Character\Character Assets\skeleton.nif")) {
		dodge::GetSingleton()->attempt_dodge(actor, dodge_directions_all);
	}

	return true;
}

void dodge::passive_dodge(RE::Actor* a_attacker)
{
	if (!a_attacker->IsPlayerRef()) {
		return; // dodge player for now.
	}
	RE::TES::GetSingleton()->ForEachReferenceInRange(a_attacker, 200, &attempt_passive_dodge);
}

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
		do_dodge(a_actor, *it);
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
	bool noObstacle = false;
	RE::NiPoint3 real_dest = a_dest;
	float expect_dist = a_actor->GetPosition().GetDistance(a_dest);
	if (a_actor->UpdateNavPos(a_actor->GetPosition(), real_dest, 4.0f, a_actor->GetBoundRadius()) 
		&& abs(real_dest.GetDistance(a_actor->GetPosition()) - expect_dist) < MAX_DIST_DIFFERENCE) 
	{
		debug::getsingleton()->debugAPI->DrawLine(a_actor->GetPosition(), real_dest, 1.f, 0xff00ff);  //green line
		canNavigate = true;
	} else {
		debug::getsingleton()->debugAPI->DrawLine(a_actor->GetPosition(), real_dest, 1.f);  //red line; can't go there
	}

	/*Cast a ray from the centre of the actor to the destination. If the raycast gets nothing, nothing significant is on the way.*/
	RE::NiPoint3 dest_center = real_dest;
	dest_center.z += a_actor->GetHeight() / 2;  //raise the height to the center of the actor
	if (DtryUtils::rayCast::cast_ray(a_actor, dest_center) == nullptr) {
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
#define DODGE_DIST 250
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
