#pragma once

#include "cocos2d.h"
#include <map>
#include "GameDefinitions.h"
#include <vector>

USING_NS_CC;

class LevelSix : public Scene
{
public:
    static Scene* createScene();
    virtual bool init();
    virtual ~LevelSix();
    CREATE_FUNC(LevelSix);

    void update(float dt) override;

private:
    void problemLoading(const char* filename);
    void addControlButtons();
    void resetGame();
    void activateTrapAtPosition(int col, int row);
    void addTrap(TrapType type, int col, int row);
    void addTrapTrigger(int col, int row);
    void activateSpikeAt(const Vec2& spikePos);
    void activateOpeningFloorAt(const Vec2& floorPos);

    void onTouchesBegan(const std::vector<Touch*>& touches, Event* event);
    void onTouchesMoved(const std::vector<Touch*>& touches, Event* event);
    void onTouchesEnded(const std::vector<Touch*>& touches, Event* event);
    void onTouchesCancelled(const std::vector<Touch*>& touches, Event* event);

    bool isMovingLeft;
    bool isMovingRight;
    bool isJumping;
    float moveDirection;
    Sprite* sprite1;

    Animation* dashAnimation;
    Animation* jumpAnimation;

    float verticalVelocity;
    static constexpr float GRAVITY = -1000.0f;
    static constexpr float JUMP_SPEED = 370.0f;

    Size visibleSize;
    Vec2 origin;

    LayerColor* jumpButton;
    LayerColor* leftButton;
    LayerColor* rightButton;

    std::vector<std::vector<int>> mapMatrix;
    float mapOffsetX;
    float mapOffsetY;

    float startX;
    float startY;

    std::vector<DrawNode*> spikes;
    std::vector<LayerColor*> fallingColumns;
    std::vector<LayerColor*> openingFloors;

    std::vector<Vec2> spikePositions;
    std::vector<Vec2> columnPositions;
    std::vector<Vec2> floorPositions;
    std::vector<Vec2> trapTriggers;

    std::map<int, ControlButton> activeTouches;

    // Comparador para Vec2 en mapas
    struct Vec2Compare {
        bool operator()(const Vec2& a, const Vec2& b) const {
            if (a.x < b.x) return true;
            if (a.x > b.x) return false;
            return a.y < b.y;
        }
    };

    // Mapa de activadores a púas
    std::map<Vec2, Vec2, Vec2Compare> activatorToSpikeMap;

    // Mapa de activadores a pisos que se abren
    std::map<Vec2, std::vector<Vec2>, Vec2Compare> floorActivatorMap;

    static constexpr float TILE_SIZE = 20.0f;
    static constexpr int DASH_ANIMATION_TAG = 100;
};
