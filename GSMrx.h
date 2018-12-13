//
// Created by tomas on 12/13/18.
//

#ifndef CMPXCHG_GSMRX_H
#define CMPXCHG_GSMRX_H


#include <memory>
#include <cstdint>
#include "Tuner.h"
#include "Rx.h"
#include "gui/MainWin.h"

class GSMrx : public Tuner {
    std::unique_ptr<Rx> rx;
    std::unique_ptr<MainWin> mainWin;
    uint32_t lo;
public:
    GSMrx(uint32_t lo) : lo(lo) {}
    void receive(int &argc, char** &argv);
    void tune(float freq) override;
    void up() override;
    void down() override;

};


#endif //CMPXCHG_GSMRX_H
