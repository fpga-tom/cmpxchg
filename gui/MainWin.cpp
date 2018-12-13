//
// Created by tomas on 12/12/18.
//

#include <thread>
#include "MainWin.h"
#include "PlotArea.h"

MainWin::MainWin(int &argc, char** &argv, Tuner& tuner) : tuner(tuner) {

    winThread = std::thread([this, &argc, &argv]()-> void {
        pthread_setname_np(pthread_self(), "win");
        app = Gtk::Application::create(argc, argv);
        Gtk::Window window;
        window.set_default_size(g_iDefaultWidth, g_iDefaultHeight);
        FFTPlot fftPlot1(g_iDefaultWidth, 200, 3, *this);
        WaterfallPlot waterfallPlot1(g_iDefaultWidth, 500);

        fftPlot = std::shared_ptr<FFTPlot>(&fftPlot1);
        waterfallPlot = std::shared_ptr<WaterfallPlot>(&waterfallPlot1);

        Gtk::Box box(Gtk::ORIENTATION_VERTICAL);

        box.pack_start(fftPlot1);
        box.pack_end(waterfallPlot1);
        window.add(box);
        box.show();
        fftPlot1.show();
        waterfallPlot1.show();

//        PlotArea myArea(*this);
//        pa = std::shared_ptr<PlotArea>(&myArea);

//        window.add(myArea);
//        myArea.show();

        app->run(window);

    });

}

void MainWin::send(std::vector<std::pair<float, float>> data) {
    fftPlot->updateData(data);
    waterfallPlot->updateData(data);
//    pa->updateData(data);
}

void MainWin::y_range(uint64_t from, uint64_t to) {
    y_range_from = from;
    y_range_to = to;
}

void MainWin::x_range(uint64_t from, uint64_t to) {
    x_range_from = from;
    x_range_to = to;
}

void MainWin::tune(float freq) {
    tuner.tune(freq);
}

void MainWin::up() {
    tuner.up();
}

void MainWin::down() {
    tuner.down();
}

void MainWin::tuned_to(float freq) {
    fftPlot->tuned_to(freq);
//    pa->tuned_to(freq);
}
