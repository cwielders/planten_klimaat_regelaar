#include <DHT.h>
#include <DHT_U.h>

//#define DHTPIN1 2
//#define DHTPIN2 3
//#define DHTPIN3 4
//#define DHTPIN4 5

//DHT dht[] = {
//  {DHTPIN111, DHT22},
//  {DHTPIN112, DHT22},
//  {DHTPIN113, DHT22},
//  {DHTPIN114, DHT22},
//};
#define DHTPIN1 2
#define DHTTYPE DHT22


//Variables air humidity and temp
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value
//Variables pot humidity
int val = 0; //value for storing moisture value 
int soilsensorPin1 = A0;//Declare a variable for the soil moisture sensor 
int soilPower1 = 3;//Variable for Soil moisture Power
//Variables light sensor
int lightsensorPin1 = A1; // select the input pin for the potentiometer

class LichtSensor {

    int pin;
    float rawRange = 1024;
    float logRange = 5.0;

    public:
    Lichtensor(int pin) {
        pin = pin;
        analogReference(EXTERNAL); 
    }

    // read the raw value from the light sensor:
    String readRawValue() {
        int rawValue = analogRead(pin);
        return(rawValue);
    }

    // read the raw value from the light sensor:
    String readLogValue() {
        float logLux = rawValue * logRange / rawRange;
        int luxValue = pow(10, logLux);
        return(luxValue);
    }

}

class SoilHumiditySensor {
   
    int pin;
    int powerPin;
  
   
    public:
    SoilHumiditySensor(int pin, int powerPin) {
        pin = pin;
        powerPin = powerPin;
        pinMode(powerPin, OUTPUT);//Set D2 as an OUTPUT
        digitalWrite(powerPin, LOW);//Set to LOW so no power is flowing through the sensor
    }

    // read the raw value from the soil sensor:
    String readValue() {
        digitalWrite(powerPin, HIGH);//turn D2 "On"
        delay(10);//wait 10 milliseconds 
        int soilmoisture = analogRead(pin);//Read the SIG value form sensor 
        digitalWrite(powerPin, LOW);//turn D7 "Off"
        return(soilmoisture);
    } 

}

class LuchtVochtigheidTemperatuurSensor {

    DHT dht;

    public:
    LuchtVochtigheidTemperatuurSensor(int pin, int type) {
    dht(pin, type);
    }
   
    String readTempValue() {
       return(dht.readTemperature());
    }

    String readHumidityValue() {
        return(dht.readHumidity());    //Print temp and humidity values to serial monitor
    }
}

class Plantenbak {
  
    SoilHumiditySensor soilHumiditySensor;
    LuchtVochtigheidTemperatuurSensor luchtVochtigheidTemperatuurSensor;
    LichtSensor lichtSensor;

    public:
    Plantenbak(byte lightSensorPin, byte soilSensorPin, byte soilSensorPower, byte airHumTempSensorPin) {
        
        lightSensorTo = Lightsensorpin;
        attachsoilSensorTo = soilSensorpin;
        attachsoilsensorpowerTo = soilSensorpower;
        attachairhumtempSensorTo = airhumtempSensorpin; 
        soilHumiditySensor(attachsoilSensorTo, attachsoilsensorpowerTo);
        luchtVochtigheidTemperatuurSensor(attachairhumtempSensorTo,DHT22);
        lichtSensor(lightSensorTo);
    }
    
    void setup() {
    
       luchtVochtigheidTemperatuurSensor.begin();
      
    }

    void loop() {

        Serial.print("Humidity: ");
        Serial.print(luchtVochtigheidTemperatuurSensor.readHumidityValue);
        Serial.print(" %, Temp: ");
        Serial.print(luchtVochtigheidTemperatuurSensor.readTempValue);
        Serial.println(" Celsius");
        
        // read the raw value from the soil sensor:
        digitalWrite(soilPower1, HIGH);//turn D2 "On"
        delay(10);//wait 10 milliseconds 
        Serial.print("Soil Moisture = ");  
        Serial.println(soilHumiditySensor.readValue);
               
        // read the raw value from the light sensor:
       
        Serial.print("Raw = ");
        Serial.print(lichtSensor.readRawValue);
        
        Serial.print(" - Lux = ");
        Serial.println(lichtSensor.readLogValue);
    }

};

Plantenbak plantenbak1(lightsensorPin1,soilsensorPin1,soilPower1, DHTPIN1);
  
void setup()
{
    Serial.begin(9600);
    plantenbak1.setup();
}

void loop()
{
    delay(3000);
    plantenbak1.loop();
}
