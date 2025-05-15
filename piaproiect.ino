#include <ArduinoJson.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include "WiFi.h"
#include <WebServer.h>

WebServer server(80);

void handleRoot() {
  String response = "<!DOCTYPE html><html><head><title>Proiect API Disney</title><style>";
  response += "body { background-color: aqua;}";
  response += "</style></head><body><h1>Proiect API Disney</h1>";

  // Începeți cererea HTTP pentru a obține lista de episoade
  HTTPClient http;
  http.begin("http://proiectia.bogdanflorea.ro/api/disney-characters/characters");

  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      // Citirea răspunsului JSON și parsarea acestuia
      DynamicJsonDocument doc(8192);
      DeserializationError error = deserializeJson(doc, http.getString());
      if (!error) {
        // Parcurge fiecare element din json
        JsonArray data = doc.as<JsonArray>();
        for (JsonObject episode : data) {
          // Verifică dacă ID-ul, numele și URL-ul thumbnail-ului există în obiectul JSON
          if (episode.containsKey("_id") && episode.containsKey("name") && episode.containsKey("imageUrl")) {
            const char *id = episode["_id"];
            const char *name = episode["name"];
            const char *url = episode["imageUrl"];



            Serial.print("ID: ");

            Serial.println(id);

            // Construiește răspunsul HTML pentru clientul web
            response += "<h4>id: ";
            response += id;
            response += "</h4>";

            response += "<h3>Nume: ";
            response += name;
            response += "</h3>";

            response += "<img src='";
            response += url;
            response += "' style='max-width: 300px;'>";
          }
        }
      } else {
        response += "Eroare la parsarea JSON: ";
        response += error.c_str();
      }
    } else {
      response += "Eroare HTTP: ";
      response += httpCode;
    }
  } else {
    response += "Eroare la conectarea la server.";
  }
  http.end();
  response += "</body></html>";
  server.send(200, "text/html", response);
}

void setup() {
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Setup done");
  
  Serial.println("Scan start");
 
  // WiFi.scanNetworks will return the number of networks found.
  int n = WiFi.scanNetworks();
  Serial.println("Scan done");
  if (n == 0) {
      Serial.println("no networks found");
  } else {
      Serial.print(n);
      Serial.println(" networks found");
      Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
      for (int i = 0; i < n; ++i) {
          // Print SSID and RSSI for each network found
          Serial.printf("%2d",i + 1);
          Serial.print(" | ");
          Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
          Serial.print(" | ");
          Serial.printf("%4d", WiFi.RSSI(i));
          Serial.print(" | ");
          Serial.printf("%2d", WiFi.channel(i));
          Serial.print(" | ");
          switch (WiFi.encryptionType(i))
          {
          case WIFI_AUTH_OPEN:
              Serial.print("open");
              break;
          case WIFI_AUTH_WEP:
              Serial.print("WEP");
              break;
          case WIFI_AUTH_WPA_PSK:
              Serial.print("WPA");
              break;
          case WIFI_AUTH_WPA3_PSK:
              Serial.print("WPA3");
              break;
          case WIFI_AUTH_WPA2_WPA3_PSK:
              Serial.print("WPA2+WPA3");
              break;
          case WIFI_AUTH_WAPI_PSK:
              Serial.print("WAPI");
              break;
          default:
              Serial.print("unknown");
          }
          Serial.println();
          delay(10);
      }
  }
  Serial.println("");
 
  // Delete the scan result to free memory for code below.
  WiFi.scanDelete();

  // Conectare la rețea WiFi
  WiFi.begin("Telefon_Alex", "alexalex");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectare la WiFi...");
  }

  Serial.println("Conectat la rețea WiFi!");
  Serial.print("Adresă IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}
