#include <Wire.h>
#include <U8g2lib.h>
#include <Preferences.h>
Preferences prefs;

char loopMsg[16] = "NORMAL";
unsigned long lastDisplayUpdate = 0;

unsigned long raceStartTime = 0;
// OLED 128x32
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

// Buttons
#define BTN_UP 12
#define BTN_DOWN 23
#define BTN_SELECT 25

#define sensorNumber 8

//motorpins
#define left_motor_forward 32   //IN3(L298) //TB6612 --- AIN1
#define left_motor_backward 33   //IN4(L298) //TB6612 ---AIN2

#define right_motor_forward 26  //IN1(L298) //TB6612 -- BIN1
#define right_motor_backward 27 //IN2(L298) //TB6612 -- BIN2
//
#define left_motor_speed 14  //EnB (L298) //TB6612 --- PWMA
#define right_motor_speed 13  //EnA (L298) //TB6612 --- PWMB

// Connect your 8 IR sensor outputs here
int sensorPins[sensorNumber] = {19 , 18, 5, 17, 16, 4, 2, 15};
int sensorADC[sensorNumber];
int WeightValue[sensorNumber] = {-35, -25, -15, -5, 5, 15, 25, 35};
int bitWeight[sensorNumber] = { 1, 2, 4, 8, 16, 32, 64, 128 };
int sumOnSensor;
int sensorWight;
int bitSensor;

//PID Variables
float line_position;
float error;
float center_position = 0;
float derivative, previous_error;
int base_speed = 160;
float kp = 7;
float kd = 7;
int last_turn = 0;
int lostLineCounter = 0;
int loopState = 0;

const char* menuItems[] =
{
  "Start Run",
  "Calibrate",
  "Motor Test",
  "KP",
  "KD",
  "Base Speed"
};

int menuSize = 6;
int cursor = 0;
int topItem = 0;
enum RobotMode
{
  MENU_MODE,
  RUN_MODE,
  CALIB_MODE,
  MOTOR_TEST_MODE
};

RobotMode mode = MENU_MODE;

void setup() {

  Serial.begin(115200);

  for (int i = 0; i < sensorNumber; i++) {
  pinMode(sensorPins[i], INPUT_PULLUP);
  }

  pinMode(left_motor_forward, OUTPUT);
  pinMode(left_motor_backward, OUTPUT);
  pinMode(right_motor_forward, OUTPUT);
  pinMode(right_motor_backward, OUTPUT);

  pinMode(left_motor_speed, OUTPUT);
  pinMode(right_motor_speed, OUTPUT);

  Wire.begin(21,22);
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x12_tr);

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);

  prefs.begin("pid", false);
  kp = prefs.getFloat("kp", 7.0);
  kd = prefs.getFloat("kd", 7.0);
  base_speed = prefs.getInt("bs", 160);

}

void loop()
{
  switch(mode)
  {
    case MENU_MODE:
      readButtons();
      drawMenu();
      break;

    case RUN_MODE:
      PID_Controller(base_speed, kp, kd);
      break;

    case CALIB_MODE:
      calibrate();
      break;

    case MOTOR_TEST_MODE:
      motorTest();
      break;
  }
}

void executeMenu()
{
  switch(cursor)
  {
    case 0:
    Serial.println("RUN MODE STARTED");
    showRaceScreen();
    waitRelease();

    // reset runtime variables
    previous_error = 0;
    lostLineCounter = 0;
    loopState = 0;
    last_turn = 0;

    raceStartTime = millis();
    mode = RUN_MODE;
    break;

    case 1:
      mode = CALIB_MODE;
      break;

    case 2:
      waitRelease();
      mode = MOTOR_TEST_MODE;
      break;

    case 3:
      adjustKP();
      break;

    case 4:
      adjustKD();
      break;

    case 5:
      adjustSpeed();
      break;
  }

}

void read_black_line() {

  sumOnSensor = 0;
  sensorWight = 0;
  bitSensor = 0;

  // Read all sensors first
  for (int i = 0 ; i <sensorNumber; i++) {
    sensorADC[i] = readStable(sensorPins[i]);

    sumOnSensor += sensorADC[i];
    sensorWight += sensorADC[i] * WeightValue[i];
    bitSensor += sensorADC[i] * bitWeight[i];
  }

  // ---- PRINT HORIZONTALLY ----

  /*for (int i = 0; i < sensorNumber; i++) {
    Serial.print(sensorADC[i]);
    Serial.print(" ");
  }

  Serial.print("   ");  // space gap

  Serial.print(sumOnSensor);
  Serial.print("   ");

  Serial.print(sensorWight);
  Serial.print("   ");

  Serial.print(bitSensor);

  Serial.println();   // move to next line

  delay(50);*/
}


  


