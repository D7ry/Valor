#pragma once
#include <cmath>
#include  <random>
#include  <iterator>
#include "PCH.h"
#include "bin/offsets.h"
#define CONSOLELOG(msg) 	RE::ConsoleLog::GetSingleton()->Print(msg);
#define PI 3.1415926535897932384626
namespace inlineUtils
{
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
	/*Clamp the raw damage to be no more than the aggressor's max raw melee damage output.*/
	static void clampDmg(float& dmg, RE::Actor* aggressor) {
		auto a_weapon = inlineUtils::actor::getWieldingWeapon(aggressor);
		if (a_weapon) {
			//DEBUG("weapon to clamp damage: {}", a_weapon->GetName());
			dmg = min(dmg, a_weapon->GetAttackDamage());
		}
	}

	static bool isCasting(RE::Actor* a_actor) {
		for (int i = 0; i < 3; i++)
		{
			auto caster = a_actor->GetMagicCaster(RE::MagicSystem::CastingSource(i));
			if (!caster)
				continue;
			if (caster->state) {
				return true;
			}
		}
		return false;
	}

	/*Set the projectile's cause to a new actor; reset the projectile's collision mask.
	@param a_projectile: projectile to be reset.
	@param a_actor: new owner of the projectile.
	@param a_projectile_collidable: pointer to the projectile collidable to rset its collision mask.*/
	static void resetProjectileOwner(RE::Projectile* a_projectile, RE::Actor* a_actor, RE::hkpCollidable* a_projectile_collidable) {
		a_projectile->SetActorCause(a_actor->GetActorCause());
		a_projectile->shooter = a_actor->GetHandle();
		uint32_t a_collisionFilterInfo;
		a_actor->GetCollisionFilterInfo(a_collisionFilterInfo);
		a_projectile_collidable->broadPhaseHandle.collisionFilterInfo &= (0x0000FFFF);
		a_projectile_collidable->broadPhaseHandle.collisionFilterInfo |= (a_collisionFilterInfo << 16);
	}

	static inline bool ApproximatelyEqual(float A, float B)
	{
		return ((A - B) < FLT_EPSILON) && ((B - A) < FLT_EPSILON);
	}

	static bool PredictAimProjectile(RE::NiPoint3 a_projectilePos, RE::NiPoint3 a_targetPosition, RE::NiPoint3 a_targetVelocity, float a_gravity, RE::NiPoint3& a_projectileVelocity)
	{
		// http://ringofblades.com/Blades/Code/PredictiveAim.cs

		float projectileSpeedSquared = a_projectileVelocity.SqrLength();
		float projectileSpeed = std::sqrtf(projectileSpeedSquared);

		if (projectileSpeed <= 0.f || a_projectilePos == a_targetPosition) {
			return false;
		}

		float targetSpeedSquared = a_targetVelocity.SqrLength();
		float targetSpeed = std::sqrtf(targetSpeedSquared);
		RE::NiPoint3 targetToProjectile = a_projectilePos - a_targetPosition;
		float distanceSquared = targetToProjectile.SqrLength();
		float distance = std::sqrtf(distanceSquared);
		RE::NiPoint3 direction = targetToProjectile;
		direction.Unitize();
		RE::NiPoint3 targetVelocityDirection = a_targetVelocity;
		targetVelocityDirection.Unitize();

		float cosTheta = (targetSpeedSquared > 0)
			? direction.Dot(targetVelocityDirection)
			: 1.0f;

		bool bValidSolutionFound = true;
		float t;

		if (ApproximatelyEqual(projectileSpeedSquared, targetSpeedSquared)) {
			// We want to avoid div/0 that can result from target and projectile traveling at the same speed
			//We know that cos(theta) of zero or less means there is no solution, since that would mean B goes backwards or leads to div/0 (infinity)
			if (cosTheta > 0) {
				t = 0.5f * distance / (targetSpeed * cosTheta);
			}
			else {
				bValidSolutionFound = false;
				t = 1;
			}
		}
		else {
			float a = projectileSpeedSquared - targetSpeedSquared;
			float b = 2.0f * distance * targetSpeed * cosTheta;
			float c = -distanceSquared;
			float discriminant = b * b - 4.0f * a * c;

			if (discriminant < 0) {
				// NaN
				bValidSolutionFound = false;
				t = 1;
			}
			else {
				// a will never be zero
				float uglyNumber = sqrtf(discriminant);
				float t0 = 0.5f * (-b + uglyNumber) / a;
				float t1 = 0.5f * (-b - uglyNumber) / a;

				// Assign the lowest positive time to t to aim at the earliest hit
				t = min(t0, t1);
				if (t < FLT_EPSILON) {
					t = max(t0, t1);
				}

				if (t < FLT_EPSILON) {
					// Time can't flow backwards when it comes to aiming.
					// No real solution was found, take a wild shot at the target's future location
					bValidSolutionFound = false;
					t = 1;
				}
			}
		}

		a_projectileVelocity = a_targetVelocity + (-targetToProjectile / t);

		if (!bValidSolutionFound)
		{
			a_projectileVelocity.Unitize();
			a_projectileVelocity *= projectileSpeed;
		}

		if (!ApproximatelyEqual(a_gravity, 0.f))
		{
			float netFallDistance = (a_projectileVelocity * t).z;
			float gravityCompensationSpeed = (netFallDistance + 0.5f * a_gravity * t * t) / t;
			a_projectileVelocity.z = gravityCompensationSpeed;
		}

		return bValidSolutionFound;
	}



