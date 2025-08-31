#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>

using namespace geode::prelude;

// Объявляем класс для попапа выбора клавиши.
// Он принимает "callback" функцию, которую вызовет после выбора клавиши.
class KeybindSelectPopup : public Popup<std::function<void(cocos2d::enumKeyCodes)>> {
public:
    std::function<void(cocos2d::enumKeyCodes)> m_callback;

protected:
    // Функция настройки попапа
    bool setup(std::function<void(cocos2d::enumKeyCodes)> callback) {
        m_callback = callback;
        this->setTitle("Press Any Key");
        
        // Включаем обработку нажатий клавиатуры для этого окна
        this->setKeypadEnabled(true); 

        return true;
    }

    // Эта функция вызывается, когда пользователь нажимает любую клавишу
    void keyDown(cocos2d::enumKeyCodes key) override {
        // Если нажат Escape, просто закрываем окно
        if (key == KEY_Escape) {
            return this->onClose(nullptr);
        }
        // В противном случае, вызываем callback с выбранной клавишей и закрываемся
        m_callback(key);
        this->onClose(nullptr);
    }

public:
    // Статическая функция для удобного создания попапа
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
