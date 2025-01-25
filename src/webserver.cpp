/* #include <webserver.h>
#include <wifi.h>

WiFiWebServer Webserver::server(80);

void Webserver::begin()
{
    EspSerial.begin(115200);
    WiFi.init(EspSerial);
    if (WiFi.status() == WL_NO_MODULE)
    {
        wifi_module_failed();
    }
    WiFi.sleepMode(WIFI_NONE_SLEEP);
    WiFi.setHostname("ClusterControl ler");
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
    Webserver::server.on(F("/"), handle_root);
    Webserver::server.on(F("/api/speed"), handle_api_speed);
    Webserver::server.on(F("/api/device"), handle_api_device);
    Webserver::server.on(F("/favicon.ico"), []()
              { Webserver::server.send(404, F("text/plain"), F("")); });
    Webserver::server.onNotFound(handle_notfound);
    Webserver::server.begin();
}

void Webserver::handle()
{
    Webserver::server.handleClient();
}

void Webserver::handle_root()
{
}

void Webserver::handle_api_speed()
{
}

void Webserver::handle_api_device()
{
}

void Webserver::handle_notfound()
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

    Webserver::server.send(404, F("text/plain"), message);
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
} */