/*
  AUTOMATIC VEHICLE SPEED LIMITER
  TCS34725 + Arduino UNO + SSD1306 OLED + L293D

  TCS34725:
    SCL -> Arduino A5
    SDA -> Arduino A4
    VIN -> 5V
    GND -> GND

  OLED SSD1306 I2C:
    SCL -> Arduino A5
    SDA -> Arduino A4
    VCC -> 5V
    GND -> GND
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_TCS34725.h"

// ======================================================
// OLED CONFIGURATION
// ======================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

// ======================================================
// PIN DEFINITIONS
// ======================================================

#define LED1_PIN 4

#define V_PIN    A0
#define LEFT     A1
#define RIGHT    A2
#define SEN3_PIN A3

// L293D Motor A
#define IN1 10
#define IN2 9

// L293D Motor B
#define IN3 8
#define IN4 7

// L293D Enable pins
// Must be PWM pins on Arduino UNO
#define EN1 11
#define EN2 6

// ======================================================
// TCS34725 COLOR SENSOR
// ======================================================

Adafruit_TCS34725 tcs =
  Adafruit_TCS34725(
    TCS34725_INTEGRATIONTIME_50MS,
    TCS34725_GAIN_4X
  );

// ======================================================
// COLOR VARIABLES
// ======================================================

float red;
float green;
float blue;

// ======================================================
// MOTOR FUNCTIONS
// ======================================================

// Vehicle moves forward
void motorForward(int speedValue)
{
  analogWrite(EN1, speedValue);
  analogWrite(EN2, speedValue);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}


// Vehicle stops
void motorStop()
{
  analogWrite(EN1, 0);
  analogWrite(EN2, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}


// Vehicle turns LEFT
void turnLeft()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);

  Serial.println("LEFT");
}


// Vehicle turns RIGHT
void turnRight()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  Serial.println("RIGHT");
}


// ======================================================
// OLED DISPLAY FUNCTIONS
// ======================================================

void showLane1()
{
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("LANE 1");

  display.setCursor(0, 16);
  display.println("SPEED: 100 KM/H");

  display.setCursor(0, 32);
  display.println("DRIVING: LANE 1");

  display.display();
}


void showLane2()
{
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("LANE 2");

  display.setCursor(0, 16);
  display.println("SPEED: 80 KM/H");

  display.setCursor(0, 32);
  display.println("DRIVING: LANE 2");

  display.display();
}


void showLane3()
{
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("LANE 3");

  display.setCursor(0, 16);
  display.println("SPEED: 60 KM/H");

  display.setCursor(0, 32);
  display.println("DRIVING: LANE 3");

  display.display();
}


void showNoColor()
{
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("NO COLOR");

  display.setCursor(0, 16);
  display.println("VEHICLE STOP");

  display.display();
}


// ======================================================
// LEFT / RIGHT BUTTON CONTROL
// ======================================================

void checkTurning()
{
  if (digitalRead(LEFT) == HIGH)
  {
    turnLeft();
    delay(300);
  }
  else if (digitalRead(RIGHT) == HIGH)
  {
    turnRight();
    delay(300);
  }
}


// ======================================================
// SETUP
// ======================================================

void setup()
{
  Serial.begin(9600);

  // ----------------------------------------------------
  // I/O CONFIGURATION
  // ----------------------------------------------------

  pinMode(LED1_PIN, OUTPUT);

  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(V_PIN, INPUT);
  pinMode(LEFT, INPUT);
  pinMode(RIGHT, INPUT);
  pinMode(SEN3_PIN, INPUT);

  // ----------------------------------------------------
  // INITIAL MOTOR STATE
  // ----------------------------------------------------

  motorStop();

  // ----------------------------------------------------
  // OLED INITIALIZATION
  // ----------------------------------------------------

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C))
  {
    Serial.println("SSD1306 allocation failed");

    while (1)
    {
      // Stop here
    }
  }

  // ----------------------------------------------------
  // STARTUP DISPLAY
  // ----------------------------------------------------

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("AUTOMATIC VEHICLE");

  display.setCursor(0, 18);
  display.println("SPEED LIMITER");

  display.setCursor(0, 36);
  display.println("FOR EXPRESSWAYS");

  display.display();

  delay(3000);

  // ----------------------------------------------------
  // TCS34725 INITIALIZATION
  // ----------------------------------------------------

  if (tcs.begin())
  {
    Serial.println("TCS34725 SENSOR FOUND");

    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.println("COLOR SENSOR");

    display.setCursor(0, 18);
    display.println("SENSOR FOUND");

    display.display();

    delay(2000);
  }
  else
  {
    Serial.println("TCS34725 NOT FOUND");

    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.println("SENSOR NOT FOUND");

    display.setCursor(0, 18);
    display.println("CHECK CONNECTION");

    display.setCursor(0, 36);
    display.println("THEN RESET");

    display.display();

    while (1)
    {
      // Stop program
    }
  }

  Serial.println("SYSTEM READY");
}


// ======================================================
// MAIN LOOP
// ======================================================

void loop()
{
  // ----------------------------------------------------
  // READ COLOR SENSOR
  // ----------------------------------------------------

  tcs.setInterrupt(false);     // Turn sensor LED ON

  delay(60);

  tcs.getRGB(&red, &green, &blue);

  tcs.setInterrupt(true);      // Turn sensor LED OFF

  // ----------------------------------------------------
  // PRINT RGB VALUES
  // ----------------------------------------------------

  Serial.print("R = ");
  Serial.print(red);

  Serial.print("   G = ");
  Serial.print(green);

  Serial.print("   B = ");
  Serial.println(blue);


  // ====================================================
  // RED = LANE 1
  // SPEED = 100 KM/H
  // ====================================================

  if (
    red > 120 &&
    red > green &&
    red > blue
  )
  {
    Serial.println("COLOR = RED");

    motorForward(250);

    showLane1();

    checkTurning();
  }


  // ====================================================
  // BLUE = LANE 2
  // SPEED = 80 KM/H
  // ====================================================

  else if (
    blue > 120 &&
    blue > red &&
    blue > green
  )
  {
    Serial.println("COLOR = BLUE");

    motorForward(180);

    showLane2();

    checkTurning();
  }


  // ====================================================
  // GREEN = LANE 3
  // SPEED = 60 KM/H
  // ====================================================

  else if (
    green > 120 &&
    green > red &&
    green > blue
  )
  {
    Serial.println("COLOR = GREEN");

    motorForward(127);

    showLane3();

    checkTurning();
  }


  // ====================================================
  // NO RECOGNIZED COLOR
  // ====================================================

  else
  {
    Serial.println("COLOR = NOT RECOGNIZED");

    motorStop();

    showNoColor();
  }

  // Small delay for stable readings
  delay(50);
}