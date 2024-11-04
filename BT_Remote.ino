#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BleKeyboard.h>
#include <CtrlEnc.h>
#include <CtrlBtn.h>
// https://github.com/MistaKD/Bluetooth_Remote_VLC
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
// set menu counter
const uint8_t bitmap6[] = {0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x03, 0xff, 0xc0, 0x07, 0xc3, 0xe0, 0x0e, 0x00, 0x70, 0x1c, 0x00, 0x38, 0x38, 0x00, 0x1c, 0x30, 0x18, 0x0c, 0x70, 0x18, 0x0e, 0x70, 0x18, 0x0e, 0x60, 0x18, 0x06, 0x60, 0x18, 0x06, 0x60, 0x18, 0x06, 0x60, 0x00, 0x06, 0x70, 0x00, 0x0e, 0x70, 0x18, 0x0e, 0x30, 0x18, 0x0c, 0x38, 0x00, 0x1c, 0x1c, 0x00, 0x38, 0x0e, 0x00, 0x70, 0x07, 0xc3, 0xe0, 0x03, 0xff, 0xc0, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00};
const uint8_t bitmap9[] = {0x00, 0x18, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x1b, 0x80, 0x02, 0x19, 0xc0, 0x07, 0x19, 0xe0, 0x03, 0x99, 0xc0, 0x01, 0xdb, 0x80, 0x00, 0xff, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x7e, 0x00, 0x00, 0xff, 0x00, 0x01, 0xdb, 0x80, 0x03, 0x99, 0xc0, 0x07, 0x19, 0xe0, 0x02, 0x19, 0xc0, 0x00, 0x1b, 0x80, 0x00, 0x1f, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x18, 0x00};
const uint8_t bitmap10[] = {0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x03, 0xff, 0xc0, 0x07, 0xe7, 0xe0, 0x0f, 0x00, 0xf0, 0x1c, 0x00, 0x38, 0x38, 0x00, 0x1c, 0x38, 0x00, 0x1c, 0x70, 0xc3, 0x0e, 0x70, 0xc3, 0x0e, 0x60, 0x00, 0x06, 0x60, 0x00, 0x06, 0x60, 0x00, 0x06, 0x71, 0x81, 0x8e, 0x71, 0xc3, 0x8e, 0x70, 0xff, 0x0e, 0x38, 0x7e, 0x1c, 0x38, 0x00, 0x1c, 0x1c, 0x00, 0x38, 0x0f, 0x00, 0xf0, 0x07, 0xe7, 0xe0, 0x03, 0xff, 0xc0, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00};

int16_t MainIndex = 0;
bool Mainflag = 1;
int modval = 8;

String menuitems[8] = {"Play/Pause", "- Volume +", "- Track +", "- Seek +", "Subtitles", "Audio", "Folder", "Lock PC"};

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
BleKeyboard bleKeyboard("Media pad", "BK", 100);
bool screenstate = 0;

void onTurnleft() {
  if(bleKeyboard.isConnected()) {
    Turned(1);
  }
}

// Define an onTurnRight handler.
void onTurnRight() {
  if(bleKeyboard.isConnected()) {
    Turned(0);
  }
}

// Define an onRelease handler.
void onRls() {
  if(bleKeyboard.isConnected()) {
    ShortPress();
  }
}

// Define an onDelayedRelease handler.
void onDelayedRelease() {
  Serial.println("Long press");
  
  }



// Create a rotary encoder with the clk signal pin number, dt signal pin number,
// onTurnleft & onTurnRight handler.
CtrlEnc encoder(26, 25, onTurnleft, onTurnRight);

 // Signal pin, Bounce time, Empty onpress handler, onrelease handler, ondelayrelease handler
CtrlBtn button(27, 15, nullptr, onRls, onDelayedRelease);

void setup() {
  Serial.begin(9600);
  button.setDelayedReleaseDuration(500); // This is optional (default is 500ms).
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.display();
  Serial.println("Starting BLE work!");
  bleKeyboard.begin();
  DisconnectedScreen();
  // initialise menu

}

void loop() {
// The process method will poll the button object and handle all it's functionality.
  button.process();
  // The process method will poll the rotary encoder object and handle all it's functionality.
  encoder.process();
}

