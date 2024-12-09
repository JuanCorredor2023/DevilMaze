#include "LevelSix.h"
#include "SimpleAudioEngine.h"
#include "NextLevel.h"
#include "GameUtils.h"
#include <cstdlib> // para rand
#include <ctime>   // para time

USING_NS_CC;

Scene* LevelSix::createScene()
{
    return LevelSix::create();
}

void LevelSix::problemLoading(const char* filename)
{
    printf("Error al cargar: %s\n", filename);
    printf("Dependiendo de cómo compilaste, podrías necesitar añadir 'Resources/' delante de los nombres de los archivos en LevelSix.cpp\n");
}

LevelSix::~LevelSix()
{
    if (dashAnimation)
    {
        dashAnimation->release();
        dashAnimation = nullptr;
    }
    if (jumpAnimation)
    {
        jumpAnimation->release();
        jumpAnimation = nullptr;
    }
}

bool LevelSix::init()
{
    if (!Scene::init())
    {
        return false;
    }

    srand((unsigned int)time(nullptr)); // Inicializar la semilla de random

    isMovingLeft = false;
    isMovingRight = false;
    isJumping = false;
    moveDirection = 0.0f;
    verticalVelocity = 0.0f;

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

    auto backgroundLayer = LayerColor::create(Color4B(255, 255, 255, 255));
    this->addChild(backgroundLayer, -2);

    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("baseKuro.plist");

    Vector<SpriteFrame*> dashFramesVec;
    for (int i = 0; i <= 5; i++)
    {
        std::string frameName = "dash/dash-" + std::to_string(i) + ".png";
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame)
            dashFramesVec.pushBack(frame);
        else
            problemLoading(frameName.c_str());
    }
    dashAnimation = Animation::createWithSpriteFrames(dashFramesVec, 0.05f);
    dashAnimation->retain();

    Vector<SpriteFrame*> jumpFramesVec;
    for (int i = 0; i <= 3; i++)
    {
        std::string frameName = "jump/jump-" + std::to_string(i) + ".png";
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame)
            jumpFramesVec.pushBack(frame);
        else
            problemLoading(frameName.c_str());
    }
    jumpAnimation = Animation::createWithSpriteFrames(jumpFramesVec, 0.1f);
    jumpAnimation->retain();

    sprite1 = Sprite::createWithSpriteFrameName("move/move-0.png");
    if (sprite1 == nullptr)
    {
        problemLoading("'move/move-0.png'");
        return false;
    }
    else
    {
        this->addChild(sprite1, 1);
    }

    mapMatrix = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 4, 0, 0},
        {0, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 0},
        {0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    int numRows = static_cast<int>(mapMatrix.size());
    int numCols = static_cast<int>(mapMatrix[0].size());

    mapOffsetX = origin.x;
    mapOffsetY = origin.y;

    // Dibujar el mapa y crear trampas
    for (int row = 0; row < numRows; ++row)
    {
        for (int col = 0; col < numCols; ++col)
        {
            int tileType = mapMatrix[row][col];
            Color4B color;

            if (tileType == 1)
            {
                color = Color4B::BLACK;
                auto tile = LayerColor::create(color, TILE_SIZE, TILE_SIZE);
                tile->setPosition(Vec2(col * TILE_SIZE + mapOffsetX,
                                       (numRows - row - 1) * TILE_SIZE + mapOffsetY));
                this->addChild(tile, -1);
            }
            else if (tileType == 0)
            {
                color = Color4B::WHITE;
                auto tile = LayerColor::create(color, TILE_SIZE, TILE_SIZE);
                tile->setPosition(Vec2(col * TILE_SIZE + mapOffsetX,
                                       (numRows - row - 1) * TILE_SIZE + mapOffsetY));
                this->addChild(tile, -1);
            }
            else if (tileType == 2)
            {
                addTrap(TrapType::SPIKE, col, row);
            }
            else if (tileType == 3)
            {
                addTrap(TrapType::OPENING_FLOOR, col, row);
            }
            else if (tileType == 4)
            {
                color = Color4B(200, 200, 200, 255);
                auto tile = LayerColor::create(color, TILE_SIZE, TILE_SIZE);
                tile->setPosition(Vec2(col * TILE_SIZE + mapOffsetX,
                                       (numRows - row - 1) * TILE_SIZE + mapOffsetY));
                this->addChild(tile, -1);
            }
            else if (tileType == 5)
            {
                addTrapTrigger(col, row);
                color = Color4B::WHITE;
                auto tile = LayerColor::create(color, TILE_SIZE, TILE_SIZE);
                tile->setPosition(Vec2(col * TILE_SIZE + mapOffsetX,
                                       (numRows - row - 1) * TILE_SIZE + mapOffsetY));
                this->addChild(tile, -1);
            }
        }
    }

    int startRow = 1;
    int startCol = 1;
    startX = startCol * TILE_SIZE + TILE_SIZE / 2 + mapOffsetX;
    startY = (numRows - startRow - 1) * TILE_SIZE + TILE_SIZE / 2 + mapOffsetY;
    sprite1->setPosition(Vec2(startX, startY));

    float scaleFactor = TILE_SIZE / sprite1->getContentSize().height * 0.8f;
    sprite1->setScale(scaleFactor);

    addControlButtons();
    this->scheduleUpdate();

    // Construir los mapas de activadores a trampas según la nueva lógica:
    // Tenemos en row=5 muchos tiles 5 desde col=3 a col=18.
    // Debajo en row=6 hay tiles 3 (piso que se abre)
    // Debajo en row=7 hay tiles 2 (púas)
    // Cada activador (5) mapeará a su spike correspondiente (mismo col, row=7)
    // y su piso correspondiente (mismo col, row=6).

    for (int c = 2; c <= 18; ++c)
    {
        Vec2 activatorPos(c, 5);
        Vec2 spikePos(c+1, 7);
        Vec2 floorPos(c+1, 6);

        activatorToSpikeMap[activatorPos] = spikePos;
        floorActivatorMap[activatorPos] = { floorPos };
    }


    return true;
}

