#include "PiSpiBus.h"

#include <errno.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <system_error>

namespace {
constexpr int kBusFrequency{7800000};
constexpr size_t kMaximumPixelsForProbing{1000};
}  // namespace

using namespace neoPIxel;

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
  auto inBuffer = outBuffer;
  spi_ioc_transfer spi_transfer{};
  spi_transfer.tx_buf = static_cast<unsigned long>(outBuffer.data());
  spi_transfer.rx_buf = static_cast<unsigned long>(inBuffer.data());
  spi_transfer.len = inBuffer.length();
  spi_transfer.delay_usecs = 0;
  spi_transfer.speed_hz = kBusFrequency;
  spi_transfer.bits_per_word = 8;
  auto rc = ioctl(fd_, SPI_IOC_MESSAGE(1), &spi_transfer);
  if (rc < 0) {
    throw std::system_error(errno, std::system_category(), "tx/rx SPI Message");
  }
  for (size_t i = 0UL; i < kMaximumPixelsForProbing; ++i) {
    if (outBuffer[i] == inBuffer[i]) {
      return i;
    }
  }
  throw CountException("Couldn't count pixels");
}

PiSpiBus::~PiSpiBus() {
  close(fd_);
}

void PiSpiBus::displayPixels(PiSpiBuffer const& buffer) {
  buffer.display(fd_);
}
