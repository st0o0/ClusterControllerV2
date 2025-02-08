#include "devicehandler.h"
#include <algorithm>
#include <numeric>

DeviceHandler::DeviceHandler() {}

std::list<DeviceData>::iterator DeviceHandler::find(const std::string &name)
{
    return std::find_if(devices.begin(), devices.end(),
                        [&name](const DeviceData &device)
                        { return device.name == name; });
}

bool DeviceHandler::empty()
{
    return devices.empty();
}

void DeviceHandler::add(DeviceData data)
{
    devices.push_back(std::move(data));
}

void DeviceHandler::addOrUpdate(const std::string &name, float temperature, const std::string &timestamp)
{
    auto it = find(name);
    if (it != devices.end())
    {
        it->tempAvg.add(temperature);
        it->lastUpdate = timestamp;
    }
    else
    {
        DeviceData newDevice{name, timestamp, Moving_Average<float_t, float_t, 5>()};
        newDevice.tempAvg.add(temperature);
        add(std::move(newDevice));
    }
}

void DeviceHandler::deleteOne(const std::string &name)
{
    auto it = find(name);
    if (it != devices.end())
    {
        devices.erase(it);
    }
}

bool DeviceHandler::exists(const std::string &name)
{
    return find(name) != devices.end();
}

std::list<std::string> DeviceHandler::getDeviceNames()
{
    std::list<std::string> names;
    for (const auto &device : devices)
    {
        names.push_back(device.name);
    }
    return names;
}

DeviceData DeviceHandler::getDeviceData(const std::string &name)
{
    auto it = find(name);
    if (it != devices.end())
    {
        return *it;
    }

    return DeviceData{"", "", Moving_Average<float_t, float_t, 5>()};
}

float DeviceHandler::getAvgTemp()
{
    if (devices.empty())
    {
        return 0.0f;
    }

    float sum = std::accumulate(devices.begin(), devices.end(), 0.0f,
                                [](float acc, const DeviceData &device)
                                {
                                    return acc + device.tempAvg.average;
                                });
    return sum / devices.size();
}

float DeviceHandler::getMinTemp()
{
    if (devices.empty())
    {
        return 0.0f;
    }
    return std::min_element(devices.begin(), devices.end(),
                            [](const DeviceData &a, const DeviceData &b)
                            {
                                return a.tempAvg.average < b.tempAvg.average;
                            })
        ->tempAvg.average;
}

float DeviceHandler::getMaxTemp()
{
    if (devices.empty())
    {
        return 0.0f;
    }
    return std::max_element(devices.begin(), devices.end(),
                            [](const DeviceData &a, const DeviceData &b)
                            {
                                return a.tempAvg.average < b.tempAvg.average;
                            })
        ->tempAvg.average;
}
