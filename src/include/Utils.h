#pragma once
#include <cmath>
#include  <random>
#include  <iterator>
#include "PCH.h"
#include "bin/offsets.h"
#include "include/lib/TrueHUDAPI.h"

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

namespace inlineUtils
{
	/*Get the absolute position of a point in world, given a relative position to an actor.*/
	inline static RE::NiPoint3 get_abs_pos(RE::Actor* a_actor, RE::NiPoint3 a_relative_pos)  // Hacked this together in a very short time; there might be better solutions.
	{
		float len = sqrt(a_relative_pos.x * a_relative_pos.x + a_relative_pos.y * a_relative_pos.y);
		float actorAngleZ = a_actor->GetAngleZ();
		actorAngleZ -= 2 * actorAngleZ; /*Invert angle across axis*/

		float vecAngle = atan2(a_relative_pos.y, a_relative_pos.x);
		float vecAngleZ = actorAngleZ + vecAngle;

		float x_displacement = len * cos(vecAngleZ);
		float y_displacement = len * sin(vecAngleZ);
		float z_displacement = a_relative_pos.z;

		RE::NiPoint3 abs_pos = a_actor->GetPosition();
		abs_pos.x += x_displacement;
		abs_pos.y += y_displacement;
		abs_pos.z += z_displacement;

		return abs_pos;
	}
	
	inline static bool isEquippedShield(RE::Actor* a_actor) {
		return RE::Offset::getEquippedShield(a_actor) != nullptr;
	}

	/*Send the target flying based on causer's location.
	@param magnitude: strength of a push.*/
	inline static void PushActorAway(RE::Actor* causer, RE::Actor* target, float magnitude)
	{
		auto targetPoint = causer->GetNodeByName(causer->race->bodyPartData->parts[0]->targetName.c_str());
		RE::NiPoint3 vec = targetPoint->world.translate;
		//RE::NiPoint3 vec = causer->GetPosition();
		RE::Offset::pushActorAway(causer->currentProcess, target, vec, magnitude);
	}

	inline void SetRotationMatrix(RE::NiMatrix3& a_matrix, float sacb, float cacb, float sb)
	{
		float cb = std::sqrtf(1 - sb * sb);
		float ca = cacb / cb;
		float sa = sacb / cb;
		a_matrix.entry[0][0] = ca;
		a_matrix.entry[0][1] = -sacb;
		a_matrix.entry[0][2] = sa * sb;
		a_matrix.entry[1][0] = sa;
		a_matrix.entry[1][1] = cacb;
		a_matrix.entry[1][2] = -ca * sb;
		a_matrix.entry[2][0] = 0.0;
		a_matrix.entry[2][1] = sb;
		a_matrix.entry[2][2] = cb;
	}

