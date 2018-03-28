#include "PiSpiBus.h"

#include <vector>
#include <cstring>

namespace {
struct CommandLinePixel {
  unsigned char r, g, b;
  constexpr unsigned char red() const { return r; }
  constexpr unsigned char green() const { return g; }
  constexpr unsigned char blue() const { return b; }
};
}  // namespace

using namespace neoPIxel;

int main(int argc, char** argv) {
  PiSpiBus defaultBus;
  std::vector<CommandLinePixel> cmdPixels(argc - 1);
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
    cmdPixels[i - 1] = {red, green, blue};
  }
  auto prepped = defaultBus.preparePixels(cmdPixels.begin(), cmdPixels.end());
  defaultBus.displayPixels(*prepped);
  return 0;
}
