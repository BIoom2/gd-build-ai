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
        int m_lastPercent = -1;
    };

    int getCurrentPercent() {
        if (!m_positionSlider) return 0;
        float sliderVal = m_positionSlider->getValue();
        int percent = static_cast<int>(std::round(sliderVal * 100.f));
        if (percent < 0) percent = 0;
        if (percent > 100) percent = 100;
        return percent;
    }

    void showPercentLabel() {
        if (!m_fields->m_percentLabel) return;

        int percent = this->getCurrentPercent();
        auto str = std::to_string(percent) + "%";
        m_fields->m_percentLabel->setString(str.c_str());

        m_fields->m_percentLabel->setVisible(true);
        m_fields->m_percentLabel->setOpacity(180);
        m_fields->m_isVisible = true;
        m_fields->m_hideTimer = 1.5f;
        m_fields->m_lastPercent = percent;
    }

    void onPercentTick(float dt) {
        if (!m_fields->m_percentLabel) return;

        int percent = this->getCurrentPercent();
        if (percent != m_fields->m_lastPercent) {
            this->showPercentLabel();
            return;
        }

        if (m_fields->m_isVisible) {
            m_fields->m_hideTimer -= dt;
            if (m_fields->m_hideTimer <= 0.f) {
                m_fields->m_percentLabel->setVisible(false);
                m_fields->m_isVisible = false;
            }
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
        m_fields->m_lastPercent = this->getCurrentPercent();

        this->schedule(schedule_selector(PercentEditorUI::onPercentTick), 0.05f);

        return true;
    }
};
