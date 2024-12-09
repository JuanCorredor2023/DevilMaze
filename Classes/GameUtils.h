// GameUtils.h
#pragma once

#include "cocos2d.h"
#include <vector>

bool pointInPolygon(const cocos2d::Vec2& point, const std::vector<cocos2d::Vec2>& polygon);
bool polygonsIntersect(const std::vector<cocos2d::Vec2>& polyA, const std::vector<cocos2d::Vec2>& polyB);
