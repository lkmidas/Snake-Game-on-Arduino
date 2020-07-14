#include <MD_UISwitch.h>
#include <MD_MAX72xx.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
// Text scrolling definitions
#define SCROLL_DELAY 50 // in milliseconds
#define BUF_SIZE     75
#define CHAR_SPACING 1  // pixels between characters
// Key matrix definitions
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   13  // or SCKc
#define DATA_PIN  11  // or MOSI
#define CS_PIN    10  // or SS

// Keypad   
#define VRX_PIN     0
#define VRY_PIN     1
#define SW_PIN      2
// App states
#define MENU_1      1
#define MENU_2      2
#define MENU_DIFF   3
#define MENU_SCORE  4
#define MENU_BORDER 5
#define GAME        6
#define PAUSE       7
#define WORK_CYCLE  50 // in milliseconds
// Keypad variables
char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'},
};
char columnPins[4] = {3, 2, 1, 0};
char rowPins[4] = {4, 5, 6, 7};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, columnPins, 4, 4); // Instantiate keypad
LiquidCrystal_I2C lcd(0x27, 16, 2);                                  // Instantiate I2C LCD
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);      // Instantiate LED matrix MAX72XX

char curMessage[BUF_SIZE];

uint8_t  key = 0;
uint8_t  prev;
uint8_t  joystick = 4;
uint32_t prevMillis = 0;
uint8_t  moveCount = 0;
uint8_t  snakeLength = 1;
uint8_t  snakeCoord[200][2];
uint8_t  foodPoint[5][2] = {{10, 5}, {3, 1}, {25, 3}, {15, 7}, {11, 3}};
uint8_t  gameState = MENU_1;
uint16_t record = 0;
uint16_t score = 0;
uint8_t  difficulty = 1;
uint8_t  borderType = 0;
uint8_t  gameSpeed;

bool isLost = false;

bool inhibitUp = false;
bool inhibitDown = false;
bool inhibitLeft = false;
bool inhibitRight = false;

bool isLeft = true;
bool isRight = false;
bool isUp = false;
bool isDown = false;

uint8_t veloX = 0;
uint8_t veloY = 0;

uint8_t scrollDataSource(uint8_t dev, MD_MAX72XX::transformType_t t){
  // Callback function for data that is required for scrolling into the display
  static char *p = curMessage;
  static uint8_t state = 0;
  static uint8_t curLen, showLen;
  static uint8_t cBuf[8];
  uint8_t colData;

  // finite state machine to control what we do on the callback
  switch (state){
    case 0: // Load the next character from the font table
      showLen = mx.getChar(*p++, sizeof(cBuf)/sizeof(cBuf[0]), cBuf);
      curLen = 0;
      state++;
      // if we reached end of message, reset the message pointer
      if (*p == '\0' || isLost){
        isLost = false;
        p = curMessage; // reset the pointer to start of message
      }
      // !!no break, deliberately fall through to next state to start displaying
    case 1: // display the next part of the character
      colData = cBuf[curLen++];
      if (curLen == showLen){
        showLen = CHAR_SPACING;
        curLen = 0;
        state = 2;
      }
      break;
    case 2: // display inter-character spacing (blank column)
      colData = 0;
      curLen++;
      if (curLen == showLen){
        state = 0;
      }
      break;
    default:
      state = 0;
  }
  return colData;
}

void scrollText(void){
  static uint32_t prevTime = 0;
  // Is it time to scroll the text?
  if (millis() - prevTime >= SCROLL_DELAY){
    mx.transform(MD_MAX72XX::TSL); // scroll along - the callback will load all the data
    prevTime = millis(); // starting point for next time
  }
}

void textDisplay(void){
  mx.setShiftDataInCallback(scrollDataSource);
  strcpy(curMessage, "The Snake Game!   ");
  scrollText();
}

void setLed(int x, int y){
  mx.setPoint(y-1, x-1, 1);
  return;
}

void scanKeyPad(void){
  char temp = keypad.getKey();
  if (key != temp && temp != 0){
    key = temp;
  } else {
    key = 0;
  }
}

