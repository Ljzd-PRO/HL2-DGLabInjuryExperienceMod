#include "cbase.h"
#include "dglab_damage_handler.h"
#include "dglab_ws_client.h"
#include "npc_metropolice.h"
#include <future>

std::vector<dglab::Pulse> dglab_damage_handler::pulse_data = {
    {{100, 100, 100, 100}, {100, 100, 100, 100}},
    {{100, 100, 100, 100}, {100, 100, 100, 100}},
};

void dglab_damage_handler::DebugDamageInfo(const CTakeDamageInfo& info)
{
    char damageType[22];
    info.DebugGetDamageTypeString(info.GetDamageType(), damageType, sizeof(damageType));
    
    DevMsg("Damage Info: GetDamage=%.1f, GetDamageType=%s, GetAttacker=%s, GetWeapon=%s, GetInflictor=%s\n",
        info.GetDamage(),
        damageType,
        info.GetAttacker() ? info.GetAttacker()->GetClassname() : "Unknown",
        info.GetWeapon() ? info.GetWeapon()->GetClassname() : "Unknown",
        info.GetInflictor() ? info.GetInflictor()->GetClassname() : "Unknown"
    );
}

void dglab_damage_handler::HandleDamage(const CTakeDamageInfo& info, const CAI_BaseActor& npc)
{
    const float max_health = static_cast<float>(npc.GetMaxHealth());
    const float strength_percentage = min(info.GetDamage(), max_health) / max_health;

    dglab::client.set_strength_percentage(dglab::Channel::A, strength_percentage);
    dglab::client.add_pulses(dglab::Channel::A, pulse_data);
    dglab::client.set_strength_percentage(dglab::Channel::B, strength_percentage);
    dglab::client.add_pulses(dglab::Channel::B, pulse_data);

    DevMsg("Strength percentage: %.2f\n", strength_percentage);
}