	static void ReflectProjectile(RE::Projectile* a_projectile)
	{
		a_projectile->linearVelocity *= -1.f;

		// rotate model
		auto projectileNode = a_projectile->Get3D2();
		if (projectileNode)
		{
			RE::NiPoint3 direction = a_projectile->linearVelocity;
			direction.Unitize();

			a_projectile->data.angle.x = asin(direction.z);
			a_projectile->data.angle.z = atan2(direction.x, direction.y);

			if (a_projectile->data.angle.z < 0.0) {
				a_projectile->data.angle.z += PI;
			}

			if (direction.x < 0.0) {
				a_projectile->data.angle.z += PI;
			}

			inlineUtils::SetRotationMatrix(projectileNode->local.rotate, -direction.x, direction.y, direction.z);
		}
	}

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

	/// <summary>
	/// Change the projectile's trajectory, aiming it at the target.
	/// </summary>
	/// <param name="a_projectile">Projectile whose trajectory will be changed.</param>
	/// <param name="a_target">New target to be aimed at.</param>
	static void RetargetProjectile(RE::Projectile* a_projectile, RE::TESObjectREFR* a_target)
	{
		a_projectile->desiredTarget = a_target;

		auto projectileNode = a_projectile->Get3D2();
		auto targetHandle = a_target->GetHandle();

		RE::NiPoint3 targetPos = a_target->GetPosition();

		if (a_target->GetFormType() == RE::FormType::ActorCharacter) {
			getBodyPos(a_target->As<RE::Actor>(), targetPos);
		}

		RE::NiPoint3 targetVelocity;
		targetHandle.get()->GetLinearVelocity(targetVelocity);

		float projectileGravity = 0.f;
		if (auto ammo = a_projectile->ammoSource) {
			if (auto bgsProjectile = ammo->data.projectile) {
				projectileGravity = bgsProjectile->data.gravity;
				if (auto bhkWorld = a_projectile->parentCell->GetbhkWorld()) {
					if (auto hkpWorld = bhkWorld->GetWorld1()) {
						auto vec4 = hkpWorld->gravity;
						float quad[4];
						_mm_store_ps(quad, vec4.quad);
						float gravity = -quad[2] * *RE::Offset::g_worldScaleInverse;
						projectileGravity *= gravity;
					}
				}
			}
		}

		PredictAimProjectile(a_projectile->data.location, targetPos, targetVelocity, projectileGravity, a_projectile->linearVelocity);

		// rotate
		RE::NiPoint3 direction = a_projectile->linearVelocity;
		direction.Unitize();

		a_projectile->data.angle.x = asin(direction.z);
		a_projectile->data.angle.z = atan2(direction.x, direction.y);

		if (a_projectile->data.angle.z < 0.0) {
			a_projectile->data.angle.z += PI;
		}

		if (direction.x < 0.0) {
a_projectile->data.angle.z += PI;
		}

		inlineUtils::SetRotationMatrix(projectileNode->local.rotate, -direction.x, direction.y, direction.z);
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
