/*
 * Test stub for librosco.
 *
 * The real librosco talks to a physical MEMS ECU over a serial port, which is
 * impossible to exercise in a unit test. This file provides controllable
 * replacements for the handful of librosco functions used by the ECU classes
 * in scr/ecu so that their behaviour can be verified in isolation.
 *
 * Each stub records the pointer it was called with and returns a value that
 * the tests can configure through the rosco_stub namespace below.
 */

#include "../rosco.h"
#include "librosco_stub.h"

namespace rosco_stub
{
    bool readReturn = true;
    bool initLinkReturn = true;
    bool clearFaultsReturn = true;
    bool resetAdjustmentsReturn = true;
    bool resetECUReturn = true;

    mems_info *lastInfo = nullptr;
    uint8_t *lastBuffer = nullptr;
    mems_data *lastData = nullptr;

    void reset()
    {
        readReturn = true;
        initLinkReturn = true;
        clearFaultsReturn = true;
        resetAdjustmentsReturn = true;
        resetECUReturn = true;
        lastInfo = nullptr;
        lastBuffer = nullptr;
        lastData = nullptr;
    }
}

extern "C"
{
    bool mems_read(mems_info *info, mems_data *data)
    {
        rosco_stub::lastInfo = info;
        rosco_stub::lastData = data;
        return rosco_stub::readReturn;
    }

    bool mems_init_link(mems_info *info, uint8_t *d0_response_buffer)
    {
        rosco_stub::lastInfo = info;
        rosco_stub::lastBuffer = d0_response_buffer;
        return rosco_stub::initLinkReturn;
    }

    bool mems_clear_faults(mems_info *info)
    {
        rosco_stub::lastInfo = info;
        return rosco_stub::clearFaultsReturn;
    }

    bool mems_reset_adjustments(mems_info *info)
    {
        rosco_stub::lastInfo = info;
        return rosco_stub::resetAdjustmentsReturn;
    }

    bool mems_reset_ECU(mems_info *info)
    {
        rosco_stub::lastInfo = info;
        return rosco_stub::resetECUReturn;
    }
}
