#include "WiFi.h"
#include "time.h"
#include "ESPAsyncWebServer.h"

#include "HtmlIndex.h"

#define n_dias 7
#define n_tiempos 10
#define n_salidas 3

String ssid = "Wifi4us";
String pswd = "TLWA830RE";

const char* ntp_server = "pool.ntp.org";
const long gmt_offset_sec = -6 * 3600;
const int daylight_offset_sec = 0;

int tabla_tiempos[n_dias][n_tiempos][2 + n_salidas];

AsyncWebServer server(80);
struct tm tm_update_time;
struct tm tm_now;

void setup() {
  tm_update_time.tm_hour = 6;
  tm_update_time.tm_min = 0;
  tm_update_time.tm_sec = 0;

  Serial.begin(115200);

  Serial.println("");
  Serial.print("Conectando a red WiFi");

  WiFi.begin(ssid, pswd);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("Conectado a la red " + ssid + "\n");
  Serial.println("IP: " + WiFi.localIP().toString() + "\n");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", HTML_INDEX);
  });

  server.on("/tiempos", HTTP_GET, [](AsyncWebServerRequest *request) {
    String str_tiempos = "[";

    for (int d = 0; d < n_dias; d++) {
      str_tiempos += "[";

      for (int t = 0; t < n_tiempos; t++) {
        str_tiempos += "{\"hora\":" + String(tabla_tiempos[d][t][0]) + ",\"minuto\":" + String(tabla_tiempos[d][t][1]) + ",\"salidas\":" + "[";

        for (int s = 0; s < n_salidas; s++) {
          str_tiempos += String(tabla_tiempos[d][t][2 + s]);

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

  server.begin();

  updateSystemTime();
}

void loop() {
  time_t now = time(NULL);
  tm_now = *localtime(&now);

  checkTimeUpdate();

  Serial.println(&tm_now);

  delay(1000);
}

void updateSystemTime() {
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
