
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>

#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN1 2

// #define IDX_LAPENAAN 0
// #define IDX_LAMPENUIT 1
// #define IDX_DAUWAAN 2
// #define IDX_DAUWUIT 3
// #define IDX_DAGTEMP 4
// #define IDX_NACHTTEMP 5
// #define IDX_DAGVOCHT 6
// #define IDX_NACHTVOCHT 7
// #define IDX_BEWOLKINGAAN 8
// #define IDX_BEWOLKINGUIT 9

// #define IDX_BAK1_PIN_SOILPIN 0
// #define IDX_BAK1_PIN_SOILPOWER 1
// #define IDX_BAK1_PIN_LIGHTPIN 2
// #define IDX_BAK1_PIN_LAMPEN1 3
// #define IDX_BAK1_PIN_VENTILATOR 4
// #define IDX_BAK1_PIN_VERNEVELAAR 5
// #define IDX_BAK1_PIN_DHT 6
// #define IDX_BAK1_PIN_LAMPEN2 7

#define countof(a) (sizeof(a) / sizeof(a[0]))


float settingsPlantenbak[10] = {8, 40, 1, 8, 28, 14, 60, 90, 12, 24};// lampenaan, lampenuit, dauwaan, dauwuit, dag temperatuur, nacht temperatuur, dag vochtigheid, nacht vochtigheid, bewolkingaan, bewolkinguit)
byte pinArray1[8] = {A0, 3, A1, 4, 5, 6, 2, 7}; //1soilsensorPin1, 2soilPower1, 3lightsensorPin1, 4lampenPin1, 5ventilatorpin1, 6vernevelaarpin1, 7dhtpin, lampenpin21
// byte pinArray2[8] = {8, 9, 10, 11, 12, 13, 14, 15};
// byte pinArray2[8] = {8, 9, 10, 11, 12, 13, 14, 15};
// byte pinArray2[8] = {8, 9, 10, 11, 12, 13, 14, 15};

class LichtSensor {

    int pin;
    float rawRange = 1024;
    float logRange = 5.0;

    public:
    LichtSensor(int myPin) {
        pin = myPin;
        analogReference(EXTERNAL); 
    }

