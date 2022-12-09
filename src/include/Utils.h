#pragma once
#include <cmath>
#include  <random>
#include  <iterator>
#include "PCH.h"
#include "bin/offsets.h"
#include "include/lib/TrueHUDAPI.h"
#include "bin/settings.h"

#define CONSOLELOG(msg) RE::ConsoleLog::GetSingleton()->Print(msg);
#define PI 3.1415926535897932384626f

class debug
{
public:
	static debug* getsingleton() {
		static debug singleton;
		return &singleton;
	}

	TRUEHUD_API::IVTrueHUD3* debugAPI;
};

namespace Utils
{
	template <typename Iter, typename RandomGenerator>
	Iter select_randomly(Iter start, Iter end, RandomGenerator& g)
	{
		std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
		std::advance(start, dis(g));
		return start;
	}

	template <typename Iter>
	Iter select_randomly(Iter start, Iter end)
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		return select_randomly(start, end, gen);
	}
	
	void queueMessageBox(RE::BSFixedString a_message);

	void playSound(RE::Actor* a, RE::BGSSoundDescriptorForm* a_descriptor, float a_volumeOverride = 1);
	
	RE::NiPoint3 get_abs_pos(RE::Actor* a_actor, RE::NiPoint3 a_relative_pos);

	void PushActorAway(RE::Actor* causer, RE::Actor* target, float magnitude);

	void SetRotationMatrix(RE::NiMatrix3& a_matrix, float sacb, float cacb, float sb);

	void slowTime(float a_duration, float a_percentage);

	namespace Actor
	{
		RE::TESObjectWEAP* getWieldingWeapon(RE::Actor* a_actor);
		bool isDualWielding(RE::Actor* a_actor);
		bool isEquippedShield(RE::Actor* a_actor);
		bool isPowerAttacking(RE::Actor* a_actor);
		bool isHumanoid(RE::Actor* a_actor);
		void getBodyPos(RE::Actor* a_actor, RE::NiPoint3& pos);
		void dropShield(RE::Actor* a_actor);
	}

	namespace math
	{
#define PI 3.14159265358979323846f
#define TWO_PI 6.28318530717958647692f
		float NormalRelativeAngle(float a_angle);
	}
	
};

/*Utilities related to combat*/
namespace ValhallaUtils
{
	bool is_adversary(RE::Actor* actor1, RE::Actor* actor2);
	bool isBackFacing(RE::Actor* actor1, RE::Actor* actor2);

};

/*Some of my own convenience stuff*/
namespace DtryUtils
{
	/*Helper class to batch load forms from plugin records.*/
	class formLoader
	{
	private:
		RE::BSFixedString _pluginName;
		RE::TESDataHandler* _dataHandler;
		int _loadedForms;

	public:
		formLoader(RE::BSFixedString pluginName);

		void log();


		template <class formType>
		void load(formType*& formRet, RE::FormID formID)
		{
			formRet = _dataHandler->LookupForm<formType>(formID, _pluginName);
			if (!formRet) {
				logger::critical("Error: null formID or wrong form type when loading {} from {}", formID, _pluginName);
			}
			_loadedForms++;
		}
	};

	/*Helper class to load from a simple ini file.*/
	class settingsLoader
	{
	private:
		std::shared_ptr<CSimpleIniA> _ini;
		const char* _section;
		int _loadedSettings;
		const char* _settingsFile;
	public:
		settingsLoader(const char* settingsFile);
		/*Set the active section. Load() will load keys from this section.*/
		void setActiveSection(const char* section);
		
		void load(bool& settingRef, const char* key);
		void load(float& settingRef, const char* key);
		void load(uint32_t& settingRef, const char* key);
		void load(int& settingRef, const char* key);
		
		void log();
	};

	class rayCast
	{

#define YELLOW 0xFFFF00      // yellow
#define GREEN 0xff00ff   //green
	public:

