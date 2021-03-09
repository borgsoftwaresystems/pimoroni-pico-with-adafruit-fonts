#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "pico_explorer.hpp"

#include <stdio.h>
#include "pico/stdlib.h"

#include "../../libraries/pico_graphics/gfxfont.h"
#include "../../fonts/FreeSerif24pt7b.h"
#include "../../fonts/FreeMono12pt7b.h"
#include "../../fonts/FreeSerifBoldItalic18pt7b.h"
#include "../../fonts/FreeSansBoldOblique12pt7b.h"

using namespace pimoroni;

uint16_t buffer[PicoExplorer::WIDTH * PicoExplorer::HEIGHT];
PicoExplorer pico_explorer(buffer);  

int main() {
	stdio_init_all();

  pico_explorer.init();
  pico_explorer.set_pen(0, 0, 0);
  pico_explorer.clear();     
  pico_explorer.set_pen(128, 128, 128);   
  
  pico_explorer.text("Standard", Point(0, 10), 240, 1);
  pico_explorer.customFontSetFont((const pimoroni::GFXfont&)FreeMono12pt7b);
  pico_explorer.text("Mono 12: !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", Point(0, 40), 240, 1);
  pico_explorer.customFontSetFont((const pimoroni::GFXfont&)FreeSansBoldOblique12pt7b);  
  pico_explorer.text("Sans Bold Obl 12", Point(0, 220), 240, 1);  
  pico_explorer.customFontSetFont();
  pico_explorer.text("Standard", Point(150, 10), 240, 1);

  pico_explorer.update();
}