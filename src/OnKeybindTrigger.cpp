#include "OnKeybindTrigger.hpp"
#include "KeybindSelectPopup.hpp"
#include <Geode/ui/TextInput.hpp>
#include <Geode/ui/BasedButton.hpp>

// --- Реализация самого триггера ---

OnKeybindTrigger* OnKeybindTrigger::create() {
    auto ret = new OnKeybindTrigger();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool OnKeybindTrigger::init() {
    // 3005 - это уникальный ID нашего объекта, который мы указали в main.cpp
    if (!EffectGameObject::init(3005)) return false;
    
    // Устанавливаем иконку по умолчанию
    this->setObjectSprite(CCSprite::createWithSpriteFrameName("GJ_button_01.png"));
    
    // Задаём значения по умолчанию для нового триггера
    m_targetGroupID = 0;
    m_keyCode = KEY_None;
    m_useCondition = false;
    m_conditionItemID = 0;
    
    return true;
}

// Загружаем кастомные данные из строки сохранения уровня
void OnKeybindTrigger::customObjectSetup(std::vector<std::string>& fields, std::vector<void*>&) {
    // Geode автоматически разбивает строку сохранения по запятым
    m_targetGroupID = std::stoi(fields[1]);
    m_keyCode = static_cast<cocos2d::enumKeyCodes>(std::stoi(fields[2]));
    m_useCondition = fields[3] == "1";
    m_conditionItemID = std::stoi(fields[4]);

    // Это стандартное поле тоже нужно обновить
    this->m_targetGroupID = m_targetGroupID;
}

// Превращаем наши настройки в строку для сохранения в данных уровня
std::string OnKeybindTrigger::getSaveString(GJBaseGameLayer*) {
    return std::to_string(m_targetGroupID) + "," +
           std::to_string(static_cast<int>(m_keyCode)) + "," +
           (m_useCondition ? "1" : "0") + "," +
           std::to_string(m_conditionItemID);
}


// --- Класс для всплывающего окна настроек ---

class OnKeybindPopup : public SetupTriggerPopup {
public: 
    OnKeybindTrigger* m_trigger;

    bool init(OnKeybindTrigger* trigger, std::vector<GameObject*> const& targets) {
        if (!SetupTriggerPopup::init(trigger, targets)) return false;

        m_trigger = trigger;
        this->setTitle("OnKeybind Settings");

        // Поле для ввода Group ID
        auto groupInput = TextInput::create(100.f, "ID");
        groupInput->setString(std::to_string(m_trigger->m_targetGroupID));
        groupInput->setFilter("0123456789");
        groupInput->setID("group-id-input");
        this->addInput("Target GID", groupInput);

        // Кнопка для выбора клавиши
        auto keyButton = Button::create(keybindToString(m_trigger->m_keyCode), [this](auto) {
            KeybindSelectPopup::create([this](cocos2d::enumKeyCodes key) {
                m_trigger->m_keyCode = key;
                if(auto btn = this->m_mainLayer->getChildByIDRecursive("key-select-button")){
                   static_cast<Button*>(btn)->setLabel(keybindToString(key));
                }
            })->show();
        });
        keyButton->setID("key-select-button");
        this->addInput("Key", keyButton);

        // Чекбокс для условия
        auto conditionToggle = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(OnKeybindPopup::onToggleCondition), m_trigger->m_useCondition);
        conditionToggle->setID("use-condition-toggle");
        this->addInput("Use Cond.", CCMenu::createWithItem(conditionToggle), "Activate only if condition is met");

        // Поле для Item ID (условия)
        auto itemInput = TextInput::create(100.f, "Item ID");
        itemInput->setString(std::to_string(m_trigger->m_conditionItemID));
        itemInput->setFilter("0123456789");
        itemInput->setID("item-id-input");
        itemInput->setVisible(m_trigger->m_useCondition);
        this->addInput("Item ID", itemInput);
        
        this->updateLayout();
        return true;
    }

    void onToggleCondition(CCObject* sender) {
        bool enabled = !static_cast<CCMenuItemToggler*>(sender)->isToggled();
        if (auto input = this->m_mainLayer->getChildByIDRecursive("item-id-input")) {
            // Прячем или показываем поле ввода Item ID
            static_cast<CCNode*>(input->getParent())->setVisible(enabled);
            this->updateLayout();
        }
    }

    void onClose(CCObject* sender) override {
        // Сохраняем значения из полей ввода в переменные триггера перед закрытием
        auto groupInput = static_cast<TextInput*>(this->m_mainLayer->getChildByIDRecursive("group-id-input"));
        auto itemInput = static_cast<TextInput*>(this->m_mainLayer->getChildByIDRecursive("item-id-input"));
        auto toggle = static_cast<CCMenuItemToggler*>(this->m_mainLayer->getChildByIDRecursive("use-condition-toggle")->getChildren()->objectAtIndex(0));

        m_trigger->m_targetGroupID = valueOrDefault<int>(groupInput->getString(), 0);
        m_trigger->m_conditionItemID = valueOrDefault<int>(itemInput->getString(), 0);
        m_trigger->m_useCondition = toggle->isToggled();
        
        // Это поле тоже обновляем
        m_trigger->m_targetGroupID = m_trigger->m_targetGroupID;

        SetupTriggerPopup::onClose(sender);
    }

    static OnKeybindPopup* create(OnKeybindTrigger* trigger, std::vector<GameObject*> const& targets) {
        auto ret = new OnKeybindPopup;
        if (ret && ret->init(trigger, targets)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

// Эта функция вызывается, когда пользователь нажимает "Edit Object"
void OnKeybindTrigger::buildSetup(GameObject*, std::vector<GameObject*>* targets) {
    OnKeybindPopup::create(this, *targets)->show();
}
