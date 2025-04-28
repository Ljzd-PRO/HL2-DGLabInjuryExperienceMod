#include "dglab_ws_client.h"
#include <stdio.h>
#include <windows.h>

int main() {
    const char* ws_url = "ws://127.0.0.1:5679";
    if (dglab_ws_connect(ws_url) != 0) {
        printf("Failed to connect to WebSocket server\n");
        return 1;
    }
    printf("Connected to WebSocket server\n");

    // Test set_strength
    dglab_ws_set_strength("A", "SET_TO", 100);
    Sleep(1000);
    dglab_ws_set_strength("B", "INCREASE", 20);
    Sleep(1000);

    // Test add_pulses
    const char* pulses_json = "[[[10,10,20,30],[0,5,10,50]],[[10,10,20,30],[0,5,10,50]]]";
    dglab_ws_add_pulses("A", pulses_json);
    Sleep(1000);

    // Test clear_pulses
    dglab_ws_clear_pulses("A");
    Sleep(1000);

    dglab_ws_disconnect();
    printf("Disconnected from WebSocket server\n");
    return 0;
} 