//
// Created by Fury on 2022/9/22.
//

#ifndef HELLOWORLD_CPP_MAIN_H
#define HELLOWORLD_CPP_MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif
void circle_blink();
void blink_LED(int led, uint16_t *LEDs);
void SerialPrint(void);
void StartCompassTask(void);
void calibration(void);
#ifdef __cplusplus
}
#endif

#endif //HELLOWORLD_CPP_MAIN_H
