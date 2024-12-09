#include "NextLevel.h"
#include "LevelManager.h"

USING_NS_CC;

Scene* NextLevel::createScene()
{
    return NextLevel::create();
}

bool NextLevel::init()
{
    if (!Scene::init())
    {
        return false;
    }

    // Obtener el tamaño visible y el origen
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

    // Crear el sprite de fondo con la imagen NextLevel.png
    auto backgroundSprite = Sprite::create("NextLevel.png"); // Asegúrate de tener esta imagen
    if (backgroundSprite == nullptr)
    {
        CCLOG("Error al cargar la imagen 'NextLevel.png'");
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

    // Crear el label que indica pasar al siguiente nivel
    nextLevelLabel = Label::createWithTTF("TAP TO CONTINUE...", "fonts/GhostDied-own5a.ttf", 35);
    if (nextLevelLabel == nullptr)
    {
        CCLOG("Error al cargar la fuente 'arial.ttf'");
        return false;
    }
    else
    {
        nextLevelLabel->setPosition(Vec2(visibleSize.width / 2 + origin.x, origin.y + 80));
        nextLevelLabel->setTextColor(Color4B::WHITE);
        nextLevelLabel->enableOutline(Color4B::BLACK, 2);
        this->addChild(nextLevelLabel, 1);

        // Aplicar una animación simple
        auto up = MoveBy::create(0.5f, Vec2(0, 10));
        auto down = up->reverse();
        auto sequence = Sequence::create(up, down, nullptr);
        nextLevelLabel->runAction(RepeatForever::create(sequence));
    }

    // Agregar un listener para detectar el toque en el label
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(NextLevel::onTouchBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    return true;
}

bool NextLevel::onTouchBegan(Touch* touch, Event* event)
{
    // Convertir la ubicación del toque a coordenadas del label
    Vec2 locationInNode = nextLevelLabel->convertToNodeSpace(touch->getLocation());
    Size s = nextLevelLabel->getContentSize();
    Rect rect = Rect(0, 0, s.width, s.height);

    // Verificar si el toque está dentro del label
    if (rect.containsPoint(locationInNode))
    {
        // Avanzar al siguiente nivel
        LevelManager::getInstance().advanceLevel();

        // Obtener la siguiente escena de nivel desde LevelManager
        auto nextLevelScene = LevelManager::getInstance().getNextLevelScene();
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, nextLevelScene));
        return true;
    }
    return false;
}
