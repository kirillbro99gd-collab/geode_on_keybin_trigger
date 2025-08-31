// src/main.cpp
#include <Geode/modify/PlayLayer.hpp>
#include "OnKeybindTrigger.hpp" // Подключаем наш триггер

class $modify(PlayLayer) {
    // Эта функция вызывается каждый раз при нажатии клавиши в игре
    void keyDown(cocos2d::enumKeyCodes key) {
        // Сначала вызываем оригинальную функцию, чтобы не сломать игру
        PlayLayer::keyDown(key); 

        // Проходим по всем объектам на уровне
        if (this->m_objects) {
            for (auto* obj : cocos2d::CCArrayExt<GameObject*>(this->m_objects)) {
                // Проверяем, является ли объект нашим триггером
                if (auto trigger = typeinfo_cast<OnKeybindTrigger*>(obj)) {
                    // Если нажатая клавиша совпадает с той, что в настройках триггера
                    if (trigger->m_keyCode == key) {
                        bool conditionMet = true; // Считаем, что условие выполнено по умолчанию

                        // Если использование условия включено
                        if (trigger->m_useCondition) {
                            // Проверяем, активен ли предмет с нужным ID
                            // (Это стандартная логика для Toggle триггеров)
                            conditionMet = this->m_player1->m_playerItems->contains(std::to_string(trigger->m_conditionItemID));
                        }
                        
                        // Если все проверки пройдены, активируем группу
                        if (conditionMet) {
                            this->activateGroup(trigger->m_targetGroupID, true);
                        }
                    }
                }
            }
        }
    }
};
