#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/Slider.hpp>
#include <Geode/binding/GJBaseGameLayer.hpp>

using namespace geode::prelude;

class $modify(PercentEditorUI, EditorUI) {
    struct Fields {
        CCLabelBMFont* m_percentLabel = nullptr;
    };

    void updatePercentLabel() {
        if (!m_fields->m_percentLabel) return;
        if (!m_editorLayer) return;

        float sliderVal = m_positionSlider->getValue();
        int percent = static_cast<int>(std::round(sliderVal * 100.f));
        if (percent < 0) percent = 0;
        if (percent > 100) percent = 100;

        auto str = std::to_string(percent) + "%";
        m_fields->m_percentLabel->setString(str.c_str());
    }

    $override
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer))
            return false;

        auto label = CCLabelBMFont::create("0%", "bigFont.fnt");
        label->setScale(0.4f);
        label->setOpacity(180);
        label->setID("editor-slider-percent"_spr);

        if (m_positionSlider) {
            auto sliderPos = m_positionSlider->getPosition();
            auto sliderParent = m_positionSlider->getParent();
            label->setPosition(ccp(sliderPos.x, sliderPos.y - 20.f));
            if (sliderParent) {
                sliderParent->addChild(label, 100);
            } else {
                this->addChild(label, 100);
            }
        } else {
            auto winSize = CCDirector::sharedDirector()->getWinSize();
            label->setPosition(ccp(winSize.width / 2.f, 25.f));
            this->addChild(label, 100);
        }

        m_fields->m_percentLabel = label;
        this->updatePercentLabel();

        return true;
    }

    $override
    void sliderChanged(cocos2d::CCObject* sender) {
        EditorUI::sliderChanged(sender);
        this->updatePercentLabel();
    }

    $override
    void scrollWheel(float y, float x) {
        EditorUI::scrollWheel(y, x);
        this->schedule(schedule_selector(PercentEditorUI::onUpdatePercent), 0.f);
    }

    void onUpdatePercent(float dt) {
        this->unschedule(schedule_selector(PercentEditorUI::onUpdatePercent));
        this->updatePercentLabel();
    }

    $override
    void moveGamelayer(cocos2d::CCPoint offset) {
        EditorUI::moveGamelayer(offset);
        this->updatePercentLabel();
    }
};
