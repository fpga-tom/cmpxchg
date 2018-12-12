//
// Created by tomas on 12/12/18.
//

#ifndef CMPXCHG_PLOTAREA_H
#define CMPXCHG_PLOTAREA_H

#include <gtkmm.h>

class PlotAreaMediator {
public:
    virtual void tune(float freq) = 0;
    virtual void up() = 0;
    virtual void down() = 0;
};

class PlotArea : public Gtk::DrawingArea {

    std::vector<float> data;
    uint64_t tuner_x, tuned_x;
    PlotAreaMediator &mediator;
    Cairo::RefPtr<Cairo::ImageSurface> surface;
    std::unique_ptr<std::vector<uint8_t >> pixel_data;

    unsigned int rgb(double);
public:
    PlotArea(PlotAreaMediator &mediator);
    void updateData(std::vector<float> &data);
    void mouse(GdkEventMotion* event);
    void tune(GdkEventButton* event);
    void scroll(GdkEventScroll* event);
    virtual bool on_draw(const ::Cairo::RefPtr< ::Cairo::Context>& cr);

};


#endif //CMPXCHG_PLOTAREA_H
