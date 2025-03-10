#include "pico_graphics.hpp"


#include "gfxfont.h"


namespace pimoroni {
  PicoGraphics::PicoGraphics(uint16_t width, uint16_t height, uint16_t *frame_buffer)
  : frame_buffer(frame_buffer), bounds(0, 0, width, height), clip(0, 0, width, height) {
    set_font(&font6);
  };

  void PicoGraphics::set_font(const Font *font){
    this->font = font;
  }


  const GFXfont* gfxFont = nullptr;


  void PicoGraphics::set_pen(uint8_t r, uint8_t g, uint8_t b) {
    pen = create_pen(r, g, b);
  }

  void PicoGraphics::set_pen(Pen p) {
    pen = p;
  }

  void PicoGraphics::set_clip(const Rect &r) {
    clip = bounds.intersection(r);
  }

  void PicoGraphics::remove_clip() {
    clip = bounds;
  }

  Pen* PicoGraphics::ptr(const Rect &r) {
    return frame_buffer + r.x + r.y * bounds.w;
  }

  Pen* PicoGraphics::ptr(const Point &p) {
    return frame_buffer + p.x + p.y * bounds.w;
  }

  Pen* PicoGraphics::ptr(int32_t x, int32_t y) {
    return frame_buffer + x + y * bounds.w;
  }

  void PicoGraphics::clear() {
    rectangle(clip);
  }

  void PicoGraphics::pixel(const Point &p) {
    if(!clip.contains(p)) return;
    *ptr(p) = pen;
  }

  void PicoGraphics::pixel_span(const Point &p, int32_t l) {
    // check if span in bounds
    if( p.x + l < clip.x || p.x >= clip.x + clip.w ||
        p.y     < clip.y || p.y >= clip.y + clip.h) return;

    // clamp span horizontally
    Point clipped = p;
    if(clipped.x     <  clip.x)           {l += clipped.x - clip.x; clipped.x = clip.x;}
    if(clipped.x + l >= clip.x + clip.w)  {l  = clip.x + clip.w - clipped.x;}

    Pen *dest = ptr(clipped);
    while(l--) {
      *dest++ = pen;
    }
  }

  void PicoGraphics::rectangle(const Rect &r) {
    // clip and/or discard depending on rectangle visibility
    Rect clipped = r.intersection(clip);

    if(clipped.empty()) return;

    Pen *dest = ptr(clipped);
    while(clipped.h--) {
      // draw span of pixels for this row
      for(int32_t i = 0; i < clipped.w; i++) {
        *dest++ = pen;
      }

      // move to next scanline
      dest += bounds.w - clipped.w;
    }
  }

  void PicoGraphics::circle(const Point &p, int32_t radius) {
    // circle in screen bounds?
    Rect bounds = Rect(p.x - radius, p.y - radius, radius * 2, radius * 2);
    if(!bounds.intersects(clip)) return;

    int ox = radius, oy = 0, err = -radius;
    while (ox >= oy)
    {
      int last_oy = oy;

      err += oy; oy++; err += oy;

      pixel_span(Point(p.x - ox, p.y + last_oy), ox * 2 + 1);
      if (last_oy != 0) {
        pixel_span(Point(p.x - ox, p.y - last_oy), ox * 2 + 1);
      }

      if(err >= 0 && ox != last_oy) {
        pixel_span(Point(p.x - last_oy, p.y + ox), last_oy * 2 + 1);
        if (ox != 0) {
          pixel_span(Point(p.x - last_oy, p.y - ox), last_oy * 2 + 1);
        }

        err -= ox; ox--; err -= ox;
      }
    }
  }

  void PicoGraphics::character(const char c, const Point &p, uint8_t scale) {
    uint8_t char_index = c - 32;
    Rect char_bounds(p.x, p.y, font->widths[char_index] * scale, font->height * scale);

    if(!clip.intersects(char_bounds)) return;

    const uint8_t *d = &font->data[char_index * font->max_width];
    for(uint8_t cx = 0; cx < font->widths[char_index]; cx++) {
      for(uint8_t cy = 0; cy < font->height; cy++) {
        if((1U << cy) & *d) {
          rectangle(Rect(p.x + (cx * scale), p.y + (cy * scale), scale, scale));
        }
      }

      d++;
    }
  }

  void PicoGraphics::text(const std::string &t, const Point &p, int32_t wrap, uint8_t scale) {
    
    
    if (gfxFont != nullptr) {
      return customFontWrite(t, p, wrap, scale);
    }
    
    uint32_t co = 0, lo = 0; // character and line (if wrapping) offset

    size_t i = 0;
    while(i < t.length()) {
      // find length of current word
      size_t next_space = t.find(' ', i + 1);

      if(next_space == std::string::npos) {
        next_space = t.length();
      }

      uint16_t word_width = 0;
      for(size_t j = i; j < next_space; j++) {
        word_width += font->widths[t[j] - 32] * scale;
      }

      // if this word would exceed the wrap limit then
      // move to the next line
      if(co != 0 && co + word_width > (uint32_t)wrap) {
        co = 0;
        lo += (font->height + 1) * scale;
      }

      // draw word
      for(size_t j = i; j < next_space; j++) {
        character(t[j], Point(p.x + co, p.y + lo), scale);
        co += font->widths[t[j] - 32] * scale;
      }

      // move character offset to end of word and add a space
      co += font->widths[0] * scale;
      i = next_space + 1;
    }
  }

