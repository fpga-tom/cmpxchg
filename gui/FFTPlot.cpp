//
// Created by tomas on 12/13/18.
//

#include <iomanip>
#include "FFTPlot.h"

bool FFTPlot::on_draw(const ::Cairo::RefPtr<::Cairo::Context> &cr) {
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();

    int xc = width / 2;

    // draw texture
    cr->set_source_rgb(.0, .0, .0);
    cr->rectangle(0, 0, width, height);
    cr->fill();

    if (data.size() > 0) {
        cr->move_to(0, height - (data)[0].second * scale);
        for (int i = 0; i < data.size(); i += 1) {
            cr->line_to(i, height - (data)[i].second);
        }
        if (data.size() > 0) {
            cr->line_to(width, height);
            cr->line_to(0, height);
        }
        cr->close_path();
    }

    ::Cairo::RefPtr<Cairo::LinearGradient> pattern = Cairo::LinearGradient::create(0, 0, 0, height);
    pattern->add_color_stop_rgb(.5, .0, .0, .9);
    pattern->add_color_stop_rgb(1, 0.1, 0.1, 0.3);
    cr->set_source(pattern);
    cr->fill();

    // draw outline

    cr->set_source_rgba(0.9, 0.9, 0.9, .95);
    cr->set_line_width(1.0);
    if (data.size() > 0) {
        cr->move_to(0, height - (data)[0].second);
        for (int i = 0; i < data.size(); i += 1) {
            cr->line_to(i, height - (data)[i].second);
        }
        cr->stroke();
    }

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

    // draw scale
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

    // draw frequency
    std::ostringstream s;
    s << std::fixed;
    s << std::setprecision(1);
    float fr = (((float)tuned_x - xc) / xc);
    s << fr *(3.072 * 3/2) + freq/1e6;
    cr->move_to(10, 43);
    cr->show_text(s.str() + "Mhz");

    cr->set_source_rgba(1.0, 1.0, 1.0, 0.1);
    cr->rectangle(tuner_x - 25,0, 50, height);
    cr->fill();

    cr->set_source_rgba(0.5, 0.5, 0.5, 0.3);
    cr->rectangle(tuned_x - 25,0, 50, height);
    cr->fill();

    current = 30;
    for (int i = 0; i < 9; i++) {
        cr->move_to(0, current);
        cr->line_to(width, current);
        current += 50;
    }

    cr->stroke();

    return true;
}

FFTPlot::FFTPlot(uint32_t width, uint32_t height, float scale, FFTMediator& mediator) :
        width(width), height(height), scale(scale), mediator(mediator), tuned_x(width/2), tuner_x(width/2) {
    add_events(Gdk::EventMask::POINTER_MOTION_MASK);
    add_events(Gdk::EventMask::BUTTON_PRESS_MASK);
    add_events(Gdk::EventMask::SCROLL_MASK);
    this->signal_motion_notify_event().connect_notify(sigc::mem_fun(*this, &FFTPlot::mouse));
    this->signal_button_press_event().connect_notify(sigc::mem_fun(*this, &FFTPlot::tune));
    this->signal_scroll_event().connect_notify(sigc::mem_fun(*this, &FFTPlot::scroll));

}

void FFTPlot::updateData(std::vector<std::pair<float, float>> data) {
    this->data = data;

    auto win = get_window();
    if (win)
    {
        Gdk::Rectangle r(0, 0, get_allocation().get_width(),
                         get_allocation().get_height());
        win->invalidate_rect(r, false);
    }

}

void FFTPlot::tuned_to(float freq) {
    this->freq = freq;
}

void FFTPlot::mouse(GdkEventMotion *event) {
    tuner_x = event->x;
}

void FFTPlot::tune(GdkEventButton *event) {
    tuned_x = event->x;

    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();

    float freq = (tuner_x - width/2.) / width;
    mediator.tune(freq);

}

void FFTPlot::scroll(GdkEventScroll *event) {
    if(event->direction == GdkScrollDirection::GDK_SCROLL_UP) {
        mediator.up();
    }
    if(event->direction == GdkScrollDirection::GDK_SCROLL_DOWN) {
        mediator.down();
    }
}
