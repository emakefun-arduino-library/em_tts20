/**
 * @~Chinese
 * @file play_builtin_sounds.ino
 * @brief 示例：TTS20模块内置提示音效演示。
 * @example play_builtin_sounds.ino
 * @details 演示TTS20模块内置的15种提示音效播放功能。15 种提示音效，其中：铃声音效 5 首；信息提示音效 5 首；警示音效 5 首。
 *          - 铃声音效：ring_*，其中*的范围是1-5，共5种铃声音效。
 *          - 信息提示音效：message_*，其中*的范围是1-5，共5种信息提示音效。
 *          - 警示音效：alert_*，其中*的范围是1-5，共5种警示音效。
 *          - 注意：如果播放音效的同时还需要播放文本数据，那音效和文本数据不要放在同一播放中，即音效和文本数据分开执行。
 */
/**
 * @~English
 * @file play_builtin_sounds.ino
 * @brief Example: TTS20 module built-in prompt sound effect demonstration.
 * @example play_builtin_sounds.ino
 * @details Demonstrate the 15 built-in prompt sound effects playback functions of the TTS20 module. 15 types of alert sound effects, including 5
 * ringtone sound effects; 5 sound effects for information prompts; 5 warning sound effects.
 *          - Ringing sound effects: ring_ *, where * ranges from 1-5, with a total of 5 types of ringing sound effects.
 *          - Information prompt sound effects: message_ *, where * ranges from 1-5, with a total of 5 types of information prompt sound effects.
 *          - Warning sound effects: alert_ *, where * ranges from 1-5, with a total of 5 warning sound effects.
 *          - Attention: Attention: If you need to play text data while playing sound effects, do not play the sound effects and text data together,
 * that is, the sound effects and text data should be executed separately.
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
  if (g_tts20.Play(F("ring_1"))) {
    Serial.println(F("Play: \"ring_1\", successful"));
  } else {
    Serial.println(F("Play: \"ring_1\", failed"));
  }

  while (g_tts20.IsBusy());

  delay(2000);
}