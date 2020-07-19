

//door lock with knock based password
//for references, see project writeup

//#include <Servo.h>
#include <PWMServo.h>

const int sensor = 8;
const int servopin = 9;
PWMServo servo;
const int buzzer = 10;
const int progLED = 13;

const int lockedPos = 80;
const int unlockedPos = 180;
const int maxKnocks = 20;
const int deadzone = 100;
const int timeout = 1200;
const int slack = 200;
const byte recieveProgram = 'p';
const byte recieveUnlock = 'u';
const byte recieveLock = 'l';

const byte sendLocked = 'l';
const byte sendUnlocked = 'u';

int password[maxKnocks] = {1000, 500, 500, 1000, 2000, 1000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int passlength = 6;
int inputted[maxKnocks];
int curKnock = 0;
int numAttempts = 0;
unsigned long attemptsTimeout = 0;
bool programMode;


void setup() {
  pinMode(sensor, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(progLED, OUTPUT);
  servo.attach(servopin);
  servo.write(unlockedPos);
  delay(100);
  Serial.begin(9600);
  //Serial.print("start");
}

void loop() {

  if (Serial.available())
  {
    byte incoming = Serial.read();
    if (incoming == recieveProgram)
    {
      programMode = !programMode;
    }
    else if (incoming == recieveUnlock)
    {
      unlock();
    }
    else if (incoming == recieveLock)
    {
      lock();
    }
  }

  digitalWrite(progLED, programMode);

  if (digitalRead(sensor))
  {
    //Serial.println(passlength);
    if (programMode)
    {
      getInputted();
      setPassword();
    }
    else if (attemptsTimeout > millis())
    {
      //Serial.println(numAttempts);
      delay(200);
      digitalWrite(buzzer, HIGH);
      delay(400);
      digitalWrite(buzzer, LOW);
      delay(200);
      digitalWrite(buzzer, HIGH);
      delay(400);
      digitalWrite(buzzer, LOW);
      delay(100);
    }
    else
    {
      getInputted();
      checkPassword();
    }
  }

  if (numAttempts > 3)
  {
    attemptsTimeout = millis() + 60000;
    numAttempts--;
  }
}

void getInputted()
{
  unsigned long start = millis();
  curKnock = 0;
  int firstKnock = 0;
  for (int i = 0; i < maxKnocks; i++)
  {
    inputted[i] = 0;
  }

  delay(deadzone);

  while (((millis() - start) < timeout) && (curKnock < maxKnocks))
  {
    if (digitalRead(sensor))
    {
      if (curKnock == 0)
      {
        firstKnock = millis() - start;
        inputted[curKnock] = 1000;
      }
      else
      {
        inputted[curKnock] = ((millis() - start) * 1000) / firstKnock;
      }

      curKnock++;
      start = millis();
      delay(deadzone);
      /*
        for (int i = 0; i < maxKnocks; i++)
        {
        Serial.print(inputted[i]);
        Serial.print(", ");
        }
        Serial.print("\n");
      */
    }
  }//endwhile
}

void checkPassword()
{
  if (passlength != curKnock)
  {
    lock();
  }
  else
  {
    bool match = true;
    for (int i = 0; i < maxKnocks; i++)
    {
      match = match && (abs(inputted[i] - password[i]) < slack);
    }

    if (match)
    {
      unlock();
    }
    else
    {
      lock();
    }
  }
}

void lock()
{
  numAttempts++;
  servo.write(lockedPos);
  Serial.write(sendLocked);
  digitalWrite(buzzer, HIGH);
  delay(400);
  digitalWrite(buzzer, LOW);
  delay(100);
}

void unlock()
{
  numAttempts = 0;
  servo.write(unlockedPos);
  Serial.write(sendUnlocked);
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);
  delay(100);
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);
  delay(3100);
}

void setPassword()
{
  if (inputted[1] != 0)
  {
    passlength = curKnock;
    for (int i = 0; i < maxKnocks; i++)
    {
      password[i] = inputted[i];
    }
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    delay(100);
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    delay(100);
  }
  else
  {
    digitalWrite(buzzer, HIGH);
    delay(400);
    digitalWrite(buzzer, LOW);
    delay(100);
  }
}