void scanJoystick(void){
  if (analogRead(VRY_PIN) > 600){ // up
    prev = 1;
    joystick = 1;
  }
  if (analogRead(VRY_PIN) < 120){ // down
    prev = 0;
    joystick = 0;
  }
  if (analogRead(VRX_PIN) > 600){ // left
    prev = 2;
    joystick = 2;
  }
  if (analogRead(VRX_PIN) < 120){ // right
    prev = 3;
    joystick = 3;
  }
  if (analogRead(SW_PIN) == 0){   // click
    joystick = 5;
  }
  joystick = prev;
}

void slide(void){
  for (int x = snakeLength; x > 0; x--){
    snakeCoord[x][0] = snakeCoord[x-1][0];
    snakeCoord[x][1] = snakeCoord[x-1][1];
  }
}

void gameLoop(void){
  //lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Score: ");
  lcd.print(score);
  lcd.setCursor(0, 1);
  switch (difficulty){
    case 1:
      lcd.print("Easy");
      break;
    case 2:
      lcd.print("Normal");
      break;
    case 3:
      lcd.print("Hard");
      break;
    case 4:
      lcd.print("Hell");
      break;
  }
  moveCount++;
  // Slide according to gameSpeed
  if (moveCount == gameSpeed){
    moveCount = 0;
    slide();
  }
  // Movement control
  if (!isDown){
    if (joystick == 0){
      isLeft = false;
      isRight = false;
      isUp = true;
      isDown = false;
      if (!inhibitUp){
        veloY = 1;
        veloX = 0;
        inhibitUp = true;  
      }  
    } else {
      inhibitUp = false;
    }
  }
  if (!isUp){
    if (joystick == 1){
      isLeft = false;
      isRight = false;
      isUp = false;
      isDown = true;  
      if (!inhibitDown){
        veloY = -1;
        veloX = 0;
        inhibitDown = true;  
      }  
    } else {
      inhibitDown = false;
    }
  }
  if (!isLeft){
    if (joystick == 2){
      isLeft = false;
      isRight = true;
      isUp = false;
      isDown = false;
      if (!inhibitRight){
        veloX = 1;
        veloY = 0;
        inhibitRight = true;  
      }  
    } else {
      inhibitRight = false;
    }
  }
  if (!isRight){
    if (joystick == 3){
      isLeft = true;
      isRight = false;
      isUp = false;
      isDown = false;
      if (!inhibitLeft){
        veloX = -1;
        veloY = 0;
        inhibitLeft = true;  
      }
    } else {
      inhibitLeft = false;
    }
  }
  snakeCoord[0][1] = snakeCoord[1][1] + veloY;
  snakeCoord[0][0] = snakeCoord[1][0] + veloX;
  // Eating control
  for (int x = 0; x < 5; x++){
    if (snakeCoord[0][0] == foodPoint[x][0] && snakeCoord[0][1] == foodPoint[x][1]){
      snakeLength++;
      score += 1;
      foodPoint[x][0] = random(1, 32);
      foodPoint[x][1] = random(1, 8);
    }
  }
  mx.clear();
  // Print snek
  for (int x = 0; x < snakeLength; x++){
    setLed(snakeCoord[x][0], snakeCoord[x][1]);
  }
  // Print food
  for (int x = 0; x < 5; x++){
    setLed(foodPoint[x][0], foodPoint[x][1]);
  }
  // Teleport at border
  if (borderType == 0){
    if (snakeCoord[0][0] == 0){
      snakeCoord[0][0] = 32;
    }
    if (snakeCoord[0][0] == 33){
      snakeCoord[0][0] = 0;
    }
    if (snakeCoord[0][1] == 0){
      snakeCoord[0][1] = 8;
    }
    if (snakeCoord[0][1] == 9){
      snakeCoord[0][1] = 0;
    }
  }
}

