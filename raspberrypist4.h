#ifndef RASPBERRYPIST4_H
#define RASPBERRYPIST4_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <memory>

#include <defaultdevice.h>
#include <indiguiderinterface.h>

#include "raspberrypist4driver.h"

// #define POLLMS 250

class RaspberrypiST4Driver;

class RaspberrypiST4: public INDI::DefaultDevice, public INDI::GuiderInterface {
// class ARDUST4: public INDI::GuiderInterface, public INDI::DefaultDevice {
    public:
        RaspberrypiST4();
        virtual ~RaspberrypiST4();

        virtual bool initProperties();
        virtual bool updateProperties();
        virtual void ISGetProperties (const char *dev);
        virtual bool ISNewNumber (const char *dev, const char *name, double values[], char *names[], int n);
        virtual bool ISNewSwitch (const char *dev, const char *name, ISState *states, char *names[], int n);
        virtual bool ISNewText (const char *dev, const char *name, char *texts[], char *names[], int n);
        virtual bool ISSnoopDevice (XMLEle *root);

    protected:
        // indi : default device
        const char *getDefaultName();

        bool Connect();
        bool Disconnect();
        void debugTriggered(bool enable);
        void TimerHit();

        // indi : guider interface
        virtual IPState GuideNorth(uint32_t ms);
        virtual IPState GuideSouth(uint32_t ms);
        virtual IPState GuideEast(uint32_t ms);
        virtual IPState GuideWest(uint32_t ms);

    private:
        RaspberrypiST4Driver *driver;

        float CalcWEPulseTimeLeft();
        float CalcNSPulseTimeLeft();

        int WEDir;
        bool InWEPulse;
        float WEPulseRequest;
        struct timeval WEPulseStart;
        int WEtimerID;

        int NSDir;
        bool InNSPulse;
        float NSPulseRequest;
        struct timeval NSPulseStart;
        int NStimerID;
};

#endif // RASPBERRYPIST4_H
