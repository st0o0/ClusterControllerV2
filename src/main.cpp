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

String getTable()
{
  std::string table;
  table.append("<table style='width:100%; border: 1px solid black;'>\n");
  table.append("<tr>\n");
  table.append("<th scope='col'>Index</th>\n");
  table.append("<th scope='col'>Device Name</th>\n");
  table.append("<th scope='col'>Last Update</th>\n");
  for (std::size_t i = 0; i < 5; ++i)
  {
    table.append("<th scope='col'>Value " + std::to_string(i + 1) + "</th>\n");
  }

  table.append("<th scope='col'>Average</th>\n");
  table.append("</tr>\n");
  int rowIndex = 1;
  for (const auto &name : devices.getDeviceNames())
  {
    table.append("<tr>\n");
    table.append("<td>" + std::to_string(rowIndex) + "</td>\n");
    table.append("<td>" + name + "</td>\n");
    DeviceData deviceData = devices.getDeviceData(name);
    table.append("<td>" + deviceData.lastUpdate + "</td>\n");
    for (std::size_t i = 0; i < 5; ++i)
    {
      std::ostringstream valueStream;
      valueStream.precision(2);
      valueStream << std::fixed << deviceData.tempAvg.values[i];
      table.append("<td>" + valueStream.str() + "</td>\n");
    }
    std::ostringstream avgStream;
    avgStream.precision(2);
    avgStream << std::fixed << deviceData.tempAvg.average;
    table.append("<td>" + avgStream.str() + "</td>\n");

    table.append("</tr>\n");
    rowIndex++;
  }
  table.append("</table><br>\n");
  return String(table.c_str());
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

String deviceDataToJson(const DeviceData &data)
{
  std::string json = "{\n";
  json += "  \"name\": \"" + data.name + "\",\n";
  json += "  \"lastUpdate\": \"" + data.lastUpdate + "\",\n";
  json += "  \"averageTemperature\": " + std::to_string(data.tempAvg.average) + "\n";
  json += "}";
  return String(json.c_str());
}

String deviceNameToJson(const std::list<std::string> &data)
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
  return String(json.c_str());
}

String root_processor(const String &var)
{
  if (var == "DATE")
  {
    return getDate();
  }
  else if (var == "TABLE")
  {
    return getTable();
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