void LevelSix::addTrap(TrapType type, int col, int row)
{
    switch (type)
    {
    case TrapType::SPIKE:
    {
        auto spike = DrawNode::create();
        Vec2 points[3] = {
            Vec2(0, 0),
            Vec2(TILE_SIZE / 2, TILE_SIZE),
            Vec2(TILE_SIZE, 0)
        };
        spike->drawSolidPoly(points, 3, Color4F::RED);
        spike->setPosition(Vec2(col * TILE_SIZE + mapOffsetX,
                                (mapMatrix.size() - row - 1) * TILE_SIZE + mapOffsetY - TILE_SIZE));
        this->addChild(spike, 0);
        spike->setVisible(false);
        spikes.push_back(spike);
        spikePositions.push_back(Vec2(col, row));
        break;
    }
    case TrapType::FALLING_COLUMN:
    {
        auto column = LayerColor::create(Color4B::GRAY, TILE_SIZE, TILE_SIZE * 3);
        column->setPosition(Vec2(col * TILE_SIZE + mapOffsetX,
                                 (mapMatrix.size() - row + 2) * TILE_SIZE + mapOffsetY));
        this->addChild(column, 0);
        column->setVisible(false);
        fallingColumns.push_back(column);
        columnPositions.push_back(Vec2(col, row));
        break;
    }
    case TrapType::OPENING_FLOOR:
    {
        auto floorTile = LayerColor::create(Color4B::BLACK, TILE_SIZE, TILE_SIZE);
        floorTile->setPosition(Vec2(col * TILE_SIZE + mapOffsetX,
                                    (mapMatrix.size() - row - 1) * TILE_SIZE + mapOffsetY));
        this->addChild(floorTile, -1);
        openingFloors.push_back(floorTile);
        floorPositions.push_back(Vec2(col, row));
        break;
    }
    default:
        break;
    }
}

