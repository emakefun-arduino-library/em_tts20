/**
 * @~Chinese
 * @file play.ino
 * @brief 示例：TTS20模块播放演示。
 * @example play.ino
 * @details TTS20模块播放演示。
 */
/**
 * @~English
 * @file play.ino
 * @brief Example: TTS20 module playback demonstration.
 * @example play.ino
 * @details module playback demonstration.
 */

#include "tts20.h"

namespace {
em::Tts20 g_tts20(em::Tts20::kDefaultI2cAddress, Wire);

}  // namespace

void setup() {
  Serial.begin(115200);

  Wire.begin();

  g_tts20.Init();

  Serial.print(F("Device ID: 0x"));
  Serial.println(g_tts20.device_id(), HEX);
  Serial.print(F("Name: "));
  Serial.println(g_tts20.name());
  Serial.print(F("Firmware Version: "));
  Serial.println(g_tts20.firmware_version());
}

void loop() {
  if (g_tts20.Play(F("一二三四五"))) {
    Serial.println(F("Play: \"一二三四五\", successful"));
  } else {
    Serial.println(F("Play: \"一二三四五\", failed"));
  }

  while (g_tts20.IsBusy());

  delay(2000);
}