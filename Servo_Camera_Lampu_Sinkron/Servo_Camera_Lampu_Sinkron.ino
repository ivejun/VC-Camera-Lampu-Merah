#include <Adafruit_VC0706.h>
#include <SPI.h>
#include <SD.h>
#include <Servo.h>

#define SD_CS  53

const int lampu1[3] = {2, 3, 4};
const int lampu2[3] = {5, 6, 7};

const int lampumerah1 = 9;
const int lampumerah2 = 8;

int statelampumerah1 = 9;
int statelampumerah2 = 9;

Adafruit_VC0706 cam = Adafruit_VC0706(&Serial1);

unsigned long previousMillisServo = 0;
unsigned long previousMillisLampu = 0;

const long intervalLampuMerah1 = 1000;

Servo myservo;

bool statusservo = true;
int statuslampu1 = 0;

const int lampucam = 24;
const int lampucamera = 25;
const int buzzer = 26;
//pembolehan status lampu merah kanan atau kiri
bool lampustate1 = true;
int lampustatenow = 1;

void setup()
{
  //Serial.begin(9600);
  //LampuMerah
  for (int i = 0; i < 3; i++) {
    pinMode(lampu1[i], OUTPUT);
    pinMode(lampu2[i], OUTPUT);
  }
  //Servo
  myservo.attach(11);
  //Camera+SD
  pinMode(lampumerah1, INPUT);
  pinMode(lampumerah2, INPUT);
  pinMode(lampucamera, INPUT);
  pinMode(lampucam, INPUT);
  pinMode(buzzer, OUTPUT);

  if (!SD.begin(SD_CS)) {
    digitalWrite(lampucam, HIGH);
    Serial.println("SD failed or not present!");
    while (1);
  }

  // Start the camera
  if (!cam.begin()) {
    digitalWrite(lampucamera, HIGH);
    Serial.println("Camera failed or not present!");
    while (1);
  }

  cam.setImageSize(VC0706_640x480);
  delay(10000);
  digitalWrite(lampu2[2], HIGH);

}

void loop() {
  unsigned long currentMillis = millis();
  if (lampustate1) {
    statelampumerah1 = digitalRead(lampumerah1);
  }
  else if (!lampustate1) {
    statelampumerah2 = digitalRead(lampumerah2);
  }

  if (currentMillis - previousMillisLampu >= intervalLampuMerah1) {
    previousMillisLampu = currentMillis;
    if (lampustatenow == 1) {
      myservo.write(90);
      lampustate1 = true;
      fungsilampumerah1();
    }

    if (lampustatenow == 2) {
      myservo.write(0);
      lampustate1 = false;
      fungsilampumerah2();
    }
  }

  if (statelampumerah1 == 0) {
    takeCamera();
  }
  if (statelampumerah2 == 0) {
    takeCamera();
  }

}


void takeCamera() {
  if (cam.takePicture()) {
    digitalWrite(lampucamera, HIGH);
  } else {
  }

  char filename[13];
  strcpy(filename, "IMAGE00.JPG");
  for (int i = 0; i < 100; i++) {
    filename[5] = '0' + i / 10;
    filename[6] = '0' + i % 10;
    if (!SD.exists(filename)) {
      break;
    }
  }

  File imgFile = SD.open(filename, FILE_WRITE);

  uint16_t jpglen = cam.frameLength();

  while (jpglen > 0) {
    uint8_t *buff;
    uint8_t bytesToRead = min(32, jpglen);
    buff = cam.readPicture(bytesToRead);

    imgFile.write(buff, bytesToRead);
    jpglen -= bytesToRead;
  }

  imgFile.close();

  digitalWrite(lampucamera, LOW);
}

void fungsilampumerah1() {
  if (statuslampu1 >= 0 && statuslampu1 < 4) {
    statuslampu1 += 1;
    digitalWrite(lampu1[0], HIGH);
    digitalWrite(lampu1[1], LOW);
    digitalWrite(lampu1[2], LOW);
  }
  else if (statuslampu1 == 4) {
    statuslampu1 += 1;
    digitalWrite(lampu2[2], LOW);
    digitalWrite(lampu2[1], HIGH);

    digitalWrite(lampu1[0], LOW);
    digitalWrite(lampu1[1], HIGH);
    digitalWrite(lampu1[2], LOW);
  }
  else if (statuslampu1 > 4 && statuslampu1 < 8) {
    statuslampu1 += 1;
    digitalWrite(lampu1[0], LOW);
    digitalWrite(lampu1[1], LOW);
    digitalWrite(lampu1[2], HIGH);

    digitalWrite(lampu2[1], LOW);
    digitalWrite(lampu2[0], HIGH);
    if (statuslampu1 == 7) {
      statuslampu1 = 0;
      lampustatenow += 1;
    }
  }
}

void fungsilampumerah2() {
  if (statuslampu1 >= 0 && statuslampu1 < 4) {
    statuslampu1 += 1;

    digitalWrite(lampu2[0], HIGH);
    digitalWrite(lampu2[1], LOW);
    digitalWrite(lampu2[2], LOW);
  }
  else if (statuslampu1 == 4) {
    statuslampu1 += 1;
    digitalWrite(lampu1[2], LOW);
    digitalWrite(lampu1[1], HIGH);

    digitalWrite(lampu2[0], LOW);
    digitalWrite(lampu2[1], HIGH);
    digitalWrite(lampu2[2], LOW);
  }
  else if (statuslampu1 > 4 && statuslampu1 < 8) {
    statuslampu1 += 1;
    digitalWrite(lampu2[0], LOW);
    digitalWrite(lampu2[1], LOW);
    digitalWrite(lampu2[2], HIGH);

    digitalWrite(lampu1[0], HIGH);
    digitalWrite(lampu1[1], LOW);

    if (statuslampu1 == 7) {
      lampustatenow = 1;
      statuslampu1 = 0;
    }
  }
}

