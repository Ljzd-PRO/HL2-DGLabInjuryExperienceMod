#include "cbase.h"
#include "dglab_ws_client.h"
#include "dglab_damage_handler.h"

// ConVars to track connection status
ConVar dglab_ws_connected("dglab_ws_connected", "0", FCVAR_REPLICATED, "Indicates if DGLab WebSocket is connected");
ConVar dglab_ws_enemy_experience("dglab_ws_enemy_experience", "1", FCVAR_REPLICATED, "Enable human enemy injury experience");
ConVar dglab_ws_self_experience("dglab_ws_self_experience", "0", FCVAR_REPLICATED, "Enable self injury experience");
ConVar dglab_ws_self_strength_percentage("dglab_ws_self_strength_percentage", "0.3", FCVAR_REPLICATED, "Fixed strength percentage for self injury experience");

// Global variables for default values
char dglab_ws_default_max_strength[32];
char dglab_ws_default_min_strength[32];

// Initialize default values
void InitializeDefaultValues()
{
    Q_snprintf(dglab_ws_default_max_strength, sizeof(dglab_ws_default_max_strength), "%d", dglab::DGLAB_WS_DEFAULT_MAX_STRENGTH);
    Q_snprintf(dglab_ws_default_min_strength, sizeof(dglab_ws_default_min_strength), "%d", dglab::DGLAB_WS_DEFAULT_MIN_STRENGTH);
}

ConVar dglab_ws_max_strength_a("dglab_ws_max_strength_a", dglab_ws_default_max_strength, FCVAR_REPLICATED, "Maximum strength for DGLab A channel");
ConVar dglab_ws_min_strength_a("dglab_ws_min_strength_a", dglab_ws_default_min_strength, FCVAR_REPLICATED, "Minimum strength for DGLab A channel");
ConVar dglab_ws_max_strength_b("dglab_ws_max_strength_b", dglab_ws_default_max_strength, FCVAR_REPLICATED, "Maximum strength for DGLab B channel");
ConVar dglab_ws_min_strength_b("dglab_ws_min_strength_b", dglab_ws_default_min_strength, FCVAR_REPLICATED, "Minimum strength for DGLab B channel");

// Initialize default values at startup
class CDGLabWSDefaultInitializer
{
public:
    CDGLabWSDefaultInitializer()
    {
        InitializeDefaultValues();
    }
};
static CDGLabWSDefaultInitializer g_DGLabWSDefaultInitializer;

//-----------------------------------------------------------------------------
// Purpose: Handle the dglab_set_enemy_experience command from clients
//-----------------------------------------------------------------------------
void CC_DGLabSetEnemyExperience(const CCommand &args)
{
    if (args.ArgC() < 2)
    {
        Warning("Usage: dglab_set_enemy_experience <0|1>\n");
        return;
    }

    int value = atoi(args[1]);
    if (value != 0 && value != 1)
    {
        Warning("Value must be 0 or 1\n");
        return;
    }

    dglab_ws_enemy_experience.SetValue(value);
    dglab_damage_handler::SetEnemyExperience(value != 0);
    Msg("DGLab enemy experience %s\n", value ? "enabled" : "disabled");
}

//-----------------------------------------------------------------------------
// Purpose: Handle the dglab_set_self_experience command from clients
//-----------------------------------------------------------------------------
void CC_DGLabSetSelfExperience(const CCommand &args)
{
    if (args.ArgC() < 2)
    {
        Warning("Usage: dglab_set_self_experience <0|1>\n");
        return;
    }

    int value = atoi(args[1]);
    if (value != 0 && value != 1)
    {
        Warning("Value must be 0 or 1\n");
        return;
    }

    dglab_ws_self_experience.SetValue(value);
    dglab_damage_handler::SetSelfExperience(value != 0);
    Msg("DGLab self experience %s\n", value ? "enabled" : "disabled");
}

//-----------------------------------------------------------------------------
// Purpose: Handle the dglab_connect command from clients
//-----------------------------------------------------------------------------
void CC_DGLabConnect(const CCommand &args)
{
    if (args.ArgC() < 2)
    {
        Warning("Usage: dglab_connect <ws_url>\n");
        return;
    }

    // Decode the URL
    char decoded_url[1024];
    Q_StrSubst(args[1], "_COLON_", ":", decoded_url, sizeof(decoded_url));
    
    bool result = dglab::client.connect(decoded_url);
    
    if (result)
    {
        Msg("DGLab WebSocket connection successful!\n");
        dglab_ws_connected.SetValue(1);
    }
    else
    {
        Warning("DGLab WebSocket connection failed: %s\n", decoded_url);
        dglab_ws_connected.SetValue(0);
    }
}

//-----------------------------------------------------------------------------
// Purpose: Handle the dglab_disconnect command from clients
//-----------------------------------------------------------------------------
void CC_DGLabDisconnect(const CCommand &args)
{
    dglab::client.disconnect();
    dglab_ws_connected.SetValue(0);
    Msg("DGLab WebSocket disconnected\n");
}