	template<typename Iter, typename RandomGenerator>
	Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
		std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
		std::advance(start, dis(g));
		return start;
	}

	template<typename Iter>
	Iter select_randomly(Iter start, Iter end) {
		static std::random_device rd;
		static std::mt19937 gen(rd());
		return select_randomly(start, end, gen);
	}


	inline bool isPowerAttacking(RE::Actor* a_actor) {
		auto currentProcess = a_actor->currentProcess;
		if (currentProcess) {
			auto highProcess = currentProcess->high;
			if (highProcess) {
				auto attackData = highProcess->attackData;
				if (attackData) {
					auto flags = attackData->data.flags;
					return flags.any(RE::AttackData::AttackFlag::kPowerAttack) && !flags.any(RE::AttackData::AttackFlag::kBashAttack);
				}
			}
		}
		return false;
	}
	inline void damageav(RE::Actor* a, RE::ActorValue av, float val)
	{
		if (val == 0) {
			return;
		}
		if (a) {
			a->As<RE::ActorValueOwner>()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, av, -val);
		}
	}

	/*Try to damage this actor's actorvalue. If the actor does not have enough value, do not damage and return false;*/
	inline bool tryDamageAv(RE::Actor* a_actor, RE::ActorValue a_actorValue, float a_damage) {
		auto currentAv = a_actor->GetActorValue(a_actorValue);
		if (currentAv - a_damage <= 0) {
			return false;
		}
		damageav(a_actor, a_actorValue, a_damage);
		return true;
	}

	inline void restoreav(RE::Actor* a_actor, RE::ActorValue a_actorValue, float a_value)
	{
		if (a_value == 0) {
			return;
		}
		if (a_actor) {
			a_actor->As<RE::ActorValueOwner>()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, a_actorValue, a_value);
		}
	}

	/*Slow down game time for a set period.
	@param a_duration: duration of the slow time.
	@param a_percentage: relative time speed to normal time(1).*/
	inline void slowTime(float a_duration, float a_percentage) {
		int duration_milisec = static_cast<int>(a_duration * 1000);
		RE::Offset::SGTM(a_percentage);
		/*Reset time here*/
		auto resetSlowTime = [](int a_duration) {
			std::this_thread::sleep_for(std::chrono::milliseconds(a_duration));
			RE::Offset::SGTM(1);
		};
		std::jthread resetThread(resetSlowTime, duration_milisec);
		resetThread.detach();
	}


	inline void safeRemoveSpell(RE::SpellItem* a_spell, RE::Actor* a_actor) {
		if (a_actor && a_spell) {
			a_actor->RemoveSpell(a_spell);
		}
	}

	inline void safeApplyPerk(RE::BGSPerk* a_perk, RE::Actor* a_actor) {
		if (a_actor && a_perk && !a_actor->HasPerk(a_perk)) {
			a_actor->AddPerk(a_perk);
		}
	}

	inline void safeRemovePerk(RE::BGSPerk* a_perk, RE::Actor* a_actor) {
		if (a_actor && a_perk && a_actor->HasPerk(a_perk)) {
			a_actor->RemovePerk(a_perk);
		}
	}

	/*Complete refills this actor's actor value.
	@param a_actor actor whose actorValue will be refilled.
	@param actorValue type of actor value to refill.*/
	inline void refillActorValue(RE::Actor* a_actor, RE::ActorValue a_actorValue) {
		float av = a_actor->GetActorValue(a_actorValue);
		float pav = a_actor->GetPermanentActorValue(a_actorValue);
		if (av >= pav) {
			return;
		}
		float avToRestore = pav - av;
		restoreav(a_actor, a_actorValue, avToRestore);
	}

	namespace actor
	{
		inline RE::TESObjectWEAP* getWieldingWeapon(RE::Actor* a_actor)
		{
			auto weapon = a_actor->GetAttackingWeapon();
			if (weapon) {
				return weapon->object->As<RE::TESObjectWEAP>();
			}
			auto rhs = a_actor->GetEquippedObject(false);
			if (rhs && rhs->IsWeapon()) {
				return rhs->As<RE::TESObjectWEAP>();
			}
			auto lhs = a_actor->GetEquippedObject(true);
			if (lhs && lhs->IsWeapon()) {
				return lhs->As<RE::TESObjectWEAP>();
			}
			return nullptr;
		}

		inline bool isDualWielding(RE::Actor* a_actor) {
			auto lhs = a_actor->GetEquippedObject(true);
			auto rhs = a_actor->GetEquippedObject(false);
			if (lhs && rhs && lhs->IsWeapon() && rhs->IsWeapon()) {
				auto weaponType = rhs->As<RE::TESObjectWEAP>()->GetWeaponType();
				return weaponType != RE::WEAPON_TYPE::kTwoHandAxe && weaponType != RE::WEAPON_TYPE::kTwoHandSword;//can't be two hand sword.
			}
			return false;
		}

		inline bool isEquippedShield(RE::Actor* a_actor) {
			return RE::Offset::getEquippedShield(a_actor);
		}
	}
	
};


class ValhallaUtils
{
public:

	/*Whether the actor's back is facing the other actor's front.
	@param actor1: actor whose facing will be returned
	@param actor2: actor whose relative location to actor1 will be calculated.*/
	static bool isBackFacing(RE::Actor* actor1, RE::Actor* actor2) {
		auto angle = actor1->GetHeadingAngle(actor2->GetPosition(), false);
		if (90 < angle || angle < -90) {
			return true;
		}
		else {
			return false;
		}
	}




