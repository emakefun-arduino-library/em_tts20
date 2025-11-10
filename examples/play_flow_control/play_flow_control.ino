/**
 * @~Chinese
 * @file play_flow_control.ino
 * @brief 示例：TTS20模块播放流程控制演示。
 * @example play_flow_control.ino
 * @details 演示TTS20模块的完整播放控制流程，包括播放、暂停、恢复和停止功能。
 */
/**
 * @~English
 * @file play_flow_control.ino
 * @brief Example: TTS20 module playback flow control demonstration.
 * @example play_flow_control.ino
 * @details Demonstrate the complete playback control process of the TTS20 module, including playback, pause, resume, and stop functions.
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
  String play_text = F("一二三四五六七八九十");

  if (g_tts20.Play(play_text)) {
    Serial.print(F("Playing: "));
    Serial.println(play_text);

    delay(1000);

    if (g_tts20.Pause()) {
      Serial.println(F("Paused successful"));
      delay(1000);

      if (g_tts20.Resume()) {
        Serial.println(F("Resumed successful"));
        delay(1000);

        if (g_tts20.Stop()) {
          Serial.println(F("Stopped successful"));
        } else {
          Serial.println(F("Stop failed"));
        }

      } else {
        Serial.println(F("Resume failed"));
      }

    } else {
      Serial.println(F("Pause failed"));
    }

  } else {
    Serial.println(F("Play failed"));
  }

  while (g_tts20.IsBusy());

  delay(2000);
}