void DisplayCenter(String text, int16_t xpos ) {
  int16_t x1;
  int16_t y1;
  uint16_t width;
  uint16_t height;

  display.getTextBounds(text, 0, 0, &x1, &y1, &width, &height);
  // display on horizontal 
  display.setCursor((SCREEN_WIDTH - width) / 2, xpos);
  display.println(text); // text to display
}

void Turned(bool rightleft) {
  switch(Mainflag) {
    case 0: //Not in main menu
      if (button.isPressed()) {
        if (rightleft == 0) {  //Right turn = ENTER KEY
          bleKeyboard.press(KEY_RETURN);  
          delay(80);
          bleKeyboard.releaseAll();  
            }
        if (rightleft == 1) {  //Left turn = EXIT FUNCTION
          ExitSubmenu();  
            }

      }
      else {
        if (rightleft == 0) {  //Right turn = Down Arrow
          bleKeyboard.press(KEY_DOWN_ARROW);
          delay(80);
          bleKeyboard.releaseAll();     
            }
        if (rightleft == 1) {  //Left turn = Up Arrow
          bleKeyboard.press(KEY_UP_ARROW);     
          delay(80);
          bleKeyboard.releaseAll();
            }

      }

      break;

    case 1: //In main menu
      
      if (button.isPressed()) {
        switch(MainIndex) {
          case 0: // play/pause - Do Nothing
            break;
          case 1: // Volume +/-
            if (rightleft == 0) {  //Right turn
              bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
            }
            if (rightleft == 1) {  //Left turn
              bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
            }
            break;
          case 2: // Episode +/-
            if (rightleft == 0) {  //Right turn
              
              bleKeyboard.write(KEY_MEDIA_NEXT_TRACK);
            }
            if (rightleft == 1) {  //Left turn
              
              bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
            }
            break;
          case 3: // Seek <>
            if (rightleft == 0) {  //Right turn
              bleKeyboard.press(KEY_LEFT_ALT);
              bleKeyboard.press(KEY_RIGHT_ARROW);
              delay(20);
              bleKeyboard.releaseAll();
            }
            if (rightleft == 1) {  //Left turn
              
              bleKeyboard.press(KEY_LEFT_ALT);
              bleKeyboard.press(KEY_LEFT_ARROW);
              delay(20);
              bleKeyboard.releaseAll();
            }
            break;
          case 4: // Subtitle toggle
            break;
          case 5: // Audio track toggle
            break;
          case 6: // Select folder to play
            break;
          case 7: // Lock screen
            break;
          
        }

      }

      else {   //Button is not pressed
        switch(rightleft) {
          case 0: //Encoder turned right, add one to the MainIndex
            //update menu screen
            MainIndex = abs((MainIndex + (modval + 1)) % modval);
            UpdateMenu();
            break;
          case 1: //Encoder turned left, subtract one to the MainIndex
            //update menu screen
            MainIndex = abs((MainIndex + (modval - 1)) % modval);
            UpdateMenu();
            break;
        }

      }
      break;
  }

}

