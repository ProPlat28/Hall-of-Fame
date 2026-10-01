#include <Geode/Geode.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/binding/GJSearchObject.hpp>
#include <Geode/binding/LevelBrowserLayer.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>

using namespace geode::prelude;

class $modify(CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init()) return false;

        auto menu = this->getChildByID("creator-buttons-menu");
        if (!menu) return true;

        auto buttonType = Mod::get()->getSettingValue<std::string>("button-type");

        CCSprite* sprMapPacks = nullptr;
        if (buttonType == "Bonus") {
            sprMapPacks = CCSprite::create("Bonus.png"_spr);
        } else {
            sprMapPacks = CCSprite::create("HallOfFame.png"_spr);
        }
        if (!sprMapPacks) return true;

        std::map<std::string, CCSprite*> idsToBtns = {
            { "map-packs-button", sprMapPacks },
        };

        for (auto& pair : idsToBtns) {
            auto id = pair.first.c_str();
            auto superExpertLoaded = (strcmp("map-packs-button", id) == 0) && Loader::get()->isModLoaded("xanii.super_expert");

            if (!menu->getChildByID(id)) continue;

            auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(menu->getChildByID(id));
            if (!btn) continue;

            auto contentSize = btn->getContentSize();
            auto existingSprite = btn->getChildByType<CCSprite*>(0);
            if (!existingSprite) continue;

            pair.second->setScale(existingSprite->getScale());

            if (!superExpertLoaded) {
                btn->setNormalImage(pair.second);
            }

            btn->setContentSize(contentSize);
        }

        return true;
    }

    void onMapPacks(CCObject* target) {
        auto buttonType = Mod::get()->getSettingValue<std::string>("button-type");

        auto type = (buttonType == "Bonus")
            ? SearchType::Bonus
            : SearchType::HallOfFame;

        auto search = GJSearchObject::create(type);
        CCDirector::get()->pushScene(
            CCTransitionFade::create(0.5f, LevelBrowserLayer::scene(search))
        );
    }
};

class $modify(LevelBrowserLayer) {
    static void onModify(auto& self) {
        (void) self.setHookPriority("LevelBrowserLayer::init", Priority::Late);
    }

    bool init(GJSearchObject* search) {
        if (!LevelBrowserLayer::init(search)) return false;

        if (search && search->m_searchType == SearchType::Bonus) {
            if (auto bi = this->getChildByID("header-sprite"))
                bi->setVisible(false);

            if (auto label = this->getChildByType<CCLabelBMFont>(0))
                label->setVisible(false);

            if (auto list = this->m_list) {
                if (auto title = list->getChildByType<CCLabelBMFont>(0))
                    title->setVisible(false);

                for (auto child : CCArrayExt<CCNode*>(list->getChildren())) {
                    if (auto spr = typeinfo_cast<CCSprite*>(child)) {
                        auto tex = spr->getTexture();
                        if (!tex || tex->getContentSize().width <= 1.f)
                            spr->setVisible(false);
                    }
                }
            }

            if (auto title = CCSprite::create("BonusTitle.png"_spr)) {
                auto winSize = CCDirector::get()->getWinSize();
                title->setPosition({winSize.width / 2.f, winSize.height - 25.f});
                this->addChild(title, 100);
            }
        }
        return true;
    }
};