int readStable(int pin) {
  int count = 0;

  // Take 5 fast readings
  for (int i = 0; i <5; i++) {
    count += digitalRead(pin);
  }

  // Majority decision
  if (count >= 3)
    return 1;
  else
    return 0;
}

/////////////////////////////////sensor part ends/////////////////////////////////////

 void motor(int LPWM, int RPWM) {
  if (LPWM > 0) {
    digitalWrite(left_motor_forward, HIGH);
    digitalWrite(left_motor_backward, LOW);
  } else {
    digitalWrite(left_motor_forward, LOW);
    digitalWrite(left_motor_backward, HIGH);
  }

  if (RPWM > 0) {
    digitalWrite(right_motor_forward, HIGH);
    digitalWrite(right_motor_backward, LOW);
  } else {
    digitalWrite(right_motor_forward, LOW);
    digitalWrite(right_motor_backward, HIGH);
  }

  LPWM = constrain(LPWM, -255, 255);
  RPWM = constrain(RPWM, -255, 255);

  //Serial.print(String(LPWM) + ",  " + String(RPWM) + ",  ");
  //Serial.println();

  analogWrite(left_motor_speed, abs(LPWM));
  analogWrite(right_motor_speed, abs(RPWM));
}


void motorTest()
{
  int speed = 150;

  while(1)
  {
    u8g2.clearBuffer();

    u8g2.drawStr(0,10,"MOTOR TEST");

    if(digitalRead(BTN_UP)==LOW)
    {
      motor(speed,speed);
      u8g2.drawStr(0,25,"FWD");
    }
    else if(digitalRead(BTN_DOWN)==LOW)
    {
      motor(-speed,-speed);
      u8g2.drawStr(0,25,"REV");
    }
    else
    {
      motor(0,0);
      u8g2.drawStr(0,25,"STOP");
    }

    char buf[20];
    sprintf(buf,"SPD %d",speed);

    u8g2.drawStr(60,25,buf);

    u8g2.sendBuffer();

    if(digitalRead(BTN_SELECT)==LOW)
    {
      motor(0,0);
      delay(300);
      mode = MENU_MODE;
      return;
    }
  }
}

///////////////////////////motor code ends here/////////////////////////////////////////////////////////////////
void calibrate()
{
  read_black_line();

  u8g2.clearBuffer();

  u8g2.drawStr(0,10,"CALIBRATION");

  char buf[20];
  sprintf(buf,"%d %d %d %d %d %d %d %d",
  sensorADC[0],sensorADC[1],sensorADC[2],sensorADC[3],
  sensorADC[4],sensorADC[5],sensorADC[6],sensorADC[7]);

  u8g2.drawStr(0,25,buf);

  u8g2.sendBuffer();

  if(digitalRead(BTN_SELECT)==LOW)
  {
    mode = MENU_MODE;
    delay(300);
  }
}
//////////////////////////////PID starts/////////////////////////////////////////////////////////////////////////////


