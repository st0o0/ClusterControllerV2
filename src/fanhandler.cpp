#include <fanhandler.h>

void FanHandler::begin()
{
    fanconfig inFanConfig = config.InFan;
    fanconfig outFanConfig = config.OutFan;
    pinMode(inFanConfig.sensorPin, INPUT_PULLDOWN);
    pinMode(outFanConfig.sensorPin, INPUT_PULLDOWN);
    pinMode(inFanConfig.pwmPin, OUTPUT);
    pinMode(outFanConfig.pwmPin, OUTPUT);

    analogWriteResolution(8);
    analogWriteFrequency(25000);
    inputFan.begin();
    outputFan.begin();
}

uint FanHandler::getInFanSpeed()
{
    return inputFan.getSpeed();
}

uint FanHandler::getOutFanSpeed()
{
    return outputFan.getSpeed();
}

bool FanHandler::setFanSpeed(uint value)
{
    outputFan.setDutyCycle(value);
    inputFan.setDutyCycle(value);
    return true;
}

bool FanHandler::setInFanSpeed(uint value)
{
    inputFan.setDutyCycle(value);
    return true;
}

bool FanHandler::setOutFanSpeed(uint value)
{
    outputFan.setDutyCycle(value);
    return true;
}

FanHandler::FanHandler(fanhandlerconfig &init_config)
{
    config = init_config;
    fanconfig inFanConfig = config.InFan;
    fanconfig outFanConfig = config.OutFan;
    inputFan = FanController(inFanConfig.sensorPin, inFanConfig.sensorThreshold, inFanConfig.pwmPin, 0);
    outputFan = FanController(outFanConfig.sensorPin, outFanConfig.sensorThreshold, outFanConfig.pwmPin, 1);
}