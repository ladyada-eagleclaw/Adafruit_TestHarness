// SPDX-License-Identifier: MIT
/** @file Arduino.h Minimal Arduino declarations for host tests. */
// Minimal host compatibility. This is not an Arduino core or hardware emulator.
#pragma once
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <algorithm>
using std::min;
class __FlashStringHelper; ///< Arduino flash-string tag; host strings stay in
                           ///< RAM.
#define F(text) \
  ((const __FlashStringHelper*)(text)) ///< Tag a flash-string literal.
#define HEX 16                         ///< Arduino hexadecimal print base.

/// Mutable 32-bit clock, in milliseconds. Assignment supports rollover tests.
inline uint32_t& testClock() {
  static uint32_t value = 0;
  return value;
}

/// Advance per millis() call; default 1 ms keeps polling tests deterministic.
inline uint32_t& testClockStep() {
  static uint32_t value = 1;
  return value;
}

/// Return the current fake time, then advance by testClockStep().
inline unsigned long millis() {
  uint32_t value = testClock();
  testClock() += testClockStep();
  return value;
}

/// Advance fake time without sleeping, including 32-bit wraparound.
inline void delay(unsigned long milliseconds) {
  testClock() += (uint32_t)milliseconds;
}

/// Sink for optional diagnostic output; does not test formatting or UART
/// timing.
class TestSerial {
 public:
  /// Discard a diagnostic value.
  template <typename T>
  void print(T) {}
  /// Discard a diagnostic value and its print-format argument.
  template <typename T>
  void print(T, int) {}
  /// Discard a diagnostic line.
  template <typename T>
  void println(T) {}
  /// Discard an empty diagnostic line.
  void println() {}
};
extern TestSerial Serial; ///< Shared diagnostic sink defined by Arduino.cpp.

// Arduino sketches commonly obtain Stream through Arduino.h alone.
#include "Stream.h"
