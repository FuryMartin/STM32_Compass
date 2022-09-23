/*
#include "main.h"
#include "cpp_main.h"
#include "cmsis_os.h"
#include <type_traits>
#include <string>
using namespace std;

void blink_LED(int led, uint16_t *LEDs){
    string string1 = "LED:" + to_string(led) + "\n";
    HAL_GPIO_TogglePin(GPIOE, LEDs[led]);
    osDelay(1000);
    HAL_GPIO_TogglePin(GPIOE, LEDs[led]);
}

void circle_blink()
{
    uint16_t LEDs[] = {GPIO_PIN_8,GPIO_PIN_9,GPIO_PIN_10,GPIO_PIN_11,GPIO_PIN_12,GPIO_PIN_13,GPIO_PIN_14,GPIO_PIN_15};
    int length = extent<decltype(LEDs), 0>::value;
    int status = 0;
    for(int current_led = 0;;)
    {
        int dat;
        xQueueReceive(msgQ, &dat, 200);
        if (dat == 1){
            status = !status;
        }
        //blink_LED(current_led, LEDs);
        HAL_GPIO_TogglePin(GPIOE, LEDs[current_led]);
        osDelay(1000);
        HAL_GPIO_TogglePin(GPIOE, LEDs[current_led]);

        if(status == 1)
        {
            current_led++;
            current_led %= length;
        }else if(status == 0)
        {
            current_led--;
            if (current_led == -1)
                current_led = length-1;
        }
    }
}
*/