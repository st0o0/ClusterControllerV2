#include <main.h>

WiFiWebServer server(80);

const int FAN_OFF_TEMP = 0;
const int FAN_MAX_TEMP = 15;

const size_t maxTempValues = 5;
std::map<std::string, std::vector<float>> data;
boolean checked = 0;
int setFanSpeed = 0;
int fanSpeed = 0;

int calc_temp()
{
  float result = 0;
  for (auto const &x : data)
  {
    float acc = std::accumulate(std::begin(x.second), std::end(x.second), 0.0);
    result = result + (acc / maxTempValues);
  }

  if (data.empty())
    return 0;
  return result / (double)data.size();
}

const char *get_table()
{
  static std::string table;
  table.clear();
  table.append("<table style='width:100%'>");
  table.append("<tr>");
  table.append("<th scope='col'>&nbsp;</th>");
  for (auto const &x : data)
  {
    table += "<th scope='col'>" + x.first + "</th>";
  }
  for (size_t i = 0; i < maxTempValues; ++i)
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
  return table.c_str();
}

void old_root()
{
#define BUFFER_SIZE 8192

  char temp[BUFFER_SIZE];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  int day = hr / 24;

  snprintf(temp, BUFFER_SIZE - 1, "<!DOCTYPE html>\
<html lang='de'>\
<head>\
    <meta http-equiv='refresh' content='60' />\
    <title>%s</title>\
    <style>\
        body {\
            background-color: #cccccc;\
            font-family: Arial, Helvetica, Sans-Serif;\
            Color: #000088;\
        }\
        input[type='range'] {\
            vertical-align: middle;\
        }\
        table {\
            border-spacing: 0em;\
            border: 1px solid black;\
        }\
        td,\
        th {\
            border: 1px solid black;\
        }\
        td {\
            text-align: center;\
            vertical-align: middle;\
        }\
    </style>\
</head>\
<body>\
    <h1>Hello from %s</h1>\
    <h3>running WiFiWebServer</h3>\
    <h3>on %s</h3>\
    <p>Uptime: %d d %02d:%02d:%02d</p>\
    </br>\
    <form oninput='x.value=parseInt(range.value)' method='post' action='/setfanspeed'>\
        <Label for='checkbox'>Disable auto fanspeed: </Label>\
        <input id='checkbox' type='checkbox' value='true' %s name='check'>\
        <input type='hidden' name='check' value='false'>\
        </br>\
        <Label for='range'>manual fanspeed: </Label>\
        <input id='range' type='range' min='0' max='100' value='%d' name='speed'>\
        </br>\
        </br>\
        <Label for='output'>setfanspeed: </Label>\
        <output id='output' name='x'>%d</output>\
        </br>\
        </br>\
        <input type='submit' value='set FanSpeed'>\
    </form>\
    </br>\
    </br>\
    <Label>FanSpeed: %d</Label>\
    </br>\
    </br>\
    </br>\
    %s\
</body>\
</html>",
           "BOARD_NAME", "BOARD_NAME", "SHIELD_TYPE", day, hr % 24, min % 60, sec % 60, checked ? "checked='checked'" : "", setFanSpeed, setFanSpeed, fanSpeed, get_table());

  // server.send(200, F("text/html"), F(temp));
}

void old_handle_fanspeed()
{
  // if (server.method() == HTTP_POST)
  // {
  //   setFanSpeed = server.arg("speed").toInt();
  //   std::string checkValue = server.arg("check").c_str();
  //   checked = to_bool(checkValue);
  //   server.sendHeader("Location", "/");
  //   server.send(303, "");
  // }
}

void old_handle_temp()
{
  // if (server.method() == HTTP_POST && server.hasArg("name") && server.hasArg("temp"))
  // {
  //   std::string pi = server.arg("name").c_str();
  //   float temp = server.arg("temp").toFloat();

  //   auto it = data.find(pi);
  //   if (it != data.end())
  //   {
  //     add_value(it->second, temp, maxTempValues);
  //   }
  //   else
  //   {
  //     std::vector<float> values;
  //     add_value(values, temp, maxTempValues);
  //     data[pi.c_str()] = values;
  //   }

  //   server.send(200, "text/plain", "Data received successfully");
  // }
  // else
  // {
  //   server.send(400, "text/plain", "Missing arguments or invalid input");
  // }
}

void handle_not_found()
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

void wifi_module_failed()
{
  Serial.println();
  Serial.println(F("Communication with WiFi module failed!"));
  while (true)
  {
    delay(1500);
    Serial.println(F("Communication with WiFi module failed!"));
  };
}

void webserver_setup()
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
  server.on(F("/"), []() {});
  server.on(F("/api/speed"), []() {});
  server.on(F("/api/device"), []() {});
  server.on(F("/favicon.ico"), []()
            { server.send(404, F("text/plain"), F("")); });
  server.onNotFound(handle_not_found);
  server.begin();
}

void setup()
{
  // fanconfig inconfig;
  // inconfig.pwmPin = fanInput_PWM_PIN;
  // inconfig.sensorPin = fanInput_SENS_PIN;
  // inconfig.sensorThreshold = fanInput_THRESHOLD;
  // fanconfig outconfig;
  // outconfig.pwmPin = fanOutput_PWM_PIN;
  // outconfig.sensorPin = fanOutput_SENS_PIN;
  // outconfig.sensorThreshold = fanOutput_THRESHOLD;
  // config.InFan = inconfig;
  // config.OutFan = outconfig;
  // fanhandler = FanHandler(config);
  // devicehandler = DeviceHandler();

  // server.setup(devicehandler, fanhandler);

  Serial.begin(115200);
  delay(200);
  webserver_setup();
}

void loop()
{
  if (Serial.available())
  {
    Serial.println("HALLO");
  }
}