#include "LevelFour.h"
#include "SimpleAudioEngine.h"
#include "NextLevel.h" // Asegúrate de tener esta escena creada
#include "GameUtils.h"

USING_NS_CC;

// Método para crear la escena
Scene* LevelFour::createScene()
{
    return LevelFour::create();
}

// Destructor de LevelFour para liberar recursos
LevelFour::~LevelFour()
{
    // Liberar las animaciones retenidas
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

// Inicialización de la escena
bool LevelFour::init()
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
        {1, 0, 0, 5, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
        {1, 0, 0, 5, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
        {1, 0, 0, 5, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
        {1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 4, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1},
        {1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1}
    };

    int numRows = static_cast<int>(mapMatrix.size());
    int numCols = static_cast<int>(mapMatrix[0].size());

    mapOffsetX = origin.x;
    mapOffsetY = origin.y;

    // Inicializar el vector de tiles y colores originales
    tiles.resize(numRows, std::vector<LayerColor*>(numCols, nullptr));
    originalTileColors.resize(numRows, std::vector<Color3B>(numCols, Color3B::WHITE));

    // Guardar el mapa original
    originalMapMatrix = mapMatrix;

    // Dibujar el mapa y crear trampas
    for (int row = 0; row < numRows; ++row)
    {
        for (int col = 0; col < numCols; ++col)
        {
            int tileType = mapMatrix[row][col];
            Color4B color;

            LayerColor* tile = nullptr;

            if (tileType == 1)
            {
                // Tile de pared (negro)
                color = Color4B::BLACK;
                tile = LayerColor::create(color, TILE_SIZE, TILE_SIZE);
                tile->setPosition(Vec2(col * TILE_SIZE + mapOffsetX,
                                       (numRows - row - 1) * TILE_SIZE + mapOffsetY));
                this->addChild(tile, -1);

                // Almacenar el color original
                originalTileColors[row][col] = Color3B::BLACK;
            }
            else if (tileType == 0)
            {
                // Tile de fondo (blanco)
                color = Color4B::WHITE;
                tile = LayerColor::create(color, TILE_SIZE, TILE_SIZE);
                tile->setPosition(Vec2(col * TILE_SIZE + mapOffsetX,
                                       (numRows - row - 1) * TILE_SIZE + mapOffsetY));
                this->addChild(tile, -1);

                // Almacenar el color original
                originalTileColors[row][col] = Color3B::WHITE;
            }
            else if (tileType == 4)
            {
                // Tile especial: Gris muy claro
                color = Color4B(200, 200, 200, 255); // Gris claro
                tile = LayerColor::create(color, TILE_SIZE, TILE_SIZE);
                tile->setPosition(Vec2(col * TILE_SIZE + mapOffsetX,
                                       (numRows - row - 1) * TILE_SIZE + mapOffsetY));
                this->addChild(tile, -1);

                // Almacenar el color original
                originalTileColors[row][col] = Color3B(200, 200, 200);
            }
            else if (tileType == 5)
            {
                // Añadir un activador de trampas
                // Visualizar el activador con fondo amarillo
                color = Color4B::WHITE;
                tile = LayerColor::create(color, TILE_SIZE, TILE_SIZE);
                tile->setPosition(Vec2(col * TILE_SIZE + mapOffsetX,
                                       (numRows - row - 1) * TILE_SIZE + mapOffsetY));
                this->addChild(tile, -1);

                // Almacenar el color original
                originalTileColors[row][col] = Color3B::WHITE;
            }

            if (tile)
            {
                tiles[row][col] = tile;
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

// Función auxiliar para acceder a una tile específica
LayerColor* LevelFour::getTileAt(int col, int row)
{
    if (row >= 0 && row < tiles.size() && col >= 0 && col < tiles[0].size())
    {
        return tiles[row][col];
    }
    return nullptr;
}

// Función para activar la trampa en una posición específica
void LevelFour::activateTrapAtPosition(int col, int row)
{
    // Verificar si el activador es el tile tipo 5 y si la trampa no está ya activada
    if (mapMatrix[row][col] == 5 && !trapsActivated)
    {
        CCLOG("Activador en (%d, %d) activado. Cambiando todas las tiles a blanco.", col, row);
        trapsActivated = true;

        // Cambiar todas las tiles a blanco
        for (int r = 0; r < mapMatrix.size(); ++r)
        {
            for (int c = 0; c < mapMatrix[0].size(); ++c)
            {
                if (tiles[r][c])
                {
                    tiles[r][c]->setColor(Color3B::WHITE);
                }
            }
        }
    }
}

// Función para reiniciar el juego
void LevelFour::resetGame()
{
    // Restaurar el mapa original
    mapMatrix = originalMapMatrix;

    // Restaurar las tiles a sus colores originales si la trampa estaba activada
    if (trapsActivated)
    {
        for (int r = 0; r < mapMatrix.size(); ++r)
        {
            for (int c = 0; c < mapMatrix[0].size(); ++c)
            {
                int tileType = mapMatrix[r][c];
                LayerColor* tile = tiles[r][c];
                if (tile)
                {
                    tile->setColor(originalTileColors[r][c]);
                }
            }
        }
        trapsActivated = false;
    }

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

    // Limpiar toques activos
    activeTouches.clear();
}

// Función para añadir botones de control
void LevelFour::addControlButtons()
{
    // Tamaño y posición de los botones
    float buttonWidth = 100;
    float buttonHeight = 50;
    float padding = 10;

    // Botón de movimiento hacia la izquierda
    leftButton = LayerColor::create(Color4B(211, 211, 211, 200), buttonWidth, buttonHeight);
    leftButton->setPosition(Vec2(origin.x + padding, origin.y + padding));
    this->addChild(leftButton, 2);

    // Botón de movimiento hacia la derecha
    rightButton = LayerColor::create(Color4B(211, 211, 211, 200), buttonWidth, buttonHeight);
    rightButton->setPosition(Vec2(origin.x + buttonWidth + 2 * padding, origin.y + padding));
    this->addChild(rightButton, 2);

    // Botón de salto
    jumpButton = LayerColor::create(Color4B(211, 211, 211, 200), buttonWidth, buttonHeight);
    jumpButton->setPosition(Vec2(visibleSize.width - buttonWidth - padding, origin.y + padding));
    this->addChild(jumpButton, 2);

    // Crear un único EventListenerTouchAllAtOnce
    auto touchListener = EventListenerTouchAllAtOnce::create();

    touchListener->onTouchesBegan = CC_CALLBACK_2(LevelFour::onTouchesBegan, this);
    touchListener->onTouchesMoved = CC_CALLBACK_2(LevelFour::onTouchesMoved, this);
    touchListener->onTouchesEnded = CC_CALLBACK_2(LevelFour::onTouchesEnded, this);
    touchListener->onTouchesCancelled = CC_CALLBACK_2(LevelFour::onTouchesCancelled, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

// Método para manejar problemas de carga
void LevelFour::problemLoading(const char* filename)
{
    printf("Error al cargar: %s\n", filename);
    printf("Dependiendo de cómo compilaste, podrías necesitar añadir 'Resources/' delante de los nombres de los archivos en LevelFour.cpp\n");
}

// Manejador de toques comenzados
void LevelFour::onTouchesBegan(const std::vector<Touch*>& touches, Event* event)
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

// Manejador de toques movidos
void LevelFour::onTouchesMoved(const std::vector<Touch*>& touches, Event* event)
{
    // Implementar si necesitas manejar movimientos de toque
}

// Manejador de toques terminados
void LevelFour::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
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

// Manejador de toques cancelados
void LevelFour::onTouchesCancelled(const std::vector<Touch*>& touches, Event* event)
{
    onTouchesEnded(touches, event);
}

// Función de actualización llamada cada frame
void LevelFour::update(float dt)
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
        // Cambiar a la siguiente escena, por ejemplo, NextLevel
        auto nextLevelScene = NextLevel::createScene();
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


    // Verificar si el personaje ha caído fuera del mapa
    if (sprite1->getPositionY() < mapOffsetY - spriteSize.height)
    {
        resetGame();
    }
}
