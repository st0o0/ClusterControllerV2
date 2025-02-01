#include "Arduino.h"
#include "FanController.h"

FanController *FanController::_instances[6];

FanController::FanController(byte sensorPin, unsigned int sensorThreshold, byte pwmPin, byte pwmChannel)
{
    _sensorPin = sensorPin;
    _sensorInterruptPin = digitalPinToInterrupt(sensorPin);
    _sensorThreshold = sensorThreshold;
    _pwmPin = pwmPin;
    _pwmChannel = pwmChannel;
    _pwmDutyCycle = 100;
    _halfRevs = 0;
    _lastReading = 0;
    _lastMillis = millis();
}

FanController::FanController()
{
}

void FanController::begin()
{
    static byte instance = 0;
    _instance = instance;
    _instances[instance] = this;

    ledcSetup(_pwmChannel, 25000, 8); // 25 kHz, 8-Bit PWM
    ledcAttachPin(_pwmPin, _pwmChannel);

    pinMode(_sensorPin, INPUT_PULLUP);
    setDutyCycle(_pwmDutyCycle);
    _attachInterrupt();
    instance++;
}

unsigned int FanController::getSpeed()
{
    unsigned long elapsed = millis() - _lastMillis;
    if (elapsed > _sensorThreshold)
    {
        detachInterrupt(_sensorInterruptPin);
        double correctionFactor = 1000.0 / elapsed;
        _lastReading = (correctionFactor * _halfRevs / 2) * 60;
        _halfRevs = 0;
        _lastMillis = millis();
        _attachInterrupt();
    }
    return _lastReading;
}

void FanController::setDutyCycle(uint dutyCycle)
{
    _pwmDutyCycle = min((int)dutyCycle, 100);
    int pwmValue = map(_pwmDutyCycle, 0, 100, 0, 255);
    ledcWrite(_pwmChannel, pwmValue);
}

byte FanController::getDutyCycle()
{
    return _pwmDutyCycle;
}

void FanController::_attachInterrupt()
{
    switch (_instance)
    {
    case 0:
        attachInterrupt(digitalPinToInterrupt(_sensorPin), _triggerExt0, FALLING);
        break;
    case 1:
        attachInterrupt(digitalPinToInterrupt(_sensorPin), _triggerExt1, FALLING);
        break;
    case 2:
        attachInterrupt(digitalPinToInterrupt(_sensorPin), _triggerExt2, FALLING);
        break;
    case 3:
        attachInterrupt(digitalPinToInterrupt(_sensorPin), _triggerExt3, FALLING);
        break;
    case 4:
        attachInterrupt(digitalPinToInterrupt(_sensorPin), _triggerExt4, FALLING);
        break;
    case 5:
        attachInterrupt(digitalPinToInterrupt(_sensorPin), _triggerExt5, FALLING);
        break;
    }
}

void IRAM_ATTR FanController::_trigger()
{
    _halfRevs++;
}

void IRAM_ATTR FanController::_triggerCaller(byte instance)
{
    if (FanController::_instances[instance] != nullptr)
    {
        FanController::_instances[instance]->_trigger();
    }
}

void IRAM_ATTR FanController::_triggerExt0() { FanController::_triggerCaller(0); }
void IRAM_ATTR FanController::_triggerExt1() { FanController::_triggerCaller(1); }
void IRAM_ATTR FanController::_triggerExt2() { FanController::_triggerCaller(2); }
void IRAM_ATTR FanController::_triggerExt3() { FanController::_triggerCaller(3); }
void IRAM_ATTR FanController::_triggerExt4() { FanController::_triggerCaller(4); }
void IRAM_ATTR FanController::_triggerExt5() { FanController::_triggerCaller(5); }
