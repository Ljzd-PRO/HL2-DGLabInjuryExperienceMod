#pragma once

#include "cbase.h"

#include "ai_baseactor.h"
#include "dglab_ws_client.h"

class dglab_damage_handler
{
public:
    static void HandleDamage(const CTakeDamageInfo& info, const CAI_BaseActor& npc);
    static void DebugDamageInfo(const CTakeDamageInfo& info);
    static std::vector<dglab::Pulse> pulse_data;
};
