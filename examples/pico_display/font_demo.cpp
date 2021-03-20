#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "pico_display.hpp"

#include <stdio.h>
#include "pico/stdlib.h"

#include "../../libraries/pico_graphics/gfxfont.h"
#include "../../fonts/FreeSerif24pt7b.h"
#include "../../fonts/FreeMono12pt7b.h"
#include "../../fonts/FreeSerifBoldItalic18pt7b.h"
#include "../../fonts/FreeSansBoldOblique12pt7b.h"

using namespace pimoroni;

uint16_t buffer[PicoDisplay::WIDTH * PicoDisplay::HEIGHT];
PicoDisplay pico_display(buffer);  

int main() {
	stdio_init_all();

  pico_display.init();
  pico_display.set_backlight(100);

  pico_display.set_pen(0, 0, 0);
  pico_display.clear();     
  pico_display.set_pen(128, 128, 128);   
  
  pico_display.text("Standard", Point(0, 10), 240, 1);
  pico_display.customFontSetFont((const pimoroni::GFXfont&)FreeMono12pt7b);
  pico_display.text("Mono 12: ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", Point(0, 40), 240, 1);
  pico_display.customFontSetFont((const pimoroni::GFXfont&)FreeSansBoldOblique12pt7b);  
  pico_display.text("Sans Bold Obl 12", Point(0, 135), 240, 1);  
  pico_display.customFontSetFont();
  pico_display.text("Standard", Point(150, 10), 240, 1);

  pico_display.update();
}