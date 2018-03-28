#pragma once

#include <array>
#include <memory>
#include <vector>

namespace neoPIxel {
struct PiSpiPixel {
 private:
  static constexpr unsigned char kOne{0xf8};
  static constexpr unsigned char kZero{0xc0};
  static constexpr unsigned char kNull{0x00};

 public:
  constexpr PiSpiPixel()
      : busValues_({
            kNull, kNull, kNull, kNull, kNull, kNull, kNull, kNull,
            kNull, kNull, kNull, kNull, kNull, kNull, kNull, kNull,
            kNull, kNull, kNull, kNull, kNull, kNull, kNull, kNull,
        }) {}

  constexpr PiSpiPixel(unsigned char red,
                       unsigned char green,
                       unsigned char blue)
      : busValues_({
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

  void setGreen(unsigned char value);
  void setRed(unsigned char value);
  void setBlue(unsigned char value);

 private:
  std::array<unsigned char, 24> busValues_;
};

struct PiSpiBuffer {
  PiSpiBuffer(size_t size);
  PiSpiPixel& operator[](size_t index);
  void display(int fd) const;

 private:
  std::vector<PiSpiPixel> pixels_;
};

struct PiSpiBus {
  PiSpiBus();
  ~PiSpiBus();

  template <typename Iterator>
  std::unique_ptr<PiSpiBuffer> preparePixels(Iterator, Iterator) const;

  void displayPixels(PiSpiBuffer const&);

 private:
  std::unique_ptr<PiSpiBuffer> allocatePixels(size_t) const;
  void setRed(PiSpiBuffer&, size_t, unsigned char) const;
  void setGreen(PiSpiBuffer&, size_t, unsigned char) const;
  void setBlue(PiSpiBuffer&, size_t, unsigned char) const;
  int fd_;
};

template <typename Iterator>
std::unique_ptr<PiSpiBuffer> PiSpiBus::preparePixels(Iterator begin,
                                                     Iterator end) const {
  auto pixelCount = std::distance(begin, end);
  auto result = allocatePixels(pixelCount);
  for (size_t index = 0; begin != end; ++index, ++begin) {
    setRed(*result, index, begin->red());
    setGreen(*result, index, begin->green());
    setBlue(*result, index, begin->blue());
  }
  return result;
}

}  // namespace neoPIxel
