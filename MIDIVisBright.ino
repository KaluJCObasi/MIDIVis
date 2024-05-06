/*
 * MIDI Visualizer with Color and Brightness
 * by Kalu Obasi
 * 
 * This is a MIDI visualizer using addressable LEDs.
 * Playing a note turns an LED on; releasing a note
 * turns the LED off. Each pitch creates a unique RGB color for the LEDs. When
 * using pitch-sensitive MIDI controllers, the strength with which the controller
 * is pressed correlates to the brightness of the LEDs.
 */

#include <FastLED.h>

// ---- User Settings --------------------------
#define DATAPIN 6
#define NUMLEDS 111
#define BRIGHTNESS 64
#define BAUDRATE 9600 // For Hairless
#define NOTESTART 0
// ----------------------------------------

// Settings
static const uint8_t
  Pin = DATAPIN,
  NumLEDs = NUMLEDS,
  minNote = NOTESTART,
  maxNote = minNote + NumLEDs,
  maxBright = BRIGHTNESS;

// LED Color Values
uint8_t
  rVal = 255,
  gVal = 255,
  bVal = 255;

// Build the LED strip
CRGB leds[NumLEDs];

boolean ledState[NumLEDs];

int colors[12] = {0xFF0000, 0xFF8000, 0xFFFF00, 0x80FF00, 0x00FF00, 0x00FF80,
0x00FFFF, 0x0080FF, 0x0000FF, 0x8000FF, 0xFF00FF, 0xFF0080};

// These arrays are used to control the color of the LEDs. Each LED
// has a red value, a green value, and a blue value.
int red[12] = {255, 255, 255, 128, 0, 0, 0, 0, 0, 128, 255, 255};
int green[12] = {0, 128, 255, 255, 255, 255, 255, 128, 0, 0, 0, 0};
int blue[12] = {0, 0, 0, 0, 0, 128, 255, 255, 255, 255, 255, 128};

// Create the MIDI Instance
#include <MIDI.h>
struct CustomBaud : public midi::DefaultSettings{
    static const long BaudRate = BAUDRATE;
};
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, CustomBaud);

void setup(){
    FastLED.addLeds<WS2812B, Pin, GRB>(leds, NumLEDs);
    FastLED.setBrightness(maxBright);
    FastLED.show(); 
  
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  Serial.begin(9600);
}

void loop(){
  MIDI.read(); // Check for MIDI messages every loop
}

void handleNoteOn(byte channel, byte note, byte velocity){
  // Check if note is in range
  if(note < minNote || note > maxNote){
    return;
  }

  // Get velocity signal to change strip brightness
  FastLED.setBrightness(velocity * 2);
  
  ledON(note - minNote);
  // perhaps add mod x??
  show();
}

void handleNoteOff(byte channel, byte note, byte velocity){
  // Check if note is in range
  if(note < minNote || note > maxNote){
    return;
  }

  ledOFF(note - minNote);
  // perhaps add mod x??
  show();
}

// When a key is pressed on the controller, turn on the corresponding LEDs.
void ledON(uint8_t index){
  ledState[index] = 1;
  setLED(index);
}

// When a key is released on the controller, turn off the corresponding LEDs.
void ledOFF(uint8_t index){
  ledState[index] = 0;
  setLED(index);
}

// Set an LED with a given index to a certain color and illuminate it
// (or dim it if a "note OFF" signal is sent).
void setLED(uint8_t index){
    if(ledState[index] == 1) {

      // Each of the 12 notes in a chromatic scale has a unique color.
      // The modulo operator is used here such that every LED with a certain
      // note value has a specific color.
      int colorIndex = index % 12;
      leds[index].r = red[colorIndex];
      leds[index].g = green[colorIndex];
      leds[index].b = blue[colorIndex];

      // If the two neighboring LEDs are within the given range,
      // illuminate (or dim) them as well!
      if(index >= 0) {
        leds[index - 1].r = red[colorIndex];
        leds[index - 1].g = green[colorIndex];
        leds[index - 1].b = blue[colorIndex];
      }

      if(index <= NumLEDs) {
        leds[index + 1].r = red[colorIndex];
        leds[index + 1].g = green[colorIndex];
        leds[index + 1].b = blue[colorIndex];
      }
    }
    else {
      leds[index] = 0x000000;
      if(index > 0) {
        leds[index - 1] =  0x000000;
      }
      if(index < NumLEDs) {
        leds[index + 1] =  0x000000;
      }
    }
}

void redraw(){
  for(int i = 0; i<NumLEDs; i++){
    if(ledState[i] == 1){
      ledON(i);
    }
  }
  show();
}

void show(){
    FastLED.show();
}


