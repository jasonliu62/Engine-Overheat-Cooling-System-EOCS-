#include <Arduino.h>
#include <HttpClient.h>
#include <WiFi.h>
#include <inttypes.h>
#include <stdio.h>

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "DHT20.h"

DHT20 DHT;

int LED = 26;

int ENA = 13;
int motorPin1 = 32;
int motorPin2 = 33;

const int frequencey = 500;
const int pwm_channel = 0;
const int resolution = 8;

uint8_t count = 0;

void startMotor();
void stopMotor();

char ssid[50]; // your network SSID (name)
char pass[50]; // your network password (use for WPA, or use
// as key for WEP)

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30 * 1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;
void nvs_access()
{
  // Initialize NVS
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
      err == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
  // Open
  Serial.printf("\n");
  Serial.printf("Opening Non-Volatile Storage (NVS) handle... ");
  nvs_handle_t my_handle;
  err = nvs_open("storage", NVS_READWRITE, &my_handle);
  if (err != ESP_OK)
  {
    Serial.printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
  }
  else
  {
    Serial.printf("Done\n");
    Serial.printf("Retrieving SSID/PASSWD\n");
    size_t ssid_len;
    size_t pass_len;
    err = nvs_get_str(my_handle, "ssid", ssid, &ssid_len);
    err |= nvs_get_str(my_handle, "pass", pass, &pass_len);
    switch (err)
    {
    case ESP_OK:
      Serial.printf("Done\n");
      // Serial.printf("SSID = %s\n", ssid);
      // Serial.printf("PASSWD = %s\n", pass);
      break;
    case ESP_ERR_NVS_NOT_FOUND:
      Serial.printf("The value is not initialized yet!\n");
      break;
    default:
      Serial.printf("Error (%s) reading!\n", esp_err_to_name(err));
    }
  }
  // Close
  nvs_close(my_handle);
}

void setup()
{
  pinMode(ENA, OUTPUT);
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);

  ledcSetup(pwm_channel, frequencey, resolution);
  ledcAttachPin(ENA, pwm_channel);

  Serial.begin(9600);
  delay(1000);

  nvs_access();
  // We start by connecting to a WiFi network
  delay(1000);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());

  Serial.println(__FILE__);
  Serial.print("DHT20 LIBRARY VERSION: ");
  Serial.println(DHT20_LIB_VERSION);
  Serial.println();

  Wire.begin();
  DHT.begin(); //  ESP32 default pins 21 22

  delay(1000);
}

void loop()
{
  if (millis() - DHT.lastRead() >= 1000)
  {
    //  READ DATA
    uint32_t start = micros();
    int status = DHT.read();
    uint32_t stop = micros();

    if ((count % 10) == 0)
    {
      count = 0;
      Serial.println();
      Serial.println("Type\tHumidity (%)\tTemp (°C)\tTime (µs)\tStatus");
    }
    count++;

    Serial.print("DHT20 \t");
    //  DISPLAY DATA, sensor has only one decimal.
    Serial.print(DHT.getHumidity(), 1);
    Serial.print("\t\t");
    Serial.print(DHT.getTemperature(), 1);
    Serial.print("\t\t");
    Serial.print(stop - start);
    Serial.print("\t\t");
    switch (status)
    {
    case DHT20_OK:
      Serial.print("OK");
      break;
    case DHT20_ERROR_CHECKSUM:
      Serial.print("Checksum error");
      break;
    case DHT20_ERROR_CONNECT:
      Serial.print("Connect error");
      break;
    case DHT20_MISSING_BYTES:
      Serial.print("Missing bytes");
      break;
    case DHT20_ERROR_BYTES_ALL_ZERO:
      Serial.print("All bytes read zero");
      break;
    case DHT20_ERROR_READ_TIMEOUT:
      Serial.print("Read time out");
      break;
    case DHT20_ERROR_LASTREAD:
      Serial.print("Error read too fast");
      break;
    default:
      Serial.print("Unknown error");
      break;
    }
    Serial.print("\n");
  }

  float temp = DHT.getTemperature();

  if (temp > 25)
  {
    startMotor();
    digitalWrite(LED, HIGH);
  }
  else
  {
    stopMotor();
    digitalWrite(LED, LOW);
  }

  String tempString = "/?temp=" + String(temp);
  char tempCharArray[tempString.length() + 1];
  tempString.toCharArray(tempCharArray, tempString.length() + 1);

  int err = 0;
  WiFiClient c;
  HttpClient http(c);
  err = http.get("3.147.53.196", 5000, tempCharArray, NULL);
  if (err == 0)
  {
    Serial.println("startedRequest ok");
    err = http.responseStatusCode();
    if (err >= 0)
    {
      Serial.print("Got status code: ");
      Serial.println(err);
      // Usually you'd check that the response code is 200 or a
      // similar "success" code (200-299) before carrying on,
      // but we'll print out whatever response we get
      err = http.skipResponseHeaders();
      if (err >= 0)
      {
        int bodyLen = http.contentLength();
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println();
        Serial.println("Body returned follows:");
        // Now we've got to the body, so we can print it out
        unsigned long timeoutStart = millis();
        char c;
        // Whilst we haven't timed out & haven't reached the end of the body
        while ((http.connected() || http.available()) &&
               ((millis() - timeoutStart) < kNetworkTimeout))
        {
          if (http.available())
          {
            c = http.read();
            // Print out this character
            Serial.print(c);
            bodyLen--;
            // We read something, reset the timeout counter
            timeoutStart = millis();
          }
          else
          {
            // We haven't got any data, so let's pause to allow some to
            // arrive
            delay(kNetworkDelay);
          }
        }
      }
      else
      {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    }
    else
    {
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  }
  else
  {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  http.stop();
  // And just stop, now that we've tried a download
  delay(5000);
}

void startMotor()
{
  Serial.println("Starting Motor");
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  ledcWrite(pwm_channel, 256);
}

void stopMotor()
{
  Serial.println("Stopping Motor");
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  ledcWrite(pwm_channel, 0);
}