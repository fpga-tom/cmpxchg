//
// Created by tomas on 12/13/18.
//

#include "GSMrx.h"
#include "RxBuilder.h"
#include "GsmDemod.h"
#include "DummySink.h"
#include "Sniff.h"
#include "FFT.h"
#include "Spectrogram.h"

void GSMrx::tune(float freq) {

}

void GSMrx::up() {
    lo += 200000;
    rx->tune(lo);
    mainWin->tuned_to(lo);

}

void GSMrx::down() {
    lo -= 200000;
    rx->tune(lo);
    mainWin->tuned_to(lo);

}

void GSMrx::receive(int &argc, char **&argv) {

    RXBuilder builder;
    rx = builder
            .with_rfport("A_BALANCED")
            .with_uri("192.168.4.10")
            .enable_chn("voltage0")
            .enable_chn("voltage1")
            .enable_bbdc(true)
            .enable_rfdc(true)
            .enable_quadrature(true)
            .gain_mode("fast_attack")
            .with_lo_hz(lo)
            .with_bw(0.2)
            .with_fs(3.25)
            .with_K(0x10008);

    GsmDemod gsmDemod(0x10008, 12);
    gsmDemod.p_in().bind(rx->p_out());
    DummySink ds;
    ds.p_in().bind(gsmDemod.p_out());


    const int fft_len = 1024;
    Sniff sniff(gsmDemod.p_out(), fft_len*sizeof(std::complex<float>));
    FFT fft(fft_len);
    Spectrogram spectrogram(fft_len);
    mainWin = std::unique_ptr<MainWin>(new MainWin(argc, argv, *this));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    mainWin->tuned_to(lo);
    GtkSink gp(fft_len, *mainWin);
    fft.p_in().bind(sniff.p_out());
    spectrogram.p_in().bind(fft.p_out());
    gp.p_in().bind(spectrogram.p_out());



    pthread_setname_np(pthread_self(), "main");


    rx->start_rx();
    rx->start();
    gsmDemod.start();
    ds.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    sniff.start();
    fft.start();
    spectrogram.start();
    gp.start();

    rx->join();
    gsmDemod.join();
    ds.join();

    sniff.join();
    fft.join();
    spectrogram.join();
    gp.join();

}
