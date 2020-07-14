# Snake Game on Arduino
This is my team project for HCMUT Computer Engineering course CO2019 - *Computer Hardware Lab*: Making a snake game using the Arduino Uno board and several other modules.
## Introduction
*Snake* is the common name for a video game concept where the player maneuvers a line which grows in length, with the line itself being a primary obstacle. The concept originated in the 1976 arcade game *Blockade*, and the ease of implementing *Snake* has led to hundreds of versions (some of which have the word *snake* or *worm* in the title) for many platforms.
## Modules used in the project
- Arduino Uno
- Analog joystick
- Keypad 4x4
- I2C LCD display
- MAX7219 Dot Matrix 4-in-1
- Self-made header board (the design in Altium Designer can be found in this repository, the board was made using etching method)
## Wiring instructions
### Keypad 4x4
| Keypad 4x4 | Arduino Uno |
|:----------:|:-----------:|
|     C4     |      0      |
|     C3     |      1      |
|     C2     |      2      |
|     C1     |      3      |
|     R1     |      4      |
|     R2     |      5      |
|     R3     |      6      |
|     R4     |      7      |
### Joystick
| Analog joystick |  Arduino Uno  |
|:---------------:|:-------------:|
|        SW       | Not connected |
|       VRy       |       A1      |
|       VRx       |       A0      |
|       VCC       |       5V      |
|       GND       |      GND      |
### I2C LCD
| I2C LCD | Arduino Uno |
|:-------:|:-----------:|
|   GND   |     GND     |
|   VCC   |      5V     |
|   SDA   |      A4     |
|   SCL   |      A5     |
### Dot Matrix
| MAX7219 Dot Matrix 4-in-1 | Arduino Uno |
|:-------------------------:|:-----------:|
|            VCC            |      5V     |
|            GND            |     GND     |
|            DIN            |      11     |
|             CS            |      10     |
|            CLK            |      13     |
## Features
- The game menu and current score while ingame are displayed on the LCD.
- In the menu, the game title is scrolled on the dot matrix.
- The game menu can be navigated using the 4x4 keypad.
- The game boots into the first menu, press `1` to start the game, `2` to choose the difficulty, and `B` to switch to the second menu.
- Difficulty can be chosen using key `1` to `4`, ranging from easy as the easiest difficulty to hell as the hardest.
- In the second menu, press `3` to choose the border type of the game and `4` to view your last score and high score.
- Border type can be chosen between bordered and borderless using key `1` or `2`.
- In all of the sub-menus, press `A` to return to the first menu.
- As we start the game, the dot matrix will proceed to display the snake game. The snake's movement can be controlled using the joystick, the speed of the snake depends on the difficulty, and there are always 5 food blocks on the screen at a time. The game ends when the snake bites its own tail (and the border in bordered mode).
- Press `A` while in-game to pause the game and press `1` while pausing to quit the game and return to the main menu.
- Physically, all the modules and jumper wires are encased in a console-liked case, which is self-crafted using LEGO.
## Implementation
List of library used:
- `MD_UISwitch` from [https://github.com/MajicDesigns/MD_UISwitch](https://github.com/MajicDesigns/MD_UISwitch).
- `MD_MAX72XX` from [https://github.com/MajicDesigns/MD_MAX72XX](https://github.com/MajicDesigns/MD_MAX72XX).
- `LiquidCrystal_I2C` from [https://github.com/johnrickman/LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C).
- `Wire` from Arduino Standard Libraries.
- `Keypad` from [https://github.com/Chris--A/Keypad](https://github.com/Chris--A/Keypad).

About the code, its design logic is a state machine based design so as to improve the system's stability.
## Demonstration
The video where we demonstrate the product in details can be accessed at this link: [https://youtu.be/WMlZ-hNHN7w](https://youtu.be/WMlZ-hNHN7w).
## Contributors
- Khac Minh Dang Le: [https://github.com/LKMDang](https://github.com/LKMDang)
- Chan Duong Van: [https://github.com/shinyo-dc](https://github.com/shinyo-dc)
- Phuc Khanh Huynh: 
