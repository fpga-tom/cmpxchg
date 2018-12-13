//
// Created by tomas on 12/13/18.
//

#include "WaterfallPlot.h"

bool WaterfallPlot::on_draw(const ::Cairo::RefPtr<::Cairo::Context> &cr) {
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();

    std::copy(begin(*pixel_data), end(*pixel_data) - this->width*4, begin(*pixel_data) + this->width*4);
    if(data.size() > 0) {
        for (int i = 0; i < width; ++i) {
            uint32_t rgb_val = rgb((data)[i].second/512.);
            (*pixel_data)[i * 4] = rgb_val&0xff;
            (*pixel_data)[i * 4 + 1] = (rgb_val >> 8)&0xff;
            (*pixel_data)[i * 4 + 2] = (rgb_val >> 16)&0xff;
            (*pixel_data)[i * 4 + 3] = 0x0;
        }
    }
    cr->set_source(surface, 0, 0);
    cr->rectangle(0,0, width, height);
    cr->fill();
    return true;
}

unsigned int WaterfallPlot::rgb(double hue)
{
    int h = int(hue * 256 * 6);
    int x = h % 0x100;

    int r = 0, g = 0, b = 0;
    switch (h / 256)
    {
        case 0: r = 255; g = x;       break;
        case 1: g = 255; r = 255 - x; break;
        case 2: g = 255; b = x;       break;
        case 3: b = 255; g = 255 - x; break;
        case 4: b = 255; r = x;       break;
        case 5: r = 255; b = 255 - x; break;
    }

    return r + (g << 8) + (b << 16);
}

WaterfallPlot::WaterfallPlot(uint32_t width, uint32_t height) : width(width), height(height) {
    int stride = Cairo::ImageSurface::format_stride_for_width (Cairo::FORMAT_RGB24, width);
    pixel_data = std::unique_ptr<std::vector<uint8_t >>(new std::vector<uint8_t>(4*width*height));
    std::fill(begin(*pixel_data), end(*pixel_data), 0);
    surface = Cairo::ImageSurface::create(pixel_data->data(), Cairo::FORMAT_RGB24, width, height, stride);
}

void WaterfallPlot::updateData(std::vector<std::pair<float, float>> data) {
    this->data = data;

    auto win = get_window();
    if (win)
    {
        Gdk::Rectangle r(0, 0, get_allocation().get_width(),
                         get_allocation().get_height());
        win->invalidate_rect(r, false);
    }
}
