//
// Created by tomas on 12/12/18.
//

#include <thread>
#include "MainWin.h"
#include "PlotArea.h"

MainWin::MainWin(int &argc, char** &argv) {

    winThread = std::thread([this, &argc, &argv]()-> void {
        app = Gtk::Application::create(argc, argv);
        Gtk::Window window;
        window.set_default_size(g_iDefaultWidth, g_iDefaultHeight);
        PlotArea myArea;
        pa = std::shared_ptr<PlotArea>(&myArea);

        window.add(myArea);
        myArea.show();

        app->run(window);


    });

}

void MainWin::send(std::vector<std::pair<int, float>> data) {
    pa->updateData(data);
}

void MainWin::y_range(uint64_t from, uint64_t to) {
    y_range_from = from;
    y_range_to = to;
}

void MainWin::x_range(uint64_t from, uint64_t to) {
    x_range_from = from;
    x_range_to = to;
}