#pragma once
#include "cbase.h"
#include "dglab_ws_client.h"

class dglab_damage_handler
{
public:
    dglab_damage_handler();
    static void HandleDamage(const CTakeDamageInfo& info, const CBaseEntity& npc);
    static void DebugDamageInfo(const CTakeDamageInfo& info);
    static void SetEnemyExperience(bool enabled);
    static void SetSelfExperience(bool enabled);
    static void SetSelfStrengthPercentage(float percentage);
    static void HandlePlayerRespawn(const CBasePlayer* pPlayer);
    static void HandlePlayerDeath(const CTakeDamageInfo& info, const CBasePlayer* pPlayer);
    static std::vector<dglab::Pulse> pulse_data;
    static std::vector<dglab::Pulse> pulse_data_died;

private:
    static bool m_bEnemyExperience;
    static bool m_bSelfExperience;
    static float m_fSelfStrengthPercentage;
};
