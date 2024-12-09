#include "AppDelegate.h"
#include "StartScene.h" // Incluye la nueva escena de inicio

#define USE_AUDIO_ENGINE 1
// #define USE_SIMPLE_AUDIO_ENGINE 1 // Desactivado para usar solo AudioEngine

#if USE_AUDIO_ENGINE && USE_SIMPLE_AUDIO_ENGINE
#error "Don't use AudioEngine and SimpleAudioEngine at the same time. Please just select one in your game!"
#endif

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#elif USE_SIMPLE_AUDIO_ENGINE
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
#endif

USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

AppDelegate::AppDelegate() {}

AppDelegate::~AppDelegate() {
#if USE_AUDIO_ENGINE
    AudioEngine::end();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::end();
#endif
}

void AppDelegate::initGLContextAttrs() {
    // Atributos del contexto OpenGL
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};
    GLView::setGLContextAttrs(glContextAttrs);
}

static int register_all_packages() {
    return 0; // Flag para el administrador de paquetes
}

bool AppDelegate::applicationDidFinishLaunching() {
    // Inicializar el director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview) {
        glview = GLViewImpl::create("DevilMaze");
        director->setOpenGLView(glview);
    }

    // Mostrar FPS
    director->setDisplayStats(true);

    // Establecer FPS
    director->setAnimationInterval(1.0f / 60);

    // Configurar resolución de diseño
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
    auto frameSize = glview->getFrameSize();
    if (frameSize.height > mediumResolutionSize.height) {
        director->setContentScaleFactor(MIN(largeResolutionSize.height / designResolutionSize.height,
                                            largeResolutionSize.width / designResolutionSize.width));
    } else if (frameSize.height > smallResolutionSize.height) {
        director->setContentScaleFactor(MIN(mediumResolutionSize.height / designResolutionSize.height,
                                            mediumResolutionSize.width / designResolutionSize.width));
    } else {
        director->setContentScaleFactor(MIN(smallResolutionSize.height / designResolutionSize.height,
                                            smallResolutionSize.width / designResolutionSize.width));
    }

    register_all_packages();

    // Reproducir música de fondo en bucle
    int musicId = AudioEngine::play2d("background_music.mp3", true);
    if (musicId == AudioEngine::INVALID_AUDIO_ID) {
        CCLOG("Error: No se pudo cargar la música de fondo.");
    } else {
        CCLOG("Música de fondo reproducida con éxito.");
    }

    // Crear y ejecutar la escena inicial
    auto scene = StartScene::createScene();
    director->runWithScene(scene);

    return true;
}

void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    SimpleAudioEngine::getInstance()->pauseAllEffects();
#endif
}

void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    SimpleAudioEngine::getInstance()->resumeAllEffects();
#endif
}
