# Pimoroni Pico Libraries and Examples including Adafruit custom font support

This repository is a fork of the pimoroni-pico library v. 0.0.8 Alpha that contains the C/C++ and MicroPython libraries for Pimoroni's range of Raspberry Pi Pico addons. This fork has been modified with adjusted versions of various functions from the Adafruit GFX library to allow the use of GNU FreeFonts for the Pimoroni Pico Explorer Base and Pimoroni Pico Display Pack.

After compilation the following new uf2's are created:

\build\examples\pico_explorer\font_demo_explorer.uf2
\build\examples\pico_display\font_demo_display.uf2

They will display the standard Pimoromi font together with one or more additional GNU FreeFonts.

# To use the new fonts

A number of fonts have been included in the fonts directory. To use a particular font do the following:

Add the following near the top of your .cpp file:

```
#include "libraries/pico_graphics/gfxfont.h"
#include "fonts/FreeSerif24pt7b.h" //Replace this with the name of the font you would like to use
```

**Note**: You may have to adjust the path depending on where your .cpp file is in relation to the fonts folder and libraries folder.

Then, for the Pico Explorer Base:

```
pico_explorer.customFontSetFont((const pimoroni::GFXfont&)FreeSerif24pt7b);
```

and then just write out as you normally would with the pico_explorer.text function e.g.

```
pico_explorer.text("Hello new fonts", Point(0, 220), 240, 1);
```

To switch back to the standard Pimoroni font:

```
pico_explorer.customFontSetFont();
```

For the Pico Display Pack just replace pico_explorer above with pico_display.

Include as many fonts as you need in the include statements but please bear in mind that each font will take up memory / storage.

**Note**: As far as I am aware the new fonts behave like the existing Pimoroni font with the exception that only character wrapping is supported at this time. i.e. it will not word wrap.**

**NOte**: Sorry, there is no micro Python support at the moment. I will look into that.

