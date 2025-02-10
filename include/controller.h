#ifndef controller_h
#define controller_h

#include <defines.h>
#include <devicehandler.h>
#include <fanhandler.h>

struct controllerconfig
{
    int fan_off_temp;
    int fan_max_temp;
};

class Controller
{
private:
    controllerconfig *config;
    FanHandler *fanHandler;
    DeviceHandler *deviceHandler;
    bool automode = true;
    int calced_speed = 0;
    int inputfan = 100;
    int outputfan = 100;
    int calc_fancurve(float temp);

public:
    Controller(controllerconfig *init, FanHandler *fans, DeviceHandler *devices);
    void begin();
    void handle();
    int getCalcedSpeed();
    int getOutputFanSpeed();
    int getInputFanSpeed();
    void setFanSpeed(int inputFan, int outputFan);
    bool getAutomode();
    void setAutomode(bool value);
};

#endif