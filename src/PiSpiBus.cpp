#include "PiSpiBus.h"

#include <errno.h>
#include <unistd.h>
#include <wiringPiSPI.h>

#include <system_error>

namespace {
constexpr unsigned char kOne{0xf8};
constexpr unsigned char kZero{0xc0};
constexpr unsigned char kNull{0x00};
constexpr size_t kBusResetLength{45};
constexpr size_t kBusFrequency{7000000};

struct PiSpiPixel {
  constexpr PiSpiPixel()
      : busValues({
            kNull, kNull, kNull, kNull, kNull, kNull, kNull, kNull,
            kNull, kNull, kNull, kNull, kNull, kNull, kNull, kNull,
            kNull, kNull, kNull, kNull, kNull, kNull, kNull, kNull,
        }) {}

  constexpr PiSpiPixel(unsigned char red,
                       unsigned char green,
                       unsigned char blue)
      : busValues({
            (green & 0x80) ? kOne : kZero, (green & 0x40) ? kOne : kZero,
            (green & 0x20) ? kOne : kZero, (green & 0x10) ? kOne : kZero,
            (green & 0x08) ? kOne : kZero, (green & 0x04) ? kOne : kZero,
            (green & 0x02) ? kOne : kZero, (green & 0x01) ? kOne : kZero,
            (red & 0x80) ? kOne : kZero,   (red & 0x40) ? kOne : kZero,
            (red & 0x20) ? kOne : kZero,   (red & 0x10) ? kOne : kZero,
            (red & 0x08) ? kOne : kZero,   (red & 0x04) ? kOne : kZero,
            (red & 0x02) ? kOne : kZero,   (red & 0x01) ? kOne : kZero,
            (blue & 0x80) ? kOne : kZero,  (blue & 0x40) ? kOne : kZero,
            (blue & 0x20) ? kOne : kZero,  (blue & 0x10) ? kOne : kZero,
            (blue & 0x08) ? kOne : kZero,  (blue & 0x04) ? kOne : kZero,
            (blue & 0x02) ? kOne : kZero,  (blue & 0x01) ? kOne : kZero,
        }) {}

  void setGreen(unsigned char value) {
    busValues[0] = (value & 0x80) ? kOne : kZero;
    busValues[1] = (value & 0x40) ? kOne : kZero;
    busValues[2] = (value & 0x20) ? kOne : kZero;
    busValues[3] = (value & 0x10) ? kOne : kZero;
    busValues[4] = (value & 0x08) ? kOne : kZero;
    busValues[5] = (value & 0x04) ? kOne : kZero;
    busValues[6] = (value & 0x02) ? kOne : kZero;
    busValues[7] = (value & 0x01) ? kOne : kZero;
  }

  void setRed(unsigned char value) {
    busValues[8] = (value & 0x80) ? kOne : kZero;
    busValues[9] = (value & 0x40) ? kOne : kZero;
    busValues[10] = (value & 0x20) ? kOne : kZero;
    busValues[11] = (value & 0x10) ? kOne : kZero;
    busValues[12] = (value & 0x08) ? kOne : kZero;
    busValues[13] = (value & 0x04) ? kOne : kZero;
    busValues[14] = (value & 0x02) ? kOne : kZero;
    busValues[15] = (value & 0x01) ? kOne : kZero;
  }

  void setBlue(unsigned char value) {
    busValues[16] = (value & 0x80) ? kOne : kZero;
    busValues[17] = (value & 0x40) ? kOne : kZero;
    busValues[18] = (value & 0x20) ? kOne : kZero;
    busValues[19] = (value & 0x10) ? kOne : kZero;
    busValues[20] = (value & 0x08) ? kOne : kZero;
    busValues[21] = (value & 0x04) ? kOne : kZero;
    busValues[22] = (value & 0x02) ? kOne : kZero;
    busValues[23] = (value & 0x01) ? kOne : kZero;
  }

 private:
  std::array<unsigned char, 24> busValues;
}
}  // namespace

namespace neoPIxel {
struct PiSpiBuffer {
  PiSpiBuffer(size_t size) : pixels_(size + kBusResetLength) {}
  PiSpiPixel& operator[](size_t index) { return pixels_[index]; }
  void display(int fd) const {
    unsigned char* pBuffer = pixels_.data();
    size_t bufferSize = pixels_.size() * sizeof(PiSpiPixel);
    while (true) {
      auto wrote = write(fd, pBuffer, bufferSize);
      if (wrote < 0) {
        throw std::system_error(errno, std::system_category());
      }
      if (wrote == bufferSize) {
        break;
      }
      pBuffer += wrote;
      bufferSize -= wrote;
    }
  }

 private:
  std::vector<PiSpiPixel> pixels_;
};
}  // namespace neoPIxel
using namespace neoPIxel;

PiSpiBus::PiSpiBus() {
  fd_ = wiringPiSPISetup(0, kBusFrequency);
  if (fd_ < 0) {
    throw std::system_error(errno, std::system_category());
  }
}

PiSpiBus::~PiSpiBus() {
  close(fd_);
}

void displayPixels(PiSpiBuffer const& buffer) {
  buffer.display(fd_);
}

std::unique_ptr<PiSpiBuffer> PiSpiBus::allocatePixels(size_t count) {
  return std::make_unique<PiSpiBuffer>(count);
}

void PiSpiBus::setRed(PiSpiBuffer& buffer, size_t index, unsigned char red) {
  buffer[index].setRed(red);
}

void PiSpiBus::setGreen(PiSpiBuffer& buffer,
                        size_t index,
                        unsigned char green) {
  buffer[index].setGreen(green);
}

void PiSpiBus::setBlue(PiSpiBuffer& buffer, size_t index, unsigned char blue) {
  buffer[index].setBlue(blue);
}
