
//DHT11
#include <dht11.h>
dht11 DHT11;
#define DHT11PIN 13

//Flam Sensor
#define flamPin 0
int flam;

//motion sensor
int pin = 12;
int value = 0;

//Smoke & Gas sensor
#define smokePin 16
#define gasPin 14
int gas, smoke;

//Soil sensor
#define soilPin A0

//RFID sensor
#include <rdm6300.h>
#define RDM6300_RX_PIN 2

Rdm6300 rdm6300;
String key;
String masterKey = "4208420";
String card_no = "";

//i2c
#include <Wire.h>

//flag
int card_flag = 0, motion_flag = 0, pump_flag = 0, smoke_flag = 0, gas_flag = 0;
int flam_flag = 0;

//wifi
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#define FIREBASE_HOST "low-cast-iot-home-automation.firebaseio.com"
#define FIREBASE_AUTH "Xc7vmeakCLEC49hdWiH8a6pfX2uaHAJIBK3vqRSv"
#define WIFI_SSID "rezvee"
#define WIFI_PASSWORD "23456789"


//firebase variabel

String f_fan, f_pump, f_redLed, f_whiteLed;
int f_whiteLedFlag = 0;
int i = 0, j = 0, k = 0, l = 0;

void setup()
{
  Serial.begin(9600);

  //i2c
//  Wire.begin(5, 4);

  //RFID sensor
//  rdm6300.begin(RDM6300_RX_PIN);
//  Serial.println("\nPlace RFID tag near the rdm6300...");

  //flam sensor
  pinMode(flamPin, INPUT);

  //Motion Sensor
  pinMode(pin, INPUT);

  //Smoke & Gas Sensor
  pinMode(gasPin, INPUT);
  pinMode(smokePin, INPUT);

  //Soil sensor
  pinMode(soilPin, INPUT);

  //  wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected:");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
  
  if (Firebase.failed())
  {
    Serial.print("Firebase Error:");
    Serial.println(Firebase.error());
    firebasereconnect();
    return;
  }
  else {
    Serial.println("Firebase Worked");
  }



}

void rfidReader() {

  if (rdm6300.update())
  {
    Serial.println("New Card Found");
    key = rdm6300.get_tag_id();
    Serial.println(key);

    rdm6300.begin(RDM6300_RX_PIN);
    Serial.println("\nPlace RFID tag near the rdm6300...");

  }

  delay(10);
}


float temp, hum;
void valDHT()   //DHT11 sensor value read
{
  int chk = DHT11.read(DHT11PIN);

  temp = (float)DHT11.temperature;
  hum = (float)DHT11.humidity;

//  Serial.print("Temp: ");
//  Serial.print(temp);
//  Serial.println(" C");
//
//  Serial.print("RelF: ");
//  Serial.print(hum);
//  Serial.println(" %");
//
//  delay(2000);
}


void valFlam()//Flam Sensor
{
  flam = digitalRead(flamPin);
  //Serial.print("Flam Value= ");
  //Serial.println(flam);
  if (flam == 0)
  {
    // Serial.println("Flame Detected ");
    Wire.beginTransmission(8);
    Wire.write("rLedOn");
    Wire.endTransmission();
    delay(1000);
    flam_flag = 0;
  }
  if (flam == 1)
  {
    if (flam_flag == 0)
    {
      //Serial.println("Flame not Detected ");
      Wire.beginTransmission(8);
      Wire.write("rLedOff");
      Wire.endTransmission();
      delay(1000);
      flam_flag++;
    }
  }

}


void valMotion()//motion
{

  value = digitalRead(pin);

  if (value == HIGH) {
    Serial.println("Motion Detected!");
    Wire.beginTransmission(8);
    Wire.write("wLedOn");
    Wire.endTransmission();
    delay(1000);
    motion_flag = 0;

  } else {
     Serial.println("Motion Ende!");
    if (motion_flag == 0 && f_whiteLedFlag == 0) {
      //Serial.println("Motion Ende!");
      Wire.beginTransmission(8);
      Wire.write("wLedOff");
      Wire.endTransmission();
      delay(1000);
      motion_flag++;
    }

  }
}

int gas_value, smoke_value;
void valSG()
{
  gas = digitalRead(gasPin);
  Serial.print("Gas= ");
  Serial.println(gas);
  smoke = digitalRead(smokePin);
  Serial.print("smoke= ");
  Serial.println(smoke);
  delay(100);

  if (gas == 0)
  {
    gas_value = 1;
    //Serial.println("Gas Detected");
    Wire.beginTransmission(8);
    Wire.write("rLedOn");
    Wire.endTransmission();
    delay(1000);
    gas_flag = 0;
  }
  else
  {
    gas_value = 0;

    if (gas_flag == 0)
    {
      // Serial.println("No Gas Detected");
      Wire.beginTransmission(8);
      Wire.write("rLedOff");
      Wire.endTransmission();
      delay(1000);
      gas_flag++;
    }
  }

  if (smoke == 0)
  {
    smoke_value = 1;
    //Serial.println("Somke Detected");
    Wire.beginTransmission(8);
    Wire.write("rLedOn");
    Wire.endTransmission();
    delay(1000);
    smoke_flag = 0;
  }
  else
  {
    smoke_value = 0;

    if (smoke_flag == 0)
    {
      //Serial.println("No Somke Detected");
      Wire.beginTransmission(8);
      Wire.write("rLedOff");
      Wire.endTransmission();
      delay(1000);
      smoke_flag++;
    }
  }

}
int sv;
void valSoil()
{
  sv = analogRead(soilPin);
  Serial.println(sv);

  if ( sv <= 570  )
  {
    if (pump_flag == 1) {
      Serial.println("No Need Water");
      Wire.beginTransmission(8);
      Wire.write("pumpOff");
      Wire.endTransmission();
      delay(1000);
      pump_flag = 0;
    }
  }
  else
  {
    Serial.println("Need Water");
    Wire.beginTransmission(8);
    Wire.write("pumpOn");
    Wire.endTransmission();
    delay(1000);
    pump_flag = 1;
  }

  delay(200);
}


