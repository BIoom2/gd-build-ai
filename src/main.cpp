#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/Slider.hpp>
#include <Geode/binding/GJBaseGameLayer.hpp>

using namespace geode::prelude;

class $modify(PercentEditorUI, EditorUI) {
    struct Fields {
        CCLabelBMFont* m_percentLabel = nullptr;
        float m_hideTimer = 0.f;
        bool m_isVisible = false;
    };

    void showPercentLabel() {
        if (!m_fields->m_percentLabel) return;
        if (!m_positionSlider) return;
        if (!m_editorLayer) return;

        float sliderVal = m_positionSlider->getValue();
        int percent = static_cast<int>(std::round(sliderVal * 100.f));
        if (percent < 0) percent = 0;
        if (percent > 100) percent = 100;

        auto str = std::to_string(percent) + "%";
        m_fields->m_percentLabel->setString(str.c_str());

        m_fields->m_percentLabel->setVisible(true);
        m_fields->m_percentLabel->setOpacity(180);
        m_fields->m_isVisible = true;
        m_fields->m_hideTimer = 1.5f;

        this->unschedule(schedule_selector(PercentEditorUI::onHideTick));
        this->schedule(schedule_selector(PercentEditorUI::onHideTick), 0.05f);
    }

    void onHideTick(float dt) {
        m_fields->m_hideTimer -= dt;
        if (m_fields->m_hideTimer <= 0.f) {
            this->unschedule(schedule_selector(PercentEditorUI::onHideTick));
            if (m_fields->m_percentLabel) {
                m_fields->m_percentLabel->setVisible(false);
            }
            m_fields->m_isVisible = false;
        }
    }

    $override
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer))
            return false;

        auto label = CCLabelBMFont::create("0%", "bigFont.fnt");
        label->setScale(0.4f);
        label->setOpacity(180);
        label->setID("editor-slider-percent"_spr);
        label->setVisible(false);

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

        return true;
    }

    $override
    void sliderChanged(cocos2d::CCObject* sender) {
        EditorUI::sliderChanged(sender);
        this->showPercentLabel();
    }

    $override
    void scrollWheel(float y, float x) {
        EditorUI::scrollWheel(y, x);
        this->schedule(schedule_selector(PercentEditorUI::onUpdatePercent), 0.05f);
    }

    void onUpdatePercent(float dt) {
        this->unschedule(schedule_selector(PercentEditorUI::onUpdatePercent));
        this->showPercentLabel();
    }

    $override
    void moveGamelayer(cocos2d::CCPoint offset) {
        EditorUI::moveGamelayer(offset);
        this->showPercentLabel();
    }
};
