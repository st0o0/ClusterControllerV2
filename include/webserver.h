/* #ifndef webserver_h
#define webserver_h

#include <defines.h>
#include <wifi.h>
#include <devicehandler.h>
#include <fanhandler.h>


struct RequestData
{
    std::string device;
    std::string timestamp;
    float temperature;
};

class Webserver
{
private:
    static WiFiWebServer server;
    static DeviceHandler deviceHandler;
    static FanHandler fanHandler;
    static void begin();
    static void handle_root();
    static void handle_api_speed();
    static void handle_api_device();
    static void handle_notfound();

public:
    static void handle();
};

static void wifi_module_failed();

#endif */