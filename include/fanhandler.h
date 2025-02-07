#ifndef fanhandler_h
#define fanhandler_h

#include <defines.h>
#include <fancontroller.h>

struct fanconfig
{
    uint sensorThreshold;
    uint pwmPin;
    uint sensorPin;
};

struct fanhandlerconfig
{
    fanconfig InFan;
    fanconfig OutFan;
};

class FanHandler
{
private:
    FanController inputFan;
    FanController outputFan;
    fanhandlerconfig config;

public:
    FanHandler(fanhandlerconfig &init_config);
    void begin();
    uint getInFanSpeed();
    uint getOutFanSpeed();
    bool setFanSpeed(uint value);
    bool setInFanSpeed(uint value);
    bool setOutFanSpeed(uint value);
};

#endif