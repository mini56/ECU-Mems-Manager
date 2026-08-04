#ifndef LIBROSCO_STUB_H
#define LIBROSCO_STUB_H

#include "../rosco.h"

/*
 * Controls and observations for the librosco test stub (librosco_stub.cpp).
 * Tests set the *Return flags to drive the behaviour of the stubbed
 * librosco calls and inspect the last* pointers to confirm the arguments
 * that the ECU classes forwarded.
 */
namespace rosco_stub
{
    extern bool readReturn;
    extern bool initLinkReturn;
    extern bool clearFaultsReturn;
    extern bool resetAdjustmentsReturn;
    extern bool resetECUReturn;

    extern mems_info *lastInfo;
    extern uint8_t *lastBuffer;
    extern mems_data *lastData;

    void reset();
}

#endif // LIBROSCO_STUB_H
