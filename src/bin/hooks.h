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
	
	typedef uintptr_t ptr_CombatPath;
	class on_combatBehavior_backoff_createPath
	{
	public:
		static void install()
		{
			auto& trampoline = SKSE::GetTrampoline();
			REL::Relocation<std::uintptr_t> on_combatBehavior_backoff{ RELOCATION_ID(46724, 47920) }; //SE::sub_1407D8C90+2D0	call    Character__sub_14083BE20 AE:: sub_140804020+284	call    sub_14086B7C0
			_create_path = trampoline.write_call<5>(on_combatBehavior_backoff.address() + RELOCATION_OFFSET(0x2D0, 0x284), create_path); //SE::
			logger::info("hook:on_combatBehavior_backoff_createPath");
		}
		
	private:
		static ptr_CombatPath create_path(RE::Actor* a_actor, RE::NiPoint3* a_newPos, float a3, int speed_ind);
		static inline REL::Relocation<decltype(create_path)> _create_path;
		
	};

	class on_combatBehavior_circle_createPath
	{
	public:
		static void install()
		{
			auto& trampoline = SKSE::GetTrampoline();
			REL::Relocation<std::uintptr_t> on_combatBehavior_backoff{ RELOCATION_ID(46720, 47916) };  //SE::sub_1407D8200+593	call    Character__sub_14083BE20 AE:: 140803580+5F0 CALL 14086B7C0
			_create_path = trampoline.write_call<5>(on_combatBehavior_backoff.address() + RELOCATION_OFFSET(0x593, 0x5F0), create_path);
			logger::info("hook:on_combatBehaiovr_circle_createPath");
		}
		
	private:
		static ptr_CombatPath create_path(RE::Actor* a_actor, RE::NiPoint3* a_newPos, float a3, int speed_ind);
		static inline REL::Relocation<decltype(create_path)> _create_path;
	};
	
	class on_combatBehavior_fallback_createPath
	{
	public:
		static void install()
		{
			auto& trampoline = SKSE::GetTrampoline();
			REL::Relocation<std::uintptr_t> on_combatBehavior_backoff{ RELOCATION_ID(46712, 47908) };  //SE::sub_1407D73D0+299	call    Character__sub_14083BE20 AE::sub_140802680+257	call    sub_14086B7C0
			_create_path = trampoline.write_call<5>(on_combatBehavior_backoff.address() + RELOCATION_OFFSET(0x299, 0x257), create_path);
			logger::info("hook:on_combatBehavior_fallback_createPath");
		}
		
	private:
		static ptr_CombatPath create_path(RE::Actor* a_actor, RE::NiPoint3* a_newPos, float a3, int speed_ind);
		static inline REL::Relocation<decltype(create_path)> _create_path;
	};

	class on_combatBehavior_dodgethreat_createPath
	{
	public:
		static void install()
		{
			auto& trampoline = SKSE::GetTrampoline();
			REL::Relocation<std::uintptr_t> on_combatBehavior_backoff{ RELOCATION_ID(46590, 47802) };  //SE::sub_1407D1740+82E	call    Character__sub_14083BE20 AE::1407FC830 + 7A0 call 14086B7C0
			_create_path = trampoline.write_call<5>(on_combatBehavior_backoff.address() + RELOCATION_OFFSET(0x82E, 0x7A0), create_path);
			logger::info("hook:on_combatBehavior_dodgethreat_createPath");
		}
		
	private:
		static ptr_CombatPath create_path(RE::Actor* a_actor, RE::NiPoint3* a_newPos, float a3, int speed_ind);
		static inline REL::Relocation<decltype(create_path)> _create_path;
	};
	
	
	class on_melee_hit
	{
	public:
		static void install()
		{
			REL::Relocation<uintptr_t> hook{ RELOCATION_ID(37673, 38627) };  //140628C20       14064E760
			auto& trampoline = SKSE::GetTrampoline();
			_ProcessHit = trampoline.write_call<5>(hook.address() + RELOCATION_OFFSET(0x3C0, 0x4A8), processHit);
			logger::info("hook:on_melee_hit");
		}

	private:
		static void processHit(RE::Actor* victim, RE::HitData& hitData);
		static inline REL::Relocation<decltype(processHit)> _ProcessHit;  //140626400       14064BAB0
	};


	class on_set_rotation
	{
	public:
		static void install()
		{
			auto& trampoline = SKSE::GetTrampoline();
			REL::Relocation<uintptr_t> hook1{ RELOCATION_ID(32042, 32796) };  // 4EC300, 504B30  // synchronized anims
			REL::Relocation<uintptr_t> hook2{ RELOCATION_ID(36365, 37356) };  // 5D87F0, 5FD7E0

			_Actor_SetRotationX = trampoline.write_call<5>(hook1.address() + RELOCATION_OFFSET(0x4DC, 0x667), Actor_SetRotationX);  // 4EC7DC
			_Actor_SetRotationZ = trampoline.write_call<5>(hook2.address() + RELOCATION_OFFSET(0x9C7, 0xA87), Actor_SetRotationZ);  // 5D91B7
		}

	private:
		static void Actor_SetRotationX(RE::Actor* a_this, float a_angle);
		static void Actor_SetRotationZ(RE::Actor* a_this, float a_angle);
		static inline REL::Relocation<decltype(Actor_SetRotationX)> _Actor_SetRotationX;
		static inline REL::Relocation<decltype(Actor_SetRotationZ)> _Actor_SetRotationZ;
	};


	inline void alloc() 
	{
		SKSE::AllocTrampoline(1 << 8);
	}

};
