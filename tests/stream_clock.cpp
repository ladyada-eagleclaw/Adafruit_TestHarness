// SPDX-License-Identifier: MIT
#include <assert.h>

#include "Adafruit_TestHarness.h"

int main() {
  testClock() = UINT32_MAX;
  testClockStep() = 1;
  assert(millis() == UINT32_MAX);
  assert(millis() == 0);
  testClockStep() = 0;
  assert(millis() == 1 && millis() == 1);
  delay(10);
  assert(millis() == 11);
  testClock() = UINT32_MAX - 2;
  delay(5);
  assert(millis() == 2);
  testClockStep() = 1;

  adafruit_test::FakeStream port;
  assert(!port.available() && port.read() == -1 && port.peek() == -1);
  port.push({0, 0xFF, 42});
  assert(port.peek() == 0 && port.peek() == 0 && port.available() == 3);
  port.flush();
  assert(port.read() == 0 && port.read() == 0xFF);
  port.readFailure = true;
  assert(port.available() == 1 && port.read() == -1 && port.peek() == -1);
  port.readFailure = false;
  port.endless = true;
  assert(port.read() == 42 && port.read() == 0 && port.available() == 1);
  port.endless = false;
  assert(port.read() == -1);
  assert(port.readCount == 7);

  int replies = 0;
  port.reply = [&]() {
    ++replies;
    port.push({0xA5});
  };
  uint8_t bytes[] = {1, 2, 3};
  assert(port.write(bytes, 3) == 3);
  assert(port.output == std::vector<uint8_t>({1, 2, 3}));
  assert(port.read() == 0xA5 && replies == 1);
  port.shortWrite = 1;
  assert(port.write(bytes, 3) == 2);
  assert(port.output == std::vector<uint8_t>({1, 2, 3, 1, 2}));
  assert(port.chunks == std::vector<size_t>({3, 2}) && replies == 1);
  assert(port.write(bytes, 0) == 0 && port.write(nullptr, 3) == 0);
  assert(port.chunks.size() == 2);
  port.shortWrite = 2;
  assert(port.write((uint8_t)9) == 0 && port.output.size() == 5);
  port.shortWrite = -1;
  Stream& stream = port;
  assert(stream.write((uint8_t)8) == 1 && port.output.back() == 8);
  assert(replies == 2);

  adafruit_test::FakeStream other;
  assert(other.output.empty() && other.input.empty());
}
