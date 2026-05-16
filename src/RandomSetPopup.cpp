#include "RandomSetPopup.hpp"

#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/GJGarageLayer.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/SimplePlayer.hpp>

#include <algorithm>
#include <random>

using namespace geode::prelude;

namespace {
    // Number of player colors available in GD 2.2 (IDs 0..106 inclusive).
    constexpr int COLOR_COUNT = 107;

    std::mt19937& rng() {
        static std::mt19937 engine{ std::random_device{}() };
        return engine;
    }

    int randomInRange(int minV, int maxV) {
        if (maxV < minV) {
            return minV;
        }
        std::uniform_int_distribution<int> dist(minV, maxV);
        return dist(rng());
    }

    int safeCount(GameManager* gm, IconType type) {
        int count = gm->countForType(type);
        return count > 0 ? count : 1;
    }
}

RandomSetPopup* RandomSetPopup::create(GJGarageLayer* garageLayer) {
    auto ret = new RandomSetPopup();
    if (ret->initAnchored(380.f, 260.f, garageLayer, "GJ_square01.png")) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool RandomSetPopup::setup(GJGarageLayer* garageLayer) {
    m_garageLayer = garageLayer;
    this->setTitle("Random Set");

    auto size = m_size;
    const float centerX = size.width / 2.f;

    static const char* names[8] = {
        "Cube", "Ship", "Ball", "UFO", "Wave", "Robot", "Spider", "Swing"
    };
    static const IconType types[8] = {
        IconType::Cube, IconType::Ship, IconType::Ball, IconType::Ufo,
        IconType::Wave, IconType::Robot, IconType::Spider, IconType::Swing
    };

    // 2x4 grid of icon previews
    constexpr float cellW = 78.f;
    constexpr float cellH = 70.f;
    const float gridStartX = centerX - cellW * 1.5f;
    const float gridStartY = size.height - 65.f;

    for (int i = 0; i < 8; ++i) {
        const int row = i / 4;
        const int col = i % 4;
        const float x = gridStartX + col * cellW;
        const float y = gridStartY - row * cellH;

        auto bg = CCScale9Sprite::create("square02b_001.png");
        bg->setContentSize({ 60.f, 55.f });
        bg->setPosition({ x, y });
        bg->setOpacity(60);
        m_mainLayer->addChild(bg);

        auto preview = SimplePlayer::create(1);
        preview->updatePlayerFrame(1, types[i]);
        preview->setPosition({ x, y + 5.f });
        preview->setScale(0.9f);
        m_mainLayer->addChild(preview);
        m_previews[i] = preview;

        auto label = CCLabelBMFont::create(names[i], "bigFont.fnt");
        label->setPosition({ x, y - 22.f });
        label->setScale(0.4f);
        m_mainLayer->addChild(label);
    }

    // Color swatches row
    static const char* swatchLabels[3] = { "C1", "C2", "Glow" };
    for (int i = 0; i < 3; ++i) {
        const float x = centerX - 80.f + i * 80.f;
        const float y = 60.f;

        auto holder = CCNode::create();
        holder->setContentSize({ 70.f, 30.f });
        holder->setAnchorPoint({ 0.5f, 0.5f });
        holder->setPosition({ x, y });
        m_mainLayer->addChild(holder);

        auto lbl = CCLabelBMFont::create(swatchLabels[i], "goldFont.fnt");
        lbl->setPosition({ 35.f, 25.f });
        lbl->setScale(0.45f);
        holder->addChild(lbl);

        auto swatch = CCSprite::create("square02b_001.png");
        if (swatch) {
            swatch->setScaleX(0.18f);
            swatch->setScaleY(0.18f);
            swatch->setPosition({ 35.f, 8.f });
            holder->addChild(swatch);
            m_colorSwatches[i] = swatch;
        }
    }

    m_glowLabel = CCLabelBMFont::create("", "goldFont.fnt");
    m_glowLabel->setPosition({ centerX, 30.f });
    m_glowLabel->setScale(0.45f);
    m_mainLayer->addChild(m_glowLabel);

    // Bottom Set / Next buttons
    auto bottomMenu = CCMenu::create();
    bottomMenu->setPosition({ centerX, 25.f });
    m_mainLayer->addChild(bottomMenu);

    auto setBtnSpr = ButtonSprite::create("Set", "bigFont.fnt", "GJ_button_01.png", 0.7f);
    setBtnSpr->setScale(0.8f);
    auto setBtn = CCMenuItemSpriteExtra::create(
        setBtnSpr, this, menu_selector(RandomSetPopup::onSet)
    );
    setBtn->setPosition({ -65.f, 0.f });
    bottomMenu->addChild(setBtn);

    auto nextBtnSpr = ButtonSprite::create("Next", "bigFont.fnt", "GJ_button_02.png", 0.7f);
    nextBtnSpr->setScale(0.8f);
    auto nextBtn = CCMenuItemSpriteExtra::create(
        nextBtnSpr, this, menu_selector(RandomSetPopup::onNext)
    );
    nextBtn->setPosition({ 65.f, 0.f });
    bottomMenu->addChild(nextBtn);

    generateSet();
    updatePreviews();

    return true;
}

void RandomSetPopup::generateSet() {
    auto gm = GameManager::sharedState();

    m_set.cube   = randomInRange(1, safeCount(gm, IconType::Cube));
    m_set.ship   = randomInRange(1, safeCount(gm, IconType::Ship));
    m_set.ball   = randomInRange(1, safeCount(gm, IconType::Ball));
    m_set.ufo    = randomInRange(1, safeCount(gm, IconType::Ufo));
    m_set.wave   = randomInRange(1, safeCount(gm, IconType::Wave));
    m_set.robot  = randomInRange(1, safeCount(gm, IconType::Robot));
    m_set.spider = randomInRange(1, safeCount(gm, IconType::Spider));
    m_set.swing  = randomInRange(1, safeCount(gm, IconType::Swing));

    m_set.color1 = randomInRange(0, COLOR_COUNT - 1);
    int c2 = randomInRange(0, COLOR_COUNT - 1);
    int safety = 0;
    while (c2 == m_set.color1 && safety++ < 8) {
        c2 = randomInRange(0, COLOR_COUNT - 1);
    }
    m_set.color2 = c2;
    m_set.glow = randomInRange(0, COLOR_COUNT - 1);
    m_set.useGlow = randomInRange(0, 1) == 1;
}

void RandomSetPopup::updatePreviews() {
    auto gm = GameManager::sharedState();
    const int ids[8] = {
        m_set.cube, m_set.ship, m_set.ball, m_set.ufo,
        m_set.wave, m_set.robot, m_set.spider, m_set.swing
    };
    const IconType types[8] = {
        IconType::Cube, IconType::Ship, IconType::Ball, IconType::Ufo,
        IconType::Wave, IconType::Robot, IconType::Spider, IconType::Swing
    };

    const auto c1 = gm->colorForIdx(m_set.color1);
    const auto c2 = gm->colorForIdx(m_set.color2);
    const auto cg = gm->colorForIdx(m_set.glow);

    for (int i = 0; i < 8; ++i) {
        if (!m_previews[i]) continue;
        m_previews[i]->updatePlayerFrame(ids[i], types[i]);
        m_previews[i]->setColor(c1);
        m_previews[i]->setSecondColor(c2);
        if (m_set.useGlow) {
            m_previews[i]->setGlowOutline(cg);
        } else {
            m_previews[i]->disableGlowOutline();
        }
    }

    if (m_colorSwatches[0]) m_colorSwatches[0]->setColor(c1);
    if (m_colorSwatches[1]) m_colorSwatches[1]->setColor(c2);
    if (m_colorSwatches[2]) {
        m_colorSwatches[2]->setColor(cg);
        m_colorSwatches[2]->setOpacity(m_set.useGlow ? 255 : 80);
    }
    if (m_glowLabel) {
        m_glowLabel->setString(m_set.useGlow ? "Glow: ON" : "Glow: OFF");
    }
}

void RandomSetPopup::applySet() {
    auto gm = GameManager::sharedState();

    gm->setPlayerFrame(m_set.cube);
    gm->setPlayerShip(m_set.ship);
    gm->setPlayerBall(m_set.ball);
    gm->setPlayerBird(m_set.ufo);
    gm->setPlayerDart(m_set.wave);
    gm->setPlayerRobot(m_set.robot);
    gm->setPlayerSpider(m_set.spider);
    gm->setPlayerSwing(m_set.swing);

    gm->setPlayerColor(m_set.color1);
    gm->setPlayerColor2(m_set.color2);
    gm->m_playerGlow = m_set.useGlow;
    if (m_set.useGlow) {
        gm->setPlayerGlowColor(m_set.glow);
    }
}

void RandomSetPopup::onSet(CCObject*) {
    applySet();

    if (m_garageLayer) {
        // Re-select the currently selected tab to redraw the icon preview,
        // and refresh the color swatches in the garage.
        m_garageLayer->selectTab(m_garageLayer->m_selectedIconType);
        m_garageLayer->updatePlayerColors();
    }

    this->onClose(nullptr);
}

void RandomSetPopup::onNext(CCObject*) {
    generateSet();
    updatePreviews();
}