    void initialisatie() {
            Serial.println("lichtsensor geinitialiseerd");
            Serial.print("lichtsensorpin = ");
            Serial.println(pin);
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
   
    byte pin;
    byte powerPin;
   
    public:
    SoilHumiditySensor(byte myPin, byte myPowerPin) {
        pin = myPin;
        powerPin = myPowerPin;
        pinMode(powerPin, OUTPUT);//Set D2 as an OUTPUT
        digitalWrite(powerPin, LOW);//Set to LOW so no power is flowing through the sensor
    }

    void initialisatie() {
            Serial.println("SoilHumiditySensor geinitialiseerd");
            Serial.print("SoilHumiditySensorPin = ");
            Serial.println(pin);
            Serial.print("SoilHumiditySensorPowerPin = ");
            Serial.println(powerPin);
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
    byte pin;

    public:
    LuchtVochtigheidTemperatuurSensor(byte myPin) :
        dht(myPin, DHT22) 
        {
                dht.begin();
        pin = myPin;
        }

    void initialisatie() {
        Serial.println("LuchtVochtigheidTemperatuurSensor geinitialiseerd");
        Serial.print("LuchtVochtigheidTemperatuurSensorPin = ");
        Serial.println(pin);
    }

    float readTempValue() {
        return(dht.readTemperature());
    }

    float readHumidityValue() {
        return(dht.readHumidity());    //Print temp and humidity values to serial monitor
    }
};

class KlimaatRegelaar {
    byte lampenPin1;
    byte lampenPin2;
    byte nevelPin;
    byte ventilatorPin;
    float startDag;
    float startNacht;
    float startDauw;
    float eindDauw;
    float dagTemp;
    float nachtTemp;
    float dagVochtigheid;
    float nachtVochtigheid;
    float startBewolking;
    float eindBewolking;
    boolean isDag = false;
    boolean isDauw = false;
    boolean isRegenWolk = false;
    boolean ventilatorIsAan = false;
    boolean vernevelaarIsAan = false;
    boolean lampIsAan2 = false;
    boolean luchtIsDroog = false;

    public:
    KlimaatRegelaar(byte myLampenPin1, byte myLampenPin2, byte myNevelPin, byte myVentilatorPin, float settings[]) 
   
        {
        lampenPin1 = myLampenPin1;
        lampenPin2 = myLampenPin2;
        nevelPin = myNevelPin;
        ventilatorPin = myVentilatorPin; 
        startDag = settings[0];
        startNacht = settings[1];
        startDauw = settings[2];
        eindDauw = settings[3];
        dagTemp = settings[4];
        nachtTemp = settings[5];
        dagVochtigheid = settings[6];
        nachtVochtigheid = settings[7];
        startBewolking = settings[8];
        eindBewolking = settings[9];
        pinMode(lampenPin1, OUTPUT);
        digitalWrite(lampenPin1, LOW);
        pinMode(lampenPin2, OUTPUT);
        digitalWrite(lampenPin1, LOW);
        pinMode(nevelPin, OUTPUT);
        digitalWrite(nevelPin, LOW);
        pinMode(ventilatorPin, OUTPUT);
        digitalWrite(nevelPin, LOW);
        digitalWrite(ventilatorPin, LOW);
        // lampenaan, lampenuit, dauwaan, dauwuit, dag temperatuur, nacht temperatuur, dag vochtigheid, nacht vochtigheid, bewolkingaan, bewolkinguit)
        }

    void initialisatie() {
        Serial.println("KlimaatRegelaar geinitialiseerd");
        Serial.print("lampenPin1 = ");
        Serial.println(lampenPin1);
        Serial.print("lampenPin1 = ");
        Serial.println(lampenPin2);
        Serial.print("nevelPin = ");
        Serial.println(nevelPin);
        Serial.print("ventilatorPin = ");
        Serial.println(ventilatorPin);
        Serial.println();
        Serial.print("startDag = ");
        Serial.println(startDag);
        Serial.print("startNacht = ");
        Serial.println(startNacht);
        Serial.print("startDauw = ");
        Serial.println(startDauw);
        Serial.print("eindDauw = ");
        Serial.println(eindDauw);
        Serial.print("dagTemp = ");
        Serial.println(dagTemp);
        Serial.print("nachtTemp = ");
        Serial.println(nachtTemp);
        Serial.print("dagVochtigheid = ");
        Serial.println(dagVochtigheid);
        Serial.print("nachtVochtigheid = ");
        Serial.println(nachtVochtigheid);
        Serial.print("startBewolking = ");
        Serial.println(startBewolking);
        Serial.print("eindBewolking = ");
        Serial.println(eindBewolking);
    }

    float huidigeTijd(RtcDateTime now) {

        float uurNu = now.Second();// terugveranderen naat hour()
        float minuutNu = now.Minute();
        float uurMinuutNu = uurNu + (minuutNu / 60);
        return(uurMinuutNu);
    }

    void regelLicht(RtcDateTime now) {
            
        float uurMinuutNu = huidigeTijd(now);        
        Serial.println(uurMinuutNu);
                
        if (!isDag && uurMinuutNu >= startDag && uurMinuutNu <= startNacht) {
            digitalWrite(lampenPin1, HIGH);
            digitalWrite(lampenPin2, HIGH);
            Serial.println("Lampen aangeschakeld");
            isDag = true;
            //lampIsAan1 = true;
            lampIsAan2 = true;
        }
        if (isDag&& (uurMinuutNu <= startDag || uurMinuutNu >= startNacht)) {
            digitalWrite(lampenPin1, LOW); 
            digitalWrite(lampenPin2, LOW); 
            Serial.println("Lampen uitgeschakeld"); 
            isDag = false;
            lampIsAan2 = false;
        }
    }

    void regelRegenWolken(RtcDateTime now, float temperatuur, float luchtVochtigheid) {
                
        float uurMinuutNu = huidigeTijd(now); 
        if (!isRegenWolk && uurMinuutNu >= startBewolking && uurMinuutNu <= eindBewolking && temperatuur > nachtTemp) {
            isRegenWolk = true;
            if (!vernevelaarIsAan) {
                digitalWrite(nevelPin, HIGH);
                Serial.println("vernevelaar aan (regenwolken)");
                vernevelaarIsAan = true;
            }    
            if (lampIsAan2) {
                digitalWrite(lampenPin2, LOW);
                Serial.println("lampen2 is uit (regenwolken)");
                lampIsAan2 = false;
            }
        }

        if (isRegenWolk && (uurMinuutNu <= startBewolking || uurMinuutNu >= eindBewolking ||temperatuur < nachtTemp || luchtVochtigheid == 100)) {
            isRegenWolk = false;
            if (vernevelaarIsAan) {
                digitalWrite(nevelPin, LOW); 
                Serial.println("vernevelaar uit (regenwolken)"); 
                vernevelaarIsAan = false;
            }  
        }  
        if (isRegenWolk && (uurMinuutNu <= startBewolking || uurMinuutNu >= eindBewolking)) { 
            if (!lampIsAan2 && isDag) {
                 isRegenWolk = false;
                digitalWrite(lampenPin2, HIGH); 
                Serial.println("lampen2 aan (regenwolken)"); 
                lampIsAan2 = true;
            }
        }
    }
    
    void regelDauw(RtcDateTime now, float temperatuur, float luchtVochtigheid) {
        
        float uurMinuutNu = huidigeTijd(now); 
        if (!isDauw && uurMinuutNu >= startDauw && uurMinuutNu <= eindDauw && temperatuur > nachtTemp) {
            if (!vernevelaarIsAan) {
                digitalWrite(nevelPin, HIGH);
                Serial.println("vernevelaar aan (dauw)");
                vernevelaarIsAan = true;
                }    
            if (!ventilatorIsAan) {
                digitalWrite(ventilatorPin, HIGH);
                Serial.println("ventilator aan (dauw)");
                ventilatorIsAan = true;
                }
            isDauw = true;
        }

        if (isDauw && (uurMinuutNu <= startDauw || uurMinuutNu >= eindDauw || temperatuur < nachtTemp || luchtVochtigheid == 100)) {
            if (vernevelaarIsAan && luchtVochtigheid > dagVochtigheid) {
                digitalWrite(nevelPin, LOW); 
                Serial.println("vernevelaar uit (dauw)"); 
                vernevelaarIsAan = false;
            }  
            if (ventilatorIsAan) {
                digitalWrite(ventilatorPin, LOW); 
                Serial.println("ventilator uit (dauw)"); 
                ventilatorIsAan = false;
            }
            isDauw = false;
        }
    }

    void regelVochtigheid(float temperatuur, float luchtVochtigheid) {

        if (luchtVochtigheid < dagVochtigheid && temperatuur > nachtTemp) {
            luchtIsDroog = true;
            if (!vernevelaarIsAan) {
                digitalWrite(nevelPin, HIGH);
                Serial.print("vernevelaar aan (luchtvochtigheid)");
                vernevelaarIsAan = true;
            } 
        }
        if (vernevelaarIsAan && !isDauw && !isRegenWolk && (luchtVochtigheid > dagVochtigheid || temperatuur < nachtTemp)) {
            digitalWrite(nevelPin, LOW);
            Serial.print("vernevelaar uit (luchtvochtigheid)");
            vernevelaarIsAan = false;
        }
        if (luchtVochtigheid > dagVochtigheid ) { 
            luchtIsDroog = false;
        }
    }

    void regelTemperatuur(float temperatuur, float luchtVochtigheid){

        if (temperatuur > dagTemp) {
            if (!ventilatorIsAan) {
                digitalWrite(ventilatorPin, HIGH);
                Serial.print("vernevelaar aan (temperatuur)");
                ventilatorIsAan = true;
            }
            if (lampIsAan2) {
                digitalWrite(lampenPin2, LOW);
                Serial.println("lampen2 is uit (temperatuur)");
                lampIsAan2 = false;
            }
            if (!vernevelaarIsAan) {
                digitalWrite(nevelPin, HIGH);
                Serial.print("vernevelaar aan (temperatuur)");
                vernevelaarIsAan = false;
            }
        }
        if (temperatuur < dagTemp) {
            if (ventilatorIsAan && !isDauw) {
                digitalWrite(ventilatorPin, LOW);
                Serial.print("vernevelaar uit (temperatuur)");
                ventilatorIsAan = false;
            }
            if (!lampIsAan2 && !isRegenWolk) {
                digitalWrite(lampenPin2, HIGH);
                Serial.println("lampen2 aan (temperatuur)");
                lampIsAan2 = true;
            }
            if (vernevelaarIsAan && !isDauw && !isRegenWolk && !luchtIsDroog) {
                digitalWrite(nevelPin, LOW);
                Serial.print("vernevelaar uit (temperatuur)");
                vernevelaarIsAan = false;
            }
        }
    }

    void standen() {
        Serial.print("isDag = ");
        Serial.println(isDag);
        Serial.print("vernevelaarIsAan = ");
        Serial.println(vernevelaarIsAan);
        Serial.print("isDauw = ");
        Serial.println(isDauw);
        Serial.print("ventilatorIsAan = ");
        Serial.println(ventilatorIsAan);
        Serial.print("isRegenWolk = ");
        Serial.println(isRegenWolk);
        Serial.print("lampenAan2 = ");
        Serial.println(lampIsAan2);
    }
};

class Klok {
    
    RtcDS3231<TwoWire> Rtc;
    
    public:
    Klok(): Rtc(Wire)
    {
        Serial.print("Klok geinitieerd");
    }

    void setup() {
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
             printDateTime(compiled);
             printDateTime(now);
             Serial.println("RTC is older than compile time!  (Updating DateTime)");

            Rtc.SetDateTime(compiled);
             printDateTime(compiled);

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
    
    // int[] getUurMinuutNu(RtcDateTime now) {
    //     int uurNu = now.hour();
    //     int minuutNu = now.Minute();
    //     int huidigeTijdUurMinuut[2] = {uurNu, minuutNu};
    //     return(huidigeTijdUurMinuut);
    //}

    RtcDateTime getTime(){
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
                Serial.println("RTC lost confidence in the DateTime! Check battery");
            }
        }

        RtcDateTime now = Rtc.GetDateTime();
        printDateTime(now);
        Serial.println();

        return(now);
    }
    
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
};

class Plantenbak {

