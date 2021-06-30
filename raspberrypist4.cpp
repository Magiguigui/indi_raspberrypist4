#include "raspberrypist4.h"

// We declare an auto pointer to gpGuide.
std::unique_ptr<RaspberrypiST4> gpGuide(new RaspberrypiST4());

void ISGetProperties(const char *dev) {
        gpGuide->ISGetProperties(dev);
}

void ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int num) {
        gpGuide->ISNewSwitch(dev, name, states, names, num);
}

void ISNewText(	const char *dev, const char *name, char *texts[], char *names[], int num) {
        gpGuide->ISNewText(dev, name, texts, names, num);
}

void ISNewNumber(const char *dev, const char *name, double values[], char *names[], int num) {
        gpGuide->ISNewNumber(dev, name, values, names, num);
}

void ISNewBLOB (const char *dev, const char *name, int sizes[], int blobsizes[], char *blobs[], char *formats[], char *names[], int n) {
  INDI_UNUSED(dev);
  INDI_UNUSED(name);
  INDI_UNUSED(sizes);
  INDI_UNUSED(blobsizes);
  INDI_UNUSED(blobs);
  INDI_UNUSED(formats);
  INDI_UNUSED(names);
  INDI_UNUSED(n);
}

void ISSnoopDevice (XMLEle *root) {
    INDI_UNUSED(root);
}

RaspberrypiST4::RaspberrypiST4() {
    driver = new RaspberrypiST4Driver();
    WEDir = NSDir = 0;
    InWEPulse = InNSPulse = false;
    WEPulseRequest = NSPulseRequest =0;
    WEtimerID = NStimerID = 0;
}

RaspberrypiST4::~RaspberrypiST4() {
    delete (driver);
}

const char * RaspberrypiST4::getDefaultName() {
    return "Raspberry Pi ST4";
}

bool RaspberrypiST4::Connect() {
    IDMessage(getDeviceName(), "RASPBERRYPI ST4 is online.");
    driver->connect();
    driver->setDebug(isDebug());
    return true;
}

bool RaspberrypiST4::Disconnect() {
    IDMessage(getDeviceName(), "RASPBERRYPI ST4 is offline.");
    driver->disconnect();
}

bool RaspberrypiST4::initProperties() {
    INDI::DefaultDevice::initProperties();
    initGuiderProperties(getDeviceName(), MOTION_TAB);
    addDebugControl();
    return true;
}

bool RaspberrypiST4::updateProperties() {
    INDI::DefaultDevice::updateProperties();
    if (isConnected()) {
        defineNumber(&GuideNSNP);
        defineNumber(&GuideWENP);
    } else {
        deleteProperty(GuideNSNP.name);
        deleteProperty(GuideWENP.name);
    }
    return true;
}

void RaspberrypiST4::ISGetProperties (const char *dev) {
    INDI::DefaultDevice::ISGetProperties(dev);
}

bool RaspberrypiST4::ISNewNumber (const char *dev, const char *name, double values[], char *names[], int n) {
    if(strcmp(dev, getDeviceName()) == 0) {
        if (!strcmp(name, GuideNSNP.name) || !strcmp(name, GuideWENP.name)) {
            processGuiderProperties(name, values, names, n);
            return true;
        }
    }
    return INDI::DefaultDevice::ISNewNumber(dev, name, values, names, n);
}

bool RaspberrypiST4::ISNewSwitch (const char *dev, const char *name, ISState *states, char *names[], int n) {
    return INDI::DefaultDevice::ISNewSwitch(dev, name, states, names, n);
}

bool RaspberrypiST4::ISNewText (const char *dev, const char *name, char *texts[], char *names[], int n) {
    return INDI::DefaultDevice::ISNewText(dev, name, texts, names, n);
}

bool RaspberrypiST4::ISSnoopDevice (XMLEle *root) {
    return INDI::DefaultDevice::ISSnoopDevice(root);
}

void RaspberrypiST4::debugTriggered(bool enable) {
    driver->setDebug(enable);
}

float RaspberrypiST4::CalcWEPulseTimeLeft() {
    double timesince;
    double timeleft;
    struct timeval now;
    gettimeofday(&now,NULL);
    timesince = (double)(now.tv_sec * 1000.0 + now.tv_usec / 1000) - (double)(WEPulseStart.tv_sec * 1000.0 + WEPulseStart.tv_usec / 1000);
    timesince = timesince / 1000;
    timeleft = WEPulseRequest - timesince;
    return timeleft;
}

