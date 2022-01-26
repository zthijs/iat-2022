// Importeren van modules voor componenten.
#include <LedControl.h>
#include <Servo.h>

// Pins voor de afstand senor definiëren
#define echoPin 2
#define trigPin 3

// Servo definiëren.
Servo servo;

// Operatoren definiëren. (+, -, ÷)
byte plus[8] = {0x00,0x18,0x18,0x7E,0x7E,0x18,0x18,0x00};
byte minus[8] ={0x00,0x00,0x00,0x7E,0x7E,0x00,0x00,0x00,};
byte divide[8] = {0x18,0x18,0x00,0x7E,0x7E,0x00,0x18,0x18};
byte equal[8] = {0x00,0x7E,0x7E,0x00,0x00,0x7E,0x7E,0x00};

// Slot icon.
byte gesloten[8] ={0x00,0x18,0x24,0x24,0x7E,0x7E,0x7E,0x7E};
byte open[8] = {0x00,0x18,0x24,0x20,0x7E,0x7E,0x7E,0x7E};


// Getallen. (11, 20, 13, 65, 95)
byte elf[8] = {0x00,0x66,0x22,0x22,0x22,0x22,0x22,0x00};
byte twintig[8] = {0x00,0xF7,0x15,0xF5,0x85,0x85,0xF7,0x00};
byte derTien[8] = {0x00,0x6F,0x21,0x2F,0x21,0x21,0x2F,0x00};
byte vijfEnZestig[8] = {0x00,0xEF,0x88,0xEF,0xA1,0xA1,0xEF,0x00};
byte vijfEnNegintig[8] = {0x00,0x77,0x54,0x77,0x11,0x11,0x77,0x00};

// Feedback voor gebruiker.
byte check[8] = {0x00,0x01,0x03,0x06,0x8C,0xD8,0x70,0x20};
byte cross[8] ={0xC3,0xE7,0x7E,0x3C,0x3C,0x7E,0xE7,0xC3};

// Controler aanmaken voor de ledmatrix.
LedControl lc = LedControl(12, 10, 11, 0);

// Functie die wordt opgeroepen als de arduino wordt gestart.
void setup() {

    // Zet de ledmatrix aan, op volle felheid en wis alles.
    lc.shutdown(0, false);
    lc.setIntensity(0, 15);
    lc.clearDisplay(0);

    // Wijs de code de fysieke arduino pinnen toe.
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    servo.attach(9);

    // Zet het slot open
    servo.write(90);

    // Laat een open slot zien op de display.
    printByte(open);

    // Wacht 6 seconden.
    delay(6000);
    
    // Sluit het slot.
    servo.write(0);

    // Laat een gesloten slot zien op de display.
    printByte(gesloten);

    // Wacht 3 seconden.
    delay(3000);

}

// De hoofd functie van onze geo-chache.
void loop() {

    // Maak een while-loop zodat we gebruik kunnen maken van break.
    while (true)
    {
        // Bij elk nieuw spel zet het slot voor de zekerheid nog een keer op slot.
        servo.write(0);

        // Laat de eerste som zien.
        if(question(elf, plus, elf, 22) == false){

            // Als er fout is geantwoord gaan we uit de loop en beginnen we opnieuw.
            break;
        }

        // Laat de tweede som zien.
        if(question(vijfEnZestig, divide, derTien, 5) == false){
            
            // Als er fout is geantwoord gaan we uit de loop en beginnen we opnieuw.
            break;
        }

        // Laat de derde som zien.
        if(question(vijfEnNegintig, minus, vijfEnZestig, 30) == false){
            
            // Als er fout is geantwoord gaan we uit de loop en beginnen we opnieuw.
            break;
        }

        // Als je op dit punt van de loop komt heb je alles goed.

        // Laat een gesloten slot zien op de ledmatrix.
        printByte(gesloten);

        // Wacht 1 seconden.
        delay(1000);

        // Draai de servo naar 90 graden en laat een open slot zien op de ledmatrix.
        servo.write(90);
        printByte(open);

        // Wacht 30 seconden.
        delay(30000);

        // Stop de while-loop en begin opnieuw.
        break;

    }
    
}

// De functie die voor elke vraag wordt opgeroepen. Parameters(eerste getal, operator, tweede getal, correcte antwoord)
bool question(byte first[8], byte oper[8], byte second[8] , int answer){

    // Stel de fout marge voor het goede antwoord in.
    int maxx = answer + 4;
    int minn = answer - 4;

    // Laat het eerste getal zien en wacht 1.5 seconden.
    printByte(first);
    delay(1500);

    // Laat de operator zien en wacht 1.5 seconden.
    printByte(oper);
    delay(1500);

    // Laat het tweede getal zien en wacht 1.5 seconden.
    printByte(second);
    delay(1500);

    // Laat het = teken zien en wacht 5 seconden.
    printByte(equal);
    delay(5000);

    // Lees de door de gebruiker gegeven waarde uit. (in CM)
    int distance = getDistance();

    // Kijk of het antwoord tussen de fout marge past.
    if (distance < maxx && distance > minn) {

        // Zo ja, laat een check teken zien en wacht 3 seconden.
        printByte(check);
        delay(3000);

        // Return waar.
        return true;

    } else {

        // Zo nee, laat een kruis zien en wacht 3 seconden.
        printByte(cross);
        delay(3000);

        // Return onwaar.
        return false;
    }

}


// Functie voor het schrijven naar de ledmatrix. Parameters(beeld)
void printByte(byte character[]) {
    int i = 0;
    // Voor elke byte in de array.
    for (i = 0; i < 8; i++) {

        // Schrijf de waarde naar de ledmatrix.
        lc.setRow(0, i, character[i]);
    }
}

// Functie voor het ontvangen en berekenen van de afstand.
int getDistance () {

    // Maak de variabelen aan.
    long duration;
    int distance;

    // Schrijf een lage waarde naar de triger pin van de sensor.
    digitalWrite(trigPin, LOW);

    // Wacht 2 micro seconden.
    delayMicroseconds(2);

    // Schrijf een hoge waarde naar de triger pin van de sensor.
    digitalWrite(trigPin, HIGH);

    // Wacht 10 micro seconden.
    delayMicroseconds(10);

    // Schrijf een lage waarde naar de triger pin van de sensor.
    digitalWrite(trigPin, LOW);

    // Hoelang duurt het voordat de verstuurde pulse ontvangen wordt.
    duration = pulseIn(echoPin, HIGH);

    // Bereken de afstand doormiddel van een berekening. (duur x geluidssnelheid ÷ 2)
    distance = duration * 0.034 / 2;

    // Geef de afstand terug.
    return distance;
}