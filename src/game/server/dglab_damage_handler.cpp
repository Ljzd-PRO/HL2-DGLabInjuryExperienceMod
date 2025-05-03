#include "cbase.h"
#include "dglab_damage_handler.h"
#include "dglab_ws_client.h"
#include "npc_metropolice.h"
#include <future>
#include "npc_combines.h"

#define PULSE_DATA_DIED_SIZE 50

// The waveform length is 200ms, and the frequency is set to half of the waveform length (100ms) to ensure output
// If you need to increase the waveform length, make sure the corresponding frequency allows for at least one output within the waveform length
std::vector<dglab::Pulse> dglab_damage_handler::pulse_data = {
    {{100, 100, 100, 100}, {100, 100, 100, 100}},
    {{100, 100, 100, 100}, {100, 100, 100, 100}},
};

std::vector<dglab::Pulse> dglab_damage_handler::pulse_data_died;

bool dglab_damage_handler::m_bEnemyExperience = true;
bool dglab_damage_handler::m_bSelfExperience = false;
float dglab_damage_handler::m_fSelfStrengthPercentage = 0.5f;

dglab_damage_handler::dglab_damage_handler()
{
    // Initialize pulse_data_died with PULSE_DATA_DIED_SIZE identical pulses
    pulse_data_died.resize(PULSE_DATA_DIED_SIZE);
    for (auto& pulse : pulse_data_died)
    {
        pulse = {{180, 180, 180, 180}, {100, 100, 100, 100}};
    }
}

void dglab_damage_handler::SetEnemyExperience(bool enabled)
{
    m_bEnemyExperience = enabled;
}

void dglab_damage_handler::SetSelfExperience(bool enabled)
{
    m_bSelfExperience = enabled;
}

void dglab_damage_handler::SetSelfStrengthPercentage(float percentage)
{
    m_fSelfStrengthPercentage = percentage;
}

void dglab_damage_handler::DebugDamageInfo(const CTakeDamageInfo& info)
{
    char damageType[22];
    info.DebugGetDamageTypeString(info.GetDamageType(), damageType, sizeof(damageType));
    
    DevMsg("DGLabIEMod> Damage Info: GetDamage=%.1f, GetDamageType=%s, GetAttacker=%s, GetWeapon=%s, GetInflictor=%s\n",
        info.GetDamage(),
        damageType,
        info.GetAttacker() ? info.GetAttacker()->GetClassname() : "Unknown",
        info.GetWeapon() ? info.GetWeapon()->GetClassname() : "Unknown",
        info.GetInflictor() ? info.GetInflictor()->GetClassname() : "Unknown"
    );
}

void dglab_damage_handler::HandlePlayerDeath(const CTakeDamageInfo& info, const CBasePlayer* pPlayer)
{
    if (!pPlayer || !m_bSelfExperience)
    {
        return;
    }

    // Set maximum strength and use death pulse pattern
    dglab::client.set_strength_percentage(dglab::Channel::A, 1.0f);
    dglab::client.add_pulses(dglab::Channel::A, pulse_data_died);
    dglab::client.set_strength_percentage(dglab::Channel::B, 1.0f);
    dglab::client.add_pulses(dglab::Channel::B, pulse_data_died);

    DevMsg("DGLabIEMod> Player death pulses sent with max strength\n");
}

void dglab_damage_handler::HandleDamage(const CTakeDamageInfo& info, const CBaseEntity& npc)
{
    const CNPC_MetroPolice* pMetroPolice = dynamic_cast<const CNPC_MetroPolice*>(&npc);
    const CNPC_CombineS* pCombineS = dynamic_cast<const CNPC_CombineS*>(&npc);
    const CBasePlayer* pPlayer = dynamic_cast<const CBasePlayer*>(&npc);

    if (!pMetroPolice && !pCombineS && !pPlayer) return;
    if (((pMetroPolice || pCombineS) && !m_bEnemyExperience) || (pPlayer && !m_bSelfExperience)) return;
    
    float strength_percentage;
    const std::vector<dglab::Pulse>* pPulseData;

    if (pPlayer)
    {
        if (pPlayer->IsDead())
        {
            HandlePlayerDeath(info, pPlayer);
            return;
        }
        strength_percentage = m_fSelfStrengthPercentage;
        pPulseData = &pulse_data;
    }
    else
    {
        const float max_health = static_cast<float>(npc.GetMaxHealth());
        strength_percentage = min(info.GetDamage(), max_health) / max_health;
        pPulseData = &pulse_data;
    }

    // Based on testing, it's better to complete operations on one channel before moving to another
    // Otherwise, it might cause the other waveform to not be output
    dglab::client.set_strength_percentage(dglab::Channel::A, strength_percentage);
    dglab::client.add_pulses(dglab::Channel::A, *pPulseData);
    dglab::client.set_strength_percentage(dglab::Channel::B, strength_percentage);
    dglab::client.add_pulses(dglab::Channel::B, *pPulseData);

    DevMsg("DGLabIEMod> Strength percentage: %.2f\n", strength_percentage);
}

void dglab_damage_handler::HandlePlayerRespawn(const CBasePlayer* pPlayer)
{
    if (!pPlayer || !m_bSelfExperience)
    {
        return;
    }

    // Clear pulses for both channels
    dglab::client.clear_pulses(dglab::Channel::A);
    dglab::client.clear_pulses(dglab::Channel::B);
    DevMsg("DGLabIEMod> Cleared pulses on player respawn\n");
}
