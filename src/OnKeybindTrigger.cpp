// src/OnKeybindTrigger.cpp
#include "OnKeybindTrigger.hpp"
#include <Geode/ui/BasedButton.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/ui/ToggleButton.hpp>
#include "KeybindSelectPopup.hpp" // Мы создадим этот файл далее

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
    if (!EffectGameObject::init(3005)) { // 3005 - это временный ID объекта
        return false;
    }
    this->setObjectSprite(CCSprite::createWithSpriteFrameName("GJ_button_01.png")); // Ставим иконку
    return true;
}

// --- Сохранение и загрузка кастомных настроек ---

void OnKeybindTrigger::customObjectSetup(std::vector<std::string>& fields, std::vector<void*>&) {
    // Загружаем данные из строки сохранения уровня
    m_targetGroupID = std::stoi(fields[1]);
    m_keyCode = static_cast<cocos2d::enumKeyCodes>(std::stoi(fields[2]));
    m_useCondition = fields[3] == "1";
    m_conditionItemID = std::stoi(fields[4]);

    // Важно для EffectGameObject
    this->m_targetGroupID = m_targetGroupID; 
}

std::string OnKeybindTrigger::getSaveString(GJBaseGameLayer*) {
    // Сохраняем наши кастомные данные в строку уровня
    return std::to_string(m_targetGroupID) + "," +
           std::to_string(static_cast<int>(m_keyCode)) + "," +
           (m_useCondition ? "1" : "0") + "," +
           std::to_string(m_conditionItemID);
}

// --- Создание меню настроек ---

class OnKeybindPopup : public SetupTriggerPopup {
public: 
    OnKeybindTrigger* m_trigger;
    TextInput* m_keyLabel;

    bool init(OnKeybindTrigger* trigger, std::vector<GameObject*> const& targets) {
        if (!SetupTriggerPopup::init(trigger, targets)) return false;

        m_trigger = trigger;
        this->setTitle("OnKeybind Settings");

        // Поле для ввода Group ID
        this->m_fields->addChild(TextAlertPopup::create("Target Group ID", 0.5f, 120.f, "Target Group ID"));
        auto groupInput = TextInput::create(100.f, "ID");
        groupInput->setString(std::to_string(m_trigger->m_targetGroupID));
        groupInput->setFilter("0123456789");
        groupInput->setID("group-id-input");
        this->m_fields->addChild(groupInput);

        // Кнопка для выбора клавиши
        this->m_fields->addChild(TextAlertPopup::create("Key", 0.5f, 120.f, "Key to press"));
        auto keyButton = Button::create(keybindToString(m_trigger->m_keyCode), [this](auto) {
            KeybindSelectPopup::create([this](cocos2d::enumKeyCodes key) {
                // Когда клавиша выбрана в попапе, обновляем её
                m_trigger->m_keyCode = key;
                static_cast<Button*>(this->m_fields->getChildByID("key-select-button"))->setLabel(keybindToString(key));
            })->show();
        });
        keyButton->setID("key-select-button");
        this->m_fields->addChild(keyButton);

        // Чекбокс для условия
        auto conditionToggle = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(OnKeybindPopup::onToggleCondition), m_trigger->m_useCondition);
        conditionToggle->setID("use-condition-toggle");
        this->m_fields->addChild(CCMenu::createWithItem(conditionToggle));
        this->m_fields->addChild(TextAlertPopup::create("Use Condition", 0.5f, 120.f, "Activate only if condition is met"));

        // Поле для Item ID (условия)
        auto itemInput = TextInput::create(100.f, "Item ID");
        itemInput->setString(std::to_string(m_trigger->m_conditionItemID));
        itemInput->setFilter("0123456789");
        itemInput->setID("item-id-input");
        itemInput->setVisible(m_trigger->m_useCondition);
        this->m_fields->addChild(itemInput);

        return true;
    }

    void onToggleCondition(CCObject* sender) {
        bool enabled = !static_cast<CCMenuItemToggler*>(sender)->isToggled();
        m_trigger->m_useCondition = enabled;
        if (auto input = this->m_fields->getChildByID("item-id-input")) {
            input->setVisible(enabled);
        }
    }

    void onToggled(CCObject*) override { /* ... */ }

    void onClose(CCObject* sender) override {
        // Сохраняем значения из полей ввода перед закрытием
        auto groupInput = static_cast<TextInput*>(this->m_fields->getChildByID("group-id-input"));
        auto itemInput = static_cast<TextInput*>(this->m_fields->getChildByID("item-id-input"));
        
        m_trigger->m_targetGroupID = atoi(groupInput->getString().c_str());
        m_trigger->m_conditionItemID = atoi(itemInput->getString().c_str());

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

void OnKeybindTrigger::buildSetup(GameObject*, std::vector<GameObject*>* targets) {
    OnKeybindPopup::create(this, *targets)->show();
}
