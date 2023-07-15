#include "WiFi.h"
#include "time.h"
#include "ESPAsyncWebServer.h"
#include "ArduinoJson.h"
#include "SPI.h"
#include "Wire.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#include "HtmlIndex.h"

#define n_dias 7
#define n_tiempos 10
#define n_salidas 3

#define OLED_W 128
#define OLED_H 64
#define OLED_RESET -1
#define OLED_SCR_ADDR 0x3C

String ssid = "Wifi4us";
String pswd = "TLWA830RE";

const char* ntp_server = "pool.ntp.org";
const long gmt_offset_sec = -6 * 3600;
const int daylight_offset_sec = 0;

int tabla_tiempos[n_dias][n_tiempos][2 + n_salidas];

AsyncWebServer server(80);
struct tm tm_update_time;
struct tm tm_now;
String ip;
Adafruit_SSD1306 display(OLED_W, OLED_H, &Wire, OLED_RESET);

void setup() {
  for (int i = 0; i < n_dias; i++) {
    for (int j = 0; j < n_tiempos; j++) {
      tabla_tiempos[i][j][0] = -1;
    }
  }

  tm_update_time.tm_hour = 6;
  tm_update_time.tm_min = 0;
  tm_update_time.tm_sec = 0;

  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_SCR_ADDR)) { // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  Serial.println("");
  Serial.print("Conectando a red WiFi");
  display.clearDisplay();
  display.setCursor(0, 30);
  display.println(F("Conectando a WiFi..."));
  display.display();

  WiFi.begin(ssid, pswd);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  ip = WiFi.localIP().toString();

  Serial.println("");
  Serial.println("Conectado a la red " + ssid + "\n");
  Serial.println("IP: " + ip + "\n");
  display.clearDisplay();
  oledPrintNetworkInfo();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("[GET] /");

    request->send(200, "text/html", HTML_INDEX);
  });

  server.on("/tiempos", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("[GET] /tiempos");

    String str_tiempos = "[";

    for (int d = 0; d < n_dias; d++) {
      str_tiempos += "[";

      for (int t = 0; t < n_tiempos; t++) {
        str_tiempos += "{\"hora\":" + String(tabla_tiempos[d][t][0]) + ",\"minuto\":" + String(tabla_tiempos[d][t][1]) + ",\"salidas\":" + "[";

        for (int s = 0; s < n_salidas; s++) {
          str_tiempos += tabla_tiempos[d][t][2 + s] == 1 ? "true" : "false";

          if (s < n_salidas - 1) {
            str_tiempos += ",";
          }
        }

        str_tiempos += "]}";

        if (t < n_tiempos - 1) {
          str_tiempos += ",";
        }
      }

      str_tiempos += "]";

      if (d < n_dias - 1) {
        str_tiempos += ",";
      }
    }

    str_tiempos += "]";

    request->send(200, "application/json", str_tiempos);
  });

  server.on("/tiempos", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      Serial.println("[POST] /tiempos");

      // https://arduinojson.org/v5/assistant/
      const int capacity = (n_dias * n_tiempos) * JSON_ARRAY_SIZE(2 + n_salidas) + JSON_ARRAY_SIZE(n_dias) + n_dias * JSON_ARRAY_SIZE(n_tiempos);
      StaticJsonDocument<capacity> doc;
      deserializeJson(doc, (const char*) data);

      for (int i = 0; i < n_dias; i++) {
        for (int j = 0; j < n_tiempos; j++) {
          tabla_tiempos[i][j][0] = -1;
        }
      }

      JsonArray arr_i = doc.as<JsonArray>();
      for (int i = 0; i < arr_i.size(); i++) {
        JsonVariant arr_j = arr_i[i].as<JsonArray>();
        for (int j = 0; j < arr_j.size(); j++) {
          JsonVariant arr_k = arr_j[j].as<JsonArray>();
          for (int k = 0; k < arr_k.size(); k++) {
            tabla_tiempos[i][j][k] = arr_k[k].as<int>();
          }
        }
      }

      request->send(200);
  });

  server.begin();

  updateSystemTime();
}

void loop() {
  time_t now = time(NULL);
  tm_now = *localtime(&now);

  checkTimeUpdate();

  oledPrintTime();
  oledPrintNetworkInfo();

  Serial.println(&tm_now);

  delay(1000 - (millis() % 1000));
}

void updateSystemTime() {
  display.fillRect(0, 56, 128, 10, SSD1306_BLACK);
  display.setCursor(0, 56);
  display.println(F("Actualizando hora..."));
  display.display();

  Serial.println("Actualizando hora...");

  configTime(gmt_offset_sec, daylight_offset_sec, ntp_server);

  time_t now;

  while (true) {
    time(&now);

    if (now > (2023 - 1970) * 365 * 24 * 60 * 60) {
        break;
    }
  }

  Serial.println("Hora actualizada\n");
}

void checkTimeUpdate() {
  if (
    tm_update_time.tm_hour == tm_now.tm_hour &&
    tm_update_time.tm_min == tm_now.tm_min &&
    tm_update_time.tm_sec == tm_now.tm_sec
  ) {
    updateSystemTime();
  }
}

void oledPrintNetworkInfo() {
  display.setCursor(0, 0);
  display.println(F("Conectado"));
  display.setCursor(0, 12);
  display.println(F(("SSID: " + ssid).c_str()));
  display.setCursor(0, 38);
  display.println(F(("IP: " + ip).c_str()));
  display.display();
}

void oledPrintTime() {
  String h = (tm_now.tm_hour < 10 ? "0" : "") + String(tm_now.tm_hour);
  String m = (tm_now.tm_min < 10 ? "0" : "") + String(tm_now.tm_min);
  String s = (tm_now.tm_sec < 10 ? "0" : "") + String(tm_now.tm_sec);

  display.fillRect(0, 56, 128, 10, SSD1306_BLACK);
  display.setCursor(41, 56);
  display.println(F((h + ":" + m + ":" + s).c_str()));
  display.display();
}