//-----------------------------------------------------------------------------
// Purpose: Handle the dglab_set_max_strength command from clients
//-----------------------------------------------------------------------------
void CC_DGLabSetMaxStrengthA(const CCommand &args)
{
    if (args.ArgC() < 2)
    {
        Warning("Usage: dglab_set_max_strength_a <strength>\n");
        return;
    }

    int strength = atoi(args[1]);
    if (strength < dglab::DGLAB_WS_MIN_STRENGTH || strength > dglab::DGLAB_WS_MAX_STRENGTH)
    {
        Warning("Strength must be between %d and %d\n", dglab::DGLAB_WS_MIN_STRENGTH, dglab::DGLAB_WS_MAX_STRENGTH);
        return;
    }

    dglab::client.set_max_strength(dglab::Channel::A, strength);
    dglab_ws_max_strength_a.SetValue(strength);
    Msg("DGLab A channel max strength set to %d\n", strength);
}

void CC_DGLabSetMinStrengthA(const CCommand &args)
{
    if (args.ArgC() < 2)
    {
        Warning("Usage: dglab_set_min_strength_a <strength>\n");
        return;
    }

    int strength = atoi(args[1]);
    if (strength < dglab::DGLAB_WS_MIN_STRENGTH || strength > dglab::DGLAB_WS_MAX_STRENGTH)
    {
        Warning("Strength must be between %d and %d\n", dglab::DGLAB_WS_MIN_STRENGTH, dglab::DGLAB_WS_MAX_STRENGTH);
        return;
    }

    dglab::client.set_min_strength(dglab::Channel::A, strength);
    dglab_ws_min_strength_a.SetValue(strength);
    Msg("DGLab A channel min strength set to %d\n", strength);
}

void CC_DGLabSetMaxStrengthB(const CCommand &args)
{
    if (args.ArgC() < 2)
    {
        Warning("Usage: dglab_set_max_strength_b <strength>\n");
        return;
    }

    int strength = atoi(args[1]);
    if (strength < dglab::DGLAB_WS_MIN_STRENGTH || strength > dglab::DGLAB_WS_MAX_STRENGTH)
    {
        Warning("Strength must be between %d and %d\n", dglab::DGLAB_WS_MIN_STRENGTH, dglab::DGLAB_WS_MAX_STRENGTH);
        return;
    }

    dglab::client.set_max_strength(dglab::Channel::B, strength);
    dglab_ws_max_strength_b.SetValue(strength);
    Msg("DGLab B channel max strength set to %d\n", strength);
}

void CC_DGLabSetMinStrengthB(const CCommand &args)
{
    if (args.ArgC() < 2)
    {
        Warning("Usage: dglab_set_min_strength_b <strength>\n");
        return;
    }

    int strength = atoi(args[1]);
    if (strength < dglab::DGLAB_WS_MIN_STRENGTH || strength > dglab::DGLAB_WS_MAX_STRENGTH)
    {
        Warning("Strength must be between %d and %d\n", dglab::DGLAB_WS_MIN_STRENGTH, dglab::DGLAB_WS_MAX_STRENGTH);
        return;
    }

    dglab::client.set_min_strength(dglab::Channel::B, strength);
    dglab_ws_min_strength_b.SetValue(strength);
    Msg("DGLab B channel min strength set to %d\n", strength);
}

//-----------------------------------------------------------------------------
// Purpose: Handle the dglab_set_self_strength_percentage command from clients
//-----------------------------------------------------------------------------
void CC_DGLabSetSelfStrengthPercentage(const CCommand &args)
{
    if (args.ArgC() < 2)
    {
        Warning("Usage: dglab_set_self_strength_percentage <percentage>\n");
        return;
    }

    float value = atof(args[1]);
    if (value < 0.0f || value > 1.0f)
    {
        Warning("Percentage must be between 0.0 and 1.0\n");
        return;
    }

    dglab_ws_self_strength_percentage.SetValue(value);
    dglab_damage_handler::SetSelfStrengthPercentage(value);
    Msg("DGLab self strength percentage set to %.2f\n", value);
}

// Register console commands
static ConCommand dglab_connect("dglab_connect", CC_DGLabConnect, "Connect to DGLab WebSocket server");
static ConCommand dglab_disconnect("dglab_disconnect", CC_DGLabDisconnect, "Disconnect from DGLab WebSocket server");
static ConCommand dglab_set_max_strength_a("dglab_set_max_strength_a", CC_DGLabSetMaxStrengthA, "Set maximum strength for DGLab A channel");
static ConCommand dglab_set_min_strength_a("dglab_set_min_strength_a", CC_DGLabSetMinStrengthA, "Set minimum strength for DGLab A channel");
static ConCommand dglab_set_max_strength_b("dglab_set_max_strength_b", CC_DGLabSetMaxStrengthB, "Set maximum strength for DGLab B channel");
static ConCommand dglab_set_min_strength_b("dglab_set_min_strength_b", CC_DGLabSetMinStrengthB, "Set minimum strength for DGLab B channel");
static ConCommand dglab_set_enemy_experience("dglab_set_enemy_experience", CC_DGLabSetEnemyExperience, "Enable/disable enemy damage experience");
static ConCommand dglab_set_self_experience("dglab_set_self_experience", CC_DGLabSetSelfExperience, "Enable/disable self damage experience");
static ConCommand dglab_set_self_strength_percentage("dglab_set_self_strength_percentage", CC_DGLabSetSelfStrengthPercentage, "Set fixed strength percentage for self injury experience"); 