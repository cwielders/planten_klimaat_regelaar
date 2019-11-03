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
    Sensor(int pin) {
        pin = pin;
        analogReference(EXTERNAL); 
    }

    // read the raw value from the light sensor:
    String readValue() {
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

    DHT *myDht;

    public:
    LuchtVochtigheidTemperatuurSensor(int pin, int type) {
        DHT dht(pin, type);
        myDht = dht;
    }
   
    String readTempValue() {
       return(dht.readTemperature());
    }

    String readHumidityValue() {
        return(dht.readHumidity());    //Print temp and humidity values to serial monitor
    }
}

class Plantenbak {
  
    SoilHumiditySensor *soilHumiditySensor;
    LuchtVochtigheidTemperatuurSensor *luchtVochtigheidTemperatuurSensor;
    LichtSensor *lichtSensor;

    public:
    Plantenbak(byte lightSensorPin, byte soilSensorPin, byte soilSensorPower, byte airHumTempSensorPin) {
        
        lichtSensor = LichtSensor LichtSensor(lightsensorpin);
        lightSensorTo = Lightsensorpin;
        attachsoilSensorTo = soilSensorpin;
        attachsoilsensorpowerTo = soilSensorpower;
        attachairhumtempSensorTo = airhumtempSensorpin;
    }
    
    void setup() {
    
        dht1.begin();
       
        
    }

    void loop() {

        // read the values from the air humidity and temeprature sensor:
        hum = dht1.readHumidity();
        temp= dht1.readTemperature();
        //Print temp and humidity values to serial monitor
        Serial.print("Humidity: ");
        Serial.print(hum);
        Serial.print(" %, Temp: ");
        Serial.print(temp);
        Serial.println(" Celsius");
        
        // read the raw value from the soil sensor:
        digitalWrite(soilPower1, HIGH);//turn D2 "On"
        delay(10);//wait 10 milliseconds 
        int soilmoisture = analogRead(attachsoilSensorTo);//Read the SIG value form sensor 
        digitalWrite(attachsoilsensorpowerTo, LOW);//turn D7 "Off"
        Serial.print("Soil Moisture = ");    
        //get soil moisture value from the function below and print it
        Serial.println(soilmoisture);
        
        // read the raw value from the light sensor:
        int rawValue = analogRead(attachlightSensorTo);
        Serial.print("Raw = ");
        Serial.print(rawValue);
        float logLux = rawValue * logRange / rawRange;
        int luxvalue = pow(10, logLux);
        Serial.print(" - Lux = ");
        Serial.println(luxvalue);
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