	/*Play sound with formid at a certain actor's position.
	@param a: actor on which to play sonud.
	@param formid: formid of the sound descriptor.*/
	static void playSound(RE::Actor* a, RE::BGSSoundDescriptorForm* a_descriptor, float a_volumeOverride = 1)
	{

		RE::BSSoundHandle handle;
		handle.soundID = static_cast<uint32_t>(-1);
		handle.assumeSuccess = false;
		*(uint32_t*)&handle.state = 0;

		RE::Offset::soundHelper_a(RE::BSAudioManager::GetSingleton(), &handle, a_descriptor->GetFormID(), 16);
		if (RE::Offset::set_sound_position(&handle, a->data.location.x, a->data.location.y, a->data.location.z)) {
			handle.SetVolume(a_volumeOverride);
			RE::Offset::soundHelper_b(&handle, a->Get3D());
			RE::Offset::soundHelper_c(&handle);
		}
	}

	static void playSound(RE::Actor* a, std::vector<RE::BGSSoundDescriptorForm*> sounds)
	{
		playSound(a, *inlineUtils::select_randomly(sounds.begin(), sounds.end()));
	}

	/*
	@return actor a and actor b's absolute distance, if the radius is bigger than distance.
	@return -1, if the distance exceeds radius.*/
	static float getInRange(RE::Actor* a, RE::Actor* b, float maxRange) {
		float dist = a->GetPosition().GetDistance(b->GetPosition());
		if (dist <= maxRange) {
			return dist;
		}
		else {
			return -1;
		}
	}

	static void queueMessageBox(RE::BSFixedString a_message) {
		auto factoryManager = RE::MessageDataFactoryManager::GetSingleton();
		auto uiStrHolder = RE::InterfaceStrings::GetSingleton();
		auto factory = factoryManager->GetCreator<RE::MessageBoxData>(uiStrHolder->messageBoxData);
		auto messageBox = factory->Create();
		messageBox->unk4C = 4;
		messageBox->unk38 = 10;
		messageBox->bodyText = a_message;
		auto gameSettings = RE::GameSettingCollection::GetSingleton();
		auto sOk = gameSettings->GetSetting("sOk");
		messageBox->buttonText.push_back(sOk->GetString());
		messageBox->QueueMessage();
	}
#pragma endregion
	


