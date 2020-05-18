#include <SoftwareSerial.h>
#include <Keypad.h>
#include <Wire.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <DHT.h>
#include <EEPROM.h>
#define SS_PIN 53
#define RST_PIN 49
#define DHTPIN 11
#define DHTTYPE DHT22

const char trigPin0 = 7;
const char echoPin0 = 6;
const char trigPin1 = 5;
const char echoPin1 = 4;
const int ledCheck = 8;
int pulseWidth0;
int pulseWidth1;
int distance0;
int distance1;
int check0 = 0;
int check0_a = 0;
int check0_b = 0;
int check1 = 0;
int check1_a = 0;
int check1_b = 0;
int person = 0;
int initPulse = 0;

// ------------------------------------------------------------

int otpPasswd[4];
int pth = 0;
String str;

// ------------------------------------------------------------

int tru = 0;
int count = 0;
char key;
char secretCode[4];  // 비밀번호를 설정
const int ROWS = 4;    // 행(rows) 개수
const int COLS = 4;    // 열(columns) 개수
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {26, 27, 28, 29};   // R1, R2, R3, R4 단자가 연결된 아두이노 핀 번호
byte colPins[COLS] = {25, 24, 23, 22};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ------------------------------------------------------------

MFRC522 mfrc522(SS_PIN, RST_PIN); // Instance of the class
const int mainLED = 9;
int checkIn = 0;
int id_count = 0;
byte card[] = {54, 75, 152, 172};

// ------------------------------------------------------------

Servo myservo;

// ------------------------------------------------------------

DHT dht(DHTPIN, DHTTYPE);
int delayCount = 0;
float h;
float t;

// ------------------------------------------------------------


int gapMAC = 18;
int countMAC;
int minMAC, maxMAC;
String strMAC;

// ------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  SPI.begin();
  dht.begin();

// ------------------------------------------------------------

  randomSeed(analogRead(0));
  pinMode(39, OUTPUT);
  pinMode(40, OUTPUT);
  pinMode(41, OUTPUT);

// ------------------------------------------------------------

  pinMode(trigPin0, OUTPUT);
  pinMode(echoPin0, INPUT);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(ledCheck, OUTPUT);
  digitalWrite(trigPin0, LOW);
  digitalWrite(trigPin1, LOW);

// ------------------------------------------------------------

  myservo.attach(2); //서보모터 핀 설정
  myservo.write(0);

// ------------------------------------------------------------

  mfrc522.PCD_Init();
  Serial.println("RFID reading UID");
  pinMode(mainLED, OUTPUT);
}

// ------------------------------------------------------------

