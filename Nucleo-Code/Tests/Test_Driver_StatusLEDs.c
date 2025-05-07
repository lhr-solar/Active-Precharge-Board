#include "statusLEDs.h"

int main(void){
    Status_Leds_Init();
    Status_Leds_All_On();
    while(1){
        // for(int i = 0; i < NUM_STATUS_LED; i++){
        //     Status_Leds_Toggle(i);
        // }
        // HAL_Delay(1000);
    }
    return 0;
}