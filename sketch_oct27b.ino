
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>



#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN1 2

#define countof(a) (sizeof(a) / sizeof(a[0]))


float settingsPlantenbak[8] = {8, 21, 1, 8, 28, 18, 60, 90};// lampenaan, lampenuit, dauwaan, dauwuit, dag temperatuur, nacht temperatuur, dag vochtigheid, nacht vochtigheid)
byte pinArray[7] = {A0, 3, A1, 10, 11, 12, 2}; //1soilsensorPin1, 2soilPower1, 3lightsensorPin1, 4lampenPin1, 5ventilatorpin1, 6vernevelaarpin1, 7dhtpin1
//Variables pot humidity
// int soilsensorPin1 = A0;//Declare a variable for the soil moisture sensor 
// int soilPower1 = 3;//Variable for Soil moisture Power
// //Variables light sensor
// int lightsensorPin1 = A1; // select the input pin for the potentiometer
// //Variables lampen
// byte lampenPin1 = 10;



class LichtSensor {

    int pin;
    float rawRange = 1024;
    float logRange = 5.0;

    public:
    LichtSensor(int pin) {
        pin = pin;
        analogReference(EXTERNAL); 
        Serial.print("lichtsensor geinitialiseerd");
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
        Serial.print("SoilHumiditySensor geinitialiseerd");
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
    Serial.print("LuchtVochtigheidTemperatuurSensor geinitialiseerd");     
    }

    float readTempValue() {
        return(dht.readTemperature());
    }

