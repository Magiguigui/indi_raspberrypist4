#include "raspberrypist4driver.h"

RaspberrypiST4Driver::RaspberrypiST4Driver() :
        rightAscension(16, 20),   // RA+ pin  // RA- pin
        declination(19, 26),      // DEC+ pin // DEC- pin
        debug(false) { }


void RaspberrypiST4Driver::connect() {
    this->rightAscension.connect();
    this->declination.connect();
}

void RaspberrypiST4Driver::disconnect() {
    this->rightAscension.disconnect();
    this->declination.disconnect();
}

void RaspberrypiST4Driver::setDebug(bool enable) {
    debug = enable;
}

bool RaspberrypiST4Driver::startPulse(int direction) {
    switch (direction) {
        case NORTH:
            if (debug) IDLog("Start North\n");
            rightAscension.plus();
            break;
        case WEST:
            if (debug) IDLog("Start West\n");
            declination.plus();
            break;
        case SOUTH:
            if (debug) IDLog("Start South\n");
            rightAscension.minus();
            break;
        case EAST:
            if (debug) IDLog("Start East\n");
            declination.minus();
            break;
    }
    return true;
}

bool RaspberrypiST4Driver::stopPulse(int direction) {
    switch (direction) {
        case NORTH:
            if (debug) IDLog("Stop North\n");
            rightAscension.reset();
            break;
        case WEST:
            if (debug) IDLog("Stop West\n");
            declination.reset();
            break;
        case SOUTH:
            if (debug) IDLog("Stop South\n");
            rightAscension.reset();
            break;
        case EAST:
            if (debug) IDLog("Stop East\n");
            declination.reset();
            break;
    }
    return true;
}
