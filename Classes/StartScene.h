#pragma once

#include "cocos2d.h"

class StartScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    CREATE_FUNC(StartScene);

private:
    // Método para manejar el toque en el label
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);

    // Variables de tamaño de pantalla y origen
    cocos2d::Size visibleSize;
    cocos2d::Vec2 origin;

    // Label que actúa como botón
    cocos2d::Label* nextSceneLabel;
};
