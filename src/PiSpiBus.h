#pragma once

#include <memory>

namespace neoPIxel {
struct PiSpiBuffer;

struct PiSpiBus {
  PiSpiBus();
  ~PiSpiBus();

  template <typename Iterator>
  std::unique_ptr<PiSpiBuffer> preparePixels(Iterator, Iterator) const;

  void displayPixels(PiSpiBuffer const&);

 private:
  std::unique_ptr<PiSpiBuffer> allocatePixels(size_t);
  void setRed(PiSpiBuffer&, size_t, unsigned char);
  void setGreen(PiSpiBuffer&, size_t, unsigned char);
  void setBlue(PiSpiBuffer&, size_t, unsigned char);
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
}

}  // namespace neoPIxel