    KlimaatRegelaar klimaatRegelaar;
    SoilHumiditySensor soilHumiditySensor;
    LuchtVochtigheidTemperatuurSensor luchtVochtigheidTemperatuurSensor;
    LichtSensor lichtSensor;
    boolean ventilatorIsUit = true;
    boolean vernevelaarIsUit = true;

    public:
    Plantenbak(byte (&myPins)[8], float mySettingsPlantenbak[]) :

        soilHumiditySensor(myPins[0], myPins[1]),
        lichtSensor(myPins[2]),
        luchtVochtigheidTemperatuurSensor(myPins[6]),
        klimaatRegelaar(myPins[3] , myPins[7], myPins[5], myPins[4], mySettingsPlantenbak)
    {}
    //1soilsensorPin1, 2soilPower1, 3lightsensorPin1, 4lampenPin1, 5ventilatorpin1, 6vernevelaarpin1, 7dhtpin1

    void setup() {
               
        lichtSensor.initialisatie();
        soilHumiditySensor.initialisatie();
        luchtVochtigheidTemperatuurSensor.initialisatie();
        klimaatRegelaar.initialisatie();
        Serial.println("plantenbak geinitieerd");
    }

    void loop(RtcDateTime RtcObjectHuidigeTijd) {
        
        float luchtVochtigheid = luchtVochtigheidTemperatuurSensor.readHumidityValue();
        Serial.print("Humidity: ");
        Serial.print(luchtVochtigheid);
        float temperatuur = luchtVochtigheidTemperatuurSensor.readTempValue();
        Serial.print(" %, Temp: ");
        Serial.print(temperatuur);
        Serial.println(" Celsius");
        
        // read the raw value from the soil sensor:
        Serial.print("Soil Moisture = ");  
        Serial.println(soilHumiditySensor.readValue());
        
        // read the raw value from the light sensor:
        Serial.print("Raw = ");
        Serial.print(lichtSensor.readRawValue());
        
        Serial.print(" - Lux = ");
        Serial.println(lichtSensor.readLogValue());
    
        klimaatRegelaar.regelLicht(RtcObjectHuidigeTijd);
        klimaatRegelaar.regelDauw(RtcObjectHuidigeTijd, temperatuur, luchtVochtigheid);
        klimaatRegelaar.regelRegenWolken(RtcObjectHuidigeTijd, temperatuur, luchtVochtigheid);
        klimaatRegelaar.regelVochtigheid(temperatuur, luchtVochtigheid);
        klimaatRegelaar.regelTemperatuur(temperatuur, luchtVochtigheid);
        klimaatRegelaar.standen();
    }    
};

Klok klok;
Plantenbak plantenbak1(pinArray1, settingsPlantenbak);
//Plantenbak plantenbak1(pinArray1, settingsPlantenbak);

void setup()
{
    Serial.begin(9600);
    klok.setup();
    plantenbak1.setup();
}

void loop()
{
    RtcDateTime tijd = klok.getTime();
    Serial.println(tijd);
    plantenbak1.loop(tijd);
    delay(3000);
}