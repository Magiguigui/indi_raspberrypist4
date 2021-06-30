#ifndef RASPBERRYPIST4DRIVERAXIS_H
#define RASPBERRYPIST4DRIVERAXIS_H

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <string>

#include <errno.h>

#define HIGH 1
#define LOW 0

class Axis {
    private:
        int plusPin, plusFd;
        int minusPin, minusFd;

    public:
        Axis(int plusPin, int minusPin) {
            this->plusPin = plusPin;
            this->minusPin = minusPin;
        }

        void connect() {
            this->plusFd = this->setupPin(this->plusPin);
            this->minusFd = this->setupPin(this->minusPin);
        }

        void disconnect() {
            close(this->plusFd);
            close(this->minusFd);
        }

        void plus() {
            writePin(this->minusFd, LOW);
            writePin(this->plusFd, HIGH);
        }

        void minus() {
            writePin(this->plusFd, LOW);
            writePin(this->minusFd, HIGH);
        }

        void reset() {
            writePin(this->minusFd, LOW);
            writePin(this->plusFd, LOW);
        }

    private:
        int setupPin(int pin) {
            std::string pinString = std::to_string(pin);

            int exportFD = open("/sys/class/gpio/export", O_WRONLY);
            if (exportFD == -1) {
                perror("MyUnable to open /sys/class/gpio/export");
            } else {
                if (write(exportFD, pinString.c_str(), pinString.size()) != pinString.size()) {
                    perror("MyError writing to /sys/class/gpio/export");
                }
                close(exportFD);
            }

            sleep(1);

            int directionFD = open(("/sys/class/gpio/gpio" + pinString + "/direction").c_str(), O_WRONLY);
            if (directionFD == -1) {
                perror(("MyUnable to open /sys/class/gpio/gpio" + pinString + "/direction").c_str());
            } else {
                if (write(directionFD, "out", 3) != 3) {
                    perror(("MyError writing out to /sys/class/gpio/gpio" + pinString + "/direction").c_str());
                }
                close(directionFD);
            }

            sleep(1);

            int pinFD = open(("/sys/class/gpio/gpio" + pinString + "/value").c_str(), O_WRONLY);
            if (pinFD == -1) {
                perror(("/sys/class/gpio/gpio" + pinString + "/value").c_str());
                exit(1);
            }
            return pinFD;
        }

        void writePin(int fd, bool value) {
            if (write(fd, value ? "1" : "0", 1) != 1) {
                perror("Error writing");
                exit(1);
            }
        }
};

#endif //RASPBERRYPIST4DRIVERAXIS_H
