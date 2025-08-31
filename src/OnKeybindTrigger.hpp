#pragma once
#include <Geode/Geode.hpp>
#include <Geode/modify/EffectGameObject.hpp>

using namespace geode::prelude;

// Объявляем класс нашего кастомного триггера.
// Он наследуется от EffectGameObject, как и большинство триггеров в игре.
class OnKeybindTrigger : public EffectGameObject {
public:
    // --- Переменные для хранения настроек ---
    int m_targetGroupID;
    cocos2d::enumKeyCodes m_keyCode;
    bool m_useCondition;
    int m_conditionItemID;

    // Стандартные функции для создания объекта
    static OnKeybindTrigger* create();
    virtual bool init();

    // Функции для сохранения и загрузки кастомных данных в уровень
    virtual void customObjectSetup(std::vector<std::string>& fields, std::vector<void*>&) override;
    virtual std::string getSaveString(GJBaseGameLayer*) override;
    
    // Функция, которая будет вызывать меню настроек
    virtual void buildSetup(GameObject* obj, std::vector<GameObject*>* objs) override;
};
