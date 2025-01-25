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

static void wifi_module_failed();

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
    void handle();
    class WebServer
    {
    private:
        static void handle_root();
        static void handle_api_speed();
        static void handle_api_device();
        static void handle_notfound();

    public:
        void begin();
        void handle();
    };
};

#endif