//
// Created by tomas on 12/13/18.
//

#ifndef CMPXCHG_FFTPLOT_H
#define CMPXCHG_FFTPLOT_H


#include <gtkmm.h>

class FFTMediator {
public:
    virtual void tune(float freq) = 0;
    virtual void up() = 0;
    virtual void down() = 0;
};

class FFTPlot : public Gtk::DrawingArea {
    bool on_draw(const ::Cairo::RefPtr< ::Cairo::Context>& cr) override;
    uint32_t width, height;
    float scale;
    float freq;
    int64_t tuned_x, tuner_x;
    FFTMediator &mediator;

    std::vector<std::pair<float, float>> data;
public:
    FFTPlot(uint32_t width, uint32_t height, float scale, FFTMediator& mediator);
    void set_scale(float scale) { this->scale = scale; }
    void updateData(std::vector<std::pair<float, float>> data);
    void tuned_to(float freq);
    void mouse(GdkEventMotion* event);
    void tune(GdkEventButton* event);
    void scroll(GdkEventScroll* event);
};


#endif //CMPXCHG_FFTPLOT_H
