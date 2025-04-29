#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Default max strength for both channels
#define DGLAB_WS_DEFAULT_MAX_STRENGTH 120

// Channel enumeration
typedef enum {
    DGLAB_WS_CHANNEL_A = 0,
    DGLAB_WS_CHANNEL_B = 1
} dglab_ws_channel_t;

// Operation type enumeration
typedef enum {
    DGLAB_WS_OP_SET_TO = 0,
    DGLAB_WS_OP_INCREASE = 1,
    DGLAB_WS_OP_DECREASE = 2
} dglab_ws_operation_type_t;

// Pulse structure
typedef struct {
    int frequency[4];
    int strength[4];
} dglab_ws_pulse_t;

// Connect to WebSocket server
int dglab_ws_connect(const char* ws_url);
// Disconnect
void dglab_ws_disconnect();
// Set strength
int dglab_ws_set_strength(dglab_ws_channel_t channel, dglab_ws_operation_type_t operation_type, int value);
// Add pulses
int dglab_ws_add_pulses(dglab_ws_channel_t channel, const dglab_ws_pulse_t* pulses, int pulse_count);
// Clear pulses
int dglab_ws_clear_pulses(dglab_ws_channel_t channel);
// Check if connected
int dglab_ws_is_connected();
// Set max strength
int dglab_ws_set_max_strength(dglab_ws_channel_t channel, int max_strength);
// Set strength percentage
int dglab_ws_set_strength_percentage(dglab_ws_channel_t channel, float percentage);

#ifdef __cplusplus
}
#endif 