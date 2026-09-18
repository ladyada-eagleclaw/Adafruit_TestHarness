// SPDX-License-Identifier: MIT
/** @file Adafruit_TestHarness.h Shared fake transports for host tests. */
#pragma once
#include <limits.h>

#include <deque>
#include <functional>
#include <vector>

#include "Arduino.h"

/// Test-only helpers; never compiled into an Arduino sketch.
namespace adafruit_test {

/// Queued input, captured output, and explicit transport-failure injection.
class FakeStream : public Stream {
 public:
  std::deque<uint8_t> input;   ///< Bytes available to the library.
  std::vector<uint8_t> output; ///< Bytes accepted by write().
  std::vector<size_t> chunks;  ///< Accepted sizes of nonempty writes.
  std::function<void()> reply; ///< Called after each complete write.
  bool endless = false;        ///< Supply zero bytes after input drains.
  bool readFailure = false;    ///< read()/peek() fail despite available().
  int shortWrite = -1;         ///< Zero-based write index accepting size-1.
  size_t readCount = 0;        ///< Number of read() attempts.

  using Stream::write;

  /// Queued bytes, or one byte when continuous-input mode is enabled.
  int available() override {
    if (!input.empty())
      return (int)std::min(input.size(), (size_t)INT_MAX);
    return endless ? 1 : 0;
  }

  /// Return the next byte without consuming it.
  int peek() override {
    if (readFailure)
      return -1;
    if (!input.empty())
      return input.front();
    return endless ? 0 : -1;
  }

  /// Consume a queued byte; failed reads leave the queue unchanged.
  int read() override {
    ++readCount;
    int value = peek();
    if (value >= 0 && !input.empty())
      input.pop_front();
    return value;
  }

  /// Writes are synchronous; flushing does not discard unread input.
  void flush() override {}

  /// Capture accepted bytes; a short write captures only the accepted prefix.
  size_t write(const uint8_t* data, size_t size) override {
    if (!size || !data)
      return 0;
    size_t accepted = size;
    if (shortWrite >= 0 && (size_t)shortWrite == chunks.size())
      --accepted;
    chunks.push_back(accepted);
    output.insert(output.end(), data, data + accepted);
    if (reply && accepted == size)
      reply();
    return accepted;
  }

  /// Append bytes without changing data already queued.
  void push(const std::vector<uint8_t>& bytes) {
    input.insert(input.end(), bytes.begin(), bytes.end());
  }
};

} // namespace adafruit_test
