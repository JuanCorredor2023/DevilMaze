#include "LevelManager.h"
#include "LevelOne.h"
#include "LevelTwo.h"
#include "LevelThree.h"
#include "LevelFour.h"
#include "LevelFive.h"
#include "LevelSix.h"
#include "StartScene.h"

cocos2d::Scene* LevelManager::getNextLevelScene()
{
    switch (currentLevel)
    {
    case 1:
        return LevelTwo::createScene();
    case 2:
        return LevelOne::createScene();
    case 3:
        return LevelFive::createScene();
    case 4:
        return LevelFour::createScene();
    case 5:
        return LevelThree::createScene();
    case 6:
        return LevelSix::createScene();
    default:
        return StartScene::createScene();
    }
}
