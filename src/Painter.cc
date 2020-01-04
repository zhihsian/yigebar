#include "Painter.h"

#include <cairo/cairo-xcb.h>

Painter::Painter(xcb_connection_t *conn,
                 xcb_window_t window_id,
                 xcb_visualtype_t *visual,
                 uint16_t w,
                 uint16_t h): conn_(conn) {
    surface_ = cairo_xcb_surface_create(conn, window_id, visual, w, h);
    cr_ = cairo_create(surface_);
}

Painter::~Painter() {
    if (surface_) cairo_surface_finish(surface_);
}

void Painter::setFontFace(const std::string &font) {
    cairo_select_font_face(cr_,
                           font.c_str(),
                           CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
}

void Painter::setFontSize(uint16_t s) {
    cairo_set_font_size(cr_, s);
}

void Painter::getTextSize(const std::string &text, uint16_t *w, uint16_t *h) {
    cairo_text_extents_t extents;
    cairo_text_extents(cr_, text.c_str(), &extents);

    *w = extents.width;
    *h = extents.height;
}

void Painter::showText(uint16_t x, uint16_t y, const std::string &text) {
    cairo_set_source_rgb(cr_, 1, 1, 1);
    cairo_move_to(cr_, x, y);
    cairo_show_text(cr_, text.c_str());
    cairo_surface_flush(surface_);
}

void Painter::drawRectangle(uint16_t x,
                        uint16_t y,
                        uint16_t width,
                        uint16_t height,
                        ColorARGB color) {
    cairo_set_source_rgb(cr_,
                         color.red / 255.0,
                         color.green / 255.0,
                         color.blue / 255.0);
    cairo_rectangle(cr_, x, y, width, height);
    cairo_fill(cr_);
    cairo_surface_flush(surface_);
}