void UpdateMenu() {
  String item1;
  String item2;
  String item3;
  String itemSelected;
  String item5;
  String item6;
  String item7;
  String item8;
  switch(MainIndex) {
    case 0: // play/pause - Do Nothing
      item1 = menuitems[5];
      item2 = menuitems[6];
      item3 = menuitems[7];
      itemSelected = menuitems[0];
      item5 = menuitems[1];
      item6 = menuitems[2];
      item7 = menuitems[3];
      item8 = menuitems[4];
      break;
    case 1: // Volume +/-
      item1 = menuitems[6];
      item2 = menuitems[7];
      item3 = menuitems[0];
      itemSelected = menuitems[1];
      item5 = menuitems[2];
      item6 = menuitems[3];
      item7 = menuitems[4];
      item8 = menuitems[5];
      break;
    case 2: // Episode +/-
      item1 = menuitems[7];
      item2 = menuitems[0];
      item3 = menuitems[1];
      itemSelected = menuitems[2];
      item5 = menuitems[3];
      item6 = menuitems[4];
      item7 = menuitems[5];
      item8 = menuitems[6];
      break;
    case 3: // Seek <>
      item1 = menuitems[0];
      item2 = menuitems[1];
      item3 = menuitems[2];
      itemSelected = menuitems[3];
      item5 = menuitems[4];
      item6 = menuitems[5];
      item7 = menuitems[6];
      item8 = menuitems[7];
      break;
    case 4: // Subtitle toggle
      item1 = menuitems[1];
      item2 = menuitems[2];
      item3 = menuitems[3];
      itemSelected = menuitems[4];
      item5 = menuitems[5];
      item6 = menuitems[6];
      item7 = menuitems[7];
      item8 = menuitems[0];
      break;
    case 5: // Audio track toggle
      item1 = menuitems[2];
      item2 = menuitems[3];
      item3 = menuitems[4];
      itemSelected = menuitems[5];
      item5 = menuitems[6];
      item6 = menuitems[7];
      item7 = menuitems[0];
      item8 = menuitems[1];
      break;
    case 6: // Select folder to play
      item1 = menuitems[3];
      item2 = menuitems[4];
      item3 = menuitems[5];
      itemSelected = menuitems[6];
      item5 = menuitems[7];
      item6 = menuitems[0];
      item7 = menuitems[1];
      item8 = menuitems[2];
      break;
    case 7: // Lock PC
      item1 = menuitems[4];
      item2 = menuitems[5];
      item3 = menuitems[6];
      itemSelected = menuitems[7];
      item5 = menuitems[0];
      item6 = menuitems[1];
      item7 = menuitems[2];
      item8 = menuitems[3];
      break;
  
  }
  display.clearDisplay();
  display.setCursor(0,0);
  //display.drawRoundRect(0, 0, 128, 64, 8, SSD1306_WHITE);
  display.setTextSize(1);            
  display.setTextColor(SSD1306_WHITE);       
  DisplayCenter(item1, 0);
  DisplayCenter(item2, 8);
  DisplayCenter(item3, 16);
  display.setTextSize(2);  
  DisplayCenter(itemSelected, 24);
  display.setTextSize(1);  
  DisplayCenter(item5, 40);
  DisplayCenter(item6, 48);
  DisplayCenter(item7, 56);
  display.display();

}

void DisconnectedScreen() {
  display.clearDisplay();
  display.drawRoundRect(0, 0, 128, 64, 8, SSD1306_WHITE);
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(6,2);             // Start at top-left corner
  display.println(" Connect");
  display.println(" Bluetooth");
  display.drawBitmap(50, 39, bitmap6, 24, 24, 1);
  display.display();
}

void ShortPress() {
  switch(Mainflag) {
    case 0: //Not in main menu
      bleKeyboard.press(KEY_TAB);
      delay(80);
      bleKeyboard.releaseAll();
      break;

    case 1: //In main menu
      switch(MainIndex) {
        case 0: // play/pause - Do Nothing
          bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
          break;
        case 1: // Volume +/-
          break;
        case 2: // Episode +/-
          break;
        case 3: // Seek <>
          break;
        case 4: // Subtitle toggle
          bleKeyboard.press('v');
          delay(100);
          bleKeyboard.releaseAll();
          break;
        case 5: // Audio track toggle
          bleKeyboard.press('b');
          delay(100);
          bleKeyboard.releaseAll();
          break;
        case 6: // Select folder to play
          bleKeyboard.press(KEY_LEFT_CTRL);
          bleKeyboard.press('f');
          delay(100);
          bleKeyboard.releaseAll();
          EnterSubmenu();

          break;
        case 7: // Lock Screen
          bleKeyboard.press(KEY_LEFT_GUI);
          bleKeyboard.press('l');
          delay(100);
          bleKeyboard.releaseAll();
          break;
      }
  }
}

void BrowserScreen() {
  display.setCursor(0,0);
  display.clearDisplay();
  display.setTextSize(2);            
  display.setTextColor(SSD1306_WHITE);       
  display.println("Click =Tab");
  display.println("Turn=Up/Dn");
  display.println("HRt =Enter");
  display.println("HLft =Exit");
  display.display();
}

void ExitSubmenu() {
  Mainflag = 1;
  UpdateMenu();
}

void EnterSubmenu() {
  Mainflag = 0;
  BrowserScreen();
}
