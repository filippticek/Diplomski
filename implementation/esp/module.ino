#include <WiFi.h>
#include <math.h>
#include <ArduinoHttpClient.h>

#define SERIESRESISTOR 10000
#define NOMINAL_RESISTANCE 10000
#define NOMINAL_TEMPERATURE 20
#define BCOEFFICIENT 3950

#define DEBUG true

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10          /* Time ESP32 will go to sleep (in seconds) */

const int device_id = 1;
const int user_id = 1;
const char* token = "secureToken";
const int gpio = A0;

const char* ssid     = "tofu_2.4";
const char* password = "mariniransumakom";

const char* host = "192.168.0.23";
const int httpPort = 8080;


void wifi_setup(){
    
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
    }
   Serial.println("");
   Serial.println("WiFi connected");
   Serial.println("IP address: ");
   Serial.println(WiFi.localIP());
}

float readTemp() {
    /*double Temp;
    double ADCvalue;
    ADCvalue = analogRead(gpio);
    Serial.print("Analogue reading: ");
    Serial.println(ADCvalue);
    Temp = log(10000.0/(4096.0/ADCvalue-1)); // for pull-up configuration
    Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
    Temp = Temp - 273.15;            // Convert Kelvin to Celcius
    Serial.print("TEMP: ");
    Serial.println(Temp);
    return Temp;
    */
    float ADCvalue;
    float Resistance;
    float steinhart;

    ADCvalue = analogRead(gpio);
    Serial.print("Analog value ");
    Serial.print(ADCvalue);
    Serial.print(" = ");
    
    //convert value to resistance
    Resistance = (4095 / ADCvalue) - 1;
    Resistance = SERIESRESISTOR / Resistance;
    Serial.print(Resistance);
    Serial.println(" Ohm");
    
    steinhart = Resistance / NOMINAL_RESISTANCE; // (R/Ro)
    steinhart = log(steinhart); // ln(R/Ro)
    steinhart /= BCOEFFICIENT; // 1/B * ln(R/Ro)
    steinhart += 1.0 / (NOMINAL_TEMPERATURE + 273.15); // + (1/To)
    steinhart = 1.0 / steinhart; // Invert
    steinhart -= 273.15; // convert to C
    
    Serial.print("Temperature ");
    Serial.print(steinhart);
    Serial.println(" oC");
    return steinhart;
    
}

void sendReading(float temperature) {
    WiFiClient client;
    int err;
    String url = "/api/readings";
    String data = getJSONdata(temperature);

    Serial.print("Requesting URL: ");
    Serial.println(url);
    Serial.println(data);
   
    String contentType = "application/json";
    HttpClient hclient = HttpClient(client, host, httpPort);
    err = hclient.post(url, contentType, data);

    if (err >= 0) {
      int statusCode = hclient.responseStatusCode();
      String response = hclient.responseBody();

      Serial.print("Status code: ");
      Serial.println(statusCode);
      Serial.print("Response: ");
      Serial.println(response);
   
    } else {
      Serial.println("Destination unreachable");
    }
    hclient.stop();
}

String getJSONdata(float temperature) {
    String id =   String(device_id);
    String temp = String(temperature);
    String tkn = String(token);
    String usr_id = String(user_id);
    //String time = String(millis());

    return String("{\"device_id\": ") + id + String(", \"user_id\": ") + usr_id + String(", \"temperature\": ") + temp + String(", \"token\": \"") + String(token) + String("\"}");
}

void deepSleep() {
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
    Serial.println("Going to sleep now");
    Serial.flush(); 
    esp_deep_sleep_start();
}

void setup() {
    Serial.begin(115200);
    delay(10);
    float temperature; 
    temperature = readTemp();
    wifi_setup();
    sendReading(temperature);
    deepSleep();
}

void loop() {
}
