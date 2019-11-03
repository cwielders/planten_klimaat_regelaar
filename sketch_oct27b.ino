class A {

    String message;

    public:
    A(String myMessage) {
        message = myMessage;
    }

    void printMessage() {
        Serial.print("yes");
    }

};

class B {

    A *a;

    public:
    B(String message) {
        A myA(message);
        a = &myA;
    }

    void printMessage() {
        a->printMessage;
    }

};

void setup() {
    Serial.begin(9600);
}

B b("Mijn B message");
void loop() {
    delay(1000);
    b.printMessage();
}