void LevelSix::addTrapTrigger(int col, int row)
{
    Vec2 triggerPos(col, row);
    trapTriggers.push_back(triggerPos);
}

void LevelSix::addControlButtons()
{
    float buttonWidth = 100;
    float buttonHeight = 50;
    float padding = 10;

    leftButton = LayerColor::create(Color4B(211, 211, 211, 200), buttonWidth, buttonHeight);
    leftButton->setPosition(Vec2(origin.x + padding, origin.y + padding));
    this->addChild(leftButton, 2);

    rightButton = LayerColor::create(Color4B(211, 211, 211, 200), buttonWidth, buttonHeight);
    rightButton->setPosition(Vec2(origin.x + buttonWidth + 2 * padding, origin.y + padding));
    this->addChild(rightButton, 2);

    jumpButton = LayerColor::create(Color4B(211, 211, 211, 200), buttonWidth, buttonHeight);
    jumpButton->setPosition(Vec2(visibleSize.width - buttonWidth - padding, origin.y + padding));
    this->addChild(jumpButton, 2);

    auto touchListener = EventListenerTouchAllAtOnce::create();
    touchListener->onTouchesBegan = CC_CALLBACK_2(LevelSix::onTouchesBegan, this);
    touchListener->onTouchesMoved = CC_CALLBACK_2(LevelSix::onTouchesMoved, this);
    touchListener->onTouchesEnded = CC_CALLBACK_2(LevelSix::onTouchesEnded, this);
    touchListener->onTouchesCancelled = CC_CALLBACK_2(LevelSix::onTouchesCancelled, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void LevelSix::onTouchesBegan(const std::vector<Touch*>& touches, Event* event)
{
    for (auto& touch : touches)
    {
        int touchID = touch->getID();
        Vec2 touchLocation = touch->getLocation();

        if (leftButton->getBoundingBox().containsPoint(touchLocation))
        {
            isMovingLeft = true;
            moveDirection = -1.0f;
            sprite1->setFlippedX(true);

            if (!sprite1->getActionByTag(DASH_ANIMATION_TAG))
            {
                auto dashAnimate = Animate::create(dashAnimation);
                auto repeatDash = RepeatForever::create(dashAnimate);
                repeatDash->setTag(DASH_ANIMATION_TAG);
                sprite1->runAction(repeatDash);
            }

            activeTouches[touchID] = ControlButton::LEFT;
        }
        else if (rightButton->getBoundingBox().containsPoint(touchLocation))
        {
            isMovingRight = true;
            moveDirection = 1.0f;
            sprite1->setFlippedX(false);

            if (!sprite1->getActionByTag(DASH_ANIMATION_TAG))
            {
                auto dashAnimate = Animate::create(dashAnimation);
                auto repeatDash = RepeatForever::create(dashAnimate);
                repeatDash->setTag(DASH_ANIMATION_TAG);
                sprite1->runAction(repeatDash);
            }

            activeTouches[touchID] = ControlButton::RIGHT;
        }
        else if (jumpButton->getBoundingBox().containsPoint(touchLocation))
        {
            if (!isJumping)
            {
                isJumping = true;
                verticalVelocity = JUMP_SPEED;

                sprite1->stopActionByTag(DASH_ANIMATION_TAG);
                auto jumpAnimate = Animate::create(jumpAnimation);
                auto jumpAction = Sequence::create(jumpAnimate, CallFunc::create([this]() {
                    if (isMovingLeft || isMovingRight)
                    {
                        if (!sprite1->getActionByTag(DASH_ANIMATION_TAG))
                        {
                            auto dashAnimate = Animate::create(dashAnimation);
                            auto repeatDash = RepeatForever::create(dashAnimate);
                            repeatDash->setTag(DASH_ANIMATION_TAG);
                            sprite1->runAction(repeatDash);
                        }
                    }
                    else
                    {
                        sprite1->setSpriteFrame("move/move-0.png");
                    }
                }), nullptr);

                jumpAction->setTag(DASH_ANIMATION_TAG);
                sprite1->runAction(jumpAction);
            }

            activeTouches[touch->getID()] = ControlButton::JUMP;
        }
    }
}

void LevelSix::onTouchesMoved(const std::vector<Touch*>& touches, Event* event)
{
}

void LevelSix::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    for (auto& touch : touches)
    {
        int touchID = touch->getID();
        auto it = activeTouches.find(touchID);
        if (it != activeTouches.end())
        {
            ControlButton button = it->second;

            switch (button)
            {
            case ControlButton::LEFT:
                isMovingLeft = false;
                if (!isMovingRight)
                {
                    moveDirection = 0.0f;
                    sprite1->stopActionByTag(DASH_ANIMATION_TAG);
                    if (isJumping)
                    {
                        sprite1->setSpriteFrame("jump/jump-3.png");
                    }
                    else
                    {
                        sprite1->setSpriteFrame("move/move-0.png");
                    }
                }
                break;

            case ControlButton::RIGHT:
                isMovingRight = false;
                if (!isMovingLeft)
                {
                    moveDirection = 0.0f;
                    sprite1->stopActionByTag(DASH_ANIMATION_TAG);
                    if (isJumping)
                    {
                        sprite1->setSpriteFrame("jump/jump-3.png");
                    }
                    else
                    {
                        sprite1->setSpriteFrame("move/move-0.png");
                    }
                }
                break;

            case ControlButton::JUMP:
                break;

            default:
                break;
            }

            activeTouches.erase(it);
        }
    }
}

void LevelSix::onTouchesCancelled(const std::vector<Touch*>& touches, Event* event)
{
    onTouchesEnded(touches, event);
}

void LevelSix::update(float dt)
{
    Vec2 previousPosition = sprite1->getPosition();

    if (moveDirection != 0.0f)
    {
        float moveSpeed = 100.0f;
        sprite1->setPositionX(sprite1->getPositionX() + moveDirection * moveSpeed * dt);
    }

    if (isJumping)
    {
        verticalVelocity += GRAVITY * dt;
        sprite1->setPositionY(sprite1->getPositionY() + verticalVelocity * dt);
    }

    Vec2 spritePosition = sprite1->getPosition();
    Size spriteSize = sprite1->getContentSize() * sprite1->getScale();

    int leftCell = static_cast<int>((spritePosition.x - spriteSize.width / 2 - mapOffsetX) / TILE_SIZE);
    int rightCell = static_cast<int>((spritePosition.x + spriteSize.width / 2 - mapOffsetX) / TILE_SIZE);
    int bottomCell = static_cast<int>((spritePosition.y - spriteSize.height / 2 - mapOffsetY) / TILE_SIZE);
    int topCell = static_cast<int>((spritePosition.y + spriteSize.height / 2 - mapOffsetY) / TILE_SIZE);

    bottomCell = static_cast<int>(mapMatrix.size()) - bottomCell - 1;
    topCell = static_cast<int>(mapMatrix.size()) - topCell - 1;

    leftCell = std::max(0, std::min(leftCell, static_cast<int>(mapMatrix[0].size()) - 1));
    rightCell = std::max(0, std::min(rightCell, static_cast<int>(mapMatrix[0].size()) - 1));
    bottomCell = std::max(0, std::min(bottomCell, static_cast<int>(mapMatrix.size()) - 1));
    topCell = std::max(0, std::min(topCell, static_cast<int>(mapMatrix.size()) - 1));

    bool collided = false;

    for (int row = topCell; row <= bottomCell; ++row)
    {
        for (int col = leftCell; col <= rightCell; ++col)
        {
            int tileValue = mapMatrix[row][col];
            if (tileValue == 1)
            {
                collided = true;
                break;
            }
        }
        if (collided)
            break;
    }

    if (collided)
    {
        sprite1->setPosition(previousPosition);
        verticalVelocity = 0;
        isJumping = false;
    }

    int groundRow = static_cast<int>((spritePosition.y - spriteSize.height / 2 - 1 - mapOffsetY) / TILE_SIZE);
    groundRow = static_cast<int>(mapMatrix.size()) - groundRow - 1;
    groundRow = std::max(0, std::min(groundRow, static_cast<int>(mapMatrix.size()) - 1));

    bool onGround = false;
    for (int col = leftCell; col <= rightCell; ++col)
    {
        int tileValue = mapMatrix[groundRow][col];
        if (tileValue == 1 || tileValue == 4 || tileValue == 3)
        {
            onGround = true;
            break;
        }
    }

    if (!onGround)
    {
        if (!isJumping)
        {
            isJumping = true;
            verticalVelocity = 0;
        }
    }
    else
    {
        if (isJumping)
        {
            isJumping = false;
            verticalVelocity = 0;

            sprite1->stopActionByTag(DASH_ANIMATION_TAG);

            if (moveDirection != 0.0f)
            {
                auto dashAnimate = Animate::create(dashAnimation);
                auto repeatDash = RepeatForever::create(dashAnimate);
                repeatDash->setTag(DASH_ANIMATION_TAG);
                sprite1->runAction(repeatDash);
            }
            else
            {
                sprite1->setSpriteFrame("move/move-0.png");
            }
        }
    }

    int playerCol = static_cast<int>((spritePosition.x - mapOffsetX) / TILE_SIZE);
    int playerRow = static_cast<int>((spritePosition.y - mapOffsetY) / TILE_SIZE);
    playerRow = static_cast<int>(mapMatrix.size()) - playerRow - 1;

    playerCol = std::max(0, std::min(playerCol, static_cast<int>(mapMatrix[0].size()) - 1));
    playerRow = std::max(0, std::min(playerRow, static_cast<int>(mapMatrix.size()) - 1));

    int currentTile = mapMatrix[playerRow][playerCol];
    if (currentTile == 5)
    {
        activateTrapAtPosition(playerCol, playerRow);
        mapMatrix[playerRow][playerCol] = 0;
    }

    if (currentTile == 4 && moveDirection == 0.0f && !isJumping)
    {
        auto nextLevelScene = NextLevel::createScene();
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, nextLevelScene));
    }

    // Polígono del personaje
    std::vector<Vec2> playerPoly;
    {
        Rect playerRect = sprite1->getBoundingBox();
        playerPoly.push_back(Vec2(playerRect.getMinX(), playerRect.getMinY()));
        playerPoly.push_back(Vec2(playerRect.getMinX(), playerRect.getMaxY()));
        playerPoly.push_back(Vec2(playerRect.getMaxX(), playerRect.getMaxY()));
        playerPoly.push_back(Vec2(playerRect.getMaxX(), playerRect.getMinY()));
    }

    for (size_t i = 0; i < spikes.size(); i++)
    {
        auto spike = spikes[i];
        if (spike->isVisible())
        {
            Vec2 spikePos = spike->getPosition();
            std::vector<Vec2> spikePoly;
            spikePoly.push_back(spikePos + Vec2(0, 0));
            spikePoly.push_back(spikePos + Vec2(TILE_SIZE / 2, TILE_SIZE));
            spikePoly.push_back(spikePos + Vec2(TILE_SIZE, 0));

            if (polygonsIntersect(playerPoly, spikePoly))
            {
                resetGame();
                return;
            }
        }
    }

    for (auto column : fallingColumns)
    {
        if (column->isVisible())
        {
            Rect columnRect = column->getBoundingBox();
            Rect playerRect = sprite1->getBoundingBox();
            if (columnRect.intersectsRect(playerRect))
            {
                resetGame();
                return;
            }
        }
    }

    if (sprite1->getPositionY() < mapOffsetY - spriteSize.height)
    {
        resetGame();
    }
}

