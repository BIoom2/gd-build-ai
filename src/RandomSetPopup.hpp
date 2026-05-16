#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>

class GJGarageLayer;
class SimplePlayer;

class RandomSetPopup : public geode::Popup<GJGarageLayer*> {
protected:
    struct IconSet {
        int cube = 1;
        int ship = 1;
        int ball = 1;
        int ufo = 1;
        int wave = 1;
        int robot = 1;
        int spider = 1;
        int swing = 1;
        int color1 = 0;
        int color2 = 3;
        int glow = 12;
        bool useGlow = false;
    };

    GJGarageLayer* m_garageLayer = nullptr;
    IconSet m_set;
    SimplePlayer* m_previews[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
    cocos2d::CCSprite* m_colorSwatches[3] = { nullptr, nullptr, nullptr };
    cocos2d::CCLabelBMFont* m_glowLabel = nullptr;

    bool setup(GJGarageLayer* garageLayer) override;
    void generateSet();
    void updatePreviews();
    void applySet();
    void onSet(cocos2d::CCObject* sender);
    void onNext(cocos2d::CCObject* sender);

public:
    static RandomSetPopup* create(GJGarageLayer* garageLayer);
};
