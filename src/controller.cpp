#include <controller.h>

Controller::Controller(controllerconfig *init, FanHandler *fans, DeviceHandler *devices) : config(init), fanHandler(fans), deviceHandler(devices)
{
}

void Controller::begin()
{
    fanHandler->begin();
}

void Controller::handle()
{
    auto avg_temp = deviceHandler->getAvgTemp();
    calced_speed = calc_fancurve(avg_temp);
    if (automode)
    {

        fanHandler->setInFanSpeed((calced_speed * 0.1) + calced_speed);
        fanHandler->setOutFanSpeed(calced_speed);
    }
}

int Controller::getCalcedSpeed()
{
    return calced_speed;
}

bool Controller::getAutomode()
{
    return automode;
}

void Controller::setAutomode(bool value)
{
    automode = value;
}

int Controller::getInputFanSpeed()
{
    return inputfan;
}

int Controller::getOutputFanSpeed()
{
    return outputfan;
}

void Controller::setFanSpeed(int inputFan, int outputFan)
{
    inputfan = inputFan;
    outputfan = outputFan;
    fanHandler->setInFanSpeed(inputfan);
    fanHandler->setOutFanSpeed(outputfan);
}

int Controller::calc_fancurve(float temp)
{
    return min(max((float)0, 100 * (temp - config->fan_off_temp) / (config->fan_max_temp - config->fan_off_temp)), (float)100);
}