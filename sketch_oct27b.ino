
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>
RtcDS3231<TwoWire> Rtc(Wire);


#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN1 2

//Variables air humidity and temp
//int chk;

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
    LichtSensor(int pin) {
        pin = pin;
        analogReference(EXTERNAL); 
    }

    // read the raw value from the light sensor:
    float readRawValue() {
        float rawValue = analogRead(pin);
        return(rawValue);
    }

    // read the raw value from the light sensor:
    float readLogValue() {
        float rawValue = analogRead(pin);
        float logLux = rawValue * logRange / rawRange;
        float luxValue = pow(10, logLux);
        return(luxValue);
    }

};

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
    float readValue() {
        digitalWrite(powerPin, HIGH);//turn D2 "On"
        delay(10);//wait 10 milliseconds 
        float soilmoisture = analogRead(pin);//Read the SIG value form sensor 
        digitalWrite(powerPin, LOW);//turn D7 "Off"
        return(soilmoisture);
    } 

};

class LuchtVochtigheidTemperatuurSensor {

    DHT dht;

    public:
    LuchtVochtigheidTemperatuurSensor(byte pin) :
    dht(pin, DHT22) {
        dht.begin();   
    }
   
    float readTempValue() {
       return(dht.readTemperature());
    }

    float readHumidityValue() {
        return(dht.readHumidity());    //Print temp and humidity values to serial monitor
    }
};

class Plantenbak {
  
    SoilHumiditySensor soilHumiditySensor;
    LuchtVochtigheidTemperatuurSensor luchtVochtigheidTemperatuurSensor;
    LichtSensor lichtSensor;

    public:
    Plantenbak(byte lightSensorPin, byte soilSensorPin, byte soilSensorPower, byte airHumTempSensorPin):
      
      soilHumiditySensor(soilSensorPin, soilSensorPower),
      lichtSensor(lightSensorPin),
      luchtVochtigheidTemperatuurSensor(airHumTempSensorPin)
    {
    }    

    void loop() {

        Serial.print("Humidity: ");
        Serial.print(luchtVochtigheidTemperatuurSensor.readHumidityValue());
        Serial.print(" %, Temp: ");
        Serial.print(luchtVochtigheidTemperatuurSensor.readTempValue());
        Serial.println(" Celsius");
        
        // read the raw value from the soil sensor:
        Serial.print("Soil Moisture = ");  
        Serial.println(soilHumiditySensor.readValue());
               
        // read the raw value from the light sensor:
        Serial.print("Raw = ");
        Serial.print(lichtSensor.readRawValue());
        
        Serial.print(" - Lux = ");
        Serial.println(lichtSensor.readLogValue());
    }

};

RtcDS3231<TwoWire> rtc(Wire);
RtcDateTime compileDateTime(__DATE__, __TIME__);
Plantenbak plantenbak1(lightsensorPin1, soilsensorPin1, soilPower1, DHTPIN1);
  
void setup()
{
    //Serial.begin(9600);
    Serial.begin(57600);

    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);

        Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printDateTime(compiled);
    Serial.println();

    if (!Rtc.IsDateTimeValid()) 
    {
        if (Rtc.LastError() != 0)
        {
            // we have a communications error
            // see https://www.arduino.cc/en/Reference/WireEndTransmission for 
            // what the number means
            Serial.print("RTC communications error = ");
            Serial.println(Rtc.LastError());
        }
        else
        {
            // Common Causes:
            //    1) first time you ran and the device wasn't running yet
            //    2) the battery on the device is low or even missing

            Serial.println("RTC lost confidence in the DateTime!");

            // following line sets the RTC to the date & time this sketch was compiled
            // it will also reset the valid flag internally unless the Rtc device is
            // having an issue

            Rtc.SetDateTime(compiled);
        }
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }

    // never assume the Rtc was last configured by you, so
    // just clear them to your needed state
    Rtc.Enable32kHzPin(false);
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 
}
  

void loop()
{
    if (!Rtc.IsDateTimeValid()) 
    {
        if (Rtc.LastError() != 0)
        {
            // we have a communications error
            // see https://www.arduino.cc/en/Reference/WireEndTransmission for 
            // what the number means
            Serial.print("RTC communications error = ");
            Serial.println(Rtc.LastError());
        }
        else
        {
            // Common Causes:
            //    1) the battery on the device is low or even missing and the power line was disconnected
            Serial.println("RTC lost confidence in the DateTime!");
        }
    }

    RtcDateTime now = Rtc.GetDateTime();
    printDateTime(now);
    Serial.println();

	RtcTemperature temp = Rtc.GetTemperature();
	temp.Print(Serial);
	// you may also get the temperature as a float and print it
    // Serial.print(temp.AsFloatDegC());
    Serial.println("C");
     
    delay(3000);
    
    plantenbak1.loop();

}




#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
    {
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Day(),
            dt.Month(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}
    