void PID_Controller(int base_speed, float p, float d) {

  if(millis() - raceStartTime > 800)
{
  if(digitalRead(BTN_SELECT) == LOW)
  {
      motor(0,0);
      mode = MENU_MODE;
      delay(300);
      return;
  }
}
  read_black_line();
  updateRaceDisplay();

  if(loopState == 0 && (bitSensor & 0b00111100) == 0b00111100)
  {
      Serial.println("ENTER LOOP");
      strcpy(loopMsg, "LOOP ENTRY");
      loopState = 1;
  }


    if(loopState == 1 && (bitSensor & 0b01111110) == 0b01111110)
  {
      Serial.println("INSIDE LOOP");
      strcpy(loopMsg, "IN LOOP");
      loopState = 2;
  }

  if(loopState == 2 && (bitSensor & 0b00011000))
  {
      Serial.println("EXIT LOOP");
      strcpy(loopMsg, "LOOP EXIT");
      delay(30);

      motor(base_speed + 60, base_speed + 60);
      delay(30);

      loopState = 0;
      previous_error = 0;

      return;
  }

    // 🔹 reset to normal state
  if(loopState == 0 && (bitSensor & 0b00011000) && sumOnSensor <= 3)
  {
      strcpy(loopMsg, "NORMAL");
  }

  bool hardCurve = false;

  if((bitSensor & 0b11100000) == 0b11100000 || 
    (bitSensor & 0b00000111) == 0b00000111)
  {
      hardCurve = true;
  }

  
  
 
  // ===== 90 DEGREE Right DETECTION =====
  if((bitSensor & 0b11111000) && !(bitSensor & 0b00011000)){
  last_turn = 1;
  Serial.println("90 RIGHT DETECTED");

  // STEP 1: Move forward slightly into the corner
  motor(base_speed, base_speed);
  delay(10);   // Tune between 60–120 ms

  // STEP 2: Rotate in place (right turn)
  unsigned long startTurn = millis();

  while (millis() - startTurn < 300) {

      read_black_line();

      motor(170, -170);

      if (bitSensor & 0b00011000) {   // center sensors detect line
          previous_error = 0;
          break;
      }
  }
  // stabilization
    motor(base_speed, base_speed);
    delay(10);

    return;

}


//////////////////////90 degree left dectection////////////////////
if( (bitSensor & 0b00011111) && !(bitSensor & 0b00011000)) {
  last_turn = -1;
  Serial.println("90 LEFT DETECTED");

  // STEP 1: Move forward a bit into corner                   //loopState == 0 &&
  motor(base_speed, base_speed);
  delay(10);   // tune this (50–120ms depending on speed)

  // STEP 2: Rotate
    unsigned long startTurn = millis();

  while (millis() - startTurn < 300) {

      read_black_line();

      motor(-170, 170);

      if (bitSensor & 0b00011000) {
          previous_error = 0;
          break;
      }
  }
  // stabilization
    motor(base_speed, base_speed);
    delay(10);

    return;
}



    // ===== JUNCTION FILTER =====
  if(bitSensor == 0b11111111 )
  {
    Serial.println("JUNCTION DETECTED");

    motor(base_speed, base_speed);
    delay(15);

    return;
  }


///////////////lost line///////////////
if(sumOnSensor == 0)
{
    lostLineCounter++;

    // tolerate dotted gaps
    if(lostLineCounter < 6 )
    {
        motor(base_speed +50, base_speed +50);
        return;
    }

    // real lost line
    if(last_turn == 1)
    {
        motor(170, -170);
    }
    else if(last_turn == -1)
    {
        motor(-170, 170);
    }
    else
    {
        if(previous_error > 0)
            motor(170, -170);
        else
            motor(-170, 170);
    }

    return;
}
else
{
    lostLineCounter = 0;
}
   
 


  // ===== NORMAL PID =====
  if (sumOnSensor > 0)
    line_position = sensorWight / sumOnSensor;

  error = line_position - center_position;
  

   // ===== LINE REACQUISITION LOCK =====
  if(bitSensor & 0b00011000 && abs(error) < 5)
  {
      motor(base_speed + 20, base_speed + 20);
      delay(5);
  }

  derivative = (error - previous_error) * 0.7;

int prediction = 0;

  if(bitSensor & 0b11000000)
      prediction = 5;
  else if(bitSensor & 0b00000011)
      prediction = -5;

error += prediction;


  // reset corner memory when robot is centered
  if(abs(error) < 3)
  {
    last_turn = 0;
  }

  int dynamic_speed;

  dynamic_speed = base_speed + 40 - abs(error)*2 - abs(derivative)*3;

  
  // Straight turbo
  if(abs(error) < 3)
  {
      dynamic_speed = base_speed ;
  }

      if(loopState != 0)
  {
      dynamic_speed -= 40;
  }

  // Hard curve slowdown
  if(hardCurve)
  {
    dynamic_speed -= 40;
  }

    
  dynamic_speed = constrain(dynamic_speed, 80, 230);

  int correction = (error * p + derivative * d);

  correction = constrain(correction, -130, 130);

  int right_motor_correction = dynamic_speed - correction;
  int left_motor_correction  = dynamic_speed + correction;

   
  previous_error = error;

  motor(left_motor_correction, right_motor_correction);
}
//////////////////////////////////PID ENDS HERE//////////////////////////////////////////////////////////
void readButtons()
{
  if(digitalRead(BTN_UP) == LOW)
  {
    cursor--;

    if(cursor < 0)
      cursor = 0;

    if(cursor < topItem)
      topItem--;

    delay(180);
  }

  if(digitalRead(BTN_DOWN) == LOW)
  {
    cursor++;

    if(cursor >= menuSize)
      cursor = menuSize - 1;

    if(cursor > topItem + 2)
      topItem++;

    delay(180);
  }

  if(digitalRead(BTN_SELECT) == LOW)
    {
        executeMenu();

        while(digitalRead(BTN_SELECT) == LOW);  // wait release
        delay(150);
    }
}

