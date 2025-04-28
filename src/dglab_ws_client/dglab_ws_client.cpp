#include "dglab_ws_client.h"
#include <cstdint>
#include "easywsclient.hpp"
#include "cJSON.h"
#include <mutex>

using easywsclient::WebSocket;
static WebSocket::pointer ws = nullptr;
static std::mutex ws_mutex;
static bool connected = false;
static int max_strength_a = DGLAB_WS_DEFAULT_MAX_STRENGTH;
static int max_strength_b = DGLAB_WS_DEFAULT_MAX_STRENGTH;

// Convert channel enum to string
static const char* channel_to_string(dglab_ws_channel_t channel) {
    switch (channel) {
        case DGLAB_WS_CHANNEL_A: return "A";
        case DGLAB_WS_CHANNEL_B: return "B";
        default: return "A";
    }
}

// Convert operation type enum to string
static const char* operation_type_to_string(dglab_ws_operation_type_t op_type) {
    switch (op_type) {
        case DGLAB_WS_OP_SET_TO: return "SET_TO";
        case DGLAB_WS_OP_INCREASE: return "INCREASE";
        case DGLAB_WS_OP_DECREASE: return "DECREASE";
        default: return "SET_TO";
    }
}

extern "C" {

int dglab_ws_connect(const char* ws_url) {
    std::lock_guard<std::mutex> lock(ws_mutex);
    if (ws) {
        return 1; // Already connected
    }
    ws = WebSocket::from_url(ws_url);
    connected = (ws != nullptr);
    return connected ? 0 : -1;
}

void dglab_ws_disconnect() {
    std::lock_guard<std::mutex> lock(ws_mutex);
    if (ws) {
        ws->close();
        delete ws;
        ws = nullptr;
        connected = false;
    }
}

int dglab_ws_is_connected() {
    std::lock_guard<std::mutex> lock(ws_mutex);
    return (ws && connected) ? 1 : 0;
}

static int send_json(const char* json_str) {
    std::lock_guard<std::mutex> lock(ws_mutex);
    if (!ws || !connected) return -1;
    ws->send(json_str);
    ws->poll();
    return 0;
}

int dglab_ws_set_strength(dglab_ws_channel_t channel, dglab_ws_operation_type_t operation_type, int value) {
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "method", "set_strength");
    cJSON* params = cJSON_CreateObject();
    cJSON_AddStringToObject(params, "channel", channel_to_string(channel));
    cJSON_AddStringToObject(params, "operation_type", operation_type_to_string(operation_type));
    cJSON_AddNumberToObject(params, "value", value);
    cJSON_AddItemToObject(root, "params", params);
    char* json_str = cJSON_PrintUnformatted(root);
    int ret = send_json(json_str);
    cJSON_free(json_str);
    cJSON_Delete(root);
    return ret;
}

int dglab_ws_add_pulses(dglab_ws_channel_t channel, const dglab_ws_pulse_t* pulses, int pulse_count) {
    if (!pulses || pulse_count <= 0) {
        return -1; // Invalid parameters
    }

    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "method", "add_pulses");
    cJSON* params = cJSON_CreateObject();
    cJSON_AddStringToObject(params, "channel", channel_to_string(channel));
    
    // Create pulses array
    cJSON* pulses_array = cJSON_CreateArray();
    for (int i = 0; i < pulse_count; i++) {
        cJSON* pulse = cJSON_CreateArray();
        
        // Add time array
        cJSON* time_array = cJSON_CreateArray();
        for (int j = 0; j < 4; j++) {
            cJSON_AddItemToArray(time_array, cJSON_CreateNumber(pulses[i].frequency[j]));
        }
        cJSON_AddItemToArray(pulse, time_array);
        
        // Add value array
        cJSON* value_array = cJSON_CreateArray();
        for (int j = 0; j < 4; j++) {
            cJSON_AddItemToArray(value_array, cJSON_CreateNumber(pulses[i].strength[j]));
        }
        cJSON_AddItemToArray(pulse, value_array);
        
        cJSON_AddItemToArray(pulses_array, pulse);
    }
    
    cJSON_AddItemToObject(params, "pulses", pulses_array);
    cJSON_AddItemToObject(root, "params", params);
    
    char* json_str = cJSON_PrintUnformatted(root);
    int ret = send_json(json_str);
    cJSON_free(json_str);
    cJSON_Delete(root);
    return ret;
}

int dglab_ws_clear_pulses(dglab_ws_channel_t channel) {
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "method", "clear_pulses");
    cJSON* params = cJSON_CreateObject();
    cJSON_AddStringToObject(params, "channel", channel_to_string(channel));
    cJSON_AddItemToObject(root, "params", params);
    char* json_str = cJSON_PrintUnformatted(root);
    int ret = send_json(json_str);
    cJSON_free(json_str);
    cJSON_Delete(root);
    return ret;
}

int dglab_ws_set_max_strength(dglab_ws_channel_t channel, int max_strength) {
    std::lock_guard<std::mutex> lock(ws_mutex);
    if (max_strength < 0 || max_strength > 200) {
        return -1; // Invalid max strength value
    }
    if (channel == DGLAB_WS_CHANNEL_A) {
        max_strength_a = max_strength;
    } else {
        max_strength_b = max_strength;
    }
    return 0;
}

int dglab_ws_set_strength_percentage(dglab_ws_channel_t channel, float percentage) {
    if (percentage < 0.0f || percentage > 1.0f) {
        return -1; // Invalid percentage value
    }

    int max_strength;
    {
        std::lock_guard<std::mutex> lock(ws_mutex);
        max_strength = channel == DGLAB_WS_CHANNEL_A ? max_strength_a : max_strength_b;
    }

    int strength = static_cast<int>(max_strength * percentage);
    return dglab_ws_set_strength(channel, DGLAB_WS_OP_SET_TO, strength);
}

} // extern "C" 