    float readHumidityValue() {
        return(dht.readHumidity());    //Print temp and humidity values to serial monitor
    }
};

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
        Serial.print("Lampen geinitieerd");
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
    Klok():
    Rtc(Wire)
    //Serial.print("Klok geinitieerd")
    {}

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
                // Serial.print("RTC communications error = ");
                // Serial.println(Rtc.LastError());
            }
            else
            {
                // Common Causes:
                //    1) first time you ran and the device wasn't running yet
                //    2) the battery on the device is low or even missing

                // Serial.println("RTC lost confidence in the DateTime!");

                // following line sets the RTC to the date & time this sketch was compiled
                // it will also reset the valid flag internally unless the Rtc device is
                // having an issue

                Rtc.SetDateTime(compiled);
            }
        }

        if (!Rtc.GetIsRunning())
        {
            // Serial.println("RTC was not actively running, starting now");
            Rtc.SetIsRunning(true);
        }

        RtcDateTime now = Rtc.GetDateTime();
        if (now < compiled) 
        {
            // printDateTime(compiled);
            // printDateTime(now);
            // Serial.println("RTC is older than compile time!  (Updating DateTime)");

            Rtc.SetDateTime(compiled);
            // printDateTime(compiled);

        }
        else if (now > compiled) 
        {
            // Serial.println("RTC is newer than compile time. (this is expected)");
        }
        else if (now == compiled) 
        {
            // Serial.println("RTC is the same as compile time! (not expected but all is fine)");
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
    // }

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

class Test {
    
    byte a;
    byte b;
   
    public:
    Test(byte array[]){
        a = array[0];
        b = array[4];
        
    }
    void loop(){
        Serial.print(a);
        Serial.print(b);  
    }
};

   

class Plantenbak {
  
    SoilHumiditySensor soilHumiditySensor;
    LuchtVochtigheidTemperatuurSensor luchtVochtigheidTemperatuurSensor;
    LichtSensor lichtSensor;
    Lampen lampen;
    float dauwAan;
    float dauwUit;
    float dagTemperatuur;
    float nachtTemperatuur;
    float dagVochtigheid;
    float nachtVochtigheid;
    byte ventilatorPin;
    byte vernevelaarPin;
    boolean ventilatorIsUit = true;
    boolean vernevelaarIsUit = true;
    float lampenAan;
    float lampenUit;
    byte soilsensorPin;
    byte soilPower;
    byte lightsensorPin;
    byte dhtPin;
    
    //1soilsensorPin1, 2soilPower1, 3lightsensorPin1, 4lampenPin1, 5ventilatorpin1, 6vernevelaarpin1, 7dhtpin1
    //1soilsensorPin1, 2soilPower1, 3lightsensorPin1, 4lampenPin1, 5ventilatorpin1, 6vernevelaarpin1, 7dhtpin1
    public:
    Plantenbak(byte pinArray[], float settingsPlantenbak[]):
        
        lampenAan = settingsPlantenbak[0],
        lampenUit = settingsPlantenbak[1],       
        dauwAan = settingsPlantenbak[2],
        dauwUit = settingsPlantenbak[3],
        nachtTemperatuur = settingsPlantenbak[4],
        nachtTemperatuur = settingsPlantenbak[5],
        dagVochtigheid = settingsPlantenbak[6],
        nachtVochtigheid = settingsPlantenbak[7],

        soilsensorPin = pinArray[0],
        soilPower = pinArray[1],
        lightsensorPin = pinArray[2],
        lampenPin = pinArray[3],
        ventilatorPin = pinArray[4],
        vernevelaarPin = pinArray[5],
        dhtPin = pinArray[6],

        soilHumiditySensor(soilsensorPin, soilPower),
        lichtSensor(lightsensorPin),
        luchtVochtigheidTemperatuurSensor(dhtPin),
        lampen(lampenPin, lampenAan, lampenUit),
        pinMode(vernevelaarPin, OUTPUT),
        pinMode(vernevelaarPin, OUTPUT),
        digitalWrite(ventilatorPin, LOW),
        digitalWrite(vernevelaarPin, LOW)

    {
        Serial.print("plantenbak geinitieerd")
    }

    // void setup(byte pinArray[], float settingsPlantenbak[]){
    //     lampenAan = settingsPlantenbak[0],
    //     lampenUit = settingsPlantenbak[1],
    //     dauwAan = settingsPlantenbak[2];
    //     dauwUit = settingsPlantenbak[3];
    //     nachtTemperatuur = settingsPlantenbak[4];
    //     nachtTemperatuur = settingsPlantenbak[5];
    //     dagVochtigheid = settingsPlantenbak[6];
    //     nachtVochtigheid = settingsPlantenbak[7];
    //     ventilatorPin = pinArray[4];
    //     vernevelaarPin = pinArray[5];
    //     pinMode(vernevelaarPin, OUTPUT);
    //     pinMode(vernevelaarPin, OUTPUT);
    //     digitalWrite(ventilatorPin, LOW);
    //     digitalWrite(vernevelaarPin, LOW);
    //     Serial.print("plantenbak geinitieerd");
    //     }

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
    
        lampen.regelLicht(RtcObjectHuidigeTijd);
        dauw(RtcObjectHuidigeTijd, temperatuur, luchtVochtigheid);
        increaseHumidity(RtcObjectHuidigeTijd, temperatuur, luchtVochtigheid);
        decreaseTemperature(RtcObjectHuidigeTijd, temperatuur, luchtVochtigheid);
    }

    void decreaseTemperature(RtcDateTime rtc, float temp, float vocht){

        int temperatuur = temp;
        int luchtVochtigheid = vocht;//vragen aan pim of dauwUit hier hernoemd moet worden
        RtcDateTime RtcObjectHuidigeTijd = rtc;//vragen aan pim of dauwUit hier hernoemd moet worden
        int maxTemperatuur;
        int maxVochtigheid;
        
        // pinMode(vernevelaarPin, OUTPUT); //in constructor of setup??
        digitalWrite(vernevelaarPin, LOW); //in constructor??
        int uurNu = RtcObjectHuidigeTijd.Second();// terugveranderen naat hour()
        int minuutNu = RtcObjectHuidigeTijd.Minute();
        float uurMinuutNu = uurNu + (minuutNu/60);
        //float uurMinuutNu = getTime(RtcObjectHuidigeTijd);

        if (uurMinuutNu >= lampenAan && uurMinuutNu <= lampenUit){
            maxVochtigheid = dagVochtigheid;
            maxTemperatuur = dagTemperatuur;
        }
        
        if (uurMinuutNu <= lampenAan || uurMinuutNu >= lampenUit) {
            maxVochtigheid = nachtVochtigheid;
            maxTemperatuur = nachtTemperatuur;
        }

        if (temperatuur > maxTemperatuur) {
            
            digitalWrite(ventilatorPin, HIGH);
            Serial.print("vernevelaar aangeschakeld vanwege dauw");
            ventilatorIsUit = false;
            //helft licht uit
            if (luchtVochtigheid < maxVochtigheid){
                digitalWrite(vernevelaarPin, HIGH);
                Serial.print("vernevelaar aangeschakeld vanwege hoge temperatuur");
                vernevelaarIsUit = false;// vragen aan pim : is hij dan false in alle methodes van de hele klasse of alleen in deze methode?
            }
            
            
        }
        
        
    }
    
    void increaseHumidity(RtcDateTime huidigeTijd, float temperatuur, float luchtVochtigheid){

        // temperatuur = temperatuur;
        // luchtVochtigheid = luchtVochtigheid;//vragen aan pim of dauwUit hier hernoemd moet worden
        // RtcObjectHuidigeTijd = RtcObjectHuidigeTijd;//vragen aan pim of dauwUit hier hernoemd moet worden
        int maxVochtigheid;
        
        int uurNu = huidigeTijd.Second();// terugveranderen naat hour()
        int minuutNu = huidigeTijd.Minute();
        float uurMinuutNu = uurNu + (minuutNu/60);

        // float getTime(RtcDateTime huidigeTijd) {
        //         int uurNu = huidigeTijd.Second();
        //         int minuutNu = huidigeTijd.Minute();
        //         return(uurNu + (minuutNu/60));
        //     }
        if (uurMinuutNu >= lampenAan && uurMinuutNu <= lampenUit){
            maxVochtigheid = dagVochtigheid;
        }
        
        if (uurMinuutNu <= lampenAan || uurMinuutNu >= lampenUit) {
            maxVochtigheid = nachtVochtigheid;
        }

        if (vernevelaarIsUit && luchtVochtigheid < maxVochtigheid) {
            digitalWrite(vernevelaarPin, HIGH);
            Serial.print("vernevelaar aangeschakeld vanwege lage luchtvochtigheid");
            vernevelaarIsUit = false;// vragen aan pim : is hij dan false in de hele klasse of alleen in deze methode?
        }
        
        if (!vernevelaarIsUit && luchtVochtigheid > maxVochtigheid) {
            digitalWrite(vernevelaarPin, LOW); 
            Serial.print("vernevelaar uitgeschakeld vanwege voldoende luchtvochtigheid"); 
            vernevelaarIsUit = true;
        }
    }

   
    void dauw(RtcDateTime now, float temperatuur, float luchtVochtigheid){
        
        //temperatuur = temperatuur;
        //luchtVochtigheid = luchtVochtigheid;//vragen aan pim of dauwUit hier hernoemd moet worden
        
        // pinMode(ventilatorPin, OUTPUT);//in constructor??
        // digitalWrite(ventilatorPin, LOW);//in constructor??
        // pinMode(vernevelaarPin, OUTPUT);//in constructor??
        // digitalWrite(vernevelaarPin, LOW);//in constructor??
        int uurNu = now.Second();// terugveranderen naat hour()
        int minuutNu = now.Minute();
        float uurMinuutNu = uurNu + (minuutNu/60);

        if (vernevelaarIsUit && uurMinuutNu >= dauwAan && uurMinuutNu <= dauwUit && luchtVochtigheid < nachtVochtigheid) {
            digitalWrite(vernevelaarPin, HIGH);
            Serial.print("vernevelaar aangeschakeld vanwege dauw");
            vernevelaarIsUit = false;
        }
        if (!vernevelaarIsUit && (uurMinuutNu <= dauwAan || uurMinuutNu >= dauwUit || luchtVochtigheid > nachtVochtigheid )) {
            digitalWrite(vernevelaarPin, LOW); 
            Serial.print("vernevelaar uitgeschakeld vanwege dauw afgelopen"); 
            vernevelaarIsUit = true;
        }
        if (ventilatorIsUit && uurMinuutNu >= dauwAan && uurMinuutNu <= dauwUit && temperatuur > nachtTemperatuur) {
            digitalWrite(ventilatorPin, HIGH);
            Serial.print("vernevelaar aangeschakeld vanwege dauw");
            ventilatorIsUit = false;
        }
        if (!ventilatorIsUit && (uurMinuutNu <= dauwAan || uurMinuutNu >= dauwUit || temperatuur > nachtTemperatuur)) {
            digitalWrite(ventilatorPin, LOW); 
            Serial.print("vernevelaar uitgeschakeld vanwege dauw afgelopen"); 
            ventilatorIsUit = true;
        }
    }

};

Test test(pinArray);
Klok klok();
Plantenbak plantenbak1(pinArray, settingsPlantenbak);

void setup()
{
    Serial.begin(9600);
    klok.setup();
    plantenbak.setup();
    plantenbak1.setup(pinArray, settingsPlantenbak);
}
  

void loop()
{
    
    RtcDateTime rtcTijdObject = klok.getTime();
    //int[2] uurMinuutHuidig = klok.getUurMinuutNu();
    //plantenbak1.loop(rtcTijdObject); //, uurMinuutHuidig
    delay(3000);
    test.loop();
}







    
