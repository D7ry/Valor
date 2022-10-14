#pragma once
class settings
{
public:
	static void read();
	
	/*List of settings*/
	inline static bool bPerilous_enable = true;
	static inline bool bPerilous_chargeTime_enable = true; /*Beginning of perilous attacks are performed slower to simulate a charging effect and giving the player time to react.*/ 
	static inline float fPerilous_chargeTime_multiplier = 0.1 /*Animation speed during the charge phase.*/;
	static inline float fPerilous_chargeTime_duration = 0.5; /*Duration of the charge phase.*/

	inline static bool bDodgeAI_enable = true; /*Global switch for dodge AI*/
	inline static bool bDodgeAI_passive_enable = true; /*Passive dodge AI that's built on top of vanilla's combat movement AI.*/
	inline static bool bDodgeAI_reactive_enable = true; /*Reactive dodge AI that dodges incoming attacks.*/
	static inline int iDodgeFramework = 0;            /*TKRE = 0; DMCO = 1*/
	
	static inline int iDodgeAI_reactive_reactDist = 350;      /**/
	static inline int iDodgeAI_Distance = 300;          /*Distance of a single dodge, used for pathing.*/
	static inline int iDodgeAI_permissibleDist = 200;        /*Acceptable distance of a single dodge. 
													  * Sometimes the dodger can get obstructed by a wall before completing the whole dodge while already making a big displacement.
													  * AI would take dodges that can make this displacement as a permissible(tho suboptimal) dodge and still make the dodge.*/
	inline static bool bEnableTimedBlock = true;

	static inline bool bEnableDebugDraw = false;
};
