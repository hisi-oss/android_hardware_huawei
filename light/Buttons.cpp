/*
 * Copyright (C) 2024 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "Button.h"

#include "LED.h"

namespace aidl {
namespace android {
namespace hardware {
namespace light {

class ButtonBrightness : public ButtonDevice {
  public:
    ButtonBrightness(std::string name) : mBasePath(mkButtonBasePath + name + "/") {
        if (!readFromFile(mBasePath + "max_brightness", &mMaxBrightness)) {
            mMaxBrightness = kDefaultMaxBrightness;
        }
    };

    void setButton(uint8_t value) {
        writeToFile(mBasePath + "brightness", value * mMaxBrightness / 0xFF);
    }

    bool exists() { return fileWriteable(mBasePath + "brightness"); }

  private:
    std::string mBasePath;
    uint32_t mMaxBrightness;

    inline static const std::string mkButtonBasePath = "/sys/class/leds/";
    inline static const uint32_t kDefaultMaxBrightness = 255;
};

class LEDButton : public ButtonDevice {
  public:
    LEDButton(std::string type) : mLED(type){};

    void setButton(uint8_t value) { mLED.setBrightness(value); }

    bool exists() { return mLED.exists(); }

  private:
    LED mLED;
};

static const std::string kButtonDevices[] = {
        "button-backlight",
        "button-backlight1",
};

static const std::string kKeyboardDevices[] = {
        "keyboard-backlight",
        "keyboard-backlight-ap",
};

ButtonDevice* getButtonDevice() {
    for (auto& device : kButtonDevices) {
        auto Button = new ButtonBrightness(device);
        if (Button->exists()) {
            return Button;
        }
        delete Button;
    }

    for (auto& device : kKeyboardDevices) {
        auto Button = new ButtonBrightness(device);
        if (Button->exists()) {
            return Button;
        }
        delete Button;
    }

    return nullptr;
}

}  // namespace light
}  // namespace hardware
}  // namespace android
}  // namespace aidl
