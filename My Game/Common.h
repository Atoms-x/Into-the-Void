/// \file Common.h
/// \brief Interface for the class CCommon.

#ifndef __L4RC_GAME_COMMON_H__
#define __L4RC_GAME_COMMON_H__

#include "Defines.h"
#include "TimedSpawn.h"
#include <queue>
#include <vector>

//forward declarations to make the compiler less stroppy

class CObjectManager;
class LSpriteRenderer;
class LParticleEngine2D;
class CTileManager;
class CPlayer;
class CHealthBar;
class CManaBar;
class CStaminaWheel;
class CHealthPotion;
class CManaPotion;
class CExplosion;
class CExplosion2;
class CMainMenuExit;
class CMainMenuBG;
class CMainMenuStart;
class CTimedSpawn;

/// \brief The common variables class.
///
/// CCommon is a singleton class that encapsulates things that are common to
/// different game components, including game state variables. Making it a
/// singleton class means that we can avoid passing its member variables
/// around as parameters, which makes the code minisculely faster, and more
/// importantly, makes the code more readable by reducing function clutter.

class CCommon {
protected:
  static LSpriteRenderer* m_pRenderer; ///< Pointer to renderer.
  static CObjectManager* m_pObjectManager; ///< Pointer to object manager.
  static LParticleEngine2D* m_pParticleEngine; ///< Pointer to particle engine.    

  static CTileManager* m_pTileManager; ///< Pointer to tile manager. 
  static bool m_bDrawAABBs; ///< Draw AABB flag.
  static bool m_bGodMode; ///< God mode flag.
  static bool m_bInteract;

  /// Store particle effects here if they need to be triggered at a specific time,
  /// independent of a game objects object's state.
  /// Min heap priority_queue pointer - juicy O(1) access time. Stores CTimedSpawn 
  /// objects in std vector. Comparator override of std greater. 
  static std::priority_queue< CTimedSpawn, std::vector<CTimedSpawn>, 
      std::greater<std::vector<CTimedSpawn>::value_type>> m_pTimedEvents;

  static Vector2 m_vWorldSize; ///< World height and width.
  static CPlayer* m_pPlayer; ///< Pointer to player character.
  static CHealthBar* m_pHealthBar; ///< Pointer to player Healthbar
  static CManaBar* m_pManaBar; ///< Pointer to player Manabar
  static CStaminaWheel* m_pStaminaWheel; ///< Pointer to player Stamina Wheel
  static CHealthPotion* m_pHealthPotion; ///< Pointer to HealthPotion
  static CManaPotion* m_pManaPotion; ///< Pointer to ManaPotion
  static CMainMenuExit* m_pExitButton; ///< Pointer to the Exit Button
  static CMainMenuStart* m_pStartButton; ///< Pointer to the Start Button
  static CMainMenuBG* m_pBackgroundObj; ///< Pointer to the background for the menu
  static CExplosion* m_pExplosion; ///< Pointer to an explosion
  static CExplosion2* m_pExplosion2; ///< Pointer to a different explosion
}; //CCommon

/// <summary>
/// Slime animation state enumerator. Used by CSlime and CSlimeBig
/// </summary>
enum class eSlimeState {
    IDLE,
    HOP,
    HURT,
    DEATH,
    NONE
};  //eSlimeState

/// <summary>
///  Rabite animation state enumerator. Used by CRabite
/// </summary>
enum class eRabiteState {
  IDLE,
  HOP,
  HURT,
  BITE,
  DEATH,
  NONE
}; //eRabiteState

/// <summary>
///  OakSeed animation state enumerator. Used by COakSeed
/// </summary>
enum class eOakSeedState {
  IDLE,
  WALK,
  HURT,
  DEATH,
  NONE
}; //eOakSeedState

/// <summary>
///  Player animation state enumerator. Used by CPlayer
/// </summary>
enum class ePlayerAnimationState {
  IDLE,
  RUNRIGHT,
  RUNLEFT,
  RUNDOWN,
  RUNUP,
  DASH,
  ATTACK,
  HURT,
  DEATH,
  NONE
}; //ePlayerAnimationState

enum class MapState { START, MIDDLE, END }; /// This is for managing what part of the map it is transferring to.

/// <summary>
///  Music state enumerator. Used to establish which song should be playing and give underlying logic for song loops
/// </summary>
enum class eMusicState {
  NONE,
  STORY,
  MAINMENU,
  GAMEOVER,
  BATTLE,
  EXPLORATION,
  LASTLEVEL,
  BOSS,
  BOSSDEFEATED,
  ENDING
}; //eMusicState

/// <summary>
/// Object type enumerator. Used by most objects as an identifier - mainly for collision response. 
/// </summary>
enum class eObjectType {
    UNDEFINED = 0,
    PLAYER,
    BULLET,
    SLIME,
    RABITE,
    OAKSEED,
    SWORD,
    HEALTHPOT,
    TURRET,
    MANAPOT,
    OBSTACLE,
    UI,
    LOCKBLOCK,
    EXPLOSION
}; //eObjectType

#endif //__L4RC_GAME_COMMON_H__