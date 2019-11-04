class A {

    String message;

    public:
    A(String myMessage): message(myMessage) {
    }

    void printMessage() {
        Serial.print(message);
    }

};

class B {

    A a1;
    A a2;

    public:
    B(String message1, String message2) : a1(message1), a2(message2) {
    }

    void printMessage() {
        a1.printMessage();
        Serial.print(" === ");
        a2.printMessage();
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