#include "settings.h"
#include "include/Utils.h"
#define SETTINGFILE_PATH "Data\\SKSE\\Plugins\\Valgrind\\Settings.ini"

void settings::read()
{
	DtryUtils::settingsLoader reader(SETTINGFILE_PATH);
	reader.setActiveSection("Dodge");
	reader.load(bEnableDebugDraw, "bEnableDebugDraw");
	reader.load(bEnableDodgeAI, "bEnableDodgeAI");
	reader.load(bEnableDodgeAI_passive, "bEnableDodgeAI_passive");
	reader.load(bEnableDodgeAI_active, "bEnableDodgeAI_active");
	reader.load(iDodgeFramework, "iDodgeFramework");
	reader.load(iDodgeDistance, "iDodgeDistance");
	reader.load(iPermissibleDodgeDistance, "iPermissibleDodgeDistance");
	reader.log();
	
}
