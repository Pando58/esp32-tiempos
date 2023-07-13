#include "WiFi.h"
#include "time.h"

String ssid = "Wifi4us";
String pswd = "TLWA830RE";

const char* ntp_server = "pool.ntp.org";
const long gmt_offset_sec = -6 * 3600;
const int daylight_offset_sec = 0;

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
