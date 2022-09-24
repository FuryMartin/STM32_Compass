#include "main.h"
#include "math.h"
#include "cpp_main.h"
#include "cmsis_os.h"
#include <type_traits>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <queue>
#include "stdio.h"
using namespace std;

queue<string> msgs;
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart1,(uint8_t *)&ch, 1, 0xFF);
    return ch;
}
void blink_LED(int led, uint16_t *LEDs){
    HAL_GPIO_TogglePin(GPIOE, LEDs[led]);
    printf("LED:%d\n",led);
    //msgs.push("LED:"+ to_string(led) + '\n');
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

void calibration(void){
    const uint8_t chipAddress=0b00111100;
    const uint8_t CFG_REG_A_M=0x60;
    uint8_t data[2]={CFG_REG_A_M,0x00};
    uint8_t addrx[1]={0x68};
    uint8_t addry[1]={0x6A};
    int16_t MagMinX = 10000, MagMaxX = -10000;
    int16_t MagMinY = 10000, MagMaxY = -10000;
    HAL_I2C_Master_Transmit(&hi2c1,chipAddress,data,sizeof(data),100);
    for(;;)
    {
        int16_t out_x;
        HAL_I2C_Master_Transmit(&hi2c1,chipAddress,addrx,sizeof(addrx),100);
        HAL_I2C_Master_Receive(&hi2c1,chipAddress,&out_x,sizeof(out_x),100);

        int16_t out_y;
        HAL_I2C_Master_Transmit(&hi2c1,chipAddress,addry,sizeof(addry),100);
        HAL_I2C_Master_Receive(&hi2c1,chipAddress,&out_y,sizeof(out_y),100);

        if(out_x < MagMinX) MagMinX = out_x;
        if(out_x > MagMaxX) MagMaxX = out_x;

        if(out_y < MagMinY) MagMinY = out_y;
        if(out_y > MagMaxY) MagMaxY = out_y;
        osDelay(10);

        //printf("HelloWorld");
        printf("MagMinX:%d MagMaxX:%d MagMinY:%d MagMaxY:%d\n", MagMinX,MagMaxX,MagMinY,MagMaxY);
    }
    //MagMinX:-413 MagMaxX:-71 MagMinY:-247 MagMaxY:86
    //MagMinX:-387 MagMaxX:-93 MagMinY:-249 MagMaxY:69
}

void string_print(string str)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)str.data(),str.size(),100);
}


map<string, uint16_t> direction_LEDs ={
        {"NW", GPIO_PIN_8},
        {"N", GPIO_PIN_9},
        {"NE",GPIO_PIN_10},
        {"E",GPIO_PIN_11},
        {"SE",GPIO_PIN_12},
        {"S",GPIO_PIN_13},
        {"SW",GPIO_PIN_14},
        {"W",GPIO_PIN_15},
};
void StartCompassTask(void)
{
    string led;
    string led_old = "";

    const uint8_t chipAddress=0b00111100;
    const uint8_t CFG_REG_A_M=0x60;
    uint8_t addrx[1]={0x68};
    uint8_t addry[1]={0x6A};

    uint8_t data[2]={CFG_REG_A_M,0x00};
    HAL_I2C_Master_Transmit(&hi2c1,chipAddress,data,sizeof(data),100);

    float MagMinX = -387.0, MagMaxX = -93.0;
    float MagMinY = -249.0, MagMaxY = 69.0;
    for(;;)
    {
        int16_t out_x;
        int16_t out_y;
        HAL_I2C_Master_Transmit(&hi2c1,chipAddress,addrx,sizeof(addrx),100);
        HAL_I2C_Master_Receive(&hi2c1,chipAddress,&out_x,sizeof(out_x),100);

        HAL_I2C_Master_Transmit(&hi2c1,chipAddress,addry,sizeof(addry),100);
        HAL_I2C_Master_Receive(&hi2c1,chipAddress,&out_y,sizeof(out_y),100);

        float x = ((float)out_x - MagMinX)/(MagMaxX - MagMinX)*2-1.0;
        float y = ((float)out_y - MagMinY)/(MagMaxY - MagMinY)*2-1.0;
        float theta = atan2(y,x)/3.1415926*180.0 + 180.0;
        //串口打印debug
        //string str = "x:" + to_string(out_x) +" y:" + to_string(out_y) + " theta:" + to_string((int)theta) + "\n";
        //string_print(str);
        if((theta>=0&&theta<22.5)||(theta>337.5&&theta<=360)){
            led = "S";
        }
        else if(theta>=22.5&&theta<67.5){
            led = "SW";
        }
        else if(theta>=67.5&&theta<112.5){
            led = "W";
        }
        else if(theta>=112.5&&theta<157.5){
            led = "NW";
        }
        else if(theta>=157.5&&theta<202.5){
            led = "N";
        }
        else if(theta>=202.5&&theta<247.5){
            led = "NE";
        }
        else if(theta>=247.5&&theta<=292.5){
            led = "E";
        }
        else if(theta>=292.5&&theta<=337.5){
            led = "SE";
        }
        if(led != led_old){
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOE, direction_LEDs[led],GPIO_PIN_SET);
            led_old=led;
        }
        osDelay(10);
    }
}

void circle_blink()
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
