// SPDX-License-Identifier: MIT
/** @file Stream.h Minimal host byte-stream interface. */
#pragma once
#include "Arduino.h"

/// Minimal byte-stream interface for host tests, not a complete Arduino Stream.
class Stream {
 public:
  /// Permit destruction through the interface.
  virtual ~Stream() {}
  /// Return the number of bytes ready to read.
  virtual int available() = 0;
  /// Read one byte, or -1 when unavailable.
  virtual int read() = 0;
  /// Inspect one byte without consuming it; unsupported by default.
  virtual int peek() {
    return -1;
  }
  /// Wait for pending output according to the implementation.
  virtual void flush() = 0;
  /// Forward a single byte to the bulk-write implementation.
  virtual size_t write(uint8_t value) {
    return write(&value, 1);
  }
  /// Return the number of bytes accepted from the supplied buffer.
  virtual size_t write(const uint8_t* data, size_t size) = 0;
};
