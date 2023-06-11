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
	reader.FETCH(bDodgeAI_AttackCancel_enable);
	
	reader.FETCH(iDodgeAI_Framework);
	reader.FETCH(fDodgeAI_DodgeDist);
	fDodgeAI_DodgeDist2 = fDodgeAI_DodgeDist / SQRT2;
	reader.FETCH(fDodgeAI_DodgeDist_Permissible);
	reader.FETCH(fDodgeAI_Reactive_Dist);
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

	reader.setActiveSection("PerfectBlock");
	reader.FETCH(bPerfectBlock_enable);
	reader.FETCH(fPerfectBlock_chance_mult);


	reader.log();
	
}

void settings::init()
{
	update_handler::Register();
}

inline EventResult settings::update_handler::ProcessEvent(const SKSE::ModCallbackEvent* a_event, RE::BSTEventSource<SKSE::ModCallbackEvent>* a_eventSource)
{
	if (!a_event) {
		return EventResult::kContinue;
	}
	if (a_event->eventName == "dmenu_updateSettings" && a_event->strArg == "Valor") {
		settings::read();
	}

	return EventResult::kContinue;
}

inline bool settings::update_handler::Register()
{
	static update_handler singleton;

	auto eventSource = SKSE::GetModCallbackEventSource();

	if (!eventSource) {
		ERROR("EventSource not found!");
		return false;
	}
	eventSource->AddEventSink(&singleton);
	INFO("Register {}", typeid(singleton).name());
	return true;
}
