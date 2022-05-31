/*
  :::::::::::::::::::::::::::::::::::::::::::::::
  ' ##:::'##:'########:'########'##::::'##:'####::
  : ##::'##:: ##.... ##::: ##::: ##:..  ##:. ##:::
  : ##:'##::: ##:::: ##::: ##::: ####.####:: ##:::
  : #####:::: ########:::: ##::: ##.###:##:: ##:::
  : ##. ##::: ##.. ##::::: ##::: ##: #. ##:: ##:::
  : ##:. ##:: ##::. ##:::: ##::: ##:..' ##:: ##:::
  : ##::. ##: ##:::. ##::: ##::: ##:::: ##: ####::
  ..::::..::..:::::..:::......:::........:::....::
  ----------=Bismillah KRI 23 Aug 2021=-----------
  :::::::::::dev code by: KRTMI POLIJE::::::::::::
         -Kontes Robot Indonesia 2021-
*/

#include <DynamixelSerial.h>
#include <PS2X_lib.h>
#include <Servo.h>

#include <Wire.h>

#define CMPS11_ADDRESS 0x60  // Address of CMPS11 shifted right one bit for arduino wire library
#define ANGLE_8  1           // Register to read 8bit angle from

unsigned char high_byte, low_byte, angle8;
char pitch, roll;
unsigned int angle16;
int sudut, hold;

Servo servo1;
Servo servo2;

#define M1A 8
#define M1B 9
#define M2A 7
#define M2B 6
#define M3A 4
#define M3B 5
#define M4A 2
#define M4B 3
#define MAP(x)   (int)map(x, 0, 300, 0, 1023)
int waktu1 = 600,
    waktu2 = 150,
    waktu3 = 80,
    waktu4 = 20;

float Vx,
      Vy,
      w;

float WHEEL_BASE = 9.25, //Dari Roba Ke Roda
      WHEEL_RADIUS = 3; //Ukuran Titik tengah roda

int pin_servo1 = 12;
int pin_servo2 = 13;

//const int e18 = 10;
int LED = 11;
bool Control = false;

struct
{
  int Ctrl = 0;
  bool Push = false;
} Green, Red, Pink, Blue, R3, L2, Start, RL1, Select;

int prestate = 0;

int motor_speed_plus = 21;
int motor_speed_min  = -21;
int diagonal_speed = 9;

int error = 0,
    tambahan = 5;
byte type = 0,
     vibrate = 0;
PS2X ps2x;

void indikator() {
//  pinMode(e18, INPUT);
  pinMode(LED, OUTPUT);
}

void inisiasi_motor() {
  pinMode(M1A  , OUTPUT); pinMode(M1B  , OUTPUT);
  pinMode(M2A  , OUTPUT); pinMode(M2B  , OUTPUT);
  pinMode(M3A  , OUTPUT); pinMode(M3B  , OUTPUT);
  pinMode(M4A  , OUTPUT); pinMode(M4B  , OUTPUT);
}

void inisiasi_dynamixel() {
  Dynamixel.begin(1000000, 2);
  Serial1.begin(115200);
}

void inisiasi_joystick() {
  error = ps2x.config_gamepad( 52, 51, 53, 50, false, false);
  if (error == 0) {
    Serial.println("Stik terhubung");
  }
}

void inisiasi_servo() {
  servo1.attach(pin_servo1);
  servo2.attach(pin_servo2);
}

/* ========================================================================================================================================= */

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  inisiasi_dynamixel();
  inisiasi_joystick();
  inisiasi_servo();
  indikator();
  control(0, 0, 0);
  servoDepanZero();
  servoBelakangZero();
  dynamixel_atas();
  liftUp();
  delay(500);
  capit();

   Wire.begin();
   hold = 8;
}

void loop() {
  ps2x.read_gamepad(false, vibrate);
  jalan();
  if (hold == 0){
    //ps2x.read_gamepad(false, vibrate);
    //jalan();
    nyacapit();
    bacakompas();
  } else if (hold ==1) {
    control(0, motor_speed_plus, 0);
    delay(600);
    hold=2;} 
   else if (hold ==2) {
    control(0, 0, -7);
    bacakompas();
    if (sudut> 60){control(0, 0, 0);hold =0;}
  }
  //Serial.println("ok");
  delay(50); //Delay

  //control (10,-18,0);
//    trial_servo(3);
}

void bacakompas(){
  Wire.beginTransmission(CMPS11_ADDRESS);  //starts communication with CMPS11
  Wire.write(ANGLE_8);                     //Sends the register we wish to start reading from
  Wire.endTransmission();
 
  // Request 5 bytes from the CMPS11
  // this will give us the 8 bit bearing, 
  // both bytes of the 16 bit bearing, pitch and roll
  Wire.requestFrom(CMPS11_ADDRESS, 5);       
  
  while(Wire.available() < 5);        // Wait for all bytes to come back
  
  angle8 = Wire.read();               // Read back the 5 bytes
  high_byte = Wire.read();
  low_byte = Wire.read();
  pitch = Wire.read();
  roll = Wire.read();

  sudut = angle8;
  //Serial.println(sudut);
}
