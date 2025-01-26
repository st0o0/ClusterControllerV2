#ifndef controller_h
#define controller_h

#include <defines.h>
#include <devicehandler.h>
#include <fanhandler.h>
#include <wifi.h>

struct ControllerConfig
{
    int fan_off_temp;
    int fan_max_temp;
};

class Controller
{
private:
    ControllerConfig config;
    FanHandler &fanHandler;
    DeviceHandler &deviceHandler;
    int calc_fancurve(float temp);

public:
    Controller(ControllerConfig init_config, FanHandler &fans, DeviceHandler &devices) : fanHandler(fans), deviceHandler(devices), config(init_config) {}
    void begin();
};

#endif