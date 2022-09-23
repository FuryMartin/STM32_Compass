#include "main.h"
#include "cpp_main.h"
#include "cmsis_os.h"
#include <type_traits>
#include <string>
#include <algorithm>
#include <vector>
#include <queue>
using namespace std;

queue<string> msgs;

void blink_LED(int led, uint16_t *LEDs){
    HAL_GPIO_TogglePin(GPIOE, LEDs[led]);
    msgs.push("LED:"+ to_string(led) + '\n');
    osDelay(300);
    HAL_GPIO_TogglePin(GPIOE, LEDs[led]);
}

void SerialPrint(void)
{
    for(;;) {
        if(!msgs.empty())
        {
            string str = msgs.front();
            HAL_UART_Transmit(&huart1, (uint8_t *) str.data(), str.size(), 0xff);
            msgs.pop();
        }
        osDelay(1);
    }
}

    void
    circle_blink()
{
    uint16_t LEDs[] = {GPIO_PIN_8,GPIO_PIN_9,GPIO_PIN_10,GPIO_PIN_11,GPIO_PIN_12,GPIO_PIN_13,GPIO_PIN_14,GPIO_PIN_15};
    int length = extent<decltype(LEDs), 0>::value;
    int status = 0;
    int current_led = 0;
    for(;;)
    {
        int dat;
        xQueueReceive(msgQ, &dat, 1);
        if (dat == 1) {
            status = !status;
        }
        blink_LED(current_led, LEDs);
        if(status == 1)
        {
            current_led++;
            current_led %= length;
        }else if(status == 0) {
            current_led--;
            if (current_led == -1)
                current_led = length - 1;
        }
    }
}
