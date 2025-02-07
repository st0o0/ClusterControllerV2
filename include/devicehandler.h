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
    void add(DeviceData data);

public:
    DeviceHandler();
    void addOrUpdate(const std::string &name, float temperature, const std::string &timestamp);
    void deleteOne(const std::string &name);
    bool exists(const std::string &name);
    std::list<std::string> getDeviceNames();
    DeviceData getDeviceData(const std::string &name);
    float getAvgTemp();
    float getMinTemp();
    float getMaxTemp();
};

#endif