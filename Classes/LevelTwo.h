// LevelTwo.h
#pragma once

#include "cocos2d.h"
#include <map>
#include "GameDefinitions.h"
#include <vector>

USING_NS_CC;



class LevelTwo : public Scene
{
public:
    // Crea y devuelve un objeto de escena
    static Scene* createScene();

    // Inicializa la escena
    virtual bool init();

    // Destructor para liberar recursos
    virtual ~LevelTwo();

    // Macro para implementar el método "static create()"
    CREATE_FUNC(LevelTwo);

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

    // AnimaciTwos
    Animation* dashAnimation;
    Animation* jumpAnimation;

    // Variables de salto y gravedad
    float verticalVelocity;          // Velocidad vertical del personaje
    static constexpr float GRAVITY = -1000.0f;          // Gravedad aplicada al personaje
    static constexpr float JUMP_SPEED = 320.0f;        // Velocidad de salto del personaje

    // Tamaño de la pantalla y origen
    Size visibleSize;
    Vec2 origin;

    // BotTwos de control
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

    // Trampas y sus posiciTwos
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
        { Vec2(5,8), Vec2(5,11) },
        { Vec2(10,8), Vec2(10,11) },
        { Vec2(12,8), Vec2(12,11) },
        { Vec2(18,8), Vec2(18,11)}
        // Agrega más pares activador-trampa aquí
    };
    
    std::map<Vec2, std::vector<Vec2>, Vec2Compare> floorActivatorMap = {
        { Vec2(3,8), { Vec2(3,9), Vec2(3,10) ,Vec2(4,9), Vec2(4,10) } },
        { Vec2(3,7), { Vec2(3,9), Vec2(3,10), Vec2(4,9), Vec2(4,10) } },
        { Vec2(3,6), { Vec2(3,9), Vec2(3,10), Vec2(4,9), Vec2(4,10)} },
        
        { Vec2(7,8), { Vec2(7,9), Vec2(7,10), Vec2(8,9), Vec2(8,10) } },
        { Vec2(7,7), { Vec2(7,9), Vec2(7,10), Vec2(8,9), Vec2(8,10)  } },
        { Vec2(7,6), { Vec2(7,9), Vec2(7,10), Vec2(8,9), Vec2(8,10)  } },
        
        { Vec2(12,8), { Vec2(12,9), Vec2(12,10), Vec2(13,9), Vec2(13,10) } },
        { Vec2(12,7), { Vec2(12,9), Vec2(12,10), Vec2(13,9), Vec2(13,10)} },
        { Vec2(12,6), { Vec2(12,9), Vec2(12,10), Vec2(13,9), Vec2(13,10) } },
        
        { Vec2(15,8), { Vec2(16,9), Vec2(16,10), Vec2(15,9), Vec2(15,10) } },
        { Vec2(15,7), { Vec2(16,9), Vec2(16,10), Vec2(15,9), Vec2(15,10) } },
        { Vec2(15,6), { Vec2(16,9), Vec2(16,10), Vec2(15,9), Vec2(15,10) } },
        
        { Vec2(19,8), { Vec2(19,9), Vec2(19,10), Vec2(20,9), Vec2(20,10) } },
        { Vec2(19,7), { Vec2(19,9), Vec2(19,10), Vec2(20,9), Vec2(20,10) } },
        { Vec2(19,6), { Vec2(19,9), Vec2(19,10), Vec2(20,9), Vec2(20,10) } },
        
        { Vec2(20,8), { Vec2(19,9), Vec2(19,10), Vec2(20,9), Vec2(20,10) } },
        { Vec2(20,7), { Vec2(19,9), Vec2(19,10), Vec2(20,9), Vec2(20,10) } },
        { Vec2(20,6), { Vec2(19,9), Vec2(19,10), Vec2(20,9), Vec2(20,10) } }
        // Agrega más pares activador-piso aquí
    };

    

    // Constantes
    static constexpr float TILE_SIZE = 20.0f;    // Tamaño de cada tile en píxeles
    static constexpr int DASH_ANIMATION_TAG = 100;    // Etiqueta para acciTwos de animación de dash
};
