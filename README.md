# Adafruit TestHarness [![Host tests](https://github.com/adafruit/Adafruit_TestHarness/actions/workflows/tests.yml/badge.svg)](https://github.com/adafruit/Adafruit_TestHarness/actions/workflows/tests.yml)

Shared C++ host-test support for Adafruit Arduino libraries. Run the actual
library code on Linux with queued serial data, a controllable clock, and compiler
sanitizers. This package is for development and CI: it is not installed through
Arduino Library Manager and adds no dependency or memory use to user sketches.

The initial core is extracted from Adafruit_uBlox. It provides:

- A minimal `Arduino.h` and `Stream.h` compatibility layer.
- A 32-bit millisecond clock with controllable time and rollover.
- `adafruit_test::FakeStream` with queued input, captured output, reply hooks,
  failed reads, short writes, and continuous input.
- A Python runner using Linux g++, AddressSanitizer, UndefinedBehaviorSanitizer,
  warnings as errors, and compile/run timeouts.

It does not emulate a microcontroller or receiver. Keep device-specific packet
builders, register maps, I2C/SPI fakes, and expected results in the driver repo.
Host tests complement compilation for real boards and physical hardware tests.

## Running tests

Python 3 and Linux g++ are required; Windows developers can use WSL.
Each test source defines `main()` and uses ordinary C++ assertions.
Run this repository's tests with:

```sh
python3 run_tests.py
python3 -m unittest discover -s tests -p 'test_*.py'
```

To test another library, supply its test directory, production sources and
include directories. Repeat `--source` and `--include` as needed:

```sh
python3 ../Adafruit_TestHarness/run_tests.py \
  --test-dir extras/tests --source src/MyDriver.cpp --include src
```

The runner recursively discovers every `.cpp` in the test directory and builds
each as a separate program. Put support `.cpp` files outside that directory and
pass them with `--source`. It continues after a failed test and exits nonzero if
any test fails. Empty test discovery is also an error. Binaries are temporary.
Set `CXX` to select a compatible compiler.

Check out this repository alongside a driver in CI and pin it to a reviewed
commit. Do not add it to the driver's `library.properties` dependencies.

## Clock and stream behavior

`testClock()` is a mutable `uint32_t` millisecond value. `millis()` returns it
and then advances by `testClockStep()` (default 1). `delay(ms)` advances it without
sleeping. Set the step to zero to advance time explicitly. Automatic advancement
keeps polling loops deterministic; it does not measure real elapsed time or CPU
performance. The clock is shared within an executable and is not thread-safe.

`FakeStream::push()` queues bytes; `peek()` leaves them queued and `read()` removes
them. Empty reads return -1. With `endless` enabled, queued bytes come first, then
zero bytes continue indefinitely. `readFailure` forces reads and peeks to return
-1 without consuming data. `flush()` preserves unread input.

Writes accumulate in `output`, with accepted lengths recorded in `chunks`.
`shortWrite` selects a zero-based write call that accepts one fewer byte; only
that prefix is captured. Empty/null writes return zero without recording a call.
The `reply` callback runs after each complete nonempty write, not after a whole
protocol packet. Device tests decide when enough output exists to generate a reply.

The compatibility layer intentionally implements only the Arduino surface needed
by these tests. `Serial` discards diagnostic output. It does not validate Arduino
Print formatting, electrical behavior, interrupts, or bus timing.

MIT licensed. Please support Adafruit and open-source hardware by purchasing
products from [Adafruit](https://www.adafruit.com/).
