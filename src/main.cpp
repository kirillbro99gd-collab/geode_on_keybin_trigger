#include <Geode/modify/PlayLayer.hpp>
#include <Geode/loader/ObjectToolbox.hpp>
#include "OnKeybindTrigger.hpp" // Убедитесь, что вы создали этот файл!

using namespace geode::prelude;

/*
* =========================================================================================
* Эта часть кода отвечает за ЛОГИКУ РАБОТЫ триггера ВО ВРЕМЯ ИГРЫ
* =========================================================================================
*/
class $modify(PlayLayer) {
    // Эта функция "перехватывает" каждое нажатие клавиши, когда вы находитесь на уровне
    void keyDown(cocos2d::enumKeyCodes key) {
        // Сначала мы вызываем оригинальную функцию игры, чтобы не сломать её
        PlayLayer::keyDown(key);

        // Проверяем, существуют ли вообще объекты на уровне
        if (!this->m_objects) {
            return;
        }

        // Проходим в цикле по всем объектам на уровне
        for (auto* obj : cocos2d::CCArrayExt<GameObject*>(this->m_objects)) {
            
            // Проверяем, является ли текущий объект нашим триггером "OnKeybindTrigger"
            if (auto trigger = typeinfo_cast<OnKeybindTrigger*>(obj)) {
                
                // Если нажатая клавиша совпадает с той, что указана в настройках триггера
                if (trigger->m_keyCode == key) {
                    
                    bool conditionMet = true; // По умолчанию считаем, что условие выполнено

                    // Если в настройках триггера включено "Use Condition"
                    if (trigger->m_useCondition) {
                        // Проверяем, активен ли предмет (Item ID) с нужным ID.
                        // Это стандартная логика для Toggle-триггеров в GD.
                        if (this->m_player1 && this->m_player1->m_playerItems) {
                             conditionMet = this->m_player1->m_playerItems->contains(std::to_string(trigger->m_conditionItemID));
                        } else {
                            conditionMet = false;
                        }
                    }
                    
                    // Если все проверки пройдены, активируем целевую группу
                    if (conditionMet) {
                        this->activateGroup(trigger->m_targetGroupID, true);
                    }
                }
            }
        }
    }
};

/*
* =========================================================================================
* Эта часть кода отвечает за РЕГИСТРАЦИЮ триггера В РЕДАКТОРЕ
* =========================================================================================
*/
$on_mod(Loaded) {
    // Сообщаем игре, чтобы она добавила наш новый объект в меню редактора
    // 3005 - это ID нашего объекта. Вы можете выбрать другой, но убедитесь, что он уникален.
    // "GJ_button_01.png" - это иконка, которая будет отображаться в меню.
    ObjectToolbox::get()->addObject(3005, "GJ_button_01.png");
}
