/// \file GameDefines.h
/// \brief Game specific defines.

#ifndef __L4RC_GAME_GAMEDEFINES_H__
#define __L4RC_GAME_GAMEDEFINES_H__

#include "Defines.h"

/// \brief Sprite enumerated type.
///
/// An enumerated type for the sprites, which will be cast to an unsigned
/// integer and used for the index of the corresponding texture in graphics
/// memory. `Size` must be last.

enum class eSprite : UINT {
  Background, Bullet, Bullet2, Player, Smoke, Spark, Ant,
  SpriteSheet, AntSpriteSheet, PlayerSpriteSheet, PlayerIdleDownSpriteSheet, PlayerIdleRightSpriteSheet,
  PlayerIdleLeftSpriteSheet, PlayerIdleUpSpriteSheet, PlayerIdleDown, PlayerIdleRight, PlayerIdleLeft, PlayerIdleUp,
  PlayerRunDownSheet, PlayerRunRightSheet, PlayerRunLeftSheet, PlayerRunUpSheet, PlayerRunDown, PlayerRunRight,
  PlayerRunLeft, PlayerRunUp, PlayerAttackRightSheet, PlayerAttackLeftSheet, PlayerAttackDownSheet, PlayerAttackUpSheet,
  PlayerAttackRight, PlayerAttackLeft, PlayerAttackDown, PlayerAttackUp, Tile, Line, Slime, SlimeKing, SlimeBig, SlimeIdle,
  SlimeHurt, SlimeHop, SlimeDeath, SlimeTent, SlimeSplitSheet, SlimeSplit, HealthBarSheet, HealthBarEmpty, HealthBar1, HealthBar2, HealthBar3, HealthBar4, HealthBar5, HealthBarFull,
  ManaPotion, HealthPotion, ManaBarEmpty, ManaBar1, ManaBar2, ManaBar3, ManaBar4, ManaBar5, ManaBarFull, PlayerHitDownSheet, PlayerHitRightSheet,
  PlayerHitLeftSheet, PlayerHitUpSheet, PlayerHitDown, PlayerHitRight, PlayerHitLeft, PlayerHitUp, PlayerDeathDownSheet, PlayerDeathUpSheet,
  PlayerDeathRightSheet, PlayerDeathLeftSheet, PlayerDeathDown, PlayerDeathUp, PlayerDeathRight, PlayerDeathLeft, PlayerDashDown, PlayerDashRight,
  PlayerDashLeft, PlayerDashUp, BG, StartSheet, ExitSheet, Start, Exit, Cross, Mana, Circle, CircleNoBorder, SmoothCircle, ManaParticlesSheet, ManaParticles,
  Spikes, SlimeParticleSheet, SlimeParticles, KeySheet, Key, LockTestSheet, LockTest, MineSheet, GoldUI, GoldItems, FlowerUI, FlowerItems,
  SilverUI, SilverItems, GemUI, GemItems, GoldRock, PurpleRock, SilverRock, BrownRock, BlueRock, GreyRock, RedRock, GreenRock, BigRocks,
  BlueShard, GreyShard, RedShard, GreenShard, Grass1, Grass2, Grass3, Grass4, TallGrass1, TallGrass2, TallGrass3, TallGrass4, Shrooms,
  PlayerStaticDashDown, PlayerStaticDashUp, PlayerStaticDashLeft, PlayerStaticDashRight, PlayerDashDownSheet, PlayerDashUpSheet, PlayerDashRightSheet, PlayerDashLeftSheet,
  StaminaWheelSheet, StaminaWheelFull, StaminaWheel2, StaminaWheel1, StaminaWheelEmpty, Swing, TestSwing, PortalSheet, FireballSheet, Fireball, ExplosionSheet, Explosion, NextPortal, PrevPortal,
  TurretSheet, Turret, Explosion2Sheet, Explosion2, Explosion3Sheet, Explosion3, Rabite,
  RabiteSheet, RabiteHopUpSheet, RabiteHopDownSheet, RabiteHopLeftSheet, RabiteHopRightSheet, RabiteBiteUpSheet, RabiteBiteDownSheet, RabiteBiteLeftSheet, RabiteBiteRightSheet, 
  RabiteIdleUpSheet, RabiteIdleDownSheet, RabiteIdleLeftSheet, RabiteIdleRightSheet, RabiteHurtUpSheet, RabiteHurtDownSheet, RabiteHurtLeftSheet, RabiteHurtRightSheet, RabiteDeathSheet,
  RabiteHopUp, RabiteHopDown, RabiteHopLeft, RabiteHopRight, RabiteBiteUp, RabiteBiteDown, RabiteBiteLeft, RabiteBiteRight, RabiteIdleUp, RabiteIdleDown, RabiteIdleLeft,
  RabiteIdleRight, RabiteHurtUp, RabiteHurtDown, RabiteHurtLeft, RabiteHurtRight, RabiteDeath,
  OakSeedSheet, OakSeedWalkUpSheet, OakSeedWalkLeftSheet, OakSeedWalkRightSheet, OakSeedWalkDownSheet, OakSeedIdleUpSheet, OakSeedIdleLeftSheet, OakSeedIdleRightSheet,
  OakSeedIdleDownSheet, OakSeedDeathSheet, OakSeed, OakSeedWalkUp, OakSeedWalkLeft, OakSeedWalkRight, OakSeedWalkDown, OakSeedIdleUp, OakSeedIdleLeft, OakSeedIdleRight,
  OakSeedIdleDown, OakSeedDeath, OakSeedHurtUpSheet, OakSeedHurtLeftSheet, OakSeedHurtRightSheet, OakSeedHurtDownSheet,
  OakSeedHurtUp, OakSeedHurtLeft, OakSeedHurtRight, OakSeedHurtDown, TurretFireballSheet, TurretFireball, Explosion4Sheet, Explosion4,
  KingSlime, KingIdle, KingHop, KingHurt, KingDeath,
  MainLev,	/*it's beautiful*/ Floor, Floor1,
  WallOut1, Wall1, WallEdgeY1, WallCorner1, WallBotPoke1, WallEdgeX1,
  WallOut2, Wall2, WallEdgeY2, WallCorner2, WallBotPoke2, WallEdgeX2,
  WallOut3, Wall3, WallEdgeY3, WallCorner3, WallBotPoke3, WallEdgeX3,
  WallOut4, Wall4, WallEdgeY4, WallCorner4, WallBotPoke4, WallEdgeX4,
  WallOut5, Wall5, WallEdgeY5, WallCorner5, WallBotPoke5, WallEdgeX5,
  WallOut6, Wall6, WallEdgeY6, WallCorner6, WallBotPoke6, WallEdgeX6,
  WallOut7, Wall7, WallEdgeY7, WallCorner7, WallBotPoke7, WallEdgeX7,
  WallOut8, Wall8, WallEdgeY8, WallCorner8, WallBotPoke8, WallEdgeX8,
  WallOut9, Wall9, WallEdgeY9, WallCorner9, WallBotPoke9, WallEdgeX9,
  FadeOutSheet, FadeInSheet, FadeOut, FadeIn,
  GameOver, EndingScreen, TitlecardSheet, Titlecard,
  Rails, RailStop, Fern, BigFern, Lantern, Support1, Support2, GreyGravelSheet, GreyGravel,
  //BrownCobble, MarshGrass, RedFloor, GreenCobble, BrownGrass, BrownFloor, BlueFloor, BrightGrass, YellowFloor, GreenGrass,
  BrownCobbleSheet, BrownCobbleFloor, YellowFloorSheet, YellowGroundFloor, BrownFloorSheet, GreenCobbleSheet, GreenCobbleFloor,
  BrownGroundFloor, MarshGrassSheet, MarshGrassFloor, GrassFloorSheet, GrassGroundFloor, GrassFloorSheet2, GrassGroundFloor2, 
  PitSheet, Pit, BrownGravelSheet, BrownGravel, 
  Size  //MUST BE LAST
}; //eSprite

/// \brief Sound enumerated type.
///
/// An enumerated type for the sounds, which will be cast to an unsigned
/// integer and used for the index of the corresponding sample. `Size` must 
/// be last.

enum class eSound : UINT {
  Start, Boom, Clang, Grunt, Gun, Ricochet, Ow, Swing, Impact, Thump, BrassMusic, Achievement, Acquire, Drums, SoundTrack, 
  MainMenu, Exploration, Battle, Boss, BossDefeated, EndingCredits, GameOver, LastLevel, Story, Health, Magic,
  Size  //MUST BE LAST
}; //eSound


/// \brief Game state enumerated type.
///
/// An enumerated type for the game state, which can be either playing or
/// waiting a short interval for the level to end after winning or losing to
/// let sounds and particle effects play to the end without being cut short.

enum class eGameState {
  Playing, Waiting, Menu
}; //eGameState

#endif //__L4RC_GAME_GAMEDEFINES_H__