#pragma once

#include "cbase.h"

#include "ai_baseactor.h"
#include "dglab_ws_client.h"

class dglab_damage_handler
{
public:
    dglab_damage_handler();
    static void HandleDamage(const CTakeDamageInfo& info, const CBaseEntity& npc);
    static void DebugDamageInfo(const CTakeDamageInfo& info);
    static void SetEnemyExperience(bool enabled);
    static void SetSelfExperience(bool enabled);
    static std::vector<dglab::Pulse> pulse_data;

private:
    static bool m_bEnemyExperience;
    static bool m_bSelfExperience;
};
