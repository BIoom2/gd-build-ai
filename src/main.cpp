#include <Geode/Geode.hpp>
#include <Geode/binding/GJGarageLayer.hpp>
#include <Geode/modify/GJGarageLayer.hpp>

#include "RandomSetPopup.hpp"

using namespace geode::prelude;

class $modify(RSGarageLayer, GJGarageLayer) {
    bool init() {
        if (!GJGarageLayer::init()) {
            return false;
        }

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // Try a Geode-friendly sprite first; fall back if missing.
        auto btnSpr = CCSprite::createWithSpriteFrameName("GJ_extrasBtn_001.png");
        if (!btnSpr) {
            btnSpr = CCSprite::create("GJ_button_01.png");
        }
        btnSpr->setScale(0.85f);

        auto btn = CCMenuItemSpriteExtra::create(
            btnSpr, this,
            menu_selector(RSGarageLayer::onRandomSet)
        );
        btn->setID("random-set-button"_spr);

        auto menu = CCMenu::create();
        menu->addChild(btn);
        menu->setPosition({ winSize.width - 28.f, winSize.height - 30.f });
        menu->setID("random-set-menu"_spr);
        this->addChild(menu);

        return true;
    }

    void onRandomSet(CCObject*) {
        if (auto popup = RandomSetPopup::create(this)) {
            popup->show();
        }
    }
};
