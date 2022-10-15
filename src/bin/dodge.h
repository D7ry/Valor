#pragma once
#include "PCH.h"
#include "include/lib/TrueHUDAPI.h"
class dodge
{
private:
	std::random_device rd;
public:
	enum dodge_direction
	{
		kForward = 1,
		kRightForward = 2,
		kRight = 3,
		kRightBackward = 4,
		kBackward = 5,
		kLeftBackward = 6,
		kLeft = 7,
		kLeftForward = 8
	};
	
	
	static dodge* GetSingleton()
	{
		static dodge singleton;
		return &singleton;
	}
	
	static void attempt_active_dodge(RE::Actor* a_dodger, RE::Actor* a_attacker);
	
	void attempt_dodge(RE::Actor* a_actor, std::vector<dodge_direction> a_directions, bool a_forceDodge = false);

	void react_to_attack(RE::Actor* a_attacker);
	
	static inline const std::vector<dodge_direction> dodge_directions_tk_horizontal = { dodge_direction::kLeft, dodge_direction::kRight };
	static inline const std::vector<dodge_direction> dodge_directions_tk_back = { dodge_direction::kBackward };
	static inline const std::vector<dodge_direction> dodge_directions_tk_vertical = { dodge_direction::kForward, dodge_direction::kBackward };
	static inline const std::vector<dodge_direction> dodge_directions_tk_all = { dodge_direction::kForward, dodge_direction::kBackward, dodge_direction::kLeft, dodge_direction::kRight };

	
	static inline const std::vector<dodge_direction> dodge_directions_dmco_horizontal = { dodge_direction::kLeft, dodge_direction::kRight, dodge_direction::kLeftForward, dodge_direction::kRightForward, dodge_direction::kLeftBackward, dodge_direction::kRightBackward };
	static inline const std::vector<dodge_direction> dodge_directions_dmco_back = { dodge_direction::kBackward, dodge_direction::kLeftBackward, dodge_direction::kRightBackward };
	static inline const std::vector<dodge_direction> dodge_directions_dmco_vertical = { dodge_direction::kForward, dodge_direction::kBackward, dodge_direction::kLeftForward, dodge_direction::kRightForward, dodge_direction::kLeftBackward, dodge_direction::kRightBackward };
	static inline const std::vector<dodge_direction> dodge_directions_dmco_all = {
		dodge_direction::kForward,
		dodge_direction::kBackward,
		dodge_direction::kLeft,
		dodge_direction::kRight,
		dodge_direction::kLeftForward,
		dodge_direction::kRightForward,
		dodge_direction::kLeftBackward,
		dodge_direction::kRightBackward
	};
private:
	
	bool able_dodge(RE::Actor* a_actor);

	bool can_goto(RE::Actor* a_actor, RE::NiPoint3 a_dest);
	
	dodge_direction get_dodge_direction(RE::Actor* a_actor, RE::Actor* a_attacker);

	void do_dodge(RE::Actor* a_actor, dodge_direction a_direction);

	RE::NiPoint3 get_dodge_vector(dodge_direction a_direction);
	


	
};
