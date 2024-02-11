// https://randomnerdtutorials.com/esp32-dc-motor-l298n-motor-driver-control-speed-direction/
// https://www.youtube.com/watch?v=IuyBaxSqQ50

// Web: https://randomnerdtutorials.com/esp32-websocket-server-sensor/
// Filesystem: https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
// #include <Arduino_JSON.h>
#include "SPIFFS.h"

#define WIFI_SSID "Honors"
#define WIFI_PSWD "pencilanderaser"
// #define WIFI_SSID UHD
// #define WIFI_PSWD pencilanderaser

#define MOTOR_PIN1 27
#define MOTOR_PIN2 26
#define ENABLE_PIN1 14

#define MOTOR_PIN3 18
#define MOTOR_PIN4 19
#define ENABLE_PIN2 33

#define PWM_FREQ 30000
#define PWM_CHANNEL 0
#define PWM_RESOLUTION 8
int dutyCycle = 255;
unsigned int moveStatus = 0; // 0: stop, 1: forward, 2: none, 3: backward, 4: none

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
// JSONVar readings;

const char* PARAM_MESSAGE = "This is a web server";

void notifyClients(String sensorReadings) {
  ws.textAll(sensorReadings);
}
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;

  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    //data[len] = 0;
    String message = (char*)data;

    // Serial.println("WS Data: " + message);

    if (strstr((char*)data, "stpup") || strstr((char*)data, "stpdown") || strstr((char*)data, "stpleft") || strstr((char*)data, "stpright")) {
      Serial.println("EXE: " + message);
      moveStatus = 0;
    }
    else if (strstr((char*)data, "strup")) {
      Serial.println("EXE: " + message);
      moveStatus = 1;
    }
    else if (strstr((char*)data, "strleft")) {
      Serial.println("EXE: " + message);
      moveStatus = 2;
    }
    else if (strstr((char*)data, "strdown")) {
      Serial.println("EXE: " + message);
      moveStatus = 3;
    }
    else if (strstr((char*)data, "strright")) {
      Serial.println("EXE: " + message);
      moveStatus = 4;
    }
    else {}

    // Check if the message is "getReadings"
    //if (strcmp((char*)data, "getReadings") == 0) {
      //if it is, send current sensor readings
      // String sensorReadings = getSensorReadings();
      // Serial.print(sensorReadings);

      // notifyClients("sensorReadings");
    //}
  }
}
void onWSEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initMotors() {
  // sets the pins as outputs:
  pinMode(MOTOR_PIN1, OUTPUT);
  pinMode(MOTOR_PIN2, OUTPUT);
  pinMode(ENABLE_PIN1, OUTPUT);

  pinMode(MOTOR_PIN3, OUTPUT);
  pinMode(MOTOR_PIN4, OUTPUT);
  pinMode(ENABLE_PIN2, OUTPUT);

  // configure LED PWM functionalitites
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ENABLE_PIN1, PWM_CHANNEL);
  ledcAttachPin(ENABLE_PIN2, PWM_CHANNEL);
}

void initWIFI() {
  /*.Initialize Server */
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PSWD);

  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi Failed!\n");
  }
}

void initWebServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    // request->send(200, "text/plain", "IP: " + WiFi.localIP().toString());
    request->send(SPIFFS, "/www/index.html", "text/html");

    // TEST
    // File file = SPIFFS.open("/test_example.txt");
    // if(!file){
    //   Serial.println("Failed to open file for reading");
    //   return;
    // }

    // Serial.println("File Content:");
    // while(file.available()){
    //   Serial.write(file.read());
    // }
    // file.close();
  });
  server.on("/base.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/www/base.css", "text/css");
  });
  server.on("/websocket.mjs", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/www/websocket.mjs", "text/javascript");
  });
  server.on("/joystick.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/www/joystick.css", "text/css");
  });
  server.on("/joystick.mjs", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/www/joystick.mjs", "text/javascript");
  });
  server.on("/main.mjs", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/www/main.mjs", "text/javascript");
  });
  server.on("/lib.math.mjs", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/www/lib.math.mjs", "text/javascript");
  });

  // Send a GET request to <IP>/get?message=<message>
  // server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
  //   String message;

  //   if (request->hasParam(PARAM_MESSAGE)) {
  //       message = request->getParam(PARAM_MESSAGE)->value();
  //   } else {
  //       message = "No message sent";
  //   }

  //   request->send(200, "text/plain", "Hello, GET: " + message);
  // });

  // Send a POST request to <IP>/post with a form field message set to <message>
  // server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
  //   String message;

  //   if (request->hasParam(PARAM_MESSAGE, true)) {
  //       message = request->getParam(PARAM_MESSAGE, true)->value();
  //   } else {
  //       message = "No message sent";
  //   }

  //   request->send(200, "text/plain", "Hello, POST: " + message);
  // });

  server.onNotFound(notFound);
  server.begin();
}

void initWebSocket() {
  ws.onEvent(onWSEvent);
  server.addHandler(&ws);
}

void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");

    return;
  }
  Serial.println("SPIFFS mounted successfully");
}

void setup() {
  Serial.begin(115200);
  // Serial.print("Testing DC Motor...");

  initSPIFFS();
  initMotors();
  initWIFI();
  initWebServer();
  initWebSocket();

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  ledcWrite(PWM_CHANNEL, dutyCycle);
}

void loop() {
  // Serial.println(moveStatus);

  switch(moveStatus) {
    case 0:
      digitalWrite(MOTOR_PIN1, LOW);
      digitalWrite(MOTOR_PIN2, LOW);
      digitalWrite(MOTOR_PIN3, LOW);
      digitalWrite(MOTOR_PIN4, LOW);
      break;
    case 1: // Up
      digitalWrite(MOTOR_PIN1, LOW);
      digitalWrite(MOTOR_PIN2, HIGH);
      digitalWrite(MOTOR_PIN3, LOW);
      digitalWrite(MOTOR_PIN4, HIGH);
      break;
    case 2: // Left
      digitalWrite(MOTOR_PIN1, LOW);
      digitalWrite(MOTOR_PIN2, HIGH);
      digitalWrite(MOTOR_PIN3, HIGH);
      digitalWrite(MOTOR_PIN4, LOW);
      break;
    case 3: // Down
      digitalWrite(MOTOR_PIN1, HIGH);
      digitalWrite(MOTOR_PIN2, LOW);
      digitalWrite(MOTOR_PIN3, HIGH);
      digitalWrite(MOTOR_PIN4, LOW);
      break;
    case 4: // Right
      digitalWrite(MOTOR_PIN1, HIGH);
      digitalWrite(MOTOR_PIN2, LOW);
      digitalWrite(MOTOR_PIN3, LOW);
      digitalWrite(MOTOR_PIN4, HIGH);
      break;
  }


  // Move DC motor forward with increasing speed
  // digitalWrite(MOTOR_PIN1, HIGH);
  // digitalWrite(MOTOR_PIN2, LOW);
  // while (dutyCycle <= 255){
  //   ledcWrite(PWM_CHANNEL, dutyCycle);
  //   Serial.print("Forward with duty cycle: ");
  //   Serial.println(dutyCycle);
  //   dutyCycle = dutyCycle + 5;
  //   delay(500);
  // }
  // dutyCycle = 200;
}
