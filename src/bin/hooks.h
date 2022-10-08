#include "SKSE/Trampoline.h"
namespace hooks
{
	class on_attack_action
	{
	public:
		static void install()
		{
			auto& trampoline = SKSE::GetTrampoline();
			REL::Relocation<std::uintptr_t> AttackActionBase{ RELOCATION_ID(48139, 49170) };
			_perform_atk_action = trampoline.write_call<5>(AttackActionBase.address() + RELOCATION_OFFSET(0x4D7, 0x435), perform_atk_action);
			logger::info("hook:on_attack_action");
		}

	private:
		static bool perform_atk_action(RE::TESActionData* a_actionData);

		static inline REL::Relocation<decltype(perform_atk_action)> _perform_atk_action;
	};

	using EventResult = RE::BSEventNotifyControl;

	class on_animation_event
	{
	public:
		static void install()
		{
			REL::Relocation<uintptr_t> AnimEventVtbl_NPC{ RE::VTABLE_Character[2] };
			REL::Relocation<uintptr_t> AnimEventVtbl_PC{ RE::VTABLE_PlayerCharacter[2] };

			_ProcessEvent_NPC = AnimEventVtbl_NPC.write_vfunc(0x1, ProcessEvent_NPC);
			_ProcessEvent_PC = AnimEventVtbl_PC.write_vfunc(0x1, ProcessEvent_PC);
			logger::info("hook:on_animation_event");
		}

	private:
		static inline void ProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink, RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource);

		static EventResult ProcessEvent_NPC(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink, RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource);
		static EventResult ProcessEvent_PC(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink, RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource);

		static inline REL::Relocation<decltype(ProcessEvent_NPC)> _ProcessEvent_NPC;
		static inline REL::Relocation<decltype(ProcessEvent_NPC)> _ProcessEvent_PC;
	};


};
