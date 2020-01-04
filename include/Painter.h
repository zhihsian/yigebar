#ifndef YIGEBAR_SRC_PAINTER_H
#define YIGEBAR_SRC_PAINTER_H

#include <string>

#include <xcb/xcb.h>

class X11;

typedef struct _cairo cairo_t;
typedef struct _cairo_surface cairo_surface_t;

struct ColorARGB {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;
#else
    uint8_t alpha;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
#endif

    static ColorARGB fromul(uint32_t num) {
        return *(reinterpret_cast<ColorARGB *>(&num));
    }

    uint32_t toul() { return *(reinterpret_cast<uint32_t *>(this)); }
};

class Painter {
public:
    Painter(xcb_connection_t *conn,
            xcb_window_t window_id,
            xcb_visualtype_t *visual,
            uint16_t w,
            uint16_t h);
    ~Painter();

    void setFontFace(const std::string &font);
    void setFontSize(uint16_t s);

    void getTextSize(const std::string &text, uint16_t *w, uint16_t *h);
    void showText(uint16_t x, uint16_t y, const std::string &text);

    void drawRectangle(uint16_t x,
                       uint16_t y,
                       uint16_t width,
                       uint16_t height,
                       ColorARGB color);

private:
    xcb_connection_t *conn_;
    cairo_surface_t *surface_;
    cairo_t *cr_;
};

#endif // !YIGEBAR_SRC_PAINTER_H
