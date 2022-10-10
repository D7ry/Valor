#pragma once
class settings
{
public:
	static void read();
	
	/*List of settings*/
	inline static bool bEnablePerilousAttack = false;
	inline static bool bEnableDodgeAI = true; /*Global switch for dodge AI*/
	inline static bool bEnableDodgeAI_passive = true; /*Passive dodge AI that's built on top of vanilla's combat movement AI.*/
	inline static bool bEnableDodgeAI_active = true; /*Reactive dodge AI that dodges incoming attacks.*/
	
	inline static bool bEnableTimedBlock = true;

	static inline bool bEnableDebugDraw = false;
};
