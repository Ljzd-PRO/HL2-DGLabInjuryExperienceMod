#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Connect to WebSocket server
int dglab_ws_connect(const char* ws_url);
// Disconnect
void dglab_ws_disconnect();
// Set strength
int dglab_ws_set_strength(const char* channel, const char* operation_type, int value);
// Add pulses
int dglab_ws_add_pulses(const char* channel, const char* pulses_json);
// Clear pulses
int dglab_ws_clear_pulses(const char* channel);
// Check if connected
int dglab_ws_is_connected();

#ifdef __cplusplus
}
#endif 