  int32_t orient2d(Point p1, Point p2, Point p3) {
    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
  }

  bool is_top_left(const Point &p1, const Point &p2) {
    return (p1.y == p2.y && p1.x > p2.x) || (p1.y < p2.y);
  }

  void PicoGraphics::triangle(Point p1, Point p2, Point p3) {
    Rect triangle_bounds(
      Point(std::min(p1.x, std::min(p2.x, p3.x)), std::min(p1.y, std::min(p2.y, p3.y))),
      Point(std::max(p1.x, std::max(p2.x, p3.x)), std::max(p1.y, std::max(p2.y, p3.y))));

    // clip extremes to frame buffer size
    triangle_bounds = clip.intersection(triangle_bounds);

    // if triangle completely out of bounds then don't bother!
    if (triangle_bounds.empty()) {
      return;
    }

    // fix "winding" of vertices if needed
    int32_t winding = orient2d(p1, p2, p3);
    if (winding < 0) {
      Point t;
      t = p1; p1 = p3; p3 = t;
    }

    // bias ensures no overdraw between neighbouring triangles
    int8_t bias0 = is_top_left(p2, p3) ? 0 : -1;
    int8_t bias1 = is_top_left(p3, p1) ? 0 : -1;
    int8_t bias2 = is_top_left(p1, p2) ? 0 : -1;

    int32_t a01 = p1.y - p2.y;
    int32_t b01 = p2.x - p1.x;
    int32_t a12 = p2.y - p3.y;
    int32_t b12 = p3.x - p2.x;
    int32_t a20 = p3.y - p1.y;
    int32_t b20 = p1.x - p3.x;

    Point tl(triangle_bounds.x, triangle_bounds.y);
    int32_t w0row = orient2d(p2, p3, tl) + bias0;
    int32_t w1row = orient2d(p3, p1, tl) + bias1;
    int32_t w2row = orient2d(p1, p2, tl) + bias2;

    for (int32_t y = 0; y < triangle_bounds.h; y++) {
      int32_t w0 = w0row;
      int32_t w1 = w1row;
      int32_t w2 = w2row;

      Pen *dest = ptr(triangle_bounds.x, triangle_bounds.y + y);
      for (int32_t x = 0; x < triangle_bounds.w; x++) {
        if ((w0 | w1 | w2) >= 0) {
          *dest = pen;
        }

        dest++;

        w0 += a12;
        w1 += a20;
        w2 += a01;
      }

      w0row += b12;
      w1row += b20;
      w2row += b01;
    }
  }

  void PicoGraphics::polygon(const std::vector<Point> &points) {
    static int32_t nodes[64]; // maximum allowed number of nodes per scanline for polygon rendering

    int32_t miny = points[0].y, maxy = points[0].y;

    for (uint16_t i = 1; i < points.size(); i++) {
      miny = std::min(miny, points[i].y);
      maxy = std::max(maxy, points[i].y);
    }

    // for each scanline within the polygon bounds (clipped to clip rect)
    Point p;

    for (p.y = std::max(clip.y, miny); p.y <= std::min(clip.y + clip.h, maxy); p.y++) {
      uint8_t n = 0;
      for (uint16_t i = 0; i < points.size(); i++) {
        uint16_t j = (i + 1) % points.size();
        int32_t sy = points[i].y;
        int32_t ey = points[j].y;
        int32_t fy = p.y;
        if ((sy < fy && ey >= fy) || (ey < fy && sy >= fy)) {
          int32_t sx = points[i].x;
          int32_t ex = points[j].x;
          int32_t px = int32_t(sx + float(fy - sy) / float(ey - sy) * float(ex - sx));

          nodes[n++] = px < clip.x ? clip.x : (px >= clip.x + clip.w ? clip.x + clip.w - 1 : px);// clamp(int32_t(sx + float(fy - sy) / float(ey - sy) * float(ex - sx)), clip.x, clip.x + clip.w);
        }
      }

      uint16_t i = 0;
      while (i < n - 1) {
        if (nodes[i] > nodes[i + 1]) {
          int32_t s = nodes[i]; nodes[i] = nodes[i + 1]; nodes[i + 1] = s;
          if (i) i--;
        }
        else {
          i++;
        }
      }

      for (uint16_t i = 0; i < n; i += 2) {
        pixel_span(Point(nodes[i], p.y), nodes[i + 1] - nodes[i] + 1);
      }
    }
  }

