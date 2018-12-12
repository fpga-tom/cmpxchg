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
    yc = height / 2;

    cr->set_line_width(1.0);

    // draw red lines out from the center of the window
    cr->set_source_rgb(0.0, 0.0, 0.0);
    cr->rectangle(0,0, width, height);
    cr->fill();
    cr->set_source_rgb(0.8, 0.0, 0.0);
    cr->move_to(0,yc);
    for(int i = 0; i < data.size(); ++i) {
        cr->line_to(i, yc-data[i].second*3);
    }
    cr->stroke();

    return true;
}

void PlotArea::updateData(std::vector<std::pair<int, float>> &data) {
    this->data = data;

    auto win = get_window();
    if (win)
    {
        Gdk::Rectangle r(0, 0, get_allocation().get_width(),
                         get_allocation().get_height());
        win->invalidate_rect(r, false);
    }
}

PlotArea::PlotArea() {
}
