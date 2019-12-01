
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>

#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN1 2

#define IDX_LAPENAAN 0
#define IDX_LAMPENUIT 1
#define IDX_DAUWAAN 2
#define IDX_DAUWUIT 3
#define IDX_DAGTEMP 4
#define IDX_NACHTTEMP 5
#define IDX_DAGVOCHT 6
#define IDX_NACHTVOCHT 7
#define IDX_BEWOLKINGAAN 8
#define IDX_BEWOLKINGUIT 9

#define IDX_BAK1_PIN_SOILPIN 0
#define IDX_BAK1_PIN_SOILPOWER 1
#define IDX_BAK1_PIN_LIGHTPIN 2
#define IDX_BAK1_PIN_LAMPEN1 3
#define IDX_BAK1_PIN_VENTILATOR 4
#define IDX_BAK1_PIN_VERNEVELAAR 5
#define IDX_BAK1_PIN_DHT 6
#define IDX_BAK1_PIN_LAMPEN2 7

#define countof(a) (sizeof(a) / sizeof(a[0]))


float settingsPlantenbak[10] = {8, 21, 1, 8, 28, 18, 60, 90, 12, 14};// lampenaan, lampenuit, dauwaan, dauwuit, dag temperatuur, nacht temperatuur, dag vochtigheid, nacht vochtigheid, bewolkingaan, bewolkinguit)
byte pinArray[8] = {A0, 3, A1, 4, 5, 6, 2, 7}; //1soilsensorPin1, 2soilPower1, 3lightsensorPin1, 4lampenPin1, 5ventilatorpin1, 6vernevelaarpin1, 7dhtpin1


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
    dht(myPin, DHT22) {
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
    float starNacht;
    float startDauw;
    float eindDauw;
    float dagTemp;
    float nachtTemp;
    float dagVochtigheid;
    float nachtVochtigheid;
    float startBewolking;
    float eindBewolking;
    boolean isDag;
    boolean isDauw;
    boolean ventilatorIsAan;
    boolean vernevelaarIsAan;
    float (&settings)[10];

    public:
    KlimaatRegelaar(byte myLampenPin1, byte myLampenPin2, byte myNevelPin, byte myVentilatorPin, float (&settings)[10]) :
       
    settings(settingsPlantenbak)
   
    {
    lampenPin1 = myLampenPin1;
    lampenPin2 = myLampenPin2;
    nevelPin = myNevelPin;
    ventilatorPin = myVentilatorPin;
    byte dagVochtigheid;
    byte nachtVochtigheid;
    startDag = settings[0];
    starNacht = settings[1];
    startDauw = settings[2];
    eindDauw = settings[3];
    dagTemp = settings[4];
    nachtTemp = settings[5];
    dagVochtigheid = settings[6];
    nachtVochtigheid = settings[7];
    startBewolking = settings[8];
    eindBewolking = settings[9];
    // lampenaan, lampenuit, dauwaan, dauwuit, dag temperatuur, nacht temperatuur, dag vochtigheid, nacht vochtigheid, bewolkingaan, bewolkinguit)
    }
};

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

class Lampen {

    byte pin;
    byte uurAan;
    byte uurUit;
    boolean lampIsUit = true;

    public:
    Lampen(byte lampenPin, float aan, float uit){
        
        pin = lampenPin;
        uurUit = uit;
        uurAan = aan;
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }

    void regelLicht(RtcDateTime now) {
        int uurNu = now.Second();// terugveranderen naat hour()
        int minuutNu = now.Minute();
        float uurMinuutNu = uurNu + (minuutNu/60);

        Serial.println(uurNu);
        Serial.println(uurUit);
        Serial.println(uurMinuutNu);
        Serial.println(uurAan);
        Serial.println(lampIsUit);
        Serial.println(uurNu >= uurAan);
        Serial.println(uurNu <= uurUit);

        if (lampIsUit && uurMinuutNu >= uurAan && uurMinuutNu <= uurUit) {
            digitalWrite(pin, HIGH);
            Serial.print("Lampen aangeschakeld");
            lampIsUit = false;
        }
        if (!lampIsUit && (uurMinuutNu <= uurAan || uurMinuutNu >= uurUit)) {
            digitalWrite(pin, LOW); 
            Serial.print("Lampen uitgeschakeld"); 
            lampIsUit = true;
        }
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

    //KlimaatRegelaar klimaatRegelaar;
    SoilHumiditySensor soilHumiditySensor;
    LuchtVochtigheidTemperatuurSensor luchtVochtigheidTemperatuurSensor;
    LichtSensor lichtSensor;
    //Lampen lampen;
    boolean ventilatorIsUit = true;
    boolean vernevelaarIsUit = true;
    //byte (&pins)[8];
    float (&settings)[10];

    // #define IDX_BAK1_PIN_SOILPIN 0
    // #define IDX_BAK1_PIN_SOILPOWER 1
    // #define IDX_BAK1_PIN_LIGHTPIN 2
    // #define IDX_BAK1_PIN_LAMPEN1 3
    // #define IDX_BAK1_PIN_VENTILATOR 4
    // #define IDX_BAK1_PIN_VERNEVELAAR 5
    // #define IDX_BAK1_PIN_DHT 6
    // #define IDX_BAK1_PIN_LAMPEN2 7
    //byte mylampenPin1, byte mylampenPin2, byte mynevelPin, byte myventilatorPin,
    
    public:
    Plantenbak(byte pin0, byte pin1, byte pin2,  byte pin3, byte pin4, byte pin5, byte pin6, byte pin7, float (&settingsPlantenbak)[10]) :
        //pins(pinArray), 
        settings(settingsPlantenbak),
        soilHumiditySensor(pin0, pin1),
        lichtSensor(pin2),
        luchtVochtigheidTemperatuurSensor(pin6)
        // lampen(pins[IDX_BAK1_PIN_LAMPEN], settings[IDX_LAPENAAN], settings[IDX_LAMPENUIT])//,
        //klimaatRegelaar(pins[IDX_BAK1_PIN_LAMPEN1], pins[IDX_BAK1_PIN_LAMPEN2], pins[IDX_BAK1_PIN_VERNEVELAAR], pins[ IDX_BAK1_PIN_VENTILATOR], settings)
    {}
    
    void setup() {
        // Serial.println(pins[0]);
        // Serial.println(pins[1]);
        // Serial.println(pins[2]);
        // Serial.println(pins[3]);
        // Serial.println(pins[4]);
        // Serial.println(pins[5]);
        // Serial.println(pins[6]);
        
        lichtSensor.initialisatie();
        soilHumiditySensor.initialisatie();
        luchtVochtigheidTemperatuurSensor.initialisatie();

        // pinMode(pins[IDX_BAK1_PIN_VERNEVELAAR], OUTPUT);
        // digitalWrite(pins[IDX_BAK1_PIN_VERNEVELAAR], LOW);
        // digitalWrite(pins[IDX_BAK1_PIN_VENTILATOR], LOW);
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
    
        //lampen.regelLicht(RtcObjectHuidigeTijd);
        // dauw(RtcObjectHuidigeTijd, temperatuur, luchtVochtigheid);
        // increaseHumidity(RtcObjectHuidigeTijd, temperatuur, luchtVochtigheid);
        // decreaseTemperature(RtcObjectHuidigeTijd, temperatuur, luchtVochtigheid);
    }

    // void decreaseTemperature(RtcDateTime rtc, float temp, float vocht){

    //     int temperatuur = temp;
    //     int luchtVochtigheid = vocht;//vragen aan pim of dauwUit hier hernoemd moet worden
    //     RtcDateTime RtcObjectHuidigeTijd = rtc;//vragen aan pim of dauwUit hier hernoemd moet worden
    //     int maxTemperatuur;
    //     int maxVochtigheid;
        
    //     // pinMode(vernevelaarPin, OUTPUT); //in constructor of setup??
    //     digitalWrite(vernevelaarPin, LOW); //in constructor??
    //     int uurNu = RtcObjectHuidigeTijd.Second();// terugveranderen naat hour()
    //     int minuutNu = RtcObjectHuidigeTijd.Minute();
    //     float uurMinuutNu = uurNu + (minuutNu/60);
    //     //float uurMinuutNu = getTime(RtcObjectHuidigeTijd);

    //     if (uurMinuutNu >= lampenAan && uurMinuutNu <= lampenUit){
    //         maxVochtigheid = dagVochtigheid;
    //         maxTemperatuur = dagTemperatuur;
    //     }
        
    //     if (uurMinuutNu <= lampenAan || uurMinuutNu >= lampenUit) {
    //         maxVochtigheid = nachtVochtigheid;
    //         maxTemperatuur = nachtTemperatuur;
    //     }

    //     if (temperatuur > maxTemperatuur) {
            
    //         digitalWrite(ventilatorPin, HIGH);
    //         Serial.print("vernevelaar aangeschakeld vanwege dauw");
    //         ventilatorIsUit = false;
    //         //helft licht uit
    //         if (luchtVochtigheid < maxVochtigheid){
    //             digitalWrite(vernevelaarPin, HIGH);
    //             Serial.print("vernevelaar aangeschakeld vanwege hoge temperatuur");
    //             vernevelaarIsUit = false;// vragen aan pim : is hij dan false in alle methodes van de hele klasse of alleen in deze methode?
    //         }
            
            
    //     }
        
        
    // }
    
    // void increaseHumidity(RtcDateTime huidigeTijd, float temperatuur, float luchtVochtigheid){

    //     // temperatuur = temperatuur;
    //     // luchtVochtigheid = luchtVochtigheid;//vragen aan pim of dauwUit hier hernoemd moet worden
    //     // RtcObjectHuidigeTijd = RtcObjectHuidigeTijd;//vragen aan pim of dauwUit hier hernoemd moet worden
    //     int maxVochtigheid;
        
    //     int uurNu = huidigeTijd.Second();// terugveranderen naat hour()
    //     int minuutNu = huidigeTijd.Minute();
    //     float uurMinuutNu = uurNu + (minuutNu/60);

    //     // float getTime(RtcDateTime huidigeTijd) {
    //     //         int uurNu = huidigeTijd.Second();
    //     //         int minuutNu = huidigeTijd.Minute();
    //     //         return(uurNu + (minuutNu/60));
    //     //     }
    //     if (uurMinuutNu >= lampenAan && uurMinuutNu <= lampenUit){
    //         maxVochtigheid = dagVochtigheid;
    //     }
        
    //     if (uurMinuutNu <= lampenAan || uurMinuutNu >= lampenUit) {
    //         maxVochtigheid = nachtVochtigheid;
    //     }

    //     if (vernevelaarIsUit && luchtVochtigheid < maxVochtigheid) {
    //         digitalWrite(vernevelaarPin, HIGH);
    //         Serial.print("vernevelaar aangeschakeld vanwege lage luchtvochtigheid");
    //         vernevelaarIsUit = false;// vragen aan pim : is hij dan false in de hele klasse of alleen in deze methode?
    //     }
        
    //     if (!vernevelaarIsUit && luchtVochtigheid > maxVochtigheid) {
    //         digitalWrite(vernevelaarPin, LOW); 
    //         Serial.print("vernevelaar uitgeschakeld vanwege voldoende luchtvochtigheid"); 
    //         vernevelaarIsUit = true;
    //     }
    // }

   
    // void dauw(RtcDateTime now, float temperatuur, float luchtVochtigheid){
        
    //     //temperatuur = temperatuur;
    //     //luchtVochtigheid = luchtVochtigheid;//vragen aan pim of dauwUit hier hernoemd moet worden
        
    //     // pinMode(ventilatorPin, OUTPUT);//in constructor??
    //     // digitalWrite(ventilatorPin, LOW);//in constructor??
    //     // pinMode(vernevelaarPin, OUTPUT);//in constructor??
    //     // digitalWrite(vernevelaarPin, LOW);//in constructor??
    //     int uurNu = now.Second();// terugveranderen naat hour()
    //     int minuutNu = now.Minute();
    //     float uurMinuutNu = uurNu + (minuutNu/60);

    //     if (vernevelaarIsUit && uurMinuutNu >= dauwAan && uurMinuutNu <= dauwUit && luchtVochtigheid < nachtVochtigheid) {
    //         digitalWrite(vernevelaarPin, HIGH);
    //         Serial.print("vernevelaar aangeschakeld vanwege dauw");
    //         vernevelaarIsUit = false;
    //     }
    //     if (!vernevelaarIsUit && (uurMinuutNu <= dauwAan || uurMinuutNu >= dauwUit || luchtVochtigheid > nachtVochtigheid )) {
    //         digitalWrite(vernevelaarPin, LOW); 
    //         Serial.print("vernevelaar uitgeschakeld vanwege dauw afgelopen"); 
    //         vernevelaarIsUit = true;
    //     }
    //     if (ventilatorIsUit && uurMinuutNu >= dauwAan && uurMinuutNu <= dauwUit && temperatuur > nachtTemperatuur) {
    //         digitalWrite(ventilatorPin, HIGH);
    //         Serial.print("vernevelaar aangeschakeld vanwege dauw");
    //         ventilatorIsUit = false;
    //     }
    //     if (!ventilatorIsUit && (uurMinuutNu <= dauwAan || uurMinuutNu >= dauwUit || temperatuur > nachtTemperatuur)) {
    //         digitalWrite(ventilatorPin, LOW); 
    //         Serial.print("vernevelaar uitgeschakeld vanwege dauw afgelopen"); 
    //         ventilatorIsUit = true;
    //     }
    // }

};

Klok klok;
Plantenbak plantenbak1(pinArray[0], pinArray[1], pinArray[2], pinArray[3], pinArray[4], pinArray[5], pinArray[6], pinArray[7], settingsPlantenbak);

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