#ifndef FANCONTROLLER_H
#define FANCONTROLLER_H

#include "Arduino.h"

class FanController
{
public:
    FanController(byte sensorPin, unsigned int sensorThreshold, byte pwmPin, byte pwmChannel);
    FanController();
    void begin();
    unsigned int getSpeed();
    void setDutyCycle(uint dutyCycle);
    byte getDutyCycle();

private:
    byte _sensorPin;
    byte _sensorInterruptPin;
    unsigned int _sensorThreshold;
    byte _pwmPin;
    byte _pwmChannel;
    byte _pwmDutyCycle;
    volatile unsigned int _halfRevs;
    unsigned long _lastMillis;
    unsigned int _lastReading;

    byte _instance;
    static FanController *_instances[6];

    void _attachInterrupt();
    void IRAM_ATTR _trigger();

    static void IRAM_ATTR _triggerCaller(byte instance);

    static void IRAM_ATTR _triggerExt0();
    static void IRAM_ATTR _triggerExt1();
    static void IRAM_ATTR _triggerExt2();
    static void IRAM_ATTR _triggerExt3();
    static void IRAM_ATTR _triggerExt4();
    static void IRAM_ATTR _triggerExt5();
};

#endif
