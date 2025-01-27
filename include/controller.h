#ifndef controller_h
#define controller_h

#include <defines.h>
#include <devicehandler.h>
#include <fanhandler.h>
#include <wifi.h>

struct controllerconfig
{
    int fan_off_temp;
    int fan_max_temp;
};

class Controller
{
private:
    controllerconfig &config;
    FanHandler &fanHandler;
    DeviceHandler &deviceHandler;
    int calc_fancurve(float temp);

public:
    Controller(controllerconfig &init_config, FanHandler &fans, DeviceHandler &devices) : config(init_config), fanHandler(fans), deviceHandler(devices)  {}
    void begin();
};

#endif