	/*Get the body position of this actor.*/
	static void getBodyPos(RE::Actor* a_actor, RE::NiPoint3& pos)
	{
		if (!a_actor->race) {
			return;
		}
		RE::BGSBodyPart* bodyPart = a_actor->race->bodyPartData->parts[0];
		if (!bodyPart) {
			return;
		}
		auto targetPoint = a_actor->GetNodeByName(bodyPart->targetName.c_str());
		if (!targetPoint) {
			return;
		}

		pos = targetPoint->world.translate;
	}

};

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
		formLoader(RE::BSFixedString pluginName)
		{
			_pluginName = pluginName;
			_dataHandler = RE::TESDataHandler::GetSingleton();
			if (!_dataHandler) {
				logger::critical("Error: TESDataHandler not found.");
			}
			if (!_dataHandler->LookupModByName(pluginName)) {
				logger::critical("Error: {} not found.", pluginName);
			}
			logger::info("Loading from plugin {}...", pluginName);
		}

		void log()
		{
			logger::info("Loaded {} forms from {}", _loadedForms, _pluginName);
		}

		/*Load a form from the plugin.*/
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
		settingsLoader(const char* settingsFile)
		{
			_ini = std::make_shared<CSimpleIniA>();
			_ini->LoadFile(settingsFile);
			if (_ini->IsEmpty()) {
				logger::info("Warning: {} is empty.", settingsFile);
			}
			_settingsFile = settingsFile;
		};
		/*Set the active section. Load() will load keys from this section.*/
		void setActiveSection(const char* section)
		{
			_section = section;
		}
		/*Load a boolean value if present.*/
		void load(bool& settingRef, const char* key)
		{
			if (_ini->GetValue(_section, key)) {
				bool val = _ini->GetBoolValue(_section, key);
				settingRef = val;
				_loadedSettings++;
			}
		}
		/*Load a float value if present.*/
		void load(float& settingRef, const char* key)
		{
			if (_ini->GetValue(_section, key)) {
				float val = static_cast<float>(_ini->GetDoubleValue(_section, key));
				settingRef = val;
				_loadedSettings++;
			}
		}
		/*Load an unsigned int value if present.*/
		void load(uint32_t& settingRef, const char* key)
		{
			if (_ini->GetValue(_section, key)) {
				uint32_t val = static_cast<uint32_t>(_ini->GetDoubleValue(_section, key));
				settingRef = val;
				_loadedSettings++;
			}
		}

		/*Load an integer value if present.*/
		void load(int& settingRef, const char* key) {
			if (_ini->GetValue(_section, key)) {
				int val = static_cast<int>(_ini->GetDoubleValue(_section, key));
				settingRef = val;
				_loadedSettings++;
			}
		}

		void log() {
			logger::info("Loaded {} settings from {}", _loadedSettings, _settingsFile);
		}
	};

	class rayCast
	{

#define YELLOW 0xFFFF00      // yellow
#define GREEN 0x40FF00 //green
	public:
		
		/*Do a simple raycast at a singular point to check if anything exists there.*/
		static bool object_exits(RE::NiPoint3 a_pos) 
		{
			RE::NiPoint3 rayStart = a_pos;
			RE::NiPoint3 rayEnd = a_pos;
			rayStart.z += 30.f;
			rayEnd.z -= 30.f;
			auto havokWorldScale = RE::bhkWorld::GetWorldScale();
			RE::bhkPickData pick_data;
			pick_data.rayInput.from = rayStart * havokWorldScale;
			pick_data.rayInput.to = rayEnd * havokWorldScale;
			debug::getsingleton()->debugAPI->DrawArrow(rayStart, rayEnd, 10, 1);
			auto pc = RE::PlayerCharacter::GetSingleton();
			if (!pc) {
				return false;
			}
			if (!pc->GetParentCell() || !pc->GetParentCell()->GetbhkWorld()) {
				return false;
			}
			pc->GetParentCell()->GetbhkWorld()->PickObject(pick_data);
			if (pick_data.rayOutput.HasHit()) {
				logger::info("object exists!");
				RE::NiPoint3 hitpos = rayStart + (rayEnd - rayStart) * pick_data.rayOutput.hitFraction;
				debug::getsingleton()->debugAPI->DrawPoint(hitpos, 10, 3, GREEN);
				return true;
			}
			return false;
		}
		/*Cast a ray from the center of the actor to a_rayEnd, return the first object encountered, or nullptr if nothing is hit.*/
		static RE::TESObjectREFR* cast_ray(RE::Actor* a_actor, RE::NiPoint3 a_rayEnd) 
		{
			auto havokWorldScale = RE::bhkWorld::GetWorldScale();
			RE::bhkPickData pick_data;
			RE::NiPoint3 rayStart = a_actor->GetPosition();
			float castHeight = a_actor->GetHeight() * 0.5f;
			rayStart.z += castHeight;  //cast from center of actor
						/*Setup ray*/
			pick_data.rayInput.from = rayStart * havokWorldScale;
			pick_data.rayInput.to = a_rayEnd * havokWorldScale;
			debug::getsingleton()->debugAPI->DrawArrow(rayStart, a_rayEnd, 10, 1);
			
			/*Setup collision filter, ignoring the actor.*/
			uint32_t collisionFilterInfo = 0;
			a_actor->GetCollisionFilterInfo(collisionFilterInfo);
			uint16_t collisionGroup = collisionFilterInfo >> 16;
			pick_data.rayInput.filterInfo = (static_cast<uint32_t>(collisionGroup) << 16) | static_cast<uint32_t>(RE::COL_LAYER::kCharController);
			
			/*Do*/
			a_actor->GetParentCell()->GetbhkWorld()->PickObject(pick_data);
			if (pick_data.rayOutput.HasHit()) {
				logger::info("rayhit!");
				RE::NiPoint3 hitpos = rayStart + (a_rayEnd - rayStart) * pick_data.rayOutput.hitFraction;
				debug::getsingleton()->debugAPI->DrawPoint(hitpos, 10, 3);

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
