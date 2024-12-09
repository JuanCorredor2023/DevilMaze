// HelloWorldScene.cpp
/*#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "LevelOne.h" // Asegúrate de tener esta escena creada

USING_NS_CC;

// Implementación de la función para verificar si un punto está dentro de un polígono convexo
bool pointInPolygon(const Vec2& point, const std::vector<Vec2>& polygon) {
    int n = static_cast<int>(polygon.size());
    for (int i = 0; i < n; i++) {
        Vec2 a = polygon[i];
        Vec2 b = polygon[(i+1) % n];
        Vec2 edge = b - a;
        Vec2 vp = point - a;
        float cross = edge.cross(vp);
        if (cross < 0) {
            return false;
        }
    }
    return true;
}

// Implementación de la función para verificar si dos polígonos convexos se intersectan usando el Teorema del Eje Separador (SAT)
bool polygonsIntersect(const std::vector<Vec2>& polyA, const std::vector<Vec2>& polyB) {
    // Función interna para proyectar un polígono en un eje
    auto project = [](const std::vector<Vec2>& poly, const Vec2& axis) {
        float minProj = poly[0].dot(axis);
        float maxProj = minProj;
        for (size_t i = 1; i < poly.size(); i++) {
            float proj = poly[i].dot(axis);
            if (proj < minProj) minProj = proj;
            if (proj > maxProj) maxProj = proj;
        }
        return std::pair<float,float>(minProj, maxProj);
    };

    // Función interna para probar un eje
    auto testAxis = [&](const std::vector<Vec2>& pA, const std::vector<Vec2>& pB, const Vec2& axis) {
        auto pAProj = project(pA, axis);
        auto pBProj = project(pB, axis);
        // Si no se superponen en este eje, no hay colisión
        if (pAProj.second < pBProj.first || pBProj.second < pAProj.first) {
            return false;
        }
        return true;
    };

    // Se obtienen ejes normales de los bordes del polígono A
    for (size_t i = 0; i < polyA.size(); i++) {
        Vec2 p1 = polyA[i];
        Vec2 p2 = polyA[(i+1) % polyA.size()];
        Vec2 edge = p2 - p1;
        Vec2 axis = Vec2(-edge.y, edge.x).getNormalized();
        if (!testAxis(polyA, polyB, axis)) return false;
    }

    // Se obtienen ejes normales de los bordes del polígono B
    for (size_t i = 0; i < polyB.size(); i++) {
        Vec2 p1 = polyB[i];
        Vec2 p2 = polyB[(i+1) % polyB.size()];
        Vec2 edge = p2 - p1;
        Vec2 axis = Vec2(-edge.y, edge.x).getNormalized();
        if (!testAxis(polyA, polyB, axis)) return false;
    }

    return true;
}

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

void HelloWorld::problemLoading(const char* filename)
{
    printf("Error al cargar: %s\n", filename);
    printf("Dependiendo de cómo compilaste, podrías necesitar añadir 'Resources/' delante de los nombres de los archivos en HelloWorldScene.cpp\n");
}

HelloWorld::~HelloWorld()
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

bool HelloWorld::init()
{
    if (!Scene::init())
    {
        return false;
    }

    // Inicialización de variables
    isMovingLeft = false;
    isMovingRight = false;
    isJumping = false;
    moveDirection = 0.0f;
    verticalVelocity = 0.0f;

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

    // Crear una capa de fondo blanca
    auto backgroundLayer = LayerColor::create(Color4B(255, 255, 255, 255));
    this->addChild(backgroundLayer, -2);

    // Cargar el spritesheet en la caché
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("baseKuro.plist");

    // Crear animación de dash
    Vector<SpriteFrame*> dashFramesVec;
    for (int i = 0; i <= 5; i++)
    {
        std::string frameName = "dash/dash-" + std::to_string(i) + ".png";
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame)
        {
            dashFramesVec.pushBack(frame);
        }
        else
        {
            CCLOG("Error: Frame de dash no encontrado: %s", frameName.c_str());
            problemLoading(frameName.c_str());
        }
    }
    dashAnimation = Animation::createWithSpriteFrames(dashFramesVec, 0.05f);
    dashAnimation->retain();

    // Crear animación de salto
    Vector<SpriteFrame*> jumpFramesVec;
    for (int i = 0; i <= 3; i++)
    {
        std::string frameName = "jump/jump-" + std::to_string(i) + ".png";
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame)
        {
            jumpFramesVec.pushBack(frame);
        }
        else
        {
            CCLOG("Error: Frame de salto no encontrado: %s", frameName.c_str());
            problemLoading(frameName.c_str());
        }
    }
    jumpAnimation = Animation::createWithSpriteFrames(jumpFramesVec, 0.1f);
    jumpAnimation->retain();

    // Crear el sprite inicial del personaje
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

    // Definir el mapa como una matriz
    mapMatrix = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 5, 0, 0, 0, 0, 1}, // Activador en (8,3)
        {1, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 5, 0, 0, 0, 1, 0, 5, 5, 0, 0, 0, 1}, // Activador en (6,4)
        {1, 0, 0, 0, 0, 0, 5, 0, 1, 0, 5, 0, 1, 0, 5, 0, 1, 0, 0, 5, 0, 0, 0, 1}, // Activador en (4,5)
        {1, 0, 0, 0, 5, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 5, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 1},
        {1, 1, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 1, 1, 1},
        {1, 1, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 1, 1, 1},
        {1, 1, 1, 3, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 3, 1, 1, 1, 1}, // Púas en (4,11), (6,11), (8,11)
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
                // Tile de pared (negro)
                color = Color4B::BLACK;
                auto tile = LayerColor::create(color, TILE_SIZE, TILE_SIZE);
                tile->setPosition(Vec2(col * TILE_SIZE + mapOffsetX,
                                       (numRows - row - 1) * TILE_SIZE + mapOffsetY));
                this->addChild(tile, -1);
            }
            else if (tileType == 0)
            {
                // Tile de fondo (blanco)
                color = Color4B::WHITE;
                auto tile = LayerColor::create(color, TILE_SIZE, TILE_SIZE);
                tile->setPosition(Vec2(col * TILE_SIZE + mapOffsetX,
                                       (numRows - row - 1) * TILE_SIZE + mapOffsetY));
                this->addChild(tile, -1);
            }
            else if (tileType == 2)
            {
                // Añadir una púa
                addTrap(TrapType::SPIKE, col, row);
            }
            else if (tileType == 3)
            {
                // Añadir un piso que se abre
                addTrap(TrapType::OPENING_FLOOR, col, row);
            }
            else if (tileType == 4)
            {
                // Tile especial: Gris muy claro
                color = Color4B(200, 200, 200, 255); // Gris claro
                auto tile = LayerColor::create(color, TILE_SIZE, TILE_SIZE);
                tile->setPosition(Vec2(col * TILE_SIZE + mapOffsetX,
                                       (numRows - row - 1) * TILE_SIZE + mapOffsetY));
                this->addChild(tile, -1);
            }
            else if (tileType == 5)
            {
                // Añadir un activador de trampas
                addTrapTrigger(col, row);
                // Visualizar el activador con fondo amarillo
                color = Color4B::YELLOW;
                auto tile = LayerColor::create(color, TILE_SIZE, TILE_SIZE);
                tile->setPosition(Vec2(col * TILE_SIZE + mapOffsetX,
                                       (numRows - row - 1) * TILE_SIZE + mapOffsetY));
                this->addChild(tile, -1);
            }
        }
    }

    // Definir la posición inicial del personaje
    int startRow = 1;
    int startCol = 1;
    startX = startCol * TILE_SIZE + TILE_SIZE / 2 + mapOffsetX;
    startY = (numRows - startRow - 1) * TILE_SIZE + TILE_SIZE / 2 + mapOffsetY;
    sprite1->setPosition(Vec2(startX, startY));

    // Ajustar la escala del personaje
    float scaleFactor = TILE_SIZE / sprite1->getContentSize().height * 0.8f;
    sprite1->setScale(scaleFactor);

    // Añadir botones de control
    addControlButtons();

    // Programar la actualización cada frame
    this->scheduleUpdate();

    return true;
}

void HelloWorld::addTrap(TrapType type, int col, int row)
{
    switch (type)
    {
    case TrapType::SPIKE:
    {
        // Crear la púa como un triángulo rojo
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
        // Crear una columna gris que cae
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
        // Crear un piso que inicialmente es sólido (color negro)
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

void HelloWorld::addTrapTrigger(int col, int row)
{
    // Crear un activador de trampas en la posición dada
    Vec2 triggerPos(col, row);
    trapTriggers.push_back(triggerPos);
}

void HelloWorld::addControlButtons()
{
    // Tamaño y posición de los botones
    float buttonWidth = 100;
    float buttonHeight = 50;
    float padding = 10;

    // Botón de movimiento hacia la izquierda
    leftButton = LayerColor::create(Color4B(0, 255, 0, 128), buttonWidth, buttonHeight);
    leftButton->setPosition(Vec2(origin.x + padding, origin.y + padding));
    this->addChild(leftButton, 2);

    // Botón de movimiento hacia la derecha
    rightButton = LayerColor::create(Color4B(0, 0, 255, 128), buttonWidth, buttonHeight);
    rightButton->setPosition(Vec2(origin.x + buttonWidth + 2 * padding, origin.y + padding));
    this->addChild(rightButton, 2);

    // Botón de salto
    jumpButton = LayerColor::create(Color4B(255, 0, 0, 128), buttonWidth, buttonHeight);
    jumpButton->setPosition(Vec2(visibleSize.width - buttonWidth - padding, origin.y + padding));
    this->addChild(jumpButton, 2);

    // Crear un único EventListenerTouchAllAtOnce
    auto touchListener = EventListenerTouchAllAtOnce::create();

    touchListener->onTouchesBegan = CC_CALLBACK_2(HelloWorld::onTouchesBegan, this);
    touchListener->onTouchesMoved = CC_CALLBACK_2(HelloWorld::onTouchesMoved, this);
    touchListener->onTouchesEnded = CC_CALLBACK_2(HelloWorld::onTouchesEnded, this);
    touchListener->onTouchesCancelled = CC_CALLBACK_2(HelloWorld::onTouchesCancelled, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void HelloWorld::onTouchesBegan(const std::vector<Touch*>& touches, Event* event)
{
    for (auto& touch : touches)
    {
        int touchID = touch->getID();
        Vec2 touchLocation = touch->getLocation();

        // Verificar si el toque está en el botón de izquierda
        if (leftButton->getBoundingBox().containsPoint(touchLocation))
        {
            isMovingLeft = true;
            moveDirection = -1.0f;
            sprite1->setFlippedX(true);

            // Iniciar animación de dash si no está ya corriendo
            if (!sprite1->getActionByTag(DASH_ANIMATION_TAG))
            {
                auto dashAnimate = Animate::create(dashAnimation);
                auto repeatDash = RepeatForever::create(dashAnimate);
                repeatDash->setTag(DASH_ANIMATION_TAG);
                sprite1->runAction(repeatDash);
            }

            activeTouches[touchID] = ControlButton::LEFT;
        }
        // Verificar si el toque está en el botón de derecha
        else if (rightButton->getBoundingBox().containsPoint(touchLocation))
        {
            isMovingRight = true;
            moveDirection = 1.0f;
            sprite1->setFlippedX(false);

            // Iniciar animación de dash si no está ya corriendo
            if (!sprite1->getActionByTag(DASH_ANIMATION_TAG))
            {
                auto dashAnimate = Animate::create(dashAnimation);
                auto repeatDash = RepeatForever::create(dashAnimate);
                repeatDash->setTag(DASH_ANIMATION_TAG);
                sprite1->runAction(repeatDash);
            }

            activeTouches[touchID] = ControlButton::RIGHT;
        }
        // Verificar si el toque está en el botón de salto
        else if (jumpButton->getBoundingBox().containsPoint(touchLocation))
        {
            if (!isJumping)
            {
                isJumping = true;
                verticalVelocity = JUMP_SPEED;

                // Ejecutar animación de salto
                sprite1->stopActionByTag(DASH_ANIMATION_TAG);
                auto jumpAnimate = Animate::create(jumpAnimation);
                auto jumpAction = Sequence::create(jumpAnimate, CallFunc::create([this]() {
                    // Después de la animación de salto, reiniciar animación de dash si es necesario
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

                jumpAction->setTag(DASH_ANIMATION_TAG); // Reutilizamos la etiqueta para mantener consistencia
                sprite1->runAction(jumpAction);
            }

            activeTouches[touch->getID()] = ControlButton::JUMP;
        }
    }
}

void HelloWorld::onTouchesMoved(const std::vector<Touch*>& touches, Event* event)
{
    // Implementar si necesitas manejar movimientos de toque
}

void HelloWorld::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
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
                // No se requiere acción al soltar el botón de salto
                break;

            default:
                break;
            }

            // Remover el toque de activeTouches
            activeTouches.erase(it);
        }
    }
}

void HelloWorld::onTouchesCancelled(const std::vector<Touch*>& touches, Event* event)
{
    onTouchesEnded(touches, event);
}

void HelloWorld::update(float dt)
{
    Vec2 previousPosition = sprite1->getPosition();

    // Movimiento horizontal
    if (moveDirection != 0.0f)
    {
        float moveSpeed = 100.0f; // Velocidad de movimiento
        sprite1->setPositionX(sprite1->getPositionX() + moveDirection * moveSpeed * dt);
    }

    // Movimiento vertical (salto)
    if (isJumping)
    {
        verticalVelocity += GRAVITY * dt;
        sprite1->setPositionY(sprite1->getPositionY() + verticalVelocity * dt);
    }

    // Obtener la posición y tamaño del personaje
    Vec2 spritePosition = sprite1->getPosition();
    Size spriteSize = sprite1->getContentSize() * sprite1->getScale();

    // Calcular las celdas del mapa que ocupa el personaje
    int leftCell = static_cast<int>((spritePosition.x - spriteSize.width / 2 - mapOffsetX) / TILE_SIZE);
    int rightCell = static_cast<int>((spritePosition.x + spriteSize.width / 2 - mapOffsetX) / TILE_SIZE);
    int bottomCell = static_cast<int>((spritePosition.y - spriteSize.height / 2 - mapOffsetY) / TILE_SIZE);
    int topCell = static_cast<int>((spritePosition.y + spriteSize.height / 2 - mapOffsetY) / TILE_SIZE);

    // Invertir el eje Y para coincidir con la matriz
    bottomCell = static_cast<int>(mapMatrix.size()) - bottomCell - 1;
    topCell = static_cast<int>(mapMatrix.size()) - topCell - 1;

    // Limitar los índices de celdas al tamaño del mapa
    leftCell = std::max(0, std::min(leftCell, static_cast<int>(mapMatrix[0].size()) - 1));
    rightCell = std::max(0, std::min(rightCell, static_cast<int>(mapMatrix[0].size()) - 1));
    bottomCell = std::max(0, std::min(bottomCell, static_cast<int>(mapMatrix.size()) - 1));
    topCell = std::max(0, std::min(topCell, static_cast<int>(mapMatrix.size()) - 1));

    bool collided = false;

    // Detectar colisiones con paredes (tiles tipo 1)
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
        // Revertir la posición si hay colisión
        sprite1->setPosition(previousPosition);
        verticalVelocity = 0;
        isJumping = false;
    }

    // Verificar si el personaje está en el suelo
    int groundRow = static_cast<int>((spritePosition.y - spriteSize.height / 2 - 1 - mapOffsetY) / TILE_SIZE);
    groundRow = static_cast<int>(mapMatrix.size()) - groundRow - 1;
    groundRow = std::max(0, std::min(groundRow, static_cast<int>(mapMatrix.size()) - 1));

    bool onGround = false;
    for (int col = leftCell; col <= rightCell; ++col)
    {
        int tileValue = mapMatrix[groundRow][col];
        if (tileValue == 1 || tileValue == 4 || tileValue == 3) // Añadido tileValue == 3 para piso que se abre
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

            // Reiniciar animación de dash si el personaje está moviéndose
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

    // Obtener la celda actual del personaje
    int playerCol = static_cast<int>((spritePosition.x - mapOffsetX) / TILE_SIZE);
    int playerRow = static_cast<int>((spritePosition.y - mapOffsetY) / TILE_SIZE);
    playerRow = static_cast<int>(mapMatrix.size()) - playerRow - 1;

    playerCol = std::max(0, std::min(playerCol, static_cast<int>(mapMatrix[0].size()) - 1));
    playerRow = std::max(0, std::min(playerRow, static_cast<int>(mapMatrix.size()) - 1));

    int currentTile = mapMatrix[playerRow][playerCol];
    if (currentTile == 5) // Activador de trampas
    {
        // Activar la trampa correspondiente
        activateTrapAtPosition(playerCol, playerRow);
        // Cambiar el valor del tile para evitar reactivación inmediata
        mapMatrix[playerRow][playerCol] = 0;
    }

    // Nueva lógica para el tileType == 4
    if (currentTile == 4 && moveDirection == 0.0f && !isJumping)
    {
        // Cambiar a la siguiente escena, por ejemplo, LevelOne
        auto nextLevelScene = LevelOne::createScene();
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, nextLevelScene));
    }

    // Definir el polígono del personaje (rectángulo)
    std::vector<Vec2> playerPoly;
    {
        Rect playerRect = sprite1->getBoundingBox();
        // Esquinas del rectángulo del personaje
        playerPoly.push_back(Vec2(playerRect.getMinX(), playerRect.getMinY()));
        playerPoly.push_back(Vec2(playerRect.getMinX(), playerRect.getMaxY()));
        playerPoly.push_back(Vec2(playerRect.getMaxX(), playerRect.getMaxY()));
        playerPoly.push_back(Vec2(playerRect.getMaxX(), playerRect.getMinY()));
    }

    // Verificar colisiones con las púas usando polígonos
    for (size_t i = 0; i < spikes.size(); i++)
    {
        auto spike = spikes[i];
        if (spike->isVisible())
        {
            // Obtener las coordenadas mundiales del triángulo que representa la púa
            Vec2 spikePos = spike->getPosition();
            // Los puntos locales del triángulo
            std::vector<Vec2> spikePoly;
            spikePoly.push_back(spikePos + Vec2(0, 0));
            spikePoly.push_back(spikePos + Vec2(TILE_SIZE / 2, TILE_SIZE));
            spikePoly.push_back(spikePos + Vec2(TILE_SIZE, 0));

            // Revisar intersección poligonal
            if (polygonsIntersect(playerPoly, spikePoly))
            {
                resetGame();
                return;
            }
        }
    }

    // Verificar colisiones con las columnas (bounding box es suficiente)
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

    // Verificar si el personaje ha caído fuera del mapa
    if (sprite1->getPositionY() < mapOffsetY - spriteSize.height)
    {
        resetGame();
    }
}

void HelloWorld::activateTrapAtPosition(int col, int row)
{
    Vec2 activatorPos(col, row);

    // Verificar si es una activación para una púa
    auto it = activatorToSpikeMap.find(activatorPos);
    if (it != activatorToSpikeMap.end())
    {
        Vec2 spikePos = it->second;
        CCLOG("Activator at (%d,%d) triggered spike.", col, row);
        activateSpikeAt(spikePos);
        return;
    }

    // Verificar si es una activación para uno o más pisos que se abren
    auto floorIt = floorActivatorMap.find(activatorPos);
    if (floorIt != floorActivatorMap.end())
    {
        std::vector<Vec2> floorPositionsToActivate = floorIt->second;
        CCLOG("Activator at (%d,%d) triggered opening floor(s).", col, row);
        for (const auto& floorPos : floorPositionsToActivate)
        {
            activateOpeningFloorAt(floorPos);
        }
        return;
    }

    // Si no se encontró ninguna trampa asociada
    CCLOG("Activator at (%d,%d) has no associated trap.", col, row);
}

void HelloWorld::activateSpikeAt(const Vec2& spikePos)
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
                // Acción: mover las púas hacia arriba y reiniciar su posición después
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

void HelloWorld::activateOpeningFloorAt(const Vec2& floorPos)
{
    for (size_t i = 0; i < floorPositions.size(); ++i)
    {
        Vec2 pos = floorPositions[i];
        if (pos == floorPos)
        {
            auto floorTile = openingFloors[i];
            if (floorTile->isVisible())
            {
                // Cambiar el color a blanco
                floorTile->setColor(Color3B::WHITE);

                // Actualizar la matriz del mapa para que el tile sea no sólido
                int col = static_cast<int>(pos.x);
                int row = static_cast<int>(pos.y);
                mapMatrix[row][col] = 0; // Asumiendo que 0 es un tile no sólido

                // Hacer que el piso desaparezca visualmente
                floorTile->setVisible(false);
            }
            break;
        }
    }
}

void HelloWorld::resetGame()
{
    // Reiniciar la posición del personaje
    sprite1->setPosition(Vec2(startX, startY));
    verticalVelocity = 0;
    isJumping = false;
    moveDirection = 0.0f;
    isMovingLeft = false;
    isMovingRight = false;

    // Detener todas las acciones y reiniciar la animación del personaje
    sprite1->stopAllActions();
    sprite1->setSpriteFrame("move/move-0.png");

    // Resetear todas las púas
    for (size_t i = 0; i < spikes.size(); ++i)
    {
        auto spike = spikes[i];
        spike->stopAllActions();
        spike->setVisible(false);
        Vec2 pos = spikePositions[i];
        spike->setPosition(Vec2(pos.x * TILE_SIZE + mapOffsetX,
                                (mapMatrix.size() - pos.y - 1) * TILE_SIZE + mapOffsetY - TILE_SIZE));
    }

    // Resetear todas las columnas
    for (size_t i = 0; i < fallingColumns.size(); ++i)
    {
        auto column = fallingColumns[i];
        column->stopAllActions();
        column->setVisible(false);
        Vec2 pos = columnPositions[i];
        column->setPosition(Vec2(pos.x * TILE_SIZE + mapOffsetX,
                                 (mapMatrix.size() - pos.y + 2) * TILE_SIZE + mapOffsetY));
    }

    // Resetear todos los pisos que se abren
    for (size_t i = 0; i < openingFloors.size(); ++i)
    {
        auto floorTile = openingFloors[i];
        floorTile->stopAllActions();
        floorTile->setVisible(true);
        floorTile->setColor(Color3B::BLACK); // Restaurar color original
        Vec2 pos = floorPositions[i];
        mapMatrix[pos.y][pos.x] = 3; // Restaurar el tipo de tile en la matriz
    }

    // Resetear todos los activadores
    for (auto triggerPos : trapTriggers)
    {
        // Restaurar el tile de activador en la matriz
        int c = static_cast<int>(triggerPos.x);
        int r = static_cast<int>(triggerPos.y);
        mapMatrix[r][c] = 5; // Asumiendo que 5 es un activador de trampa
    }

    // Limpiar toques activos
    activeTouches.clear();
}
*/
