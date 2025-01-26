#include <controller.h>

int Controller::calc_fancurve(float temp)
{
    return max(0, 100 * (temp - config.fan_off_temp) / (config.fan_max_temp - config.fan_max_temp));
}

void Controller::begin()
{
    
}