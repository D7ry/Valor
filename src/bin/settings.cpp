#include "settings.h"
#include "include/Utils.h"
#define SETTINGFILE_PATH "Data\\SKSE\\Plugins\\Valgrind\\Settings.ini"

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
	reader.FETCH(iDodgeAI_permissibleDist);
	reader.FETCH(iDodgeAI_reactive_reactDist);
	reader.FETCH(bEnableDebugDraw);

	reader.log();
	
}
