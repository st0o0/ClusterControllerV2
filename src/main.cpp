#include <main.h>
#include <wifi_secrets.h>
#include <AsyncTCP.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
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

Controller ctr(cconfig, fans, devices);
AsyncWebServer server(80);

const char *get_table()
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
}

void handleRoot(AsyncWebServerRequest *request)
{

#define BUFFER_SIZE 8192

  char temp[BUFFER_SIZE];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  int day = hr / 24;

  snprintf(temp, BUFFER_SIZE - 1, "<!DOCTYPE html>\ <html lang='de'>\ <head>\ <meta http-equiv='refresh' content='60' />\ <title>ClusterController</title>\<style>\ body {\ background-color: #f5f5f5;\ font-family: Arial, Helvetica, sans-serif;\ color: #333;\ margin: 0;\ padding: 0;\ }\ \ header {\ background-color: #0078d7;\ color: white;\ padding: 1em;\ text-align: center;\ }\ \ main {\ padding: 2em;\ max-width: 800px;\ margin: 0 auto;\ background: white;\ border-radius: 8px;\ box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);\ }\ \ h1,\ h3 {\ margin: 0.5em 0;\ }\ \ form {\ margin: 2em 0;\ padding: 1em;\ background: #f9f9f9;\ border-radius: 8px;\ box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);\ }\ \ form label {\ display: block;\ margin-bottom: 0.5em;\ font-weight: bold;\ }\ \ form input[type='range'] {\ width: 100%;\ margin: 1em 0;\ }\ \ form input[type='submit'] {\ background-color: #0078d7;\ color: white;\ border: none;\ padding: 0.5em 1em;\ border-radius: 4px;\ cursor: pointer;\ font-size: 1em;\ }\ \ form input[type='checkbox'] {\ vertical-align: middle;\ margin-left: 10px;\ }\ \ form input[type='submit']:hover {\ background-color: #005bb5;\ }\ \ table {\ width: 100%;\ border-collapse: collapse;\ margin-top: 2em;\ }\ \ table,\ th,\ td {\ border: 1px solid #ddd;\ }\ \ th,\ td {\ text-align: center;\ padding: 0.5em;\ }\ \ th {\ background-color: #0078d7;\ color: white;\ }\ \ footer {\ margin-top: 2em;\ text-align: center;\ font-size: 0.9em;\ color: #666;\ }\ \ .legend {\ margin: 2em 0;\ padding: 1em;\ background: #f1f1f1;\ border-radius: 8px;\ box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);\ }\ \ .legend p {\ margin: 0.5em 0;\ }\ \ .speed-boxes {\ display: flex;\ justify-content: space-between;\ margin-bottom: 2em;\ gap: 1em; /* Sorgt für gleichmäßigen Abstand */\ }\ \ .speed-box {\ flex: 1; /* Wichtig für gleichmäßige Breite */\ background-color: #f0f0f0;\ border-radius: 8px;\ padding: 1em;\ text-align: center;\ box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);\ }\ \ .speed-box h4 {\ margin-top: 0;\ color: #0078d7;\ }\ \ .speed-box p {\ font-size: 2em;\ font-weight: bold;\ margin: 0.5em 0 0;\ }\ </style>\ </head>\ \ <body>\ <header>\ <h1>Cluster Controller Dashboard</h1>\ </header>\ \ <main>\ <h3>Welcome to the Cluster Controller System</h3>\ <p>Uptime: %d d %02d:%02d:%02d</p>\ \ <form method='post' action='/api/config'>\ <label for='autoMode'\ >Auto Mode:\ <input\ id='autoMode'\ type='checkbox'\ name='autoMode'\ onchange='toggleAutoMode(this)'\ />\ </label>\ \ <div id='manualControls' style='display: none'>\ <label for='inputFan'\ >Input Fan Speed:\ <output id='inputFanOutput'>%d</output>\ </label>\ <input\ id='inputFan'\ type='range'\ min='0'\ max='100'\ value='%d'\ name='inputFanSpeed'\ oninput='inputFanOutput.value = this.value'\ />\ \ <label for='outputFan'\ >Output Fan Speed:\ <output id='outputFanOutput'>%d</output>\ </label>\ <input\ id='outputFan'\ type='range'\ min='0'\ max='100'\ value='%d'\ name='outputFanSpeed'\ oninput='outputFanOutput.value = this.value'\ />\ </div>\ \ <input type='submit' value='Set Fan Speeds' />\ </form>\ \ <div class='speed-boxes'>\ <div class='speed-box'>\ <h4>Manual Speed</h4>\ <p id='manualSpeed'>%d%%</p>\ </div>\ <div class='speed-box'>\ <h4>Calculated Speed</h4>\ <p id='calculatedSpeed'>%d%%</p>\ </div>\ </div>\ \ <div class='legend'>\ <h4>Temperature Legend</h4>\ <p>Maximum Temperature: <span id='maxTemp'>%d °C</span></p>\ <p>Minimum Temperature: <span id='minTemp'>%d °C</span></p>\ <p>Average Temperature: <span id='avgTemp'>%d °C</span></p>\ </div>\ \ <footer>\ <p>&copy; 2025 Fan Control System</p>\ </footer>\ </main>\ \ <script>\ function toggleAutoMode(checkbox) {\ const manualControls = document.getElementById('manualControls');\ if (checkbox.checked) {\ manualControls.style.display = 'none';\ } else {\ manualControls.style.display = 'block';\ }\ }\ </script>\ </body>\ </html>",
           day, hr % 24, min % 60, sec % 60);

  request->send(200, "text/html", F(temp));
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
  Serial.begin(115200);
  delay(200);
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
  server.on("/", handleRoot);
  server.onNotFound(handle_notfound);
  server.begin();
  // ctr.begin();
}

void loop()
{
  // ctr.handle();
}