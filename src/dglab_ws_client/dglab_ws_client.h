#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include "easywsclient.hpp"

namespace dglab {

// Constants
static const int DGLAB_WS_MIN_STRENGTH = 0;
static const int DGLAB_WS_MAX_STRENGTH = 200;
static const int DGLAB_WS_DEFAULT_MIN_STRENGTH = 0;
static const int DGLAB_WS_DEFAULT_MAX_STRENGTH = 100;
static const int DGLAB_WS_POLL_INTERVAL_MS = 100; // Message processing interval in milliseconds

enum class Channel {
    A,
    B
};

enum class OperationType {
    SET_TO,
    INCREASE,
    DECREASE
};

struct Pulse {
    int frequency[4];
    int strength[4];
};

class WSClient {
public:
    WSClient() = default;
    ~WSClient();

    bool connect(const std::string& ws_url);
    void disconnect();
    bool is_connected() const;
    int set_strength(Channel channel, OperationType operation_type, int value);
    int add_pulses(Channel channel, const std::vector<Pulse>& pulses);
    int clear_pulses(Channel channel);
    bool set_max_strength(Channel channel, int max_strength);
    bool set_min_strength(Channel channel, int min_strength);
    int get_max_strength(Channel channel) const;
    int get_min_strength(Channel channel) const;
    int set_strength_percentage(Channel channel, float percentage);

private:
    std::unique_ptr<easywsclient::WebSocket> ws_;
    mutable std::mutex ws_mutex_;
    bool connected_ = false;
    int max_strength_a_ = DGLAB_WS_DEFAULT_MAX_STRENGTH;
    int min_strength_a_ = DGLAB_WS_DEFAULT_MIN_STRENGTH;
    int max_strength_b_ = DGLAB_WS_DEFAULT_MAX_STRENGTH;
    int min_strength_b_ = DGLAB_WS_DEFAULT_MIN_STRENGTH;
    
    // Last strength values for each channel
    int last_strength_a_ = 0;
    int last_strength_b_ = 0;
    
    // Message handling thread related
    std::thread message_thread_;
    std::atomic<bool> should_stop_ = false;
    void message_loop();
    void start_message_thread();
    void stop_message_thread();

    static std::string channel_to_string(Channel channel);
    static std::string operation_type_to_string(OperationType op_type);
    int send_json(const std::string& json_str);
};

// Global client instance
extern WSClient client;

} // namespace dglab 