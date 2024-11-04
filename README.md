# Bluetooth_Remote_VLC
Bluetooth remote for controlling playback of video on VLC player.
## Purpose
I am lazy. lazy enough to spend a lot of time trying to save myself a few minutes.
I wanted to watch movies in bed but I did not want to get up to change subtitles, pause etc.

Here is my janky attempt , based around an esp32, an 128xd64 OLED and a rotary encoder.
## Wiring
Soon
## Features
- Play / Pause
- Volume up / down
- Next / Prev video
- Seek in 10s increments
- Toggle subtitles
- Toggle audio track
- Lock PC Screen
- Folder selection

## Interface
### Click the rotary encoder for the following features.
- Play / Pause
- Toggle subtitles
- Toggle audio track
- Lock PC Screen
### Click and hold while turning for the following features.
- Volume up / down
- Next / Prev video
- Seek in 10s increments
### Folder selection interface.
This takes a little getting used to but is handy in a pinch.
With VLC open select this option. A folder selection window will open on the PC. Once a folder has been selected and confirmed you can exit this interface on the device by holding the encoder down and rotating left.

To "tab" through sections of the interface click the encoder.
To move up and down folders rotate the encoder. Right for up, left for down.
To confirm a folder (the return key) hold the encoder and rotate right.
When selection is finished and video start to play hold and rotate left to return to the devices main menu.
## Tweaks
If the rotary encoder is not going the right direction , on line 64 change 
`CtrlEnc encoder(26, 25, onTurnleft, onTurnRight);`
to
`CtrlEnc encoder(25, 26, onTurnleft, onTurnRight);`
## Libraries
Adafruit_GFX
and
Adafruit_SSD1306
are used to drive the OLED display

BleKeyboard by T-vK
https://github.com/T-vK/ESP32-BLE-Keyboard
Is used to emulate keyboard functions

CtrlEnc
and
CtrlBtn.h
from ctrl-arduino by joahnnes Jan Prins AKA bonkmachines are used to handle interrupts for the retary encoder and button.
https://github.com/bonkmachines/ctrl-arduino
