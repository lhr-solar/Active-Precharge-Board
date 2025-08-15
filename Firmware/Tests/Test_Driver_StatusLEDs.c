#include "StatusLEDs.h"

int main(void) {
    Status_Leds_Init();
    while (1) {
        for (int i = 0; i < NUM_STATUS_LED; i++) {
            Status_Leds_Toggle(i);
            volatile uint32_t waitTime = 0;
            while (waitTime <= 999999) {
                waitTime++;
            }
        }
    }
    return 0;
}