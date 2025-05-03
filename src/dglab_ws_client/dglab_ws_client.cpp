#include "dglab_ws_client.h"
#include <cstdint>
#include "easywsclient.hpp"
#include "cJSON.h"
#include <mutex>
#include <memory>
#include <string>
#include <stdexcept>
#include <thread>
#include <atomic>

namespace dglab {

// Define global client instance
WSClient client;
    
// WSClient implementation
WSClient::~WSClient() {
    disconnect();
}

void WSClient::message_loop() {
    while (!should_stop_) {
        {
            std::lock_guard<std::mutex> lock(ws_mutex_);
            if (ws_ && connected_) {
                ws_->poll();
                ws_->dispatch([](const std::string& message) {
                    // Handle text messages
                });
                ws_->dispatchBinary([](const std::vector<uint8_t>& message) {
                    // Handle binary messages
                });
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(DGLAB_WS_POLL_INTERVAL_MS));
    }
}

void WSClient::start_message_thread() {
    should_stop_ = false;
    message_thread_ = std::thread(&WSClient::message_loop, this);
}

void WSClient::stop_message_thread() {
    should_stop_ = true;
    if (message_thread_.joinable()) {
        message_thread_.join();
    }
}

std::string WSClient::channel_to_string(Channel channel) {
    switch (channel) {
        case Channel::A: return "A";
        case Channel::B: return "B";
        default: return "A";
    }
}

std::string WSClient::operation_type_to_string(OperationType op_type) {
    switch (op_type) {
        case OperationType::SET_TO: return "SET_TO";
        case OperationType::INCREASE: return "INCREASE";
        case OperationType::DECREASE: return "DECREASE";
        default: return "SET_TO";
    }
}

int WSClient::send_json(const std::string& json_str) {
    if (!is_connected()) return -1;
    std::lock_guard<std::mutex> lock(ws_mutex_);
    ws_->send(json_str);
    ws_->poll(); // Restore poll call to ensure message is sent
    return 0;
}

bool WSClient::connect(const std::string& ws_url) {
    if (is_connected()) disconnect();
    std::lock_guard<std::mutex> lock(ws_mutex_);
    ws_ = std::unique_ptr<easywsclient::WebSocket>(easywsclient::WebSocket::from_url(ws_url));
    connected_ = (ws_ != nullptr);
    if (connected_) {
        start_message_thread();
    }
    return connected_;
}

void WSClient::disconnect() {
    std::lock_guard<std::mutex> lock(ws_mutex_);
    stop_message_thread();
    if (ws_) {
        ws_->close();
        ws_.reset();
        connected_ = false;
    }
}

bool WSClient::is_connected() const {
    std::lock_guard<std::mutex> lock(ws_mutex_);
    return (ws_ && connected_ && ws_->getReadyState() != easywsclient::WebSocket::CLOSED);
}

int WSClient::set_strength(Channel channel, OperationType operation_type, int value) {
    // Check if the new strength value is the same as the last one
    if (channel == Channel::A && value == last_strength_a_) {
        return 0; // Skip if the value is the same
    }
    if (channel == Channel::B && value == last_strength_b_) {
        return 0; // Skip if the value is the same
    }

    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "method", "set_strength");
    cJSON* params = cJSON_CreateObject();
    cJSON_AddStringToObject(params, "channel", channel_to_string(channel).c_str());
    cJSON_AddStringToObject(params, "operation_type", operation_type_to_string(operation_type).c_str());
    cJSON_AddNumberToObject(params, "value", value);
    cJSON_AddItemToObject(root, "params", params);
    
    char* json_str = cJSON_PrintUnformatted(root);
    int ret = send_json(json_str);
    cJSON_free(json_str);
    cJSON_Delete(root);

    // Update last strength value if the operation was successful
    if (ret == 0) {
        if (channel == Channel::A) {
            last_strength_a_ = value;
        } else {
            last_strength_b_ = value;
        }
    }

    return ret;
}

int WSClient::add_pulses(Channel channel, const std::vector<Pulse>& pulses) {
    if (pulses.empty()) {
        return -1;
    }

    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "method", "add_pulses");
    cJSON* params = cJSON_CreateObject();
    cJSON_AddStringToObject(params, "channel", channel_to_string(channel).c_str());
    
    cJSON* pulses_array = cJSON_CreateArray();
    for (const auto& pulse : pulses) {
        cJSON* pulse_obj = cJSON_CreateArray();
        
        cJSON* time_array = cJSON_CreateArray();
        for (int j = 0; j < 4; j++) {
            cJSON_AddItemToArray(time_array, cJSON_CreateNumber(pulse.frequency[j]));
        }
        cJSON_AddItemToArray(pulse_obj, time_array);
        
        cJSON* value_array = cJSON_CreateArray();
        for (int j = 0; j < 4; j++) {
            cJSON_AddItemToArray(value_array, cJSON_CreateNumber(pulse.strength[j]));
        }
        cJSON_AddItemToArray(pulse_obj, value_array);
        
        cJSON_AddItemToArray(pulses_array, pulse_obj);
    }
    
    cJSON_AddItemToObject(params, "pulses", pulses_array);
    cJSON_AddItemToObject(root, "params", params);
    
    char* json_str = cJSON_PrintUnformatted(root);
    int ret = send_json(json_str);
    cJSON_free(json_str);
    cJSON_Delete(root);
    return ret;
}

int WSClient::clear_pulses(Channel channel) {
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "method", "clear_pulses");
    cJSON* params = cJSON_CreateObject();
    cJSON_AddStringToObject(params, "channel", channel_to_string(channel).c_str());
    cJSON_AddItemToObject(root, "params", params);
    
    char* json_str = cJSON_PrintUnformatted(root);
    int ret = send_json(json_str);
    cJSON_free(json_str);
    cJSON_Delete(root);
    return ret;
}

bool WSClient::set_max_strength(Channel channel, int max_strength) {
    if (max_strength < DGLAB_WS_MIN_STRENGTH || max_strength > DGLAB_WS_MAX_STRENGTH) {
        return false;
    }
    std::lock_guard<std::mutex> lock(ws_mutex_);
    if (channel == Channel::A) {
        max_strength_a_ = max_strength;
    } else {
        max_strength_b_ = max_strength;
    }
    return true;
}

bool WSClient::set_min_strength(Channel channel, int min_strength) {
    if (min_strength < DGLAB_WS_MIN_STRENGTH || min_strength > DGLAB_WS_MAX_STRENGTH) {
        return false;
    }
    std::lock_guard<std::mutex> lock(ws_mutex_);
    if (channel == Channel::A) {
        min_strength_a_ = min_strength;
    } else {
        min_strength_b_ = min_strength;
    }
    return true;
}

int WSClient::get_max_strength(Channel channel) const {
    std::lock_guard<std::mutex> lock(ws_mutex_);
    return channel == Channel::A ? max_strength_a_ : max_strength_b_;
}

int WSClient::get_min_strength(Channel channel) const {
    std::lock_guard<std::mutex> lock(ws_mutex_);
    return channel == Channel::A ? min_strength_a_ : min_strength_b_;
}

int WSClient::set_strength_percentage(Channel channel, float percentage) {
    if (percentage < 0.0f || percentage > 1.0f) {
        return -1;
    }
    const int max_strength = channel == Channel::A ? max_strength_a_ : max_strength_b_;
    const int min_strength = channel == Channel::A ? min_strength_a_ : min_strength_b_;
    const int strength = static_cast<int>(percentage * (max_strength - min_strength) + min_strength);
    return set_strength(channel, OperationType::SET_TO, strength);
}

} // namespace dglab 