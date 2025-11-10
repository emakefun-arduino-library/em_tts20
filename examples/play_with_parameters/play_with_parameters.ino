/**
 * @~Chinese
 * @file play_with_parameters.ino
 * @brief 示例：TTS20模块参数播放演示。
 * @example play_with_parameters.ino
 * @details 演示TTS20模块如何使用文本标注参数控制播放音量。
 *          - 文本标注格式：[v*]，其中*的范围是0~9，默认为中间值，9 对应到最高音量。
 *          - 更多文本标注参数见文档。
 */
/**
 * @~English
 * @file play_with_parameters.ino
 * @brief Example: TTS20 module parameter playback demonstration.
 * @example play_with_parameters.ino
 * @details Demonstrate how the TTS20 module uses text annotation parameters to control playback volume,
 *          - Text annotation format: [v *], where * ranges from 0 to 9, defaults to the middle value, and 9 corresponds to the highest volume.
 *          - More text annotation parameters can be found in the document.
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
  if (g_tts20.Play(F("[v1]音量一"))) {
    Serial.println(F("Play: \"[v1]音量一\", successful"));
  } else {
    Serial.println(F("Play: \"[v1]音量一\", failed"));
  }

  while (g_tts20.IsBusy());

  delay(2000);

  if (g_tts20.Play(F("[v9]音量九"))) {
    Serial.println(F("Play: \"[v9]音量九\", successful"));
  } else {
    Serial.println(F("Play: \"[v9]音量九\", failed"));
  }

  while (g_tts20.IsBusy());

  delay(2000);
}