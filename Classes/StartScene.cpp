// StartScene.cpp
#include "StartScene.h"
#include "LevelManager.h"
#include "LevelOne.h" // Asegúrate de que estas escenas existan

USING_NS_CC;

Scene* StartScene::createScene()
{
    return StartScene::create();
}

bool StartScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    // Obtener el tamaño visible y el origen
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

    // Crear el sprite de fondo con la imagen DevilMaze.png
    auto backgroundSprite = Sprite::create("DevilMaze.png");
    if (backgroundSprite == nullptr)
    {
        CCLOG("Error al cargar la imagen 'DevilMaze.png'");
        return false;
    }
    else
    {
        // Ajustar el tamaño del sprite al tamaño de la pantalla
        float scaleX = visibleSize.width / backgroundSprite->getContentSize().width;
        float scaleY = visibleSize.height / backgroundSprite->getContentSize().height;
        backgroundSprite->setScale(scaleX, scaleY);
        backgroundSprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        this->addChild(backgroundSprite, 0);
    }

    // Crear el label que actúa como botón
    nextSceneLabel = Label::createWithTTF("TAP TO CONTINUE...", "fonts/GhostDied-own5a.ttf", 35);
    if (nextSceneLabel == nullptr)
    {
        CCLOG("Error al cargar la fuente 'arial.ttf'");
        return false;
    }
    else
    {
        nextSceneLabel->setPosition(Vec2(visibleSize.width / 2 + origin.x, origin.y + 80));
        nextSceneLabel->setTextColor(Color4B::WHITE);
        nextSceneLabel->enableOutline(Color4B::BLACK, 2);
        this->addChild(nextSceneLabel, 1);

        // Aplicar la animación de salto
        auto up = MoveBy::create(0.5f, Vec2(0, 10));
        auto down = up->reverse();
        auto sequence = Sequence::create(up, down, nullptr);
        nextSceneLabel->runAction(RepeatForever::create(sequence));
    }

    // Agregar un listener para detectar el toque en el label
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(StartScene::onTouchBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    return true;
}

bool StartScene::onTouchBegan(Touch* touch, Event* event)
{
    // Convertir la ubicación del toque a coordenadas del label
    Vec2 locationInNode = nextSceneLabel->convertToNodeSpace(touch->getLocation());
    Size s = nextSceneLabel->getContentSize();
    Rect rect = Rect(0, 0, s.width, s.height);

    // Verificar si el toque está dentro del label
    if (rect.containsPoint(locationInNode))
    {
        // Establecer el nivel actual a 1
        LevelManager::getInstance().setCurrentLevel(1);

        // Reemplazar la escena actual con la primera escena de nivel
        auto levelScene = LevelManager::getInstance().getNextLevelScene();
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, levelScene));
        return true;
    }
    return false;
}