void drawMenu()
{
  u8g2.clearBuffer();

  for(int i = 0; i < 3; i++)
  {
    int item = topItem + i;

    if(item >= menuSize)
      break;

    if(item == cursor)
      u8g2.drawStr(0,(i+1)*10,">");

    u8g2.drawStr(10,(i+1)*10,menuItems[item]);
  }

  u8g2.sendBuffer();
}

void adjustKP()
{
  while(digitalRead(BTN_SELECT) == LOW);
  while(1)
  {
    if(digitalRead(BTN_UP)==LOW)
    {
      kp += 0.5;
      delay(150);
    }

    if(digitalRead(BTN_DOWN)==LOW)
    {
      kp -= 0.5;
      delay(150);
    }

    u8g2.clearBuffer();

    char buf[20];
    sprintf(buf,"KP %.1f",kp);

    u8g2.drawStr(0,15,buf);
    u8g2.drawStr(0,30,"SEL EXIT");

    u8g2.sendBuffer();

    if(digitalRead(BTN_SELECT)==LOW)
    {
        prefs.putFloat("kp", kp);
        delay(250);
        return;
    }
  }
}


void adjustKD()
{
  while(digitalRead(BTN_SELECT) == LOW);
  while(1)
  {
    if(digitalRead(BTN_UP)==LOW)
    {
      kd += 0.5;
      delay(150);
    }

    if(digitalRead(BTN_DOWN)==LOW)
    {
      kd -= 0.5;
      delay(150);
    }

    u8g2.clearBuffer();

    char buf[20];
    sprintf(buf,"KD %.1f",kd);

    u8g2.drawStr(0,15,buf);
    u8g2.drawStr(0,30,"SEL EXIT");

    u8g2.sendBuffer();

    if(digitalRead(BTN_SELECT)==LOW)
    {
        prefs.putFloat("kd", kd);
        delay(250);
        return;
    }
  }
}

void adjustSpeed()
{
  while(digitalRead(BTN_SELECT) == LOW);
  while(1)
  {
    if(digitalRead(BTN_UP)==LOW)
    {
      base_speed += 5;
      delay(150);
    }

    if(digitalRead(BTN_DOWN)==LOW)
    {
      base_speed -= 5;
      delay(150);
    }

    base_speed = constrain(base_speed,80,230);

    u8g2.clearBuffer();

    char buf[20];
    sprintf(buf,"Speed %d",base_speed);

    u8g2.drawStr(0,15,buf);
    u8g2.drawStr(0,30,"SEL EXIT");

    u8g2.sendBuffer();

    if(digitalRead(BTN_SELECT)==LOW)
    {
        prefs.putInt("bs", base_speed);
        delay(250);
        return;
    }
  }
}

void showRaceScreen()
{
 
  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_6x12_tr);

  u8g2.drawStr(20,12,"RACING");

  char buf1[20];
  sprintf(buf1,"KP %.1f",kp);
  u8g2.drawStr(0,26,buf1);

  char buf2[20];
  sprintf(buf2,"KD %.1f",kd);
  u8g2.drawStr(64,26,buf2);

  u8g2.sendBuffer();

  delay(800);
}

void waitRelease()
{
  while(digitalRead(BTN_SELECT) == LOW);
  delay(150);
}

void updateRaceDisplay()
{
    if(millis() - lastDisplayUpdate < 120) return;

    lastDisplayUpdate = millis();

    u8g2.clearBuffer();

    char buf1[20];
    sprintf(buf1,"KP %.1f",kp);
    u8g2.drawStr(0,12,buf1);

    char buf2[20];
    sprintf(buf2,"KD %.1f",kd);
    u8g2.drawStr(64,12,buf2);

    u8g2.drawStr(0,28,loopMsg);

    u8g2.sendBuffer();
}