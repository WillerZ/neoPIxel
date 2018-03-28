#include "PiSpiBus.h"

#include <errno.h>
#include <unistd.h>
#include <wiringPiSPI.h>

#include <system_error>

namespace {
constexpr size_t kBusResetLength{45};
constexpr size_t kBusFrequency{7000000};
}  // namespace

using namespace neoPIxel;

void PiSpiPixel::setGreen(unsigned char value) {
  busValues_[0] = (value & 0x80) ? kOne : kZero;
  busValues_[1] = (value & 0x40) ? kOne : kZero;
  busValues_[2] = (value & 0x20) ? kOne : kZero;
  busValues_[3] = (value & 0x10) ? kOne : kZero;
  busValues_[4] = (value & 0x08) ? kOne : kZero;
  busValues_[5] = (value & 0x04) ? kOne : kZero;
  busValues_[6] = (value & 0x02) ? kOne : kZero;
  busValues_[7] = (value & 0x01) ? kOne : kZero;
}

void PiSpiPixel::setRed(unsigned char value) {
  busValues_[8] = (value & 0x80) ? kOne : kZero;
  busValues_[9] = (value & 0x40) ? kOne : kZero;
  busValues_[10] = (value & 0x20) ? kOne : kZero;
  busValues_[11] = (value & 0x10) ? kOne : kZero;
  busValues_[12] = (value & 0x08) ? kOne : kZero;
  busValues_[13] = (value & 0x04) ? kOne : kZero;
  busValues_[14] = (value & 0x02) ? kOne : kZero;
  busValues_[15] = (value & 0x01) ? kOne : kZero;
}

void PiSpiPixel::setBlue(unsigned char value) {
  busValues_[16] = (value & 0x80) ? kOne : kZero;
  busValues_[17] = (value & 0x40) ? kOne : kZero;
  busValues_[18] = (value & 0x20) ? kOne : kZero;
  busValues_[19] = (value & 0x10) ? kOne : kZero;
  busValues_[20] = (value & 0x08) ? kOne : kZero;
  busValues_[21] = (value & 0x04) ? kOne : kZero;
  busValues_[22] = (value & 0x02) ? kOne : kZero;
  busValues_[23] = (value & 0x01) ? kOne : kZero;
}

PiSpiBuffer::PiSpiBuffer(size_t size) : pixels_(size + kBusResetLength) {}
PiSpiPixel& PiSpiBuffer::operator[](size_t index) {
  return pixels_[index];
}
void PiSpiBuffer::display(int fd) const {
  unsigned char const* pBuffer = static_cast<unsigned char const*>(
      static_cast<void const*>(pixels_.data()));
  size_t bufferSize = pixels_.size() * sizeof(PiSpiPixel);
  while (true) {
    auto wrote = write(fd, pBuffer, bufferSize);
    if (wrote < 0) {
      throw std::system_error(errno, std::system_category());
    }
    if (static_cast<size_t>(wrote) == bufferSize) {
      break;
    }
    pBuffer += wrote;
    bufferSize -= wrote;
  }
}

PiSpiBus::PiSpiBus() {
  fd_ = wiringPiSPISetup(0, kBusFrequency);
  if (fd_ < 0) {
    throw std::system_error(errno, std::system_category());
  }
}

PiSpiBus::~PiSpiBus() {
  close(fd_);
}

void PiSpiBus::displayPixels(PiSpiBuffer const& buffer) {
  buffer.display(fd_);
}

std::unique_ptr<PiSpiBuffer> PiSpiBus::allocatePixels(size_t count) const {
  return std::make_unique<PiSpiBuffer>(count);
}

void PiSpiBus::setRed(PiSpiBuffer& buffer,
                      size_t index,
                      unsigned char red) const {
  buffer[index].setRed(red);
}

void PiSpiBus::setGreen(PiSpiBuffer& buffer,
                        size_t index,
                        unsigned char green) const {
  buffer[index].setGreen(green);
}

void PiSpiBus::setBlue(PiSpiBuffer& buffer,
                       size_t index,
                       unsigned char blue) const {
  buffer[index].setBlue(blue);
}
