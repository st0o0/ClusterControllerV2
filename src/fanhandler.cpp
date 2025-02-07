#include <fanhandler.h>

void FanHandler::begin()
{
    analogWriteResolution(8);
    analogWriteFrequency(25000);
    inputFan.begin();
    outputFan.begin();
}

uint FanHandler::getInFanSpeed()
{
    Serial.println("Set InputSpeed To: " + String(inputFan.getDutyCycle()));
    return inputFan.getSpeed();
}

uint FanHandler::getOutFanSpeed()
{
    Serial.println("Set OutputSpeed To: " + String(outputFan.getDutyCycle()));
    return outputFan.getSpeed();
}

bool FanHandler::setFanSpeed(uint value)
{
    inputFan.setDutyCycle(value);
    outputFan.setDutyCycle(value);
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