void LevelSix::activateTrapAtPosition(int col, int row)
{
    Vec2 activatorPos(col, row);

    // Ahora se hace un random choice entre activar piso que se abre o púas
    float randomVal = CCRANDOM_0_1(); // entre 0 y 1
    if (randomVal < 0.5f)
    {
        // Activar piso
        auto floorIt = floorActivatorMap.find(activatorPos);
        if (floorIt != floorActivatorMap.end())
        {
            std::vector<Vec2> floorPositionsToActivate = floorIt->second;
            for (const auto& floorPos : floorPositionsToActivate)
            {
                activateOpeningFloorAt(floorPos);
            }
            return;
        }
    }
    else
    {
        // Activar púa
        auto spikeIt = activatorToSpikeMap.find(activatorPos);
        if (spikeIt != activatorToSpikeMap.end())
        {
            Vec2 spikePos = spikeIt->second;
            activateSpikeAt(spikePos);
            return;
        }
    }

    CCLOG("Activator at (%d,%d) has no associated trap or random choice failed.", col, row);
}

void LevelSix::activateSpikeAt(const Vec2& spikePos)
{
    for (size_t i = 0; i < spikePositions.size(); ++i)
    {
        Vec2 pos = spikePositions[i];
        if (pos == spikePos)
        {
            auto spike = spikes[i];
            if (!spike->isVisible())
            {
                spike->setVisible(true);
                auto moveUp = MoveBy::create(2.0f, Vec2(0, visibleSize.height * 2));
                auto resetFunc = CallFunc::create([this, spike, pos]() {
                    spike->stopAllActions();
                    spike->setVisible(false);
                    spike->setPosition(Vec2(pos.x * TILE_SIZE + mapOffsetX,
                                            (mapMatrix.size() - pos.y - 1) * TILE_SIZE + mapOffsetY - TILE_SIZE));
                });
                auto seq = Sequence::create(moveUp, resetFunc, nullptr);
                spike->runAction(seq);
            }
            break;
        }
    }
}

