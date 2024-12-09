// GameDefinitions.h
#pragma once

#include "cocos2d.h"
#include <map>
#include <vector>

// Usar el espacio de nombres de Cocos2d para facilitar el acceso a clases como Vec2
USING_NS_CC;

// Enumeración para los botones de control
enum class ControlButton
{
    LEFT,
    RIGHT,
    JUMP
};

// Enumeración para los tipos de trampas
enum class TrapType
{
    SPIKE,
    FALLING_COLUMN,
    OPENING_FLOOR
};

// Comparador personalizado para Vec2 (necesario si se usa en std::map)
struct Vec2Compare {
    bool operator()(const Vec2& a, const Vec2& b) const {
        if (a.x < b.x) return true;
        if (a.x > b.x) return false;
        return a.y < b.y;
    }
};