		/*Do a simple raycast at a singular point to check if anything exists there.
		If anything exists, update A_POS argument to the position where raycast is hit.*/
		static bool object_exists(RE::NiPoint3& a_pos, float a_range = 30.f) 
		{
			RE::NiPoint3 rayStart = a_pos;
			RE::NiPoint3 rayEnd = a_pos;
			rayStart.z += a_range;
			rayEnd.z -= a_range;
			auto havokWorldScale = RE::bhkWorld::GetWorldScale();
			RE::bhkPickData pick_data;
			pick_data.rayInput.from = rayStart * havokWorldScale;
			pick_data.rayInput.to = rayEnd * havokWorldScale;
			if (settings::bEnableDebugDraw) {
				debug::getsingleton()->debugAPI->DrawArrow(rayStart, rayEnd, 10, 1);
			}
			auto pc = RE::PlayerCharacter::GetSingleton();
			if (!pc) {
				return false;
			}
			if (!pc->GetParentCell() || !pc->GetParentCell()->GetbhkWorld()) {
				return false;
			}
			pc->GetParentCell()->GetbhkWorld()->PickObject(pick_data);
			if (pick_data.rayOutput.HasHit()) {
				RE::NiPoint3 hitpos = rayStart + (rayEnd - rayStart) * pick_data.rayOutput.hitFraction;
				a_pos = hitpos;  //update the position to the hit position
				if (settings::bEnableDebugDraw) {
					debug::getsingleton()->debugAPI->DrawPoint(hitpos, 10, 3, GREEN);
				}
				return true;
			}
			return false;
		}
		/*Cast a ray from the center of the actor to a_rayEnd, return the first object encountered, or nullptr if nothing is hit.*/
		static RE::TESObjectREFR* cast_ray(RE::Actor* a_actor, RE::NiPoint3 a_rayEnd, float a_castPos = 0.5f, float* ret_rayDist = nullptr) 
		{
			auto havokWorldScale = RE::bhkWorld::GetWorldScale();
			RE::bhkPickData pick_data;
			RE::NiPoint3 rayStart = a_actor->GetPosition();
			float castHeight = a_actor->GetHeight() * a_castPos;
			rayStart.z += castHeight;  //cast from center of actor
						/*Setup ray*/
			pick_data.rayInput.from = rayStart * havokWorldScale;
			pick_data.rayInput.to = a_rayEnd * havokWorldScale;
			if (settings::bEnableDebugDraw) {
				debug::getsingleton()->debugAPI->DrawArrow(rayStart, a_rayEnd, 10, 1);
			}
			
			/*Setup collision filter, ignoring the actor.*/
			uint32_t collisionFilterInfo = 0;
			a_actor->GetCollisionFilterInfo(collisionFilterInfo);
			uint16_t collisionGroup = collisionFilterInfo >> 16;
			pick_data.rayInput.filterInfo = (static_cast<uint32_t>(collisionGroup) << 16) | static_cast<uint32_t>(RE::COL_LAYER::kCharController);
			
			/*Do*/
			a_actor->GetParentCell()->GetbhkWorld()->PickObject(pick_data);
			if (pick_data.rayOutput.HasHit()) {
				RE::NiPoint3 hitpos = rayStart + (a_rayEnd - rayStart) * pick_data.rayOutput.hitFraction;
				if (ret_rayDist) {
					*ret_rayDist = hitpos.GetDistance(rayStart);
				}
				if (settings::bEnableDebugDraw) {
					debug::getsingleton()->debugAPI->DrawPoint(hitpos, 10, 3);
				}

				auto collidable = pick_data.rayOutput.rootCollidable;
				if (collidable) {
					RE::TESObjectREFR* ref = RE::TESHavokUtilities::FindCollidableRef(*collidable);
					if (ref) {
						return ref;
					}
				}
			}
			return nullptr;
		}
				
		
	};
}


/*Interface for manipulating all actor's animation speeds. Manipulation is done by hooking animation_update function. Doesn't work for player.*/
class AnimSpeedManager
{
	class on_updateAnimation_internal;
	class on_updateAnimation_player;

public:
	static void setAnimSpeed(RE::ActorHandle a_actorHandle, float a_speedMult, float a_dur);
	static void revertAnimSpeed(RE::ActorHandle a_actorHandle);
	static void revertAllAnimSpeed();
	
	static void init() {
		on_updateAnimation_internal::install();
		on_updateAnimation_player::install();
	}

private:
	struct AnimSpeedData
	{
		float speedMult;
		float dur;
	};
	static inline std::unordered_map<RE::ActorHandle, AnimSpeedData> _animSpeeds = {};
	static inline std::mutex _animSpeedsLock = std::mutex();

	static void update(RE::ActorHandle a_actorHandle, float& a_deltaTime);
	
	class on_updateAnimation_player
	{
	public:
		static void install()
		{
			auto& trampoline = SKSE::GetTrampoline();
			REL::Relocation<std::uintptr_t> PlayerCharacterVtbl{ RE::VTABLE_PlayerCharacter[0] };
			_PlayerCharacter_UpdateAnimation = PlayerCharacterVtbl.write_vfunc(0x7D, PlayerCharacter_UpdateAnimation);
			logger::info("hook:on_updateAnimation_player");
		}

	private:
		static void PlayerCharacter_UpdateAnimation(RE::PlayerCharacter* a_this, float a_deltaTime)
		{
			AnimSpeedManager::update(a_this->GetHandle(), a_deltaTime);
			_PlayerCharacter_UpdateAnimation(a_this, a_deltaTime);
		}
		static inline REL::Relocation<decltype(PlayerCharacter_UpdateAnimation)> _PlayerCharacter_UpdateAnimation;
	};

	class on_updateAnimation_internal
	{
	public:
		static void install()
		{
			auto& trampoline = SKSE::GetTrampoline();
			REL::Relocation<uintptr_t> hook{ RELOCATION_ID(40436, 41453) };                                                                // 6E1990, 70A840, RunOneActorAnimationUpdateJob
			_UpdateAnimationInternal = trampoline.write_call<5>(hook.address() + RELOCATION_OFFSET(0x74, 0x74), UpdateAnimationInternal);  // 6E1A04, 70A8B4;
			logger::info("hook:on_updateAnimation_internal");
		}

	private:
		static void UpdateAnimationInternal(RE::Actor* a_this, float a_deltaTime)
		{
			AnimSpeedManager::update(a_this->GetHandle(), a_deltaTime);
			_UpdateAnimationInternal(a_this, a_deltaTime);
		}
		static inline REL::Relocation<decltype(UpdateAnimationInternal)> _UpdateAnimationInternal;
	};
};

constexpr uint32_t hash(const char* data, size_t const size) noexcept
{
	uint32_t hash = 5381;

	for (const char* c = data; c < data + size; ++c) {
		hash = ((hash << 5) + hash) + (unsigned char)*c;
	}

	return hash;
}

constexpr uint32_t operator"" _h(const char* str, size_t size) noexcept
{
	return hash(str, size);
}
