#include "PiSpiBus.h"

#include <errno.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <system_error>

namespace {
constexpr int kBusFrequency{7800000};
constexpr size_t kMaximumPixelsForProbing{100};
} // namespace

using namespace neoPIxel;

PiSpiBus::PiSpiBus() {
  fd_ = open("/dev/spidev0.0", O_RDWR);
  if (fd_ < 0) {
    throw std::system_error(errno, std::system_category(), "opening spidev0.0");
  }
  int mode = 0;
  auto rc = ioctl(fd_, SPI_IOC_WR_MODE, &mode);
  if (rc < 0) {
    throw std::system_error(errno, std::system_category(), "setting mode 0");
  }
  int bpw = 8;
  rc = ioctl(fd_, SPI_IOC_WR_BITS_PER_WORD, &bpw);
  if (rc < 0) {
    throw std::system_error(errno, std::system_category(),
                            "setting 8 bits per word");
  }
  int speed = kBusFrequency;
  rc = ioctl(fd_, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
  if (rc < 0) {
    throw std::system_error(errno, std::system_category(),
                            "setting bus frequency");
  }
}

size_t PiSpiBus::countPixels() const {
  PiSpiBuffer outBuffer{kMaximumPixelsForProbing};
  for (size_t i = 0UL; i < kMaximumPixelsForProbing; ++i) {
    outBuffer[i].red().setIntensity(0);
    outBuffer[i].green().setIntensity(0);
    outBuffer[i].blue().setIntensity(0);
  }
  auto length = outBuffer.length();
  auto inBuffer = std::unique_ptr<unsigned char[]>(new unsigned char[length]);
  spi_ioc_transfer spi_transfer{};
  spi_transfer.tx_buf = reinterpret_cast<unsigned long>(outBuffer.data());
  spi_transfer.rx_buf = reinterpret_cast<unsigned long>(inBuffer.get());
  spi_transfer.len = length;
  spi_transfer.delay_usecs = 0;
  spi_transfer.speed_hz = kBusFrequency;
  spi_transfer.bits_per_word = 8;
  auto rc = ioctl(fd_, SPI_IOC_MESSAGE(1), &spi_transfer);
  if (rc < 0) {
    throw std::system_error(errno, std::system_category(), "tx/rx SPI Message");
  }
  for (size_t i = 0UL; i < length; ++i) {
    if (inBuffer[i] != 0) {
      return i / sizeof(PiSpiPixel);
    }
  }
  throw CountException();
}

PiSpiBus::~PiSpiBus() { close(fd_); }

void PiSpiBus::displayPixels(PiSpiBuffer const &buffer) {
  auto pBuffer = buffer.data();
  auto bufferSize = buffer.length();
  while (true) {
    auto wrote = write(fd_, pBuffer, bufferSize);
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

char const *CountException::what() const noexcept {
  return "Could not count pixels";
}
