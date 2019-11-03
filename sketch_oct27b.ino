void setup() {
    Serial.begin(9600);
}

void loop() {
    delay(1000);
    A a("Global scope message");
    B b;
    b.printMessage();
}

class A {

    String message;

    public:
    A(String myMessage) {
        message = myMessage;
    }

    void printMessage() {
        Serial.print(message);
    }

};

class B {

    // A *a;

    public:
    B() {
        // A myA(message);
        // a = &myA;
    }

    void printMessage() {
        a.printMessage();
    }

};