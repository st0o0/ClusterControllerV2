#include <controller.h>

Controller::Controller(controllerconfig &init, FanHandler &fans, DeviceHandler &devices) : config(init), fanHandler(fans), deviceHandler(devices)
{
}

void Controller::begin()
{
    fanHandler.begin();
}

void Controller::handle()
{
    auto avg_temp = deviceHandler.getAvgTemp();
    int calced_fanspeed = calc_fancurve(avg_temp);
    fanHandler.setFanSpeed(calced_fanspeed);
}

int Controller::calc_fancurve(float temp)
{
    return max((float)0, 100 * (temp - config.fan_off_temp) / (config.fan_max_temp - config.fan_off_temp));
}