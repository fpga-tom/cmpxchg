//
// Created by tomas on 12/12/18.
//

#ifndef CMPXCHG_MAINWIN_H
#define CMPXCHG_MAINWIN_H

#include <memory>
#include <thread>
#include <gtkmm.h>
#include "../GtkSink.h"
#include "../Tuner.h"
#include "FFTPlot.h"
#include "WaterfallPlot.h"


#define MIN_WIDTH          32
#define MIN_HEIGHT         32
#define MAX_WIDTH         1024
#define MAX_HEIGHT        768
class MainWin : public GtkProto, FFTMediator {


    std::thread winThread;

    Glib::RefPtr<Gtk::Application> app;


    const uint64_t                                      g_iDefaultWidth = MAX_WIDTH;
    const uint64_t                                     g_iDefaultHeight = MAX_HEIGHT;

    uint64_t x_range_from, x_range_to;
    uint64_t y_range_from, y_range_to;
    std::vector<std::pair<int, float>> data;
//    std::shared_ptr<PlotArea> pa;
    std::shared_ptr<FFTPlot> fftPlot;
    std::shared_ptr<WaterfallPlot> waterfallPlot;
    Tuner& tuner;

public:
    MainWin(int &argc, char** &argv, Tuner &tuner);

    void x_range(uint64_t from, uint64_t to) override;

    void y_range(uint64_t from, uint64_t to) override;

    void send(std::vector< std::pair<float, float>> data) override;

    void tune(float freq) override;

    void up() override;

    void down() override;

    void tuned_to(float freq) override;

};


#endif //CMPXCHG_MAINWIN_H
