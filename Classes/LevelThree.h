// LevelThree.h
#pragma once

#include "cocos2d.h"
#include <map>
#include "GameDefinitions.h"
#include <vector>

USING_NS_CC;



class LevelThree : public Scene
{
public:
    // Crea y devuelve un objeto de escena
    static Scene* createScene();

    // Inicializa la escena
    virtual bool init();

    // Destructor para liberar recursos
    virtual ~LevelThree();

    // Macro para implementar el método "static create()"
    CREATE_FUNC(LevelThree);

    // Método de actualización llamado cada frame
    void update(float dt) override;

private:
    // Método de utilidad para manejar problemas de carga
    void problemLoading(const char* filename);

    // Métodos para mecánicas del juego
    void addControlButtons();
    void resetGame();
    void activateTrapAtPosition(int col, int row);
    void addTrap(TrapType type, int col, int row);
    void addTrapTrigger(int col, int row);
    void activateSpikeAt(const Vec2& spikePos);
    void activateOpeningFloorAt(const Vec2& floorPos);

    // Manejadores de eventos de toque
    void onTouchesBegan(const std::vector<Touch*>& touches, Event* event);
    void onTouchesMoved(const std::vector<Touch*>& touches, Event* event);
    void onTouchesEnded(const std::vector<Touch*>& touches, Event* event);
    void onTouchesCancelled(const std::vector<Touch*>& touches, Event* event);

    // Variables de movimiento
    bool isMovingLeft;    // Indica si el personaje se está moviendo hacia la izquierda
    bool isMovingRight;   // Indica si el personaje se está moviendo hacia la derecha
    bool isJumping;       // Indica si el personaje está en el aire
    float moveDirection;  // -1 para izquierda, 1 para derecha, 0 para no moverse
    Sprite* sprite1;

    // AnimaciThrees
    Animation* dashAnimation;
    Animation* jumpAnimation;

    // Variables de salto y gravedad
    float verticalVelocity;          // Velocidad vertical del personaje
    static constexpr float GRAVITY = -1000.0f;          // Gravedad aplicada al personaje
    static constexpr float JUMP_SPEED = 1000.0f;        // Velocidad de salto del personaje

    // Tamaño de la pantalla y origen
    Size visibleSize;
    Vec2 origin;

    // BotThrees de control
    LayerColor* jumpButton;
    LayerColor* leftButton;
    LayerColor* rightButton;

    // Definición del mapa
    std::vector<std::vector<int>> mapMatrix;

    // Desplazamiento del mapa
    float mapOffsetX;
    float mapOffsetY;

    // Posición inicial del personaje
    float startX;
    float startY;

    // Trampas y sus posiciThrees
    std::vector<DrawNode*> spikes;
    std::vector<LayerColor*> fallingColumns;
    std::vector<LayerColor*> openingFloors;

    std::vector<Vec2> spikePositions;
    std::vector<Vec2> columnPositions;
    std::vector<Vec2> floorPositions;
    std::vector<Vec2> trapTriggers;

    // Mapa para rastrear toques activos
    std::map<int, ControlButton> activeTouches;

    // Mapa que relaciona activadores con trampas (púas)
    std::map<Vec2, Vec2, Vec2Compare> activatorToSpikeMap = {
        { Vec2(4,8), Vec2(4,11) },
        { Vec2(3,8), Vec2(3,11) }
        // Agrega más pares activador-trampa aquí
    };
    
    std::map<Vec2, std::vector<Vec2>, Vec2Compare> floorActivatorMap = {
        { Vec2(8,7), { Vec2(8,9)} },
        { Vec2(8,6), { Vec2(8,9)} },
        { Vec2(8,5), { Vec2(8,9)} },
        
        { Vec2(14,7), { Vec2(1,9)} },
        { Vec2(14,6), { Vec2(14,9)} },
        { Vec2(14,5), { Vec2(14,9)} }
        
        // Agrega más pares activador-piso aquí
    };

    

    // Constantes
    static constexpr float TILE_SIZE = 20.0f;    // Tamaño de cada tile en píxeles
    static constexpr int DASH_ANIMATION_TAG = 100;    // Etiqueta para acciThrees de animación de dash
};
