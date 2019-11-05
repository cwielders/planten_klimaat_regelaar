class Sensor {

    String message;                     // private instance variable

    public:
    Sensor(String myMessage):
        message(myMessage) {            // init field 'message' using 'myMessage' argument
    }

    void readvalue() {
        Serial.print(message);
    }

};

class PlantenBak {

    Sensor a1;                          // private instance variable
    Sensor a2;                          // private instance variable

    public:
    PlantenBak(String message1, String message2): 
        a1(message1), a2(message2) {    // init fields 'a1' and 'a2' using arguments
    }

    void regelKlimaat() {
        a1.readvalue();                 // will print out 'message1' passed to A instance in B constructor
        Serial.print(" === ");
        a2.readvalue();                 // will print out 'message2' passed to A instance in B constructor
        Serial.print("\n\n");
    }

};

PlantenBak bak("Message_1", "Message_2");

void setup() {
    Serial.begin(9600);
}

void loop() {
    delay(1000);
    bak.regelKlimaat();
}