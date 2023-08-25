/// \file Game.h
/// \brief Interface for the game class CGame.

#ifndef __L4RC_GAME_GAME_H__
#define __L4RC_GAME_GAME_H__

#include "Component.h"
#include "Common.h"
#include "ObjectManager.h"
#include "Settings.h"
#include "Player.h"

#include <string>
#include <vector>

/// \brief The game class.
///
/// The game class is the object-oriented implementation of the game. This class
/// must contain the following public member functions. `Initialize()` does
/// initialization and will be run exactly once at the start of the game.
/// `ProcessFrame()` will be called once per frame to create and render the
/// next animation frame. `Release()` will be called at game exit but before
/// any destructors are run.

class CGame: 
  public LComponent, 
  public LSettings,
  public CCommon{ 

  private:
    bool m_bDrawFrameRate = false; ///< Draw the frame rate.
    eGameState m_eGameState = eGameState::Playing; ///< Game state.
    int m_nCurrentLevelIndex = 0;
    int m_nPortDirection = 0;
    bool t = false;
    bool m_bGameOverScreen = false;
    bool m_bFadeInAnimated = false;
    bool m_bMainMenu = false;
    bool m_bAtEndScreen = false;
    bool m_bTitleSequence = false;
    bool m_bInMenu = false; ///< If the player is in a menu
    Vector3 m_vInitCameraPos; ///< Initial camera position
    std::vector<char *> maps; ///< Maps to be used in the game.
    int Clamp(int, int, int);
    bool oneTimeReset = false;
    std::vector<char*> mapmasks; 
    std::vector<char*> flavor;
    std::vector<char*> masks;
    void Restart();
    void PlayTitleSequence();
    void LoadImages(); ///< Load images.
    void LoadSounds(); ///< Load sounds.
    void CreateMenu(); ///< Load menu
    void BeginGame(); ///< Begin playing the game.
    void KeyboardHandler(); ///< The keyboard handler.
    //void ControllerHandler(); ///< The controller handler.
    void RenderFrame(); ///< Render an animation frame.
    void DrawFrameRateText(); ///< Draw frame rate text to screen.
    void DrawGodModeText(); ///< Draw god mode text if in god mode.
    void CreateObjects(); ///< Create game objects.
    void FollowCamera(); ///< Make camera follow player character.
    void ProcessGameState(); ///< Process game state.
    void SpawnMany(eSprite type, std::vector<Vector2> positions); ///< Spawn any amount of the type at given positions
    void UI(); ///< Update UI on screen
    void GameOver();
    void FinishGame();
    Vector2 Vec3toVec2(const Vector3);
    //int Timer(); ///< Timer
    //int ShakeTimer(); ///< Timer

    eMusicState state{ eMusicState::NONE};

  public:
    ~CGame(); ///< Destructor.
    void Goto(int); ///< Goto the level at the index;
    void Initialize(); ///< Initialize the game.
    void Pause(); ///< Pause the game.
    void ProcessFrame(); ///< Process an animation frame.
    void Release(); ///< Release the renderer.
}; //CGame

#endif //__L4RC_GAME_GAME_H__
