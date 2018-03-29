#include "PiSpiBus.h"

#include <cstring>
#include <iostream>

using namespace neoPIxel;

int main(int argc, char **argv) {
  PiSpiBus defaultBus;
  size_t count = 0;
  try {
    count = defaultBus.countPixels();
  } catch (CountException const &) {
    std::cerr << "Counting failed." << std::endl;
  }
  std::cout << "There are " << count << " lights." << std::endl;
  auto pixels = PiSpiBuffer(argc - 1);
  for (int i = 1; i < argc; ++i) {
    auto len = std::strlen(argv[i]);
    unsigned char red, green, blue;
    if (len >= 6) {
      sscanf(argv[i], "%2hhx%2hhx%2hhx", &red, &green, &blue);
    } else if (len >= 3) {
      sscanf(argv[i], "%1hhx%1hhx%1hhx", &red, &green, &blue);
      red = (red << 4) | red;
      green = (green << 4) | green;
      blue = (blue << 4) | blue;
    } else {
      continue;
    }
    auto &pixel = pixels[i - 1];
    pixel.red().setIntensity(red);
    pixel.green().setIntensity(green);
    pixel.blue().setIntensity(blue);
  }
  defaultBus.displayPixels(pixels);
  return 0;
}
