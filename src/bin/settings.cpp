#include "settings.h"
#include "include/Utils.h"
#define SETTINGFILE_PATH "Data\\SKSE\\Plugins\\Valor\\Settings.ini"

#define FETCH(setting) load(setting, #setting)
void settings::read()
{
	DtryUtils::settingsLoader reader(SETTINGFILE_PATH);
	reader.setActiveSection("Dodge");
	reader.FETCH(bDodgeAI_enable);
	reader.FETCH(bDodgeAI_passive_enable);
	reader.FETCH(bDodgeAI_reactive_enable);
	
	reader.FETCH(iDodgeFramework);
	reader.FETCH(iDodgeAI_Distance);
	iDodgeAI_Distance_2 = iDodgeAI_Distance / SQRT2;
	reader.FETCH(iDodgeAI_permissibleDist);
	reader.FETCH(iDodgeAI_reactive_reactDist);
	reader.FETCH(fDodgeAI_chanceMult);
	
	reader.FETCH(bEnableDebugDraw);

	reader.setActiveSection("Perilous");
	reader.FETCH(bPerilous_enable);
	reader.FETCH(bPerilous_chargeTime_enable);
	reader.FETCH(fPerilous_chargeTime_multiplier);
	reader.FETCH(fPerilous_chargeTime_duration);
	reader.FETCH(fPerilous_chance_multiplier);
	
	reader.setActiveSection("AttackCommitment");
	reader.FETCH(bNPCCommitment_enable);
	reader.FETCH(fNPCCommitment_AttackStartMult);
	reader.FETCH(fNPCCommitment_AttackMidMult);
	reader.FETCH(fNPCCommitment_AttackEndMult);


	reader.log();
	
}