//Checking RFID card No.
void checkKey()
{
  
  if (key == card_no)
  {
    Wire.beginTransmission(8);
    Wire.write("doorOpen");
    Wire.endTransmission();
    delay(3000);
    Serial.println("Enter as Owner");
    key = "";
    card_flag = 0;
  }
  else if (key == masterKey)
  {
    Wire.beginTransmission(8);
    Wire.write("doorOpen");
    Wire.endTransmission();
    delay(2000);
    Serial.println("Enter as Admin");
    key = "";
    card_flag = 0;
  }
  else
  {
    if (card_flag == 0)
    {
      Wire.beginTransmission(8);
      Wire.write("doorClose");
      Wire.endTransmission();
      delay(2000);

      card_flag++;
    }
  }
}


//firebase reconnect
void firebasereconnect()
{
  Serial.println("Trying to reconnect");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}


void firebaseRead()
{
  card_no = String(Firebase.getInt("/Flats/1/rfid/"));
  f_fan = Firebase.getString("/Flats/1/switch/fan/");
  f_pump = Firebase.getString("/Flats/1/switch/pump/");
  f_redLed = Firebase.getString("/Flats/1/switch/redled/");
  f_whiteLed = Firebase.getString("/Flats/1/switch/whiteled/");

  Serial.print("card_no= ");
  Serial.println(card_no);
  Serial.print("Pump= ");
  Serial.println(f_pump);
  Serial.print("Red= ");
  Serial.println(f_redLed);
  Serial.print("White Led= ");
  Serial.println(f_whiteLed);

  //fan
  if (f_fan == "on" && i == 0)
  {
    Serial.println("fan on");
    Wire.beginTransmission(8);
    Wire.write("fanOn");
    Wire.endTransmission();
    delay(1000);
    i = 1;
  }
  if (f_fan == "off" && i == 1)
  {
    Serial.println("fan off");
    Wire.beginTransmission(8);
    Wire.write("fanOff");
    Wire.endTransmission();
    delay(1000);
    i = 0;
  }

  //pump
  if (f_pump == "on" && j == 0)
  {
    Serial.println("pump on");
    Wire.beginTransmission(8);
    Wire.write("pumpOn");
    Wire.endTransmission();
    delay(1000);
    j = 1;
  }
  if (f_pump == "off" && j == 1)
  {
    Serial.println("pump off");
    Wire.beginTransmission(8);
    Wire.write("pumpOff");
    Wire.endTransmission();
    delay(1000);
    j = 0;
  }

  //red light
  if (f_redLed == "on" && k == 0)
  {
    Serial.println("Red Led on");
    Wire.beginTransmission(8);
    Wire.write("rLedOn");
    Wire.endTransmission();
    delay(1000);
    k = 1;
  }
  if (f_redLed == "off" && k == 1)
  {
    Serial.println("Red Led off");
    Wire.beginTransmission(8);
    Wire.write("rLedOff");
    Wire.endTransmission();
    delay(1000);
    k = 0;
  }

  //white light
  if (f_whiteLed == "on" && l == 0)
  {
    Serial.println("white Led on");
    Wire.beginTransmission(8);
    Wire.write("wLedOn");
    Wire.endTransmission();
    delay(1000);
    l = 1;
    f_whiteLedFlag = 1;
  }
  if (f_whiteLed == "off" && l == 1)
  {
    Serial.println("White Led off");
    Wire.beginTransmission(8);
    Wire.write("wLedOff");
    Wire.endTransmission();
    delay(1000);
    l = 0;
    f_whiteLedFlag = 0;
  }

}


void firebaseSetData() {
  Firebase.setFloat("/Flats/1/temperature/", temp);
  Firebase.setFloat("/Flats/1/humidity/", hum);
  Firebase.setInt("/Flats/1/flam/", flam);
  Firebase.setInt("/Flats/1/soil/", sv);
  Firebase.setInt("/Flats/1/gas/", gas_value);
  Firebase.setInt("/Flats/1/smoke/", smoke_value);
}

void loop()
{

  if (Firebase.failed())
  {
    Serial.print("setting number failed:");
    Serial.println(Firebase.error());
    firebasereconnect();
    return;
  }

  firebaseRead();
  

//  valDHT();
//  valFlam();
//  valMotion();
//  valSG();
//  valSoil();
//  rfidReader();
//  checkKey();
//
//  firebaseSetData();

}
