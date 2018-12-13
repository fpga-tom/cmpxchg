//
// Created by tomas on 12/13/18.
//

#ifndef CMPXCHG_WATERFALLPLOT_H
#define CMPXCHG_WATERFALLPLOT_H


#include <gtkmm.h>

class WaterfallPlot : public Gtk::DrawingArea {
    Cairo::RefPtr<Cairo::ImageSurface> surface;
    std::unique_ptr<std::vector<uint8_t >> pixel_data;
    unsigned int rgb(double);

    std::vector<std::pair<float, float>> data;

    bool on_draw(const ::Cairo::RefPtr< ::Cairo::Context>& cr) override;
    uint32_t width, height;
public:
    WaterfallPlot(uint32_t width, uint32_t height);
    void updateData(std::vector<std::pair<float, float>> data);
};


#endif //CMPXCHG_WATERFALLPLOT_H
