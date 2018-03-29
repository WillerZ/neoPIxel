#pragma once

#include <array>
#include <exception>
#include <memory>
#include <vector>

namespace neoPIxel {
struct PiSpiBus;
namespace detail {
constexpr size_t kBusResetLength{17};  // Length is in neopixels

constexpr unsigned char eightToTen(unsigned char intensity, size_t index) {
  constexpr std::array<unsigned char, 5> zerozero{0xe0, 0x38, 0x0e, 0x03, 0x80};
  constexpr std::array<unsigned char, 5> zeroone{0xe0, 0x3f, 0x0e, 0x03, 0xf0};
  constexpr std::array<unsigned char, 5> onezero{0xfc, 0x38, 0x0f, 0xc3, 0x80};
  constexpr std::array<unsigned char, 5> oneone{0xfc, 0x3f, 0x0f, 0xc3, 0xf0};
  unsigned char mask;
  unsigned char shift;
  switch (index % 10) {
    case 9:
      mask = 0xc0;
      shift = 6;
      break;
    case 8:
      mask = 0xc0;
      shift = 6;
      break;
    case 7:
      mask = 0x60;
      shift = 5;
      break;
    case 6:
      mask = 0x30;
      shift = 4;
      break;
    case 5:
      mask = 0x18;
      shift = 3;
      break;
    case 4:
      mask = 0x18;
      shift = 3;
      break;
    case 3:
      mask = 0x0c;
      shift = 2;
      break;
    case 2:
      mask = 0x06;
      shift = 1;
      break;
    case 1:
      mask = 0x03;
      shift = 0;
      break;
    case 0:
      mask = 0x03;
      shift = 0;
      break;
  }
  switch ((intensity & mask) >> shift) {
    case 0x11:
      return oneone[index % 5];
    case 0x10:
      return onezero[index % 5];
    case 0x01:
      return zeroone[index % 5];
    case 0x00:
      return zerozero[index % 5];
  }
}
}  // namespace detail
struct PiSpiSubpixel {
 public:
  constexpr PiSpiSubpixel() {}
  constexpr PiSpiSubpixel(unsigned char intensity)
      : b0_(detail::eightToTen(intensity, 0)),
        b1_(detail::eightToTen(intensity, 1)),
        b2_(detail::eightToTen(intensity, 2)),
        b3_(detail::eightToTen(intensity, 3)),
        b4_(detail::eightToTen(intensity, 4)),
        b5_(detail::eightToTen(intensity, 5)),
        b6_(detail::eightToTen(intensity, 6)),
        b7_(detail::eightToTen(intensity, 7)),
        b8_(detail::eightToTen(intensity, 8)),
        b9_(detail::eightToTen(intensity, 9)) {}

  void setIntensity(unsigned char intensity) {
    b0_ = detail::eightToTen(intensity, 0);
    b1_ = detail::eightToTen(intensity, 1);
    b2_ = detail::eightToTen(intensity, 2);
    b3_ = detail::eightToTen(intensity, 3);
    b4_ = detail::eightToTen(intensity, 4);
    b5_ = detail::eightToTen(intensity, 5);
    b6_ = detail::eightToTen(intensity, 6);
    b7_ = detail::eightToTen(intensity, 7);
    b8_ = detail::eightToTen(intensity, 8);
    b9_ = detail::eightToTen(intensity, 9);
  }

  void clear() {
    b0_ = 0;
    b1_ = 0;
    b2_ = 0;
    b3_ = 0;
    b4_ = 0;
    b5_ = 0;
    b6_ = 0;
    b7_ = 0;
    b8_ = 0;
    b9_ = 0;
  }

 private:
  unsigned char b0_;
  unsigned char b1_;
  unsigned char b2_;
  unsigned char b3_;
  unsigned char b4_;
  unsigned char b5_;
  unsigned char b6_;
  unsigned char b7_;
  unsigned char b8_;
  unsigned char b9_;
};

struct PiSpiPixel {
 public:
  constexpr PiSpiPixel() {}

  constexpr PiSpiPixel(unsigned char red,
                       unsigned char green,
                       unsigned char blue)
      : green_(green), red_(red), blue_(blue) {}

  PiSpiSubpixel& red() { return red_; }
  PiSpiSubpixel& green() { return green_; }
  PiSpiSubpixel& blue() { return blue_; }
  PiSpiSubpixel const& red() const { return red_; }
  PiSpiSubpixel const& green() const { return green_; }
  PiSpiSubpixel const& blue() const { return blue_; }

 private:
  PiSpiSubpixel green_;
  PiSpiSubpixel red_;
  PiSpiSubpixel blue_;
};

struct PiSpiBuffer {
  PiSpiBuffer(size_t size) : pixels_(size + detail::kBusResetLength) {}
  template <typename Iterator>
  PiSpiBuffer(Iterator, Iterator);

  PiSpiPixel& operator[](size_t index) { return pixels_[index]; }
  PiSpiPixel const& operator[](size_t index) const { return pixels_[index]; }

 private:
  size_t length() const { return pixels_.length() * sizeof(pixels_[0]); }
  char const* data() const {
    return static_cast<unsigned char const*>(
        static_cast<void const*>(pixels_.data()));
  }
  char* data() {
    return static_cast<unsigned char const*>(
        static_cast<void const*>(pixels_.data()));
  }

  friend struct PiSpiBus;
  std::vector<PiSpiPixel> pixels_;
};

struct PiSpiBus {
  PiSpiBus();
  ~PiSpiBus();

  void displayPixels(PiSpiBuffer const&);

  size_t countPixels() const;

 private:
  int fd_;
};

template <typename Iterator>
PiSpiBuffer::PiSpiBuffer(Iterator begin, Iterator end)
    : pixels_(size + detail::kBusResetLength) {
  for (size_t index = 0; begin != end; ++index, ++begin) {
    auto& pixel = pixels_[index];
    pixel.red().setIntensity(begin->red());
    pixel.green().setIntensity(begin->green());
    pixel.blue().setIntensity(begin->blue());
  }
  return result;
}

struct CountException : std::exception {
  using std::exception::exception();
}
}  // namespace neoPIxel
