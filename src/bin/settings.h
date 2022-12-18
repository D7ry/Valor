#pragma once
#define SQRT2 1.4142135623730950488016887242097

class settings
{
public:
	static void read();

	/*Perilous attack*/
	inline static bool bPerilous_attack_enable = true;
	static inline bool bPerilous_attack_chargeTime_enable = true; /*Beginning of perilous attacks are performed slower to simulate a charging effect and giving the player time to react.*/ 
	static inline float fPerilous_attack_chargeTime_multiplier = 0.1 /*Animation speed during the charge phase.*/;
	static inline float fPerilous_attack_chargeTime_duration = 0.5; /*Duration of the charge phase.*/
	static inline float fPerilous_attack_chance_multiplier = 1;
	/*Perilous bash*/
	static inline bool bPerilous_bash_enable = true;
	static inline float fPerilous_bash_chargeTime_multiplier = 0.1;
	static inline float fPerilous_bash_chargeTime_duration = 1;

	/*Dodging*/
	inline static bool bDodgeAI_enable = true; /*Global switch for dodge AI*/
	inline static bool bDodgeAI_passive_enable = true; /*Passive dodge AI that's built on top of vanilla's combat movement AI.*/
	inline static bool bDodgeAI_reactive_enable = true; /*Reactive dodge AI that dodges incoming attacks.*/
	static inline int iDodgeFramework = 0;            /*TKRE = 0; DMCO = 1*/
	
	static inline float fDodgeAI_chanceMult = 1; /*Chance multiplier for reactive dodge AI.*/
	static inline float iDodgeAI_reactive_reactDist = 350;               /**/
	static inline float iDodgeAI_Distance = 300;          /*Distance of a single dodge, used for pathing.*/
	static inline float iDodgeAI_Distance_2 = iDodgeAI_Distance / SQRT2; /* Cached coordinate distance.*/
	
	static inline float iDodgeAI_permissibleDist = 200; /*Acceptable distance of a single dodge. 
													  * Sometimes the dodger can get obstructed by a wall before completing the whole dodge while already making a big displacement.
													  * AI would take dodges that can make this displacement as a permissible(tho suboptimal) dodge and still make the dodge.*/
	static inline bool bEnableDebugDraw = false;
	
	/*NPC Commitment*/
	static inline bool bNPCCommitment_enable = true;
	static inline float fNPCCommitment_AttackStartMult = 1.f;
	static inline float fNPCCommitment_AttackMidMult = 0.1f;
	static inline float fNPCCommitment_AttackEndMult = 0.f;

	/*Timed block*/
	inline static bool bEnableTimedBlock = true;

};
