// NextLevel.h
#pragma once

#include "cocos2d.h"

class NextLevel : public cocos2d::Scene
{
public:
    // Método estático para crear la escena
    static cocos2d::Scene* createScene();

    // Inicializa la escena
    virtual bool init();

    // Macro para crear el método create()
    CREATE_FUNC(NextLevel);

private:
    // Método para manejar el toque en la pantalla
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);

    // Label para indicar al usuario
    cocos2d::Label* nextLevelLabel;

    // Tamaño visible y origen
    cocos2d::Size visibleSize;
    cocos2d::Vec2 origin;
};
