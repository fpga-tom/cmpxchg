//
// Created by tomas on 12/12/18.
//

#ifndef CMPXCHG_PLOTAREA_H
#define CMPXCHG_PLOTAREA_H

#include <gtkmm.h>

class PlotArea : public Gtk::DrawingArea {

    std::vector<std::pair<int, float>> data;
public:
    PlotArea();
    void updateData(std::vector<std::pair<int, float>> &data);
    virtual bool on_draw(const ::Cairo::RefPtr< ::Cairo::Context>& cr);

};


#endif //CMPXCHG_PLOTAREA_H
