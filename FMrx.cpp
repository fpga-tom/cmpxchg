//
// Created by tomas on 12/12/18.
//

#include "FMrx.h"
#include "RxBuilder.h"
#include "FmDemod.h"
#include "Sniff.h"
#include "FFT.h"
#include "Spectrogram.h"
#include "PaAudio.h"
#include "GtkSink.h"
#include "gui/MainWin.h"
#include "Nco.h"

void FMrx::play(int &argc, char** &argv)  {
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
            .with_bw(10.2)
            .with_fs(3.072*3)
            .with_K(0x20000);

    nco = std::unique_ptr<NCO>(new NCO(0x20000));
    FmDemod fm(0x20000,16*3,4);
    PaAudio pa(0x20000/64/3);
    nco->p_in().bind(rx->p_out());
    fm.p_in().bind(nco->p_out());
    pa.p_in().bind(fm.p_out());

    const int fft_len = 1024;
    Sniff sniff(rx->p_out(), fft_len*sizeof(std::complex<float>));
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

    nco->start();
    rx->start_rx();
    rx->start();
    fm.start();
    pa.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    sniff.start();
    fft.start();
    spectrogram.start();
    gp.start();

    nco->join();
    pa.join();
    rx->join();
    fm.join();
    sniff.join();
    fft.join();
    spectrogram.join();
    gp.join();
}

void FMrx::tune(float freq) {
    nco->tune(freq);
}

void FMrx::up() {
    lo += 4000000;
    rx->tune(lo);
    mainWin->tuned_to(lo);
}

void FMrx::down() {
    lo -= 4000000;
    rx->tune(lo);
    mainWin->tuned_to(lo);
}
