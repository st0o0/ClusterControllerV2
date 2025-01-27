#include <devicehandler.h>

std::list<DeviceData>::iterator DeviceHandler::find(const std::string &name)
{
    return std::find_if(devices.begin(), devices.end(), [&name](const DeviceData &device)
                        { return device.name == name; });
}

void DeviceHandler::add(DeviceData data)
{
    devices.push_back(data);
}

void DeviceHandler::addTemp(const std::string &name, float temp)
{
    auto it = find(name);
    if (it != devices.end())
    {
        (*it).tempAvg.add(temp);
    }
}

bool DeviceHandler::exists(const std::string &name)
{
    auto it = find(name);
    return it != devices.end();
}

std::list<std::string> DeviceHandler::getDeviceNames()
{
    std::list<std::string> result;
    for (auto const &device : devices)
    {
        result.push_back(device.name);
    }

    return result;
}

DeviceData DeviceHandler::getDeviceData(const std::string &name)
{
    auto it = find(name);
    return *it;
}

float DeviceHandler::getAvgTemp()
{
    float result = 0;
    for (auto const &x : devices)
    {
        result = result + x.tempAvg.average;
    }

    if (devices.empty())
        return 0;
    return result / (double)devices.size();
}