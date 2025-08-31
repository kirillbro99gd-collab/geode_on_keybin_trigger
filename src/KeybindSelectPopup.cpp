// src/KeybindSelectPopup.hpp
#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>

using namespace geode::prelude;

class KeybindSelectPopup : public Popup<std::function<void(cocos2d::enumKeyCodes)>> {
public:
    std::function<void(cocos2d::enumKeyCodes)> m_callback;

protected:
    bool setup(std::function<void(cocos2d::enumKeyCodes)> callback) {
        m_callback = callback;
        this->setTitle("Press Any Key");
        
        // Включаем обработку нажатий для этого окна
        this->setKeypadEnabled(true); 

        return true;
    }

    // Эта функция будет вызвана при нажатии любой клавиши
    void keyDown(cocos2d::enumKeyCodes key) override {
        if (key == KEY_Escape) {
            return this->onClose(nullptr);
        }
        m_callback(key);
        this->onClose(nullptr);
    }

public:
    static KeybindSelectPopup* create(std::function<void(cocos2d::enumKeyCodes)> callback) {
        auto ret = new KeybindSelectPopup();
        if (ret && ret->init(200.f, 150.f, callback)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};
