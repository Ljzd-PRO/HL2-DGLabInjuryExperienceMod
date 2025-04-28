#include "dglab_ws_client.h"
#include <cstdint>
#include "easywsclient.hpp"
#include "cJSON.h"
#include <mutex>

using easywsclient::WebSocket;
static WebSocket::pointer ws = nullptr;
static std::mutex ws_mutex;
static bool connected = false;

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

int dglab_ws_set_strength(const char* channel, const char* operation_type, int value) {
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "method", "set_strength");
    cJSON* params = cJSON_CreateObject();
    cJSON_AddStringToObject(params, "channel", channel);
    cJSON_AddStringToObject(params, "operation_type", operation_type);
    cJSON_AddNumberToObject(params, "value", value);
    cJSON_AddItemToObject(root, "params", params);
    char* json_str = cJSON_PrintUnformatted(root);
    int ret = send_json(json_str);
    cJSON_free(json_str);
    cJSON_Delete(root);
    return ret;
}

int dglab_ws_add_pulses(const char* channel, const char* pulses_json) {
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "method", "add_pulses");
    cJSON* params = cJSON_CreateObject();
    cJSON_AddStringToObject(params, "channel", channel);
    cJSON* pulses = cJSON_Parse(pulses_json);
    if (!pulses) {
        cJSON_Delete(root);
        cJSON_Delete(params);
        return -2; // Invalid JSON
    }
    cJSON_AddItemToObject(params, "pulses", pulses);
    cJSON_AddItemToObject(root, "params", params);
    char* json_str = cJSON_PrintUnformatted(root);
    int ret = send_json(json_str);
    cJSON_free(json_str);
    cJSON_Delete(root);
    return ret;
}

int dglab_ws_clear_pulses(const char* channel) {
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "method", "clear_pulses");
    cJSON* params = cJSON_CreateObject();
    cJSON_AddStringToObject(params, "channel", channel);
    cJSON_AddItemToObject(root, "params", params);
    char* json_str = cJSON_PrintUnformatted(root);
    int ret = send_json(json_str);
    cJSON_free(json_str);
    cJSON_Delete(root);
    return ret;
}

} // extern "C" 