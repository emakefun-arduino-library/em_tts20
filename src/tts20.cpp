/**
 * @file tts20.cpp
 */

#include "tts20.h"

namespace em {

namespace {

constexpr uint8_t kI2cEndTransmissionSuccess = 0;
constexpr uint8_t kCommandClearRxBuffer = 1 << 0;
constexpr uint32_t kDefaultTimeoutMs = 1000;

constexpr uint8_t kResponseSuccess = 0x41;
constexpr uint8_t kResponseBusy = 0x4E;
constexpr uint8_t kResponseIdle = 0x4F;

// Wire library buffer capacity constant
#if defined(ESP32)
constexpr uint8_t kWireBufferCapacity = 128;
#else
constexpr uint8_t kWireBufferCapacity = 32;
#endif

enum MemoryAddress : uint8_t {
  kDeviceId = 0x00,
  kMajorVersion = 0x01,
  kMinorVersion = 0x02,
  kPatchVersion = 0x03,
  kName = 0x04,
  kCommand = 0x10,
  kTxBufferFreeSpace = 0x11,
  kTxBufferIsEmpty = 0x12,
  kTxBuffer = 0x13,
  kRxBufferCapacity = 0x53,
  kRxBufferReadPtr = 0x54,
  kRxBufferWritePtr = 0x55,
  kRxBufferData = 0x56,
  kRxBufferOverflow = 0x96,
};

}  // namespace

Tts20::Tts20(const uint8_t i2c_address, TwoWire &wire) : i2c_address_(i2c_address), wire_(wire) {
}

void Tts20::Init() {
  wire_.beginTransmission(i2c_address_);
  wire_.write(kRxBufferCapacity);
  EM_CHECK_EQ(wire_.endTransmission(), kI2cEndTransmissionSuccess);

  EM_CHECK_EQ(wire_.requestFrom(i2c_address_, static_cast<uint8_t>(1)), 1);

  while (wire_.available() == 0);

  rx_buffer_capacity_ = wire_.read();

  const uint8_t init_command[] = {0xFD, 0x00, 0x12, 0x01, 0x04, 0x5B, 0x76, 0x35, 0x5D, 0x5B, 0x73,
                                  0x35, 0x5D, 0x5B, 0x74, 0x35, 0x5D, 0x5B, 0x6D, 0x30, 0x5D};

  I2cWrite(init_command, sizeof(init_command));

  EM_CHECK(ReadUntil(kResponseSuccess, kDefaultTimeoutMs));

  while (IsBusy());
}

String Tts20::firmware_version() {
  wire_.beginTransmission(i2c_address_);
  wire_.write(kMajorVersion);
  EM_CHECK_EQ(wire_.endTransmission(), kI2cEndTransmissionSuccess);

  uint8_t version[3] = {0};
  EM_CHECK_EQ(wire_.requestFrom(i2c_address_, static_cast<uint8_t>(sizeof(version))), sizeof(version));

  uint8_t offset = 0;
  while (offset < sizeof(version)) {
    if (wire_.available() > 0) {
      version[offset++] = wire_.read();
    }
  }

  return String(version[0]) + "." + String(version[1]) + "." + String(version[2]);
}

uint8_t Tts20::device_id() {
  wire_.beginTransmission(i2c_address_);
  wire_.write(kDeviceId);
  EM_CHECK_EQ(wire_.endTransmission(), kI2cEndTransmissionSuccess);

  EM_CHECK_EQ(wire_.requestFrom(i2c_address_, static_cast<uint8_t>(1)), 1);

  while (wire_.available() == 0);

  return wire_.read();
}

String Tts20::name() {
  wire_.beginTransmission(i2c_address_);
  wire_.write(kName);
  EM_CHECK_EQ(wire_.endTransmission(), kI2cEndTransmissionSuccess);

  constexpr uint8_t kLength = 8;
  EM_CHECK_EQ(wire_.requestFrom(i2c_address_, kLength), kLength);

  String result;
  while (result.length() < kLength) {
    if (wire_.available() > 0) {
      result += static_cast<char>(wire_.read());
    }
  }

  return result;
}

bool Tts20::Play(const String &text) {
  EM_CHECK(text.length() > 0);

  const uint8_t header[] = {0xFD, ((text.length() + 2) >> 8) & 0xFF, ((text.length() + 2) & 0xFF), 0x01, 0x04};

  ClearRxBuffer();

  I2cWrite(header, sizeof(header));
  I2cWrite(reinterpret_cast<const uint8_t *>(text.c_str()), text.length());

  return ReadUntil(kResponseSuccess, kDefaultTimeoutMs);
}

bool Tts20::IsBusy() {
  const auto start_time = millis();
  while (millis() - start_time < 5000) {
    ClearRxBuffer();

    const uint8_t command[] = {0xFD, 0x00, 0x01, 0x21};
    I2cWrite(command, sizeof(command));

    uint8_t data = 0;
    if (I2cRead(&data, sizeof(data), 500) == sizeof(data)) {
      if (data == kResponseBusy) {
        return true;
      } else if (data == kResponseIdle) {
        return false;
      }
    }
  }

  return true;
}

bool Tts20::Stop() {
  ClearRxBuffer();

  const uint8_t command[] = {0xFD, 0x00, 0x01, 0x02};
  I2cWrite(command, sizeof(command));

  return ReadUntil(kResponseSuccess, kDefaultTimeoutMs);
}

bool Tts20::Pause() {
  ClearRxBuffer();

  const uint8_t command[] = {0xFD, 0x00, 0x01, 0x03};
  I2cWrite(command, sizeof(command));

  return ReadUntil(kResponseSuccess, kDefaultTimeoutMs);
}

bool Tts20::Resume() {
  ClearRxBuffer();

  const uint8_t command[] = {0xFD, 0x00, 0x01, 0x04};
  I2cWrite(command, sizeof(command));

  return ReadUntil(kResponseSuccess, kDefaultTimeoutMs);
}

void Tts20::ClearRxBuffer() {
  wire_.beginTransmission(i2c_address_);
  wire_.write(kCommand);
  wire_.write(kCommandClearRxBuffer);
  EM_CHECK_EQ(wire_.endTransmission(), kI2cEndTransmissionSuccess);
}

void Tts20::I2cWrite(const uint8_t *data, const uint16_t size) {
  EM_CHECK(data != nullptr && size > 0);

  uint16_t offset = 0;

  while (offset < size) {
    wire_.beginTransmission(i2c_address_);
    wire_.write(kTxBufferFreeSpace);
    EM_CHECK_EQ(wire_.endTransmission(), kI2cEndTransmissionSuccess);

    uint8_t segment_length = 0;
    EM_CHECK_EQ(wire_.requestFrom(i2c_address_, static_cast<uint8_t>(sizeof(segment_length))), sizeof(segment_length));

    while (wire_.available() == 0);

    segment_length = wire_.read();

    if (segment_length == 0) {
      continue;
    }

    segment_length = min(segment_length, static_cast<uint8_t>(kWireBufferCapacity - 1));

    if (size - offset < segment_length) {
      segment_length = size - offset;
    }

    wire_.beginTransmission(i2c_address_);
    wire_.write(kTxBuffer);
    for (uint8_t i = 0; i < segment_length; i++) {
      wire_.write(data[offset + i]);
    }

    EM_CHECK_EQ(wire_.endTransmission(), kI2cEndTransmissionSuccess);

    offset += segment_length;
  }

  while (true) {
    wire_.beginTransmission(i2c_address_);
    wire_.write(kTxBufferIsEmpty);
    EM_CHECK_EQ(wire_.endTransmission(), kI2cEndTransmissionSuccess);

    EM_CHECK_EQ(wire_.requestFrom(i2c_address_, static_cast<uint8_t>(1)), 1);

    while (wire_.available() == 0);

    if (wire_.read() != 0) {
      break;
    }
  }
}

uint8_t Tts20::I2cRead(uint8_t *buffer, const uint8_t expected_length, const uint32_t timeout_ms) {
  EM_CHECK(buffer != nullptr && expected_length > 0);

  uint8_t actual_length = 0;
  const auto start_time = millis();

  while (actual_length < expected_length && (millis() - start_time) < timeout_ms) {
    wire_.beginTransmission(i2c_address_);
    wire_.write(kRxBufferReadPtr);
    EM_CHECK_EQ(wire_.endTransmission(), kI2cEndTransmissionSuccess);

    uint8_t data[2] = {0};
    EM_CHECK_EQ(wire_.requestFrom(i2c_address_, static_cast<uint8_t>(sizeof(data))), sizeof(data));

    uint8_t offset = 0;
    while (offset < sizeof(data)) {
      if (wire_.available() > 0) {
        data[offset++] = wire_.read();
      }
    }

    if (data[0] == data[1]) {
      continue;
    }

    const uint8_t segment_length =
        min(expected_length - actual_length, min((data[1] + rx_buffer_capacity_ - data[0]) % rx_buffer_capacity_, rx_buffer_capacity_ - data[0]));

    wire_.beginTransmission(i2c_address_);
    wire_.write(static_cast<uint8_t>(kRxBufferData + data[0]));
    EM_CHECK_EQ(wire_.endTransmission(), kI2cEndTransmissionSuccess);

    EM_CHECK_EQ(wire_.requestFrom(i2c_address_, segment_length), segment_length);

    const uint16_t target_bytes = actual_length + segment_length;
    while (actual_length < target_bytes) {
      if (wire_.available() > 0) {
        buffer[actual_length++] = wire_.read();
      }
    }

    wire_.beginTransmission(i2c_address_);
    wire_.write(kRxBufferReadPtr);
    wire_.write(static_cast<uint8_t>((data[0] + segment_length) % rx_buffer_capacity_));
    EM_CHECK_EQ(wire_.endTransmission(), kI2cEndTransmissionSuccess);
  }
  return actual_length;
}

bool Tts20::ReadUntil(const uint8_t target_byte, const uint32_t timeout_ms) {
  uint8_t data = 0;
  if (I2cRead(&data, sizeof(data), timeout_ms) == sizeof(data) && data == target_byte) {
    return true;
  }
  return false;
}

}  // namespace em