  void PicoGraphics::line(Point p1, Point p2) {
    // fast horizontal line
    if(p1.y == p2.y) {
      int32_t start = std::max(clip.x, std::min(p1.x, p2.x));
      int32_t end   = std::min(clip.x + clip.w, std::max(p1.x, p2.x));
      pixel_span(Point(start, p1.y), end - start);
      return;
    }

    // fast vertical line
    if(p1.x == p2.x) {
      int32_t start  = std::max(clip.y, std::min(p1.y, p2.y));
      int32_t length = std::min(clip.y + clip.h, std::max(p1.y, p2.y)) - start;
      Pen *dest = ptr(p1.x, start);
      while(length--) {
        *dest = pen;
        dest += bounds.w;
      }
      return;
    }

    // general purpose line
    // lines are either "shallow" or "steep" based on whether the x delta
    // is greater than the y delta
    int32_t dx = p2.x - p1.x;
    int32_t dy = p2.y - p1.y;
    bool shallow = std::abs(dx) > std::abs(dy);
    if(shallow) {
      // shallow version
      int32_t s = std::abs(dx);       // number of steps
      int32_t sx = dx < 0 ? -1 : 1;   // x step value
      int32_t sy = (dy << 16) / s;    // y step value in fixed 16:16
      int32_t x = p1.x;
      int32_t y = p1.y << 16;
      while(s--) {
        pixel(Point(x, y >> 16));
        y += sy;
        x += sx;
      }
    }else{
      // steep version
      int32_t s = std::abs(dy);       // number of steps
      int32_t sy = dy < 0 ? -1 : 1;   // y step value
      int32_t sx = (dx << 16) / s;    // x step value in fixed 16:16
      int32_t y = p1.y;
      int32_t x = p1.x << 16;
      while(s--) {
        pixel(Point(x >> 16, y));
        y += sy;
        x += sx;
      }
    }
  }


  void PicoGraphics::customFontSetFont(const GFXfont &font) {
      gfxFont = (const pimoroni::GFXfont *)&font;
      return;
  }

  void PicoGraphics::customFontSetFont() {
      gfxFont = nullptr;
      return;
  }

  //The following two functions have been taken from the Adafruit GFX library and modified
  //Please see License-Adafruit at root
  void PicoGraphics::customFontDrawChar(char letter, int x, int y, int size_x, int size_y) {
    //const GFXfont* gfxFont = &font;
    GFXglyph* gly = &gfxFont->glyph[letter - 32];
    uint16_t data = gly->bitmapOffset;
    uint16_t bo = gly->bitmapOffset;
    uint8_t w = gly->width;
    uint8_t h = gly->height;
    int8_t xo = gly->xOffset;
    int8_t yo = gly->yOffset;
    uint8_t xx, yy, bits = 0, bit = 0;
    int16_t xo16 = 0, yo16 = 0;

    if (size_x > 1 || size_y > 1) {
        xo16 = xo;
        yo16 = yo;
    }

    for (yy = 0; yy < h; yy++) {
        for (xx = 0; xx < w; xx++) {
            if (!(bit++ & 7)) {
                bits = gfxFont->bitmap[bo++];
            }
            if (bits & 0x80) { 
                if (size_x == 1 && size_y == 1) {
                    rectangle(Rect(x + xo + xx, y + yo + yy, 1, 1));
                }
                else {
                    rectangle(Rect(x + (xo16 + xx) * size_x, y + (yo16 + yy) * size_y, size_x, size_y));
                }
            }
            bits <<= 1;
        }
    }
  }

  void PicoGraphics::customFontWrite(const std::string &text, const Point &p, int32_t wrap, uint8_t scale) {    
    GFXglyph* gly;
    uint16_t data;
    uint8_t c = (uint8_t)text[0];
    size_t i = 0;

    int32_t cursor_x = p.x;
    int32_t cursor_y = p.y;

    while (c != 0) {
        if (c == '\n') {
            cursor_x = 0;
            cursor_y +=
                (int16_t)scale * (uint8_t)gfxFont->yAdvance;
        }
        else if (c != '\r') {

            gly = &gfxFont->glyph[c - 32];
            data = gly->bitmapOffset;

            uint16_t first = gfxFont->first;
            if ((c >= first) && (c <= (uint8_t)gfxFont->last)) {
                GFXglyph* gly = &gfxFont->glyph[c - 32];
                uint8_t w = gly->width,
                    h = gly->height;
                if ((w > 0) && (h > 0)) { // Is there an associated bitmap?
                    int16_t xo = (int8_t)gly->xOffset; // sic
                    if (wrap && ((cursor_x + scale * (xo + w)) > wrap)) {
                        cursor_x = 0;
                        cursor_y += (int16_t)scale *
                            (uint8_t)gfxFont->yAdvance;
                    }
                    customFontDrawChar(c, cursor_x, cursor_y, scale, scale);
                }
                cursor_x +=
                    (uint8_t)gly->xAdvance * (int16_t)scale;
            }
        }     
        c = (uint8_t)text[++i];
    }
 
    return;
  } 


}
