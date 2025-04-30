#include "cbase.h"
#include "dglab_ws_client.h"
#include <string>

// ConVars to track connection status
ConVar dglab_ws_connected("dglab_ws_connected", "0", FCVAR_REPLICATED, "Indicates if DGLab WebSocket is connected");
ConVar dglab_ws_max_strength("dglab_ws_max_strength", std::to_string(dglab::DGLAB_WS_DEFAULT_MAX_STRENGTH).c_str(), FCVAR_REPLICATED, "Maximum strength for DGLab pulses");

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
void CC_DGLabSetMaxStrength(const CCommand &args)
{
    if (args.ArgC() < 2)
    {
        Warning("Usage: dglab_set_max_strength <strength>\n");
        return;
    }

    int strength = atoi(args[1]);
    if (strength < dglab::DGLAB_WS_MIN_STRENGTH || strength > dglab::DGLAB_WS_MAX_STRENGTH)
    {
        Warning("Strength must be between %d and %d\n", dglab::DGLAB_WS_MIN_STRENGTH, dglab::DGLAB_WS_MAX_STRENGTH);
        return;
    }

    dglab::client.set_max_strength(dglab::Channel::A, strength);
    dglab::client.set_max_strength(dglab::Channel::B, strength);
    dglab_ws_max_strength.SetValue(strength);
    Msg("DGLab max strength set to %d\n", strength);
}

// Register console commands
static ConCommand dglab_connect("dglab_connect", CC_DGLabConnect, "Connect to DGLab WebSocket server");
static ConCommand dglab_disconnect("dglab_disconnect", CC_DGLabDisconnect, "Disconnect from DGLab WebSocket server");
static ConCommand dglab_set_max_strength("dglab_set_max_strength", CC_DGLabSetMaxStrength, "Set maximum strength for DGLab pulses"); 