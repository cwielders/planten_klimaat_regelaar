class A {

    String message;                     // private instance variable

    public:
    A(String myMessage):
        message(myMessage) {            // init field 'message' using 'myMessage' argument
    }

    void printMessage() {
        Serial.print(message);
    }

};

class B {

    A a1;                               // private instance variable
    A a2;                               // private instance variable

    public:
    B(String message1, String message2): 
        a1(message1), a2(message2) {    // init fields 'a1' and 'a2' using arguments
    }

    void printMessage() {
        a1.printMessage();              // will print out 'message1' passed to A instance in B constructor
        Serial.print(" === ");
        a2.printMessage();              // will print out 'message2' passed to A instance in B constructor
        Serial.print("\n\n");
    }

};

B b("Message_1", "Message_2");

void setup() {
    Serial.begin(9600);
}

void loop() {
    delay(1000);
    b.printMessage();
}