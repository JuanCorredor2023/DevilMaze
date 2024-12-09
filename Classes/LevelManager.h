#pragma once

#include "cocos2d.h"

// Forward declarations
class LevelOne;
class LevelTwo;
class LevelThree;
class LevelFour;
class LevelFive;
class LevelSix;
class StartScene;

class LevelManager
{
public:
    // Singleton para acceder globalmente
    static LevelManager& getInstance()
    {
        static LevelManager instance;
        return instance;
    }

    // Obtener y establecer el nivel actual
    int getCurrentLevel() const { return currentLevel; }
    void setCurrentLevel(int level) { currentLevel = level; }

    // Método para avanzar al siguiente nivel
    void advanceLevel() { currentLevel++; }

    // Método para obtener la siguiente escena de nivel
    cocos2d::Scene* getNextLevelScene();

private:
    // Constructor privado para el patrón singleton
    LevelManager() : currentLevel(1) {}
    // Evitar la copia
    LevelManager(const LevelManager&) = delete;
    LevelManager& operator=(const LevelManager&) = delete;

    int currentLevel;
};
