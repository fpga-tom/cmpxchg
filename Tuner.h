//
// Created by tomas on 12/12/18.
//

#ifndef CMPXCHG_TUNER_H
#define CMPXCHG_TUNER_H


class Tuner {
public:
    virtual void tune(float freq) = 0;
    virtual void up() = 0;
    virtual void down() = 0;
};


#endif //CMPXCHG_TUNER_H
