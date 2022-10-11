#pragma once
#include "PCH.h"
class perilous
{
public:
	static perilous* GetSingleton()
	{
		static perilous singleton;
		return &singleton;
	}

	void init();

	void attempt_start_perilous_attack(RE::Actor* a_actor);

	void attempt_end_perilous_attack(RE::Actor* a_actor);

	float getCharge(RE::ActorHandle a_actorHandle, float a_deltaTime, bool a_bUpdate);

private:
	void addCharge(RE::ActorHandle a_refHandle, float a_chargeLength);

	bool should_perilous_attack(RE::Actor* a_actor);

	void perform_perilous_attack(RE::Actor* a_actor);

	RE::SpellItem* perilousSpell;

	RE::TESEffectShader* temp;

	RE::BGSArtObject* perilousHitEffectArt;


	RE::BGSSoundDescriptorForm* perilousSound;

	std::unordered_map<RE::ActorHandle, float> activeCharges;

	std::mutex activeChargesLock;
	
};
