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
	static inline int iDodgeFramework = 0;            /*TKRE = 0; DMCO = 1*/
	static inline int iDodgeDistance = 275;          /*Distance of a single dodge, used for pathing.*/
	static inline int iPermissibleDodgeDistance = 200;        /*Acceptable distance of a single dodge. 
													  * Sometimes the dodger can get obstructed by a wall before completing the whole dodge while already making a big displacement.
													  * AI would take dodges that can make this displacement as a permissible(tho suboptimal) dodge and still make the dodge.*/
	inline static bool bEnableTimedBlock = true;

	static inline bool bEnableDebugDraw = false;
};
