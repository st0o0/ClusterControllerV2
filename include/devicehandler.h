#ifndef devicehandler_h
#define devicehandler_h

#include <defines.h>

struct DeviceData
{
    std::string name;
    std::string lastUpdate;
    Moving_Average<float_t, float_t, 5> tempAvg;
};

class DeviceHandler
{
private:
    std::list<DeviceData> devices;
    std::list<DeviceData>::iterator find(const std::string &name);

public:
    DeviceHandler();
    void add(DeviceData data);
    void addTemp(const std::string &name, float temp);
    bool exists(const std::string &name);
    std::list<std::string> getDeviceNames();
    DeviceData getDeviceData(const std::string &name);
    float getAvgTemp();
};

#endif