## IoT Hebrew User Interface Project
By: Eitan Shaked, Ofir Mirovsky, Ido Merenstein  
  
## Details about the project
The project demonstrates advanced Hebrew language support in embedded systems using the LVGL graphics library on ESP32. The system showcases complex Hebrew UI components including RTL (right-to-left) text direction, Hebrew fonts with Niqqud support, expandable news cards, image galleries, pull-to-refresh mechanisms, responsive tabbed interfaces, and a comprehensive settings modal. The system is designed to serve as a reference implementation for developers building Hebrew-language embedded applications and IoT devices with touch interfaces.
 
## Folder description :
* ESP32: source code for the esp side (firmware).
* Documentation: wiring diagram + basic operating instructions
* Unit Tests: tests for individual hardware components (input / output devices)
* flutter_app : dart code for our Flutter app.
* Parameters: contains description of parameters and settings that can be modified IN YOUR CODE
* Assets: link to 3D printed parts, Audio files used in this project, Fritzing file for connection diagram (FZZ format) etc

## ESP32 SDK version used in this project: 
ESP-IDF v6.12.0

## Arduino/ESP32 libraries used in this project:
* lvgl/lvgl - version 9.3.0
* bodmer/TFT_eSPI - version 2.5.43

## Connection diagram:
![wiring](https://github.com/eitansh21/IoT_LVGL_GUI_Microcontrollers/blob/main/Documentation/connection_diagram.png)

## Project Poster:
 
This project is part of ICST - The Interdisciplinary Center for Smart Technologies, Taub Faculty of Computer Science, Technion
https://icst.cs.technion.ac.il/
