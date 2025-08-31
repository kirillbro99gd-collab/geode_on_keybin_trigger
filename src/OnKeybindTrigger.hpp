// src/OnKeybindTrigger.hpp
#pragma once
#include <Geode/Geode.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/EffectGameObject.hpp>

using namespace geode::prelude;

class OnKeybindTrigger : public EffectGameObject {
public:
    // --- Переменные для хранения настроек ---
    int m_targetGroupID = 0;
    cocos2d::enumKeyCodes m_keyCode = KEY_None; // Какую клавишу нажимать
    bool m_useCondition = false;                 // Использовать ли условие
    int m_conditionItemID = 0;                   // ID предмета для условия

    static OnKeybindTrigger* create();

    virtual bool init();
    virtual void customObjectSetup(std::vector<std::string>&, std::vector<void*>&) override;
    virtual std::string getSaveString(GJBaseGameLayer*) override;
    
    // Функция для вызова меню настроек
    virtual void buildSetup(GameObject*, std::vector<GameObject*>*) override;
};
