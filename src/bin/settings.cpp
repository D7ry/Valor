#include "settings.h"
#include "include/Utils.h"
#define SETTINGFILE_PATH "Data\\SKSE\\Plugins\\Valor\\Settings.ini"

#define FETCH(setting) load(setting, #setting)
void settings::read()
{
	DtryUtils::settingsLoader reader(SETTINGFILE_PATH);
	reader.setActiveSection("Dodge");
	reader.FETCH(bDodgeAI_Enable);
	reader.FETCH(bDodgeAI_Passive_enable);
	reader.FETCH(bDodgeAI_Reactive_enable);
	
	reader.FETCH(iDodgeAI_Framework);
	reader.FETCH(iDodgeAI_DodgeDist);
	fDodgeAI_DodgeDist2 = iDodgeAI_DodgeDist / SQRT2;
	reader.FETCH(fDodgeAI_DodgeDist_Permissible);
	reader.FETCH(iDodgeAI_Reactive_Dist);
	reader.FETCH(fDodgeAI_Chance_Mult);
	
	reader.FETCH(bDodgeAI_DebugDraw_Enable);

	reader.setActiveSection("Perilous");
	reader.FETCH(bPerilousAttack_Enable);
	reader.FETCH(bPerilousAttack_ChargeTime_Enable);
	reader.FETCH(fPerilousAttack_ChargeTime_Mult);
	reader.FETCH(fPerilousAttack_ChargeTime_Duration);
	reader.FETCH(fPerilousAttack_Chance_Mult);
	
	reader.FETCH(bPerilousBash_Enable);
	reader.FETCH(fPerilousBash_ChargeTime_Mult);
	reader.FETCH(fPerilousBash_ChargeTime_Duration);
	
	reader.setActiveSection("AttackCommitment");
	reader.FETCH(bNPCCommitment_Enable);
	reader.FETCH(fNPCCommitment_AttackStartMult);
	reader.FETCH(fNPCCommitment_AttackMidMult);
	reader.FETCH(fNPCCommitment_AttackEndMult);


	reader.log();
	
}
