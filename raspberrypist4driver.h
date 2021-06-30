#ifndef RASPBERRYPIST4DRIVER_H
#define RASPBERRYPIST4DRIVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <iostream>

#include <indidevapi.h>

#include "raspberrypist4driveraxis.h"

enum {
    NORTH,
    SOUTH,
    EAST,
    WEST,
};

class RaspberrypiST4Driver {
    private:
        Axis rightAscension;
        Axis declination;
        bool debug;

    public:
        RaspberrypiST4Driver();

        void connect();
        void disconnect();

        void setDebug(bool enable);

        bool startPulse(int direction);
        bool stopPulse(int direction);
};

#endif // RASPBERRYPIST4DRIVER_H
