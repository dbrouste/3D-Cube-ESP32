#include <Arduino.h>

//Based on http://colinord.blogspot.com/2015/01/arduino-oled-module-with-3d-demo.html
//3D_Cube for Arduino OLED module by Colin Ord, 9/1/2015
//A port of my original JustBasic Cube_3D demo to the Arduino Uno using U8G library.

//#include "U8glib.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "3D.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     16 // Reset pin # (or -1 if sharing Arduino reset pin)
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
TwoWire twi = TwoWire(1); // our own TwoWire instance on bus 1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &twi, OLED_RESET);

float tx, nx, p;
float ty, ny, py;
float rot, rotx, roty, rotz, rotxx, rotyy, rotzz, rotxxx, rotyyy, rotzzz;
int i; //0 to 360
int fl, scale; //focal length
int wireframe[12][3];

int originx = 64;
int originy = 32; //32

int front_depth = 20;
int back_depth = -20;

unsigned long t1 = 0;
unsigned long t2 = 0;
int fps = 0;

//Store cube vertices
int cube_vertex[8][3] = {
 { -20, -20, front_depth},
 {20, -20, front_depth},
 {20, 20, front_depth},
 { -20, 20, front_depth},
 { -20, -20, back_depth},
 {20, -20, back_depth},
 {20, 20, back_depth},
 { -20, 20, back_depth}
};

int fd = 0; //0=orthographic

void draw_vertices(void)
{
    //Serial.print((int) rotxxx);Serial.println((int) rotyyy);
 display.drawPixel((int) rotxxx,(int) rotyyy, WHITE);
}


void draw_wireframe(void)
{
 display.drawLine(wireframe[0][0], wireframe[0][1], wireframe[1][0], wireframe[1][1], WHITE);
 display.drawLine(wireframe[1][0], wireframe[1][1], wireframe[2][0], wireframe[2][1], WHITE);
 display.drawLine(wireframe[2][0], wireframe[2][1], wireframe[3][0], wireframe[3][1], WHITE);
 display.drawLine(wireframe[3][0], wireframe[3][1], wireframe[0][0], wireframe[0][1], WHITE);

//cross face above
 display.drawLine(wireframe[1][0], wireframe[1][1], wireframe[3][0], wireframe[3][1], WHITE);
 display.drawLine(wireframe[0][0], wireframe[0][1], wireframe[2][0], wireframe[2][1], WHITE);

 display.drawLine(wireframe[4][0], wireframe[4][1], wireframe[5][0], wireframe[5][1], WHITE);
 display.drawLine(wireframe[5][0], wireframe[5][1], wireframe[6][0], wireframe[6][1], WHITE);
 display.drawLine(wireframe[6][0], wireframe[6][1], wireframe[7][0], wireframe[7][1], WHITE);
 display.drawLine(wireframe[7][0], wireframe[7][1], wireframe[4][0], wireframe[4][1], WHITE);

 display.drawLine(wireframe[0][0], wireframe[0][1], wireframe[4][0], wireframe[4][1], WHITE);
 display.drawLine(wireframe[1][0], wireframe[1][1], wireframe[5][0], wireframe[5][1], WHITE);
 display.drawLine(wireframe[2][0], wireframe[2][1], wireframe[6][0], wireframe[6][1], WHITE);
 display.drawLine(wireframe[3][0], wireframe[3][1], wireframe[7][0], wireframe[7][1], WHITE);
}

void draw_fps(unsigned long texte)
{
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(WHITE); // Draw white text
    display.setCursor(0, 0);     // Start at top-left corner
    display.cp437(true);         // Use full 256 char 'Code Page 437' font
    display.print(texte); 
    display.println("fps"); 
}

void setup(void)
{
    Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  twi.begin(4,15);  // Needs to come before display.begin is used
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
    //Serial.println("test");
}

void loop(void)
{
//picture loop
 for (int angle = 0; angle <= 360; angle = angle + 3) {
     //Serial.println(angle);
    t1 = millis();
    display.display();
  //do {
   for (int i = 0; i < 8; i++) {

    rot = angle * 0.0174532; //0.0174532 = one degree
//rotateY
    rotz = cube_vertex[i][2] * cos(rot) - cube_vertex[i][0] * sin(rot);
    rotx = cube_vertex[i][2] * sin(rot) + cube_vertex[i][0] * cos(rot);
    roty = cube_vertex[i][1];
//rotateX
    rotyy = roty * cos(rot) - rotz * sin(rot);
    rotzz = roty * sin(rot) + rotz * cos(rot);
    rotxx = rotx;
//rotateZ
    rotxxx = rotxx * cos(rot) - rotyy * sin(rot);
    rotyyy = rotxx * sin(rot) + rotyy * cos(rot);
    rotzzz = rotzz;

//orthographic projection
    rotxxx = rotxxx + originx;
    rotyyy = rotyyy + originy;

//store new vertices values for wireframe drawing
    wireframe[i][0] = rotxxx;
    wireframe[i][1] = rotyyy;
    wireframe[i][2] = rotzzz;

    display.clearDisplay();
 
    draw_vertices();
   }

   draw_wireframe();

   
   t2 = millis();
   fps = (int) 1000/(t2-t1);
   draw_fps(fps);
  //} //while (display.nextPage());
 }
}
