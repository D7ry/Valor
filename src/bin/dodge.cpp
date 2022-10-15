#include "dodge.h"
#include "include/Utils.h"
#include "include/lib/TrueHUDAPI.h"
#include <algorithm>
#define PI 3.1415926535f

/*Get the dodge chance of a reactive dodger in case of an incoming attack.*/
float get_dodge_chance(RE::Actor* a_dodger) {
	if (a_dodger->GetActorRuntimeData().combatController) {
		RE::TESCombatStyle* style = a_dodger->GetActorRuntimeData().combatController->combatStyle;
		if (style) {
			return style->generalData.avoidThreatChance;
		}
	}
	return 0.f;
}
/*Trigger reactive AI surrounding the attacker.*/
void dodge::react_to_attack(RE::Actor* a_attacker)
{
	if (!settings::bDodgeAI_reactive_enable) {
		return;
	}

	RE::TES::GetSingleton()->ForEachReference([&](RE::TESObjectREFR& _refr) {
		if (!_refr.IsDisabled() && _refr.GetFormType() == RE::FormType::ActorCharacter && _refr.GetPosition().GetDistance(a_attacker->GetPosition()) < settings::iDodgeAI_reactive_reactDist) {
			RE::Actor* refr = _refr.As<RE::Actor>();
			if (!refr || refr->IsPlayerRef() || refr->IsDead() || !refr->Is3DLoaded() || refr->IsInKillMove() || !ValhallaUtils::is_adversary(refr, a_attacker)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			if (!Utils::Actor::isHumanoid(refr)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			if (ValhallaUtils::isBackFacing(a_attacker, refr)) { //no need to react to an attack if the attacker isn't facing you.
				return RE::BSContainer::ForEachResult::kContinue;
			}
			switch (settings::iDodgeFramework) {
			case 0:
				dodge::GetSingleton()->attempt_dodge(refr, {dodge_direction::kBackward, dodge_direction::kLeft, dodge_direction::kRight, dodge_direction::kForward });
				break;
			case 1:
				dodge::GetSingleton()->attempt_dodge(refr, dodge_directions_dmco_all);
				break;
			}
		}
		return RE::BSContainer::ForEachResult::kContinue;
	});
}

/*Attempt to dodge an incoming threat, choosing one of the directions from A_DIRECTIONS.*/
void dodge::attempt_dodge(RE::Actor* a_actor, std::vector<dodge_direction> a_directions, bool a_forceDodge)
{
	
	if (!settings::bDodgeAI_enable) {
		return;
	}
	
	float dodge_chance = a_forceDodge ? 1.f : get_dodge_chance(a_actor);
	
	std::mt19937 gen(rd());
	/*Check dodge chance using PRNG*/
	std::uniform_real_distribution<> dis(0.f, 1.f);
	if (dis(gen) > dodge_chance) {
		return;
	}

	if (!able_dodge(a_actor)) {
		return;
	}

	/*Shuffle directions*/
	std::shuffle(a_directions.begin(), a_directions.end(), rd);  //shuffle directions

	for (auto it = a_directions.begin(); it != a_directions.end(); ++it) {
		dodge_direction direction = *it;
		RE::NiPoint3 dodge_dest = Utils::get_abs_pos(a_actor, get_dodge_vector(*it));
		if (can_goto(a_actor, dodge_dest)) {
			do_dodge(a_actor, direction);
			return;
		}
	}

}



/*Check if the actor is able to dodge.*/
bool dodge::able_dodge(RE::Actor* a_actor)
{
	if (a_actor->AsActorState()->GetAttackState() != RE::ATTACK_STATE_ENUM::kNone) {
		return false;
	}
		
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
		if (settings::bEnableDebugDraw) {
			debug::getsingleton()->debugAPI->DrawLine(a_actor->GetPosition(), dest, 1.f, 0xff00ff);  //green line
		}
		
		/*Cast 4 rays from the actor, parallel to the dodging path to check for any obstacles.*/
		float obstacleDist = 0; /*Distance to the obstacle, if any*/
		dest.z += a_actor->GetHeight() * 1 / 4;
		noObstacle &= DtryUtils::rayCast::cast_ray(a_actor, dest, 0.25f, &obstacleDist) == nullptr || obstacleDist >= settings::iDodgeAI_permissibleDist;
		dest.z += a_actor->GetHeight() * 1 / 4;
		noObstacle &= DtryUtils::rayCast::cast_ray(a_actor, dest, 0.5f, &obstacleDist) == nullptr || obstacleDist >= settings::iDodgeAI_permissibleDist;
		dest.z += a_actor->GetHeight() * 1 / 4;
		noObstacle &= DtryUtils::rayCast::cast_ray(a_actor, dest, 0.75f, &obstacleDist) == nullptr || obstacleDist >= settings::iDodgeAI_permissibleDist;
	}
	

	return canNavigate && noObstacle;
	
}


/*Get the direction the actor should dodge in.*/
dodge::dodge_direction dodge::get_dodge_direction(RE::Actor* a_actor, RE::Actor* a_attacker)
{
	return dodge_direction::kForward; /*defaults to backward dodging for now*/
}

void dodge::do_dodge(RE::Actor* a_actor, dodge_direction a_direction)
{
	switch (a_direction) {
	case dodge_direction::kForward:
		switch (settings::iDodgeFramework) {
		case 0: 
			a_actor->NotifyAnimationGraph("TKDodgeForward");
			break;
		case 1:
			a_actor->NotifyAnimationGraph("Dodge");
		}
		break;
	case dodge_direction::kBackward:
		switch (settings::iDodgeFramework) {
		case 0:
			a_actor->NotifyAnimationGraph("TKDodgeBack");
			break;
		}
		break;
	case dodge_direction::kLeft:
		switch (settings::iDodgeFramework) {
		case 0:
			a_actor->NotifyAnimationGraph("TKDodgeLeft");
			break;
		}
		break;
	case dodge_direction::kRight:
		switch (settings::iDodgeFramework) {
		case 0:
			a_actor->NotifyAnimationGraph("TKDodgeRight");
			break;
		}
		break;
	/*Only possible for DMCO*/
	case dodge_direction::kLeftBackward:
		break;
	case dodge_direction::kLeftForward:
		break;
	case dodge_direction::kRightBackward:
		break;
	case dodge_direction::kRightForward:
		break;
	}
}
/*Get relative dodge vector*/
RE::NiPoint3 dodge::get_dodge_vector(dodge_direction a_direction)
{
	RE::NiPoint3 ret;
	switch (a_direction) {
	case kForward:
		ret.x = 0;
		ret.y = settings::iDodgeAI_Distance;
		ret.z = 0;
		break;
	case kBackward:
		ret.x = 0;
		ret.y = -settings::iDodgeAI_Distance;
		ret.z = 0;
		break;
	case kLeft:
		ret.x = -settings::iDodgeAI_Distance;
		ret.y = 0;
		ret.z = 0;
		break;
	case kRight:
		ret.x = settings::iDodgeAI_Distance;
		ret.y = 0;
		ret.z = 0;
		break;
	}
	return ret;
}
