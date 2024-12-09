#pragma once

#include "cocos2d.h"
#include <map>
#include "GameDefinitions.h"
#include <vector>

USING_NS_CC;

class LevelFour : public Scene
{
public:
    // Crea y devuelve un objeto de escena
    static Scene* createScene();

    // Inicializa la escena
    virtual bool init();

    // Destructor para liberar recursos
    virtual ~LevelFour();

    // Macro para implementar el método "static create()"
    CREATE_FUNC(LevelFour);

    // Método de actualización llamado cada frame
    void update(float dt) override;

private:
    // Método de utilidad para manejar problemas de carga
    void problemLoading(const char* filename);

    // Métodos para mecánicas del juego
    void addControlButtons();
    void resetGame();
    void activateTrapAtPosition(int col, int row);
    // void addTrap(TrapType type, int col, int row); // Eliminado

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

    // Animaciones
    Animation* dashAnimation;
    Animation* jumpAnimation;

    // Variables de salto y gravedad
    float verticalVelocity;          // Velocidad vertical del personaje
    static constexpr float GRAVITY = -1000.0f;          // Gravedad aplicada al personaje
    static constexpr float JUMP_SPEED = 320.0f;        // Velocidad de salto del personaje

    // Tamaño de la pantalla y origen
    Size visibleSize;
    Vec2 origin;

    // Botones de control
    LayerColor* jumpButton;
    LayerColor* leftButton;
    LayerColor* rightButton;

    // Definición del mapa
    std::vector<std::vector<int>> mapMatrix;
    std::vector<std::vector<int>> originalMapMatrix; // Mapa original para restaurar

    // Desplazamiento del mapa
    float mapOffsetX;
    float mapOffsetY;

    // Posición inicial del personaje
    float startX;
    float startY;

    // Mapa para rastrear toques activos
    std::map<int, ControlButton> activeTouches;

    // Trampa: Al activar, todas las tiles se vuelven blancas
    std::vector<std::vector<Color3B>> originalTileColors; // Almacena los colores originales de las tiles
    bool trapsActivated = false; // Indica si la trampa está activada

    // Constantes
    static constexpr float TILE_SIZE = 20.0f;    // Tamaño de cada tile en píxeles
    static constexpr int DASH_ANIMATION_TAG = 100;    // Etiqueta para acciones de animación de dash

    // Vector para almacenar referencias a todas las tiles
    std::vector<std::vector<LayerColor*>> tiles; // Almacena las referencias a las tiles para acceder fácilmente

    // Función auxiliar para acceder a una tile específica
    LayerColor* getTileAt(int col, int row);
};
