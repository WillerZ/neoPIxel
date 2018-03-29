#pragma once

#include <array>
#include <exception>
#include <memory>
#include <vector>

namespace neoPIxel {
struct PiSpiBus;
namespace detail {
constexpr size_t kBusResetLength{6}; // Length is in neopixels
constexpr unsigned char kOne{0xf8};
constexpr unsigned char kZero{0xc0};
} // namespace detail

struct PiSpiSubpixel {
public:
  constexpr PiSpiSubpixel() : bits_() {}
  constexpr PiSpiSubpixel(unsigned char intensity)
      : bits_({intensity & 0x80 ? detail::kOne : detail::kZero,
               intensity & 0x40 ? detail::kOne : detail::kZero,
               intensity & 0x20 ? detail::kOne : detail::kZero,
               intensity & 0x10 ? detail::kOne : detail::kZero,
               intensity & 0x08 ? detail::kOne : detail::kZero,
               intensity & 0x04 ? detail::kOne : detail::kZero,
               intensity & 0x02 ? detail::kOne : detail::kZero,
               intensity & 0x01 ? detail::kOne : detail::kZero}) {}

  void setIntensity(unsigned char intensity) {
    bits_[0] = intensity & 0x80 ? detail::kOne : detail::kZero;
    bits_[1] = intensity & 0x40 ? detail::kOne : detail::kZero;
    bits_[2] = intensity & 0x20 ? detail::kOne : detail::kZero;
    bits_[3] = intensity & 0x10 ? detail::kOne : detail::kZero;
    bits_[4] = intensity & 0x08 ? detail::kOne : detail::kZero;
    bits_[5] = intensity & 0x04 ? detail::kOne : detail::kZero;
    bits_[6] = intensity & 0x02 ? detail::kOne : detail::kZero;
    bits_[7] = intensity & 0x01 ? detail::kOne : detail::kZero;
  }

  void clear() {
    bits_[0] = 0;
    bits_[1] = 0;
    bits_[2] = 0;
    bits_[3] = 0;
    bits_[4] = 0;
    bits_[5] = 0;
    bits_[6] = 0;
    bits_[7] = 0;
  }

private:
  std::array<unsigned char, 8> bits_;
};

struct PiSpiPixel {
public:
  constexpr PiSpiPixel() {}

  constexpr PiSpiPixel(unsigned char red, unsigned char green,
                       unsigned char blue)
      : green_(green), red_(red), blue_(blue) {}

  PiSpiSubpixel &red() { return red_; }
  PiSpiSubpixel &green() { return green_; }
  PiSpiSubpixel &blue() { return blue_; }
  PiSpiSubpixel const &red() const { return red_; }
  PiSpiSubpixel const &green() const { return green_; }
  PiSpiSubpixel const &blue() const { return blue_; }

private:
  PiSpiSubpixel green_;
  PiSpiSubpixel red_;
  PiSpiSubpixel blue_;
};

struct PiSpiBuffer {
  PiSpiBuffer(size_t size) : pixels_(size + detail::kBusResetLength) {}
  template <typename Iterator> PiSpiBuffer(Iterator, Iterator);

  PiSpiPixel &operator[](size_t index) { return pixels_[index]; }
  PiSpiPixel const &operator[](size_t index) const { return pixels_[index]; }

private:
  size_t length() const { return pixels_.size() * sizeof(pixels_[0]); }
  unsigned char const *data() const {
    return static_cast<unsigned char const *>(
        static_cast<void const *>(pixels_.data()));
  }
  unsigned char *data() {
    return static_cast<unsigned char *>(static_cast<void *>(pixels_.data()));
  }

  friend struct PiSpiBus;
  std::vector<PiSpiPixel> pixels_;
};

struct PiSpiBus {
  PiSpiBus();
  ~PiSpiBus();

  void displayPixels(PiSpiBuffer const &);

  size_t countPixels() const;

private:
  int fd_;
};

template <typename Iterator>
PiSpiBuffer::PiSpiBuffer(Iterator begin, Iterator end)
    : pixels_(std::distance(begin, end) + detail::kBusResetLength) {
  for (size_t index = 0; begin != end; ++index, ++begin) {
    auto &pixel = pixels_[index];
    pixel.red().setIntensity(begin->red());
    pixel.green().setIntensity(begin->green());
    pixel.blue().setIntensity(begin->blue());
  }
}

struct CountException : std::exception {
  CountException() = default;
  char const *what() const noexcept override;
};
} // namespace neoPIxel