void stateMachine(void){
  switch (gameState){
    case MENU_1:
      //mx.clear();
      textDisplay();
      lcd.setCursor(0, 0);
      lcd.print("1. Start game");
      lcd.setCursor(0, 1);
      lcd.print("2. Difficulty   ");
      if (key == '1'){
        mx.clear();
        lcd.clear();
        snakeCoord[1][0] = snakeCoord[0][0] = 5;
        snakeCoord[1][1] = snakeCoord[0][1] = 5;
        gameSpeed = 5 - difficulty;
        snakeLength = 1;
        score = 0;
        gameState = GAME;
      }
      if (key == '2'){
        lcd.clear();
        gameState = MENU_DIFF;
      }
      if (key == 'B'){
        lcd.clear();
        gameState = MENU_2;
      }
      break;
    case MENU_2:
      textDisplay();
      lcd.setCursor(0, 0);
      lcd.print("3. Set border");
      lcd.setCursor(0, 1);
      lcd.print("4. Score");
      if (key == '3'){
        lcd.clear();
        gameState = MENU_BORDER;
      }
      if (key == '4'){
        lcd.clear();
        gameState = MENU_SCORE;
      }
      if (key == 'B'){
        lcd.clear();
        gameState = MENU_1;
      }
      break;
    case MENU_BORDER:
      textDisplay();
      lcd.setCursor(0, 0);
      lcd.print("1. Bordered");
      lcd.setCursor(0, 1);
      lcd.print("2. Borderless");
      if (key == '1'){
        borderType = 1;
        lcd.clear();
        gameState = MENU_1;
      }
      if (key == '2'){
        borderType = 0;
        lcd.clear();
        gameState = MENU_1;
      }
      if (key == 'A'){
        lcd.clear();
        gameState = MENU_1;
      }
      break;
    case MENU_DIFF:
      textDisplay();
      lcd.setCursor(0, 0);
      lcd.print("1. Easy  2. Norm");
      lcd.setCursor(0, 1);
      lcd.print("3. Hard  4. Hell");
      if (key == '1'){
        difficulty = 1;
        lcd.clear();
        gameState = MENU_1;
      }
      if (key == '2'){
        difficulty = 2;
        lcd.clear();
        gameState = MENU_1;
      }
      if (key == '3'){
        difficulty = 3;
        lcd.clear();
        gameState = MENU_1;
      }
      if (key == '4'){
        difficulty = 4;
        lcd.clear();
        gameState = MENU_1;
      }
      if (key == 'A'){
        lcd.clear();
        gameState = MENU_1;
      }
      break;
    case MENU_SCORE:
      textDisplay();
      lcd.setCursor(0, 0);
      lcd.print("Last: ");
      lcd.print(score);
      lcd.setCursor(0, 1);
      lcd.print("High: ");
      lcd.print(record);
      if (key == 'A'){
        lcd.clear();
        gameState = MENU_1;
      }
      break;
    case GAME:
      gameLoop();
      // Snek touches body
      for (int x = 1; x <= snakeLength; x++){
        if (snakeCoord[0][0] == snakeCoord[x][0] && snakeCoord[0][1] == snakeCoord[x][1]){
          if (score > record){
            record = score;
          }
          isLost = true;
          moveCount = 0;
          gameState = MENU_1;
        }
      }
      // Snek touches border
      if (borderType == 1){
        if (snakeCoord[0][0] == 0 || snakeCoord[0][0] == 33 || snakeCoord[0][1] == 0 || snakeCoord[0][1] == 9){
          if (score > record){
            record = score;
          }
          isLost = true;
          moveCount = 0;
          gameState = MENU_1;
        }
      }
      // Pause game
      if (key == 'A'){
        lcd.clear();
        gameState = PAUSE;
      }
      break;
    case PAUSE:
      lcd.setCursor(0, 0);
      lcd.print("Game paused");
      lcd.setCursor(0, 1);
      lcd.print("Press 1 to quit");
      if (key == 'A'){
        lcd.clear();
        gameState = GAME;
      }
      if (key == '1'){
        if (score > record){
          record = score;
        }
        isLost = true;
        moveCount = 0;
        gameState = MENU_1;
      }
      break;
    default:
      gameState = MENU_1;
  }
}

void setup(){
  // Initialize I2C LCD
  lcd.begin();
  lcd.backlight();
  // Initialize LED matrix MD72XX
  mx.begin();
  // Print greetings
  lcd.setCursor(0, 0);
  lcd.print("Project:");
  lcd.setCursor(0, 1);
  lcd.print("The Snek Game");
  delay(2000);
}

void loop(){
  if (millis() - prevMillis >= WORK_CYCLE){
    prevMillis = millis();
    scanKeyPad();
    scanJoystick();
    stateMachine();
  }
}
