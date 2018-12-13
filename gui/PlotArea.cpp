//
// Created by tomas on 12/12/18.
//

#include <iostream>
#include <iomanip>
#include "PlotArea.h"

bool PlotArea::on_draw(const ::Cairo::RefPtr<::Cairo::Context> &cr) {
    update_in_progress = true;
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
    for(int i = 0; i < data.size(); i+=1) {
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
    for(int i = 0; i < data.size(); i+=1) {
        cr->line_to(i, yc-data[i]);
    }
    cr->stroke();

    std::copy(begin(*pixel_data), end(*pixel_data) - 1024*4, begin(*pixel_data) + 1024*4);
    if(data.size() > 0) {
        for (int i = 0; i < 1024; ++i) {
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

    cr->set_source_rgb(1.,1.,1.);

    int step = xc / 30;
    int current = xc;
    for(int i = 0; i < 30; i++) {
        cr->move_to(current, 0);
        if (i % 10 == 0)
            cr->line_to(current, 10);
        else if( i % 5 == 0)
            cr->line_to(current, 7);
        else
            cr->line_to(current, 4);
        current += step;
    }
    current = xc - step;
    for(int i = 1; i < 30; i++) {
        cr->move_to(current, 0);
        if (i % 10 == 0)
            cr->line_to(current, 10);
        else if( i % 5 == 0)
            cr->line_to(current, 7);
        else
            cr->line_to(current, 4);
        current -= step;
    }
    cr->stroke();

    current = xc;
    step = xc / 6;
    cr->set_source_rgb(0.9, 0.9, 0.9);

    cr->select_font_face("Purisa",
                           Cairo::FontSlant::FONT_SLANT_NORMAL,
                           Cairo::FontWeight::FONT_WEIGHT_BOLD);

    cr->set_font_size(13);
    for(int i = 0; i < 6; i++) {
        std::ostringstream s;
        s << std::fixed;
        s << std::setprecision(1);
        s << (i*5.f/30.f) *(3.072 * 3/2) + freq/1e6;
        cr->move_to(current-15, 23);
        cr->show_text(s.str());
        current += step;
    }

    current = xc-step;
    for(int i = 1; i < 6; i++) {
        std::ostringstream s;
        s << std::fixed;
        s << std::setprecision(1);
        s << (-i*5.f/30.f) *(3.072 * 3/2) + freq/1e6;
        cr->move_to(current-15, 23);
        cr->show_text(s.str());
        current -= step;
    }

    std::ostringstream s;
    s << std::fixed;
    s << std::setprecision(1);
    float fr = (((float)tuned_x - xc) / xc);
    s << fr *(3.072 * 3/2) + freq/1e6;
    cr->move_to(10, 43);
    cr->show_text(s.str() + "Mhz");

    update_in_progress = false;
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
    if(update_in_progress)
        return;
    this->data = data;

    auto win = get_window();
    if (win)
    {
        Gdk::Rectangle r(0, 0, get_allocation().get_width(),
                         get_allocation().get_height());
        win->invalidate_rect(r, false);
    }
}

PlotArea::PlotArea(PlotAreaMediator &mediator) : tuner_x(0), tuned_x(0), mediator(mediator), update_in_progress(false) {
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

void PlotArea::tuned_to(float freq) {
    this->freq = freq;
}

