//
// Created by tomas on 11/30/18.
//

#ifndef CMPXCHG_RXBUILDER_H
#define CMPXCHG_RXBUILDER_H


#include <memory>
#include <vector>
#include "Rx.h"

#define MHZ(x) ((long long)(x*1000000.0 + .5))

class RXBuilder {
    typedef RXBuilder Self;
    std::vector<std::string> params;
    std::vector<std::string> channels;
    std::string uri;
    uint32_t K;
public:
    RXBuilder()  {}

    Self &with_bw(float bw_mhz) {
        params.push_back("in_voltage_rf_bandwidth=" +
                                    std::to_string(MHZ(bw_mhz)));
        return *this;
    }

    Self &with_fs(float fs_mhz) {
        params.push_back("in_voltage_sampling_frequency=" +
                                    std::to_string(MHZ(fs_mhz)));
        return *this;
    }

    Self &with_lo(float lo_mhz) {
        params.push_back("out_altvoltage0_RX_LO_frequency=" +
                                    std::to_string(MHZ(lo_mhz)));
        return *this;
    }

    Self &with_lo_hz(float lo_hz) {
        params.push_back("out_altvoltage0_RX_LO_frequency=" +
                         std::to_string((long long)(lo_hz)));
        return *this;
    }

    Self &with_rfport(std::string rfport) {
        params.push_back("in_voltage0_rf_port_select=" +
                                    rfport);
        return *this;
    }

    Self &with_uri(std::string uri) {
        this->uri = uri;
        return *this;
    }

    Self &enable_chn(std::string chn) {
        channels.emplace_back(chn);
        return *this;
    }

    Self &enable_quadrature(bool quadrature) {
        params.push_back("in_voltage_quadrature_tracking_en=" +
                                    std::to_string(quadrature));
        return *this;
    }

    Self &enable_rfdc(bool rfdc) {
        params.push_back("in_voltage_rf_dc_offset_tracking_en=" +
                                    std::to_string(rfdc));
        return *this;
    }

    Self &enable_bbdc(bool bbdc) {
        params.push_back("in_voltage_bb_dc_offset_tracking_en=" +
                                    std::to_string(bbdc));
        return *this;
    }

    Self &gain_mode(std::string gain1_str) {
        params.push_back("in_voltage0_gain_control_mode=" +
                                    gain1_str);
        return *this;
    }

    Self& with_K(uint32_t K) {
        this->K = K;
        return *this;
    }

    Self& with_frames(uint32_t n_frames) {
        n_frames = n_frames;
        return *this;
    }

    operator std::unique_ptr<Rx>() {
        return std::unique_ptr<Rx>(new Rx(K, uri, this->params, this->channels));
    }

};


#endif //CMPXCHG_RXBUILDER_H