float RaspberrypiST4::CalcNSPulseTimeLeft() {
    double timesince;
    double timeleft;
    struct timeval now;
    gettimeofday(&now,NULL);
    timesince = (double)(now.tv_sec * 1000.0 + now.tv_usec/  1000) - (double)(NSPulseStart.tv_sec * 1000.0 + NSPulseStart.tv_usec / 1000);
    timesince = timesince / 1000;
    timeleft = NSPulseRequest - timesince;
    return timeleft;
}


void RaspberrypiST4::TimerHit() {
    float timeleft;

    if(InWEPulse) {
        timeleft = CalcWEPulseTimeLeft();
        if(timeleft < 1.0)  {
            if(timeleft > 0.25) { // a quarter of a second or more -> just set a tighter timer
                WEtimerID = SetTimer(250);
            } else {
                if(timeleft > 0.07) { // use an even tighter timer
                    WEtimerID = SetTimer(50);
                } else {
                    while(timeleft > 0) { // it's real close now, so spin on it
                        int slv;
                        slv = 100000 * timeleft;
                        usleep(slv);
                        timeleft = CalcWEPulseTimeLeft();
                    }
                    driver->stopPulse(WEDir);
                    InWEPulse = false;
                    if (!InNSPulse) { // If we have another pulse, keep going
                      SetTimer(250);
                    }
                }
            }
        } else if (!InNSPulse) {
            WEtimerID = SetTimer(250);
        }
    }

    if(InNSPulse) {
        timeleft = CalcNSPulseTimeLeft();
        if(timeleft < 1.0) {
            if(timeleft > 0.25) { //  a quarter of a second or more -> just set a tighter timer
                NStimerID = SetTimer(250);
            } else {
                if(timeleft >0.07) { //  use an even tighter timer
                    NStimerID = SetTimer(50);
                } else {
                    while(timeleft > 0) { //  it's real close now, so spin on it
                        int slv;
                        slv = 100000 * timeleft;
                        usleep(slv);
                        timeleft = CalcNSPulseTimeLeft();
                    }
                    driver->stopPulse(NSDir);
                    InNSPulse = false;
                }
            }
        } else {
            NStimerID = SetTimer(250);
        }
    }
}

IPState RaspberrypiST4::GuideNorth(uint32_t ms) {
    RemoveTimer(NStimerID);
    driver->startPulse(NORTH);
    NSDir = NORTH;
    DEBUG(INDI::Logger::DBG_DEBUG, "Starting NORTH guide");
    if (ms <= POLLMS) {
        usleep(ms * 1000);
        driver->stopPulse(NORTH);
        return IPS_OK;
    }
    NSPulseRequest = ms / 1000.0;
    gettimeofday(&NSPulseStart, NULL);
    InNSPulse = true;
    NStimerID = SetTimer(ms - 50);
    return IPS_BUSY;
}

IPState RaspberrypiST4::GuideSouth(uint32_t ms) {
    RemoveTimer(NStimerID);
    driver->startPulse(SOUTH);
    DEBUG(INDI::Logger::DBG_DEBUG, "Starting SOUTH guide");
    NSDir = SOUTH;
    if (ms <= POLLMS) {
        usleep(ms * 1000);
        driver->stopPulse(SOUTH);
        return IPS_OK;
    }
    NSPulseRequest = ms / 1000.0;
    gettimeofday(&NSPulseStart, NULL);
    InNSPulse = true;
    NStimerID = SetTimer(ms - 50);
    return IPS_BUSY;
}

IPState RaspberrypiST4::GuideEast(uint32_t ms) {
    RemoveTimer(WEtimerID);
    driver->startPulse(EAST);
    DEBUG(INDI::Logger::DBG_DEBUG, "Starting EAST guide");
    WEDir = EAST;
    if (ms <= POLLMS) {
        usleep(ms * 1000);
        driver->stopPulse(EAST);
        return IPS_OK;
    }
    WEPulseRequest = ms / 1000.0;
    gettimeofday(&WEPulseStart, NULL);
    InWEPulse = true;
    WEtimerID = SetTimer(ms - 50);
    return IPS_BUSY;
}

IPState RaspberrypiST4::GuideWest(uint32_t ms) {
    RemoveTimer(WEtimerID);
    driver->startPulse(WEST);
    DEBUG(INDI::Logger::DBG_DEBUG, "Starting WEST guide");
    WEDir = WEST;
    if (ms <= POLLMS) {
        usleep(ms * 1000);
        driver->stopPulse(WEST);
        return IPS_OK;
    }
    WEPulseRequest = ms / 1000.0;
    gettimeofday(&WEPulseStart, NULL);
    InWEPulse = true;
    WEtimerID = SetTimer(ms-50);
    return IPS_BUSY;
}
