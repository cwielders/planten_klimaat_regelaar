class Sensor {

    String pin;                     // private instance variable

    public:
    Sensor(String myPin):
        pin(myPin) {                // init field 'pin' using 'myPin' argument
    }

    void readvalue() {
        Serial.print("Meetwaarde ", pin);
    }

};

class PlantenBak {

    Sensor s1;                      // private instance variable
    Sensor s2;                      // private instance variable

    public:
    PlantenBak(String pin1, String pin2): 
        s1(pin1), s2(pin2) {        // init fields 'a1' and 'a2' using arguments
    }

    void regelKlimaat() {
        s1.readvalue();             // will print out 'pin1' passed to Sensor instance in PlantenBak constructor
        Serial.print(" === ");
        s2.readvalue();             // will print out 'pin2' passed to Sensor instance in PlantenBak constructor
        Serial.print("\n\n");
    }

};

PlantenBak bak("Pin1", "Pin2");

void setup() {
    Serial.begin(9600);
}

void loop() {
    delay(1000);
    bak.regelKlimaat();
}