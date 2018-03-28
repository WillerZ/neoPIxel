# neoPIxel
A neopixel management library for Raspberry Pi

I'm using a Raspberry Pi 3 B+, but it probably works on others.

## Setup

1. [Enable the SPI master driver](https://www.raspberrypi.org/documentation/hardware/raspberrypi/spi/README.md)
2. Connect the MOSI pin (pin 19 on the P1 header) to the neopixel chain control input.
3. Not used yet, but I plan to connect the MISO pin (pin 21 on the P1 header) to the neopixel chain control input.
4. Give your neopixels an adequate power supply. I have found that the +5V pin from the Raspberry Pi is enough for small numbers of neopixels.
5. Make sure that the Raspberry Pi and the neopixels share a common ground.

## Project Design

This (ab)uses one of the SPI buses on the Raspberry Pi to produce the signal
pattern that the neopixels require.

## Signaling in theory

The SPI bus provides high-frequency signalling with the straightforward encoding
that a 1-bit is signal-high and a 0-bit is signal low. There's a separate clock
line to synchronize the signal, but we aren't going to use that.

We're operating the SPI bus at a nominal 7.8MHz, so the nominal timing of each
bit is:

| What  | High  | Low   |
| :---  |  ---: |  ---: |
| 0-bit |       | 128ns |
| 1-bit | 128ns |       |

The neopixel signal bus is lower-frequency (400 kHz). Its baseline is a square
wave. 1-bits are signalled by making the high part of the wave longer and the
low part shorter; 0-bits are signalled by making the high part shorter and the
low part longer. This pattern is self-synchronizing so there is no separate
clock line. The pixels don't process the color commands until they see a reset
signal, which is a lengthy pause in the transmission of the wave. The precise
timings we're looking for are below, [copied from here](https://cdn-learn.adafruit.com/downloads/pdf/adafruit-neopixel-uberguide.pdf):

| What  | High  | Low   |
| :---  |  ---: |  ---: |
| 0-bit | 400ns | 850ns |
| 1-bit | 800ns | 450ns |
| reset |       | >50µs |

The tolerance on the neopixel signal is ±300ns per bit.

Given we are simulating the neopixel signal with this SPI signal, here is as
close as we could possible get:

| What  | SPI signal   | High  | Low   | High   | Low   | High | Low  |
| :---  | ---          | ---:  |  ---: | ---:   | ---:  | ---: | ---: |
| 0-bit | `1110000000` | 384ns | 896ns | -16ns  | +46ns | -4%  | +5%  |
| 1-bit | `1111110000` | 768ns | 512ns | -32ns  | +62ns | -4%  | +14% |
| reset | >391 x `0`   |       | >50µs |        | +48ns |      |      |

This is very close!

## Signaling in practice

Unfortunately those SPI signal patterns are a little inconvenient for us to
produce. The raspberry pi software model uses an 8-bit byte and a design that
required sending 10 bits per neopixel bit is less-convenient than one which uses
8 bits per neopixel bit. Let's see how close we can get with that constraint:

| What  | SPI signal | High  | Low   | High   | Low    | High | Low v|
| :---  | ---        | ---:  |  ---: | ---:   | ---:   | ---: | ---: |
| 0-bit | `11000000` | 256ns | 768ns | -144ns | -82ns  | -36% | -10% |
| 1-bit | `11111000` | 640ns | 384ns | -160ns | -66ns  | -20% | -15% |
| reset | >392 x `0` |       | >50µs |        | +176ns |      |      |

This isn't great: I don't like planning to use so much of the signal tolerance
just to make things more convenient for the software, as it leaves less margin
for hardware variations. On the other hand it does seem to work pretty well with
the neopixels and Raspberry Pi that I have access to.