void LevelSix::activateOpeningFloorAt(const Vec2& floorPos)
{
    for (size_t i = 0; i < floorPositions.size(); ++i)
    {
        Vec2 pos = floorPositions[i];
        if (pos == floorPos)
        {
            auto floorTile = openingFloors[i];
            if (floorTile->isVisible())
            {
                floorTile->setColor(Color3B::WHITE);
                int col = static_cast<int>(pos.x);
                int row = static_cast<int>(pos.y);
                mapMatrix[row][col] = 0;
                floorTile->setVisible(false);
            }
            break;
        }
    }
}

void LevelSix::resetGame()
{
    sprite1->setPosition(Vec2(startX, startY));
    verticalVelocity = 0;
    isJumping = false;
    moveDirection = 0.0f;
    isMovingLeft = false;
    isMovingRight = false;

    sprite1->stopAllActions();
    sprite1->setSpriteFrame("move/move-0.png");

    for (size_t i = 0; i < spikes.size(); ++i)
    {
        auto spike = spikes[i];
        spike->stopAllActions();
        spike->setVisible(false);
        Vec2 pos = spikePositions[i];
        spike->setPosition(Vec2(pos.x * TILE_SIZE + mapOffsetX,
                                (mapMatrix.size() - pos.y - 1) * TILE_SIZE + mapOffsetY - TILE_SIZE));
    }

    for (size_t i = 0; i < fallingColumns.size(); ++i)
    {
        auto column = fallingColumns[i];
        column->stopAllActions();
        column->setVisible(false);
        Vec2 pos = columnPositions[i];
        column->setPosition(Vec2(pos.x * TILE_SIZE + mapOffsetX,
                                 (mapMatrix.size() - pos.y + 2) * TILE_SIZE + mapOffsetY));
    }

    for (size_t i = 0; i < openingFloors.size(); ++i)
    {
        auto floorTile = openingFloors[i];
        floorTile->stopAllActions();
        floorTile->setVisible(true);
        floorTile->setColor(Color3B::BLACK);
        Vec2 pos = floorPositions[i];
        mapMatrix[pos.y][pos.x] = 3;
    }

    for (auto triggerPos : trapTriggers)
    {
        int c = static_cast<int>(triggerPos.x);
        int r = static_cast<int>(triggerPos.y);
        mapMatrix[r][c] = 5;
    }

    activeTouches.clear();
}
