//
// Created by tomas on 12/12/18.
//

#include <iostream>
#include "PlotArea.h"

bool PlotArea::on_draw(const ::Cairo::RefPtr<::Cairo::Context> &cr) {
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();

    // coordinates for the center of the window
    int xc, yc;
    xc = width / 2;
    yc = height / 4;

    cr->set_line_width(1.0);

    // draw red lines out from the center of the window
    cr->set_source_rgb(.0, .0, .0);
    cr->rectangle(0,0, width, height);
    cr->fill();

//    cr->set_source_rgba(1.0, 1.0, 1.0, 0.1);
//    cr->rectangle(0,0, width, height);
//    cr->fill();
    if(data.size() > 0)
        cr->move_to(0, yc - data[0]*3);
    for(int i = 0; i < data.size(); ++i) {
        cr->line_to(i, yc-data[i]);
    }
    if(data.size() > 0) {
        cr->line_to(width, yc);
        cr->line_to(0, yc);
    }
    cr->close_path();


    ::Cairo::RefPtr<Cairo::LinearGradient> pattern = Cairo::LinearGradient::create(0, 0, 0, yc);
    pattern->add_color_stop_rgb(.5, .0, .0, .9);
    pattern->add_color_stop_rgb(1, 0.1, 0.1, 0.3);
    cr->set_source(pattern);
    cr->fill();

    cr->set_source_rgba(0.9, 0.9, 0.9, .95);
    if(data.size() > 0)
        cr->move_to(0,yc - data[0]);
    cr->set_line_width(1.0);
    for(int i = 0; i < data.size(); ++i) {
        cr->line_to(i, yc-data[i]);
    }
    cr->stroke();

    std::copy(begin(*pixel_data), end(*pixel_data) - 1024*4, begin(*pixel_data) + 1024*4);
    if(data.size() > 0) {
        for (int i = 0; i < 1024; i++) {
            uint32_t rgb_val = rgb(data[i]/512.);
            (*pixel_data)[i * 4] = rgb_val&0xff;
            (*pixel_data)[i * 4 + 1] = (rgb_val >> 8)&0xff;
            (*pixel_data)[i * 4 + 2] = (rgb_val >> 16)&0xff;
            (*pixel_data)[i * 4 + 3] = 0x0;
        }
    }
    cr->save();
    cr->translate(0, yc);
    cr->set_source(surface, 0, 0);
    cr->rectangle(0,0, width, height);
    cr->fill();

    cr->restore();
    cr->set_source_rgba(1.0, 1.0, 1.0, 0.1);
    cr->rectangle(tuner_x - 25,0, 50, height);
    cr->fill();

    cr->set_source_rgba(0.5, 0.5, 0.5, 0.3);
    cr->rectangle(tuned_x - 25,0, 50, height);
    cr->fill();



    return true;
}

unsigned int PlotArea::rgb(double hue)
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

void PlotArea::updateData(std::vector< float> &data) {
    this->data = data;

    auto win = get_window();
    if (win)
    {
        Gdk::Rectangle r(0, 0, get_allocation().get_width(),
                         get_allocation().get_height());
        win->invalidate_rect(r, false);
    }
}

PlotArea::PlotArea(PlotAreaMediator &mediator) : tuner_x(0), tuned_x(0), mediator(mediator) {
    add_events(Gdk::EventMask::POINTER_MOTION_MASK);
    add_events(Gdk::EventMask::BUTTON_PRESS_MASK);
    add_events(Gdk::EventMask::SCROLL_MASK);
    this->signal_motion_notify_event().connect_notify(sigc::mem_fun(*this, &PlotArea::mouse));
    this->signal_button_press_event().connect_notify(sigc::mem_fun(*this, &PlotArea::tune));
    this->signal_scroll_event().connect_notify(sigc::mem_fun(*this, &PlotArea::scroll));

    int stride = Cairo::ImageSurface::format_stride_for_width (Cairo::FORMAT_RGB24, 1024);
    pixel_data = std::unique_ptr<std::vector<uint8_t >>(new std::vector<uint8_t>(4*1024*600));
    std::fill(begin(*pixel_data), end(*pixel_data), 0);
    surface = Cairo::ImageSurface::create(pixel_data->data(), Cairo::FORMAT_RGB24, 1024,600, stride);

}

void PlotArea::mouse(GdkEventMotion* event) {
    tuner_x = event->x;
}

void PlotArea::tune(GdkEventButton *event) {
    tuned_x = event->x;

    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();

    float freq = (tuner_x - width/2.) / width;
    mediator.tune(freq);
}

void PlotArea::scroll(GdkEventScroll *event) {
    if(event->direction == GdkScrollDirection::GDK_SCROLL_UP) {
        mediator.up();
    }
    if(event->direction == GdkScrollDirection::GDK_SCROLL_DOWN) {
        mediator.down();
    }
}

