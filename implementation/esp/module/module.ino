#include <WiFi.h>
#include <math.h>
#include <ArduinoHttpClient.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <EEPROM.h>

//TEMPERATURE
#define GPIO A0
#define SERIESRESISTOR 10000
#define NOMINAL_RESISTANCE 10000
#define NOMINAL_TEMPERATURE 20
#define BCOEFFICIENT 3950

//DEEP SLEEP
#define uS_TO_S_FACTOR  1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP   10          /* Time ESP32 will go to sleep (in seconds) */

//BLE
#define SERVICE_UUID    "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define TOKEN_CHAR_UUID "60a5dddd-72a9-4869-9226-152453166483"
#define SSID_CHAR_UUID  "0e118ace-689e-4d42-a235-07ed47d935c9"
#define PASS_CHAR_UUID  "5e464100-2958-4f05-b357-b3a832e16d1a"
#define HOST_CHAR_UUID  "48b5aa1a-dff5-4aad-9fd7-6c172c855147"
#define ID_CHAR_UUID    "22d30bae-2fab-472f-80a3-e575eb42f240"


//EEPROM
#define EEPROM_SIZE 1
#define STATE_ADDR 0
#define ID_ADDR 5
#define TOKEN_ADDR 10
#define SSID_ADDR 50
#define PASS_ADDR 100
#define HOST_ADDR 150

int state_global = 0;

const int httpPort = 8080;

void wifi_setup(){
    const char *ssid = readEEPROMChar(SSID_ADDR);
    const char *password = readEEPROMChar(PASS_ADDR);

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
    float ADCvalue;
    float Resistance;
    float steinhart;

    ADCvalue = analogRead(GPIO);
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
    String host = readEEPROM(HOST_ADDR);

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
    String id =  readEEPROM(ID_ADDR);
    String token = readEEPROM(TOKEN_ADDR);
    String temp = String(temperature);


    return String("{\"id\": ") + id + String(", \"temperature\": ") + temp + String(", \"token\": \"") + token + String("\"}");
}

String readEEPROM(int address) {
    String data = "";
    byte temp;

    temp = EEPROM.read(address);
    while ((int)temp != 10){
      data += String(temp);
      ++address;
      temp = EEPROM.read(address);
    }
    return data;
}

const char* readEEPROMChar(int address) {
    std::string data = "";
    byte temp;

    temp = EEPROM.read(address);
    while ((int)temp != 10){
      data.append(temp, 1);
      ++address;
      temp = EEPROM.read(address);
    }
    return data.c_str();
}

void deepSleep() {
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
    Serial.println("Going to sleep now");
    Serial.flush(); 
    esp_deep_sleep_start();
}

void writeAndLogEEPROM(std::string value, int address){
  if (value.length() > 0) {
    Serial.println("*********");
    Serial.print("New value: ");
    for (int i = 0; i < value.length(); i++) {
      Serial.print(value[i]);
      EEPROM.write(address + i, value[i]);
    }
    EEPROM.write(address + value.length(), 10);
    EEPROM.commit();
    ++state_global;
    Serial.println();
    Serial.println("*********");
  }
}

class TokenCallback: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      writeAndLogEEPROM(value, TOKEN_ADDR);
    }
};

class SsidCallback: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      writeAndLogEEPROM(value, SSID_ADDR);
    }
};

class PasswordCallback: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      writeAndLogEEPROM(value, PASS_ADDR);
    }
};

class HostCallback: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      writeAndLogEEPROM(value, HOST_ADDR);
    }
};

class IdCallback: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      writeAndLogEEPROM(value, ID_ADDR);
    }
};

void ble_init() {
  BLEDevice::init("MyESP32");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *tokenCharacteristic = pService->createCharacteristic(
                                         TOKEN_CHAR_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  tokenCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  tokenCharacteristic->setCallbacks(new TokenCallback());
  //tokenCharacteristic->setValue("Token");
 
  BLECharacteristic *ssidCharacteristic = pService->createCharacteristic(
                                         SSID_CHAR_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  ssidCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  ssidCharacteristic->setCallbacks(new SsidCallback());
  //ssidCharacteristic->setValue(ssid);

  BLECharacteristic *passCharacteristic = pService->createCharacteristic(
                                         PASS_CHAR_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  passCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  passCharacteristic->setCallbacks(new PasswordCallback());
  //passCharacteristic->setValue(password);
  
  BLECharacteristic *hostCharacteristic = pService->createCharacteristic(
                                         HOST_CHAR_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  hostCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  hostCharacteristic->setCallbacks(new HostCallback());
  //hostCharacteristic->setValue(host);

  BLECharacteristic *idCharacteristic = pService->createCharacteristic(
                                         ID_CHAR_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  idCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
  idCharacteristic->setCallbacks(new HostCallback());
  //idCharacteristic->setValue(id);

  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  BLESecurity *pSecurity = new BLESecurity();
  pSecurity->setStaticPIN(123456);
}

void begin_sensor(){
    float temperature; 
    temperature = readTemp();
    wifi_setup();
    sendReading(temperature);
    deepSleep();
}


void setup() {
    int state;
    Serial.begin(115200);
    EEPROM.begin(EEPROM_SIZE);
    state = (int) EEPROM.read(STATE_ADDR);
Serial.println(state);
    if (state == 5) {
      begin_sensor();
    }
    EEPROM.write(STATE_ADDR, 0);
    EEPROM.commit();
    ble_init();
}

void loop() {

  if (state_global == 5) {
    EEPROM.write(STATE_ADDR, 5);
    EEPROM.commit();
    begin_sensor();
  }
  delay(2000);
}
