#include <main.h>
#include <wifi_secrets.h>
#include <AsyncTCP.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include "SPIFFS.h"
#include <defines.h>
#include <controller.h>
#include <fanhandler.h>
#include <devicehandler.h>

fanconfig inconfig = {
    .sensorThreshold = fanInput_THRESHOLD,
    .pwmPin = fanInput_PWM_PIN,
    .sensorPin = fanInput_SENS_PIN};

fanconfig outconfig = {
    .sensorThreshold = fanOutput_THRESHOLD,
    .pwmPin = fanOutput_PWM_PIN,
    .sensorPin = fanOutput_SENS_PIN};

fanhandlerconfig config = {
    .InFan = inconfig,
    .OutFan = outconfig};

FanHandler fans(config);
DeviceHandler devices;

controllerconfig cconfig = {
    .fan_off_temp = 35,
    .fan_max_temp = 50};

Controller ctr(&cconfig, &fans, &devices);
AsyncWebServer server(80);

String get_table()
{
  /*
    std::string table;
    table.clear();
    table.append("<table style='width:100%'>");
    table.append("<tr>");
    table.append("<th scope='col'>&nbsp;</th>");
    for (auto const &x : data)
    {
      table += "<th scope='col'>" + x.first + "</th>";
    }
    for (size_t i = 0; i < ; ++i)
    {
      table.append("</tr><tr>");
      table.append("<th scope='row'>");
      table.append(String(i).c_str());
      table.append("</th>");
      for (auto const &sv : data)
      {
        table.append("<td>");
        if (i < sv.second.size())
        {
          table.append(String(sv.second.at(i)).c_str());
        }
        else
        {
          table.append("-");
        }
        table.append("</td>");
      }
    }
    table.append("</table></br>");
    return table.c_str(); */
  return String();
}

String getDate()
{
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  int day = hr / 24;
  char date[32];
  snprintf(date, 32 - 1, "%d d %02d:%02d:%02d", day, hr % 24, min % 60, sec % 60);
  return String(date);
}

std::string deviceDataToJson(const DeviceData &data)
{
  std::string json = "{\n";
  json += "  \"name\": \"" + data.name + "\",\n";
  json += "  \"lastUpdate\": \"" + data.lastUpdate + "\",\n";
  json += "  \"averageTemperature\": " + std::to_string(data.tempAvg.average) + "\n";
  json += "}";
  return json;
}

std::string deviceNameToJson(const std::list<std::string> &data)
{
  std::string json = "[";
  for (auto it = data.begin(); it != data.end(); ++it)
  {
    json += "\"" + *it + "\"";
    if (std::next(it) != data.end())
    {
      json += ",";
    }
  }
  json += "]";
  return json;
}

String root_processor(const String &var)
{
  if (var == "DATE")
  {
    return getDate();
  }
  else if (var == "AVG_TEMP")
  {
    return String(devices.getAvgTemp());
  }
  else if (var == "MAX_TEMP")
  {
    return String(devices.getMaxTemp());
  }
  else if (var == "MIN_TEMP")
  {
    return String(devices.getMinTemp());
  }
  else if (var == "CHECKED")
  {
    if (ctr.getAutomode())
    {
      return String("checked='checked'");
    }

    return "";
  }
  else if (var == "SETINPUTFAN_SPEED")
  {
    return String(ctr.getInputFanSpeed());
  }
  else if (var == "SETOUTPUTFAN_SPEED")
  {
    return String(ctr.getOutputFanSpeed());
  }
  else if (var == "INPUTFAN_SPEED")
  {
    return String(fans.getInFanSpeed());
  }
  else if (var == "OUTPUTFAN_SPEED")
  {
    return String(fans.getOutFanSpeed());
  }

  return String();
}

void handle_post_speed(AsyncWebServerRequest *request)
{
  if (request->hasArg("device") && request->hasArg("temperature") && request->hasArg("timestamp"))
  {
    std::string device = request->arg("device").c_str();
    float temperature = request->arg("temperature").toFloat();
    std::string timestamp = request->arg("timestamp").c_str();
    devices.addOrUpdate(device, temperature, timestamp);
    request->send(202);
    return;
  }
  request->send(400);
}

void handle_get_device(AsyncWebServerRequest *request)
{
  auto deviceNames = devices.getDeviceNames();
  auto json = deviceNameToJson(deviceNames).c_str();
  request->send(200, "application/json", String(json));
}

void handle_get_one_device(AsyncWebServerRequest *request)
{
  if (request->hasArg("device"))
  {
    std::string deviceName = request->arg("device").c_str();
    auto entry = devices.getDeviceData(deviceName);
    auto json = deviceDataToJson(entry).c_str();
    request->send(200, "application/json", String(json));
    return;
  }

  request->send(404, "text/plain", "DUMM");
}

void handle_delete_one_device(AsyncWebServerRequest *request)
{
  if (request->hasArg("device"))
  {
    std::string deviceName = request->arg("device").c_str();
    devices.deleteOne(deviceName);
    request->send(200);
    return;
  }

  request->send(404, "text/plain", "DUMM");
}

void handle_post_config(AsyncWebServerRequest *request)
{

  if (request->hasArg("automode"))
  {
    bool autoMode = request->arg("automode") == "on";
    ctr.setAutomode(autoMode);
  }
  else
  {
    ctr.setAutomode(false);
  }

  if (request->hasArg("input_fan") && request->hasArg("output_fan"))
  {
    int inputFanSpeed = request->arg("input_fan").toInt();
    int outputFanSpeed = request->arg("output_fan").toInt();
    ctr.setFanSpeed(inputFanSpeed, outputFanSpeed);
  }
  else
  {
    request->send(404, "text/plain", "DUMM2");
  }

  request->redirect("/", 303);
}

void handle_notfound(AsyncWebServerRequest *request)
{
  String message = F("File Not Found\n\n");

  message += F("URI: ");
  message += request->url();
  message += F("\nMethod: ");
  message += request->method();
  message += F("\nArguments: ");
  message += request->args();
  message += F("\n");

  for (uint8_t i = 0; i < request->args(); i++)
  {
    message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
  }

  request->send(404, "text/plain", message);
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  delay(200);
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  WiFi.setHostname("ClusterControllerV3");
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.setSleep(false);
  WiFi.begin(SSID, PASS);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("WiFi Failed!\n");
    return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/site.html", "text/html", false, root_processor); });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/style.css", "text/css"); });
  server.on("/api/speed", HTTP_POST, handle_post_speed);
  server.on("/api/speed", HTTP_GET, [](AsyncWebServerRequest *request) {});
  server.on("/api/device", HTTP_GET, handle_get_one_device);
  server.on("/api/device", HTTP_DELETE, handle_get_one_device);
  server.on("/api/devices", HTTP_GET, handle_get_device);
  server.on("/api/config", HTTP_POST, handle_post_config);
  server.onNotFound(handle_notfound);
  server.begin();
  ctr.begin();
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  ctr.handle();
}