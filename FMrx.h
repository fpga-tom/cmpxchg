//
// Created by tomas on 12/12/18.
//

#ifndef CMPXCHG_FMRX_H
#define CMPXCHG_FMRX_H

#include "gui/PlotArea.h"
#include "Tuner.h"
#include "Nco.h"
#include "Rx.h"

class FMrx : public Tuner {

    std::unique_ptr<NCO> nco;
    std::unique_ptr<Rx> rx;

    uint32_t lo;

public:
    FMrx(uint32_t lo) : lo(lo) {}
    void play(int &argc, char** &argv);
    void tune(float freq) override;
    void up() override;
    void down() override;

};


#endif //CMPXCHG_FMRX_H
