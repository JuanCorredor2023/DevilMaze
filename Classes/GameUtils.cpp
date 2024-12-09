// GameUtils.cpp
#include "GameUtils.h"

using namespace cocos2d;

// Función para verificar si un punto está dentro de un polígono convexo
bool pointInPolygon(const Vec2& point, const std::vector<Vec2>& polygon) {
    int n = static_cast<int>(polygon.size());
    for (int i = 0; i < n; i++) {
        Vec2 a = polygon[i];
        Vec2 b = polygon[(i + 1) % n];
        Vec2 edge = b - a;
        Vec2 vp = point - a;
        float cross = edge.cross(vp);
        if (cross < 0) {
            return false;
        }
    }
    return true;
}

// Función para verificar si dos polígonos convexos se intersectan usando SAT
bool polygonsIntersect(const std::vector<Vec2>& polyA, const std::vector<Vec2>& polyB) {
    // Función interna para proyectar un polígono en un eje
    auto project = [](const std::vector<Vec2>& poly, const Vec2& axis) -> std::pair<float, float> {
        float minProj = poly[0].dot(axis);
        float maxProj = minProj;
        for (size_t i = 1; i < poly.size(); i++) {
            float proj = poly[i].dot(axis);
            if (proj < minProj) minProj = proj;
            if (proj > maxProj) maxProj = proj;
        }
        return {minProj, maxProj};
    };

    // Función interna para probar un eje
    auto testAxis = [&](const std::vector<Vec2>& pA, const std::vector<Vec2>& pB, const Vec2& axis) -> bool {
        auto pAProj = project(pA, axis);
        auto pBProj = project(pB, axis);
        // Si no se superponen en este eje, no hay colisión
        if (pAProj.second < pBProj.first || pBProj.second < pAProj.first) {
            return false;
        }
        return true;
    };

    // Obtener normales de los bordes del polígono A
    for (size_t i = 0; i < polyA.size(); i++) {
        Vec2 p1 = polyA[i];
        Vec2 p2 = polyA[(i + 1) % polyA.size()];
        Vec2 edge = p2 - p1;
        Vec2 axis = Vec2(-edge.y, edge.x).getNormalized();
        if (!testAxis(polyA, polyB, axis)) return false;
    }

    // Obtener normales de los bordes del polígono B
    for (size_t i = 0; i < polyB.size(); i++) {
        Vec2 p1 = polyB[i];
        Vec2 p2 = polyB[(i + 1) % polyB.size()];
        Vec2 edge = p2 - p1;
        Vec2 axis = Vec2(-edge.y, edge.x).getNormalized();
        if (!testAxis(polyA, polyB, axis)) return false;
    }

    return true;
}