void loop() {
  // read from BLE (HM-10)
  if(Serial1.available() == 18) {
    Serial.print("add mac: ");
    str = Serial1.readString();
    countMAC = EEPROM.read(0);
    countMAC++;
    EEPROM.write(0, countMAC);
    Serial.println(EEPROM.read(0));
    minMAC = (countMAC - 1) * gapMAC + 1;
    maxMAC = countMAC * gapMAC;

    int j = 0;
    
    for(int i = minMAC; i <= maxMAC; i++) {
      EEPROM.write(i, str[j]);
      j++;
    }
    
    for(int i = minMAC; i <= maxMAC; i++) {
      Serial1.print((char)EEPROM.read(i));
      Serial.print((char)EEPROM.read(i));
    }
  }
  else if(Serial1.available()) {
    Serial.print("ble: ");
    str = Serial1.readString();
    Serial.print(str);
    countMAC = EEPROM.read(0);
    maxMAC = countMAC * gapMAC;

    if(str.equals("ALL_LED_ON\n")) {
      digitalWrite(39, HIGH);
      digitalWrite(40, HIGH);
      digitalWrite(41, HIGH);
    }
    else if(str.equals("ALL_LED_OFF\n")) {
      digitalWrite(39, LOW);
      digitalWrite(40, LOW);
      digitalWrite(41, LOW);
    }

    if(str.equals("LED0_ON\n")) {
      digitalWrite(39, HIGH);
    }
    else if(str.equals("LED0_OFF\n")) {
      digitalWrite(39, LOW);
    }

    if(str.equals("LED1_ON\n")) {
      digitalWrite(40, HIGH);
    }
    else if(str.equals("LED1_OFF\n")) {
      digitalWrite(40, LOW);
    }

    if(str.equals("LED2_ON\n")) {
      digitalWrite(41, HIGH);
    }
    else if(str.equals("LED2_OFF\n")) {
      digitalWrite(41, LOW);
    }
    
    if(str.equals("callP\n")) {
      //Serial1.print(person);
      //Serial.println(person);
    }
    
    if(str.equals("callT\n")) {
      t = dht.readTemperature();
  
      if(isnan(t)) {
        Serial.println("Failed to read from DHT");
      }
      else {
        Serial.print("Temperature: ");
        //Serial1.print(t);
        Serial.print(t);
        Serial.println(" *C\t");
      }
    }
    
    if(str.equals("callH\n")) {
      h = dht.readHumidity();
  
      if(isnan(h)) {
        Serial.println("Failed to read from DHT");
      }
      else {
        Serial.print("Humidity: ");
        //Serial1.print(h);
        Serial.print(h);
        Serial.println(" %");
      }
    }
    
    if(str.equals("getMAC\n")) {
      for(int i = 1; i <= maxMAC; i++) {
        Serial1.print((char)EEPROM.read(i));
        Serial.print((char)EEPROM.read(i));
      }
    }

    for(int i = 0; i < 10; i++) {
      if(str.equals(String(i) + '\n')) {
        int delStart = i * gapMAC + 1;
        
        for(int j = delStart; j <= maxMAC - gapMAC; j++) {
          EEPROM.write(j, EEPROM.read(j + gapMAC));
        }

        countMAC--;
        EEPROM.write(0, countMAC);
        Serial.println("countMAC " + String(countMAC));
      }
    }
    
    if(str.equals("passwd\n")) {
      for (int i = 0; i < 4; i++) {
        otpPasswd[i] = random(0, 9);
        Serial.print(otpPasswd[i]);
        Serial1.print(otpPasswd[i]);
        secretCode[i] = (char)otpPasswd[i] + 48;
      }
      Serial.print('\n');
      Serial1.print('\n');
    }
  }

  // read from USB (Arduino Terminal)
  if(Serial.available()) {
    Serial.print("usb: ");
    str = Serial.readString();
    Serial1.print(str);
    Serial.print(str);
  }

// ------------------------------------------------------------

  key = keypad.getKey(); // 키패드에서 입력된 값을 가져옵니다.
  
  if(key) {
    Serial.println(key);
    if(key == secretCode[count]) {
      count++;
      tru++;
    }
    else if (key != secretCode[count]) {
      count++;
    }
    if(key == '#') {    // # 누르면 초기화
      tru = 0;
      count = 0;
      Serial.println("password reset");
    }
    if(count == 4) {
      if(tru == 4) {
        myservo.write(90); //문 열고
        Serial.println("open the door");
        delay(3000);  // 3초 뒤
        myservo.write(0); // 문 닫음
      }
      else {
        Serial.println("incorrect password");
      }
      tru = 0;
      count = 0; 
    }
  }

// ------------------------------------------------------------

  if(mfrc522.PICC_IsNewCardPresent()) {
    if(mfrc522.PICC_ReadCardSerial()) {
      Serial.print("Tag UID:");
      for(byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        if(mfrc522.uid.uidByte[i] == card[i]) {
          count++;
        }
      }
  
      Serial.println();
      mfrc522.PICC_HaltA();
  
      if(count == 4) {
        if(!checkIn) {
          analogWrite(mainLED, 22);
          checkIn = 1;
        }
        else {
          analogWrite(mainLED, 0);
          checkIn = 0;
        }
      }
      else {
        Serial.println("등록되지 않은 카드 입니다.");
      }
      count = 0;
    }
  }

// ------------------------------------------------------------

  if(checkIn = 1) {
    digitalWrite(trigPin0, HIGH);
    delay(10);
    digitalWrite(trigPin0, LOW);
    pulseWidth0 = pulseIn(echoPin0, HIGH);
    
    digitalWrite(trigPin1, HIGH);
    delay(10);
    digitalWrite(trigPin1, LOW);
    pulseWidth1 = pulseIn(echoPin1, HIGH);
    
    distance0 = pulseWidth0 / 58;
    distance1 = pulseWidth1 / 58;
  
    //Serial.print(distance0);
    //Serial.println(" cm_0");
    //Serial.print(distance1);
    //Serial.println(" cm_1");
  
    if(initPulse == 0) {
      initPulse = 1;
    }
    else {
      // 0 에서 1(인원 증가)
      if (distance0 <= 10 && check0 == 0 && check1 == 0) {
        check0 = 1;
        check0_a = 1;
      }
      else if (distance1 <= 10 && check0 == 1) { // 0 에서 1
        check0_b = 1;
      }
      else if (distance1 > 10 && check0 == 1 && check0_a == 1 && check0_b == 1) {
        person++;
        check0 = 0;
        check0_a = 0;
        check0_b = 0;
        Serial.print("person = ");
        Serial.println(person);
      }
  
      // 1 에서 0(인원 감소)
      else if(distance1 <= 10 && check0 == 0 && check1 == 0) {
        check1 = 1;
        check1_a = 1;
      }
      else if(distance0 <= 10 && check1 == 1) { // 0 에서 1
        check1_b = 1;
      }
      else if(distance0 > 10 && check1 == 1 && check1_a == 1 && check1_b == 1) {
        if (person > 0) {
          person--;
        }
        check1 = 0;
        check1_a = 0;
        check1_b = 0;
        Serial.print("person = ");
        Serial.println(person);
      }
    }
  
    if(person >= 1) {
      digitalWrite(ledCheck, HIGH);
    }
    else if(person == 0) {
      digitalWrite(ledCheck, LOW);
    }
  }
  else {
    digitalWrite(ledCheck, LOW);
  }
}
