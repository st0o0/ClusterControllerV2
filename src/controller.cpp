#include <controller.h>

WiFiWebServer server(80);

int Controller::calc_fancurve(float temp)
{
    return max(0, 100 * (temp - config.fan_off_temp) / (config.fan_max_temp - config.fan_max_temp));
}

void Controller::WebServer::begin()
{
    EspSerial.begin(115200);
    WiFi.init(EspSerial);
    if (WiFi.status() == WL_NO_MODULE)
    {
        wifi_module_failed();
    }
    WiFi.sleepMode(WIFI_NONE_SLEEP);
    WiFi.setHostname("ClusterController");
    WiFi.disconnect();
    WiFi.endAP(true);
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(F("Connecting to WiFi..."));
        int wifiStatus = WiFi.begin(ssid, password);

        if (wifiStatus == WL_CONNECTED)
        {
            WiFi.setAutoConnect(true);
            Serial.println();
            Serial.println(F("Connected to WiFi network."));
            IPAddress ip = WiFi.localIP();
            Serial.print("IP Address: ");
            Serial.println(ip);
        }
        else
        {
            WiFi.disconnect();
            Serial.println();
            Serial.println(F("Connection to WiFi network failed."));
        }
    }
    else
    {
        Serial.println(F("Already connected to WiFi."));
        IPAddress ip = WiFi.localIP();
        Serial.print("IP Address: ");
        Serial.println(ip);
    }
    server.on(F("/"), handle_root);
    server.on(F("/api/speed"), handle_api_speed);
    server.on(F("/api/device"), handle_api_device);
    server.on(F("/favicon.ico"), []()
              { server.send(404, F("text/plain"), F("")); });
    server.onNotFound(handle_notfound);
    server.begin();
}

void Controller::WebServer::handle()
{
    server.handleClient();
}

void Controller::WebServer::handle_root()
{
}

void Controller::WebServer::handle_api_device()
{
}

void Controller::WebServer::handle_api_speed()
{
}

void Controller::WebServer::handle_notfound()
{
    String message = F("File Not Found\n\n");

    message += F("URI: ");
    message += server.uri();
    message += F("\nMethod: ");
    message += (server.method() == HTTP_GET) ? F("GET") : F("POST");
    message += F("\nArguments: ");
    message += server.args();
    message += F("\n");

    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }

    server.send(404, F("text/plain"), message);
}

static void wifi_module_failed()
{
    Serial.println();
    Serial.println(F("Communication with WiFi module failed!"));
    while (true)
    {
        delay(1500);
        Serial.println(F("Communication with WiFi module failed!"));
    };
}