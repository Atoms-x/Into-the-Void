/// \file Game.cpp
/// \brief Code for the game class CGame.

#include "Game.h"

#include "GameDefines.h"
#include "SpriteRenderer.h"
#include "ComponentIncludes.h"
#include "ParticleEngine.h"
#include "Player.h" // ADDED FOR SPRITE ANIMATION
#include "Slime.h"
#include "TileManager.h"
#include "HealthBar.h"
#include "ManaBar.h"
#include "StaminaWheel.h"
#include "MainMenuExit.h"
#include "MainMenuStart.h"
#include "MainMenuBG.h"
#include "shellapi.h"
#include "Obstacle.h"
#include "ObjectManager.h"
#include "NextMapPorter.h"
#include "FadeInObject.h"
#include "FadeOutObject.h"
#include "GameOverObject.h"

/// Delete the particle engine and the object manager. The renderer needs to
/// be deleted before this destructor runs so it will be done elsewhere.

CGame::~CGame() {
  delete m_pParticleEngine;
  delete m_pObjectManager;
  delete m_pTileManager;
} //destructor

/// <summary>
/// Function called when the player dies. Spawns the graphic and lets them
/// restart.
/// </summary>

void CGame::GameOver() {
    Vector2 cPos = Vec3toVec2(m_pRenderer->GetCameraPos());
    //m_pObjectManager->clear();
    m_pObjectManager->create(eSprite::GameOver, cPos);
    if (state != eMusicState::GAMEOVER) {
      m_pAudio->stop();
      m_pAudio->loop(eSound::GameOver);
      state = eMusicState::GAMEOVER;
    }
    m_bGameOverScreen = true;
    printf("GameOverFinished func\n");
}

/// <summary>
/// This function handles ending the game and then restarting it.
/// </summary>

void CGame::FinishGame() {
    m_pObjectManager->create(eSprite::EndingScreen, Vec3toVec2(m_pRenderer->GetCameraPos()));
    m_bAtEndScreen = true;
    oneTimeReset = true;
}

/// <summary>
/// Changes a Vector3 into a compatible Vector2
/// </summary>
/// <param name="vec"></param>
/// <returns></returns>

Vector2 CGame::Vec3toVec2(const Vector3 vec)
{
    return Vector2(vec.x, vec.y);
}

/// <summary>
/// Changes the current level index by the passed value and enters that map
/// </summary>
/// <param name="level_index"></param>

void CGame::Goto(int level_index) {
    int _t = m_nCurrentLevelIndex;
    printf("In Goto\n");
    m_nCurrentLevelIndex = Clamp(m_nCurrentLevelIndex + level_index, maps.size() - 1, 0);
    if (_t == m_nCurrentLevelIndex) {
        // this means they're at the last level
        FinishGame();
        return;
    } else if (_t < m_nCurrentLevelIndex) {
        // went to the next level
        m_nPortDirection = 1;
    }
    else {
        // went to the previous level
        m_nPortDirection = -1;
    }
    BeginGame();

}

/// Create the renderer, the object manager, and the particle engine, load
/// images and sounds, and begin the game.
/// 
/// LEVEL FORMAT
/// To keep things consistent I'm naming each level "level_XX" and ask anyone else
/// to do the same

void CGame::Initialize() {
  // LOAD MAPS
  maps.push_back("Media\\Maps\\level_01.txt");
  mapmasks.push_back("Media\\Maps\\level_01MapMask.txt");
  flavor.push_back("Media\\Maps\\level_01Flavor.txt");
  masks.push_back("Media\\Maps\\level_01FlavorMask.txt");

  maps.push_back("Media\\Maps\\levelmaze.txt");
  mapmasks.push_back("Media\\Maps\\levelmazeMapMask.txt");
  flavor.push_back("Media\\Maps\\levelmazeFlavor.txt");
  masks.push_back("Media\\Maps\\levelmazeFlavorMask.txt");

  maps.push_back("Media\\Maps\\lastlevel.txt");
  mapmasks.push_back("Media\\Maps\\lastlevelMapMask.txt");
  flavor.push_back("Media\\Maps\\lastlevelFlavor.txt");
  masks.push_back("Media\\Maps\\lastlevelFlavorMask.txt");

  maps.push_back("Media\\Maps\\boss.txt");
  mapmasks.push_back("Media\\Maps\\bossMapMask.txt");
  flavor.push_back("Media\\Maps\\bossFlavor.txt");
  masks.push_back("Media\\Maps\\bossFlavorMask.txt");

  /*maps.push_back("Media\\Maps\\level_02.txt");
  mapmasks.push_back("Media\\Maps\\level_02MapMask.txt");
  flavor.push_back("Media\\Maps\\level_02Flavor.txt");
  masks.push_back("Media\\Maps\\level_02FlavorMask.txt");

  maps.push_back("Media\\Maps\\level_03.txt");
  mapmasks.push_back("Media\\Maps\\level_03MapMask.txt");
  flavor.push_back("Media\\Maps\\level_03Flavor.txt");
  masks.push_back("Media\\Maps\\level_03FlavorMask.txt");

  maps.push_back("Media\\Maps\\level_04.txt");
  mapmasks.push_back("Media\\Maps\\level_04MapMask.txt");
  flavor.push_back("Media\\Maps\\level_04Flavor.txt");
  masks.push_back("Media\\Maps\\level_04FlavorMask.txt");

  maps.push_back("Media\\Maps\\test.txt");
  maps.push_back("Media\\Maps\\small");
  maps.push_back("Media\\Maps\\map");*/
  // END LOAD MAPS
  m_pRenderer = new LSpriteRenderer(eSpriteMode::Batched2D);
  m_pRenderer->Initialize(eSprite::Size);
  m_vInitCameraPos = m_pRenderer->GetCameraPos();
  LoadImages(); //load images from xml file list

  m_pTileManager = new CTileManager((size_t)m_pRenderer->GetHeight(eSprite::WallOut1));
  m_pObjectManager = new CObjectManager; //set up the object manager 
  LoadSounds(); //load the sounds for this game

  m_pParticleEngine = new LParticleEngine2D(m_pRenderer);
  m_bInMenu = true;
  m_pAudio->loop(eSound::Story);
  state = eMusicState::STORY;
  PlayTitleSequence();

} //Initialize

void CGame::Pause() {
    m_bInMenu = !m_bInMenu;
}

/// <summary>
/// This function guarantees that a certain value a
/// is kept within the bounds of high and low.
/// example :
/// int x, y, z;
/// x = 3;
/// y = 4;
/// z = 2;
/// x = Clamp(x + 3, y, z);
/// // after this, x is equal to 4
/// </summary>
/// <param name="a"></param>
/// <param name="high"></param>
/// <param name="low"></param>
/// <returns></returns>

int CGame::Clamp(int a, int high, int low)
{
    if (a > high) {
        return high;
    }
    else if (a < low) {
        return low;
    }
    else {
        return a;
    }
}

/// Load the specific images needed for this game. This is where `eSprite`
/// values from `GameDefines.h` get tied to the names of sprite tags in
/// `gamesettings.xml`. Those sprite tags contain the name of the corresponding
/// image file. If the image tag or the image file are missing, then the game
/// should abort from deeper in the Engine code leaving you with an error
/// message in a dialog box.


void CGame::LoadImages() {
  m_pRenderer->BeginResourceUpload();

  m_pRenderer->Load(eSprite::SpriteSheet, "spritesheet"); //must be loaded before its sprites
  //m_pRenderer->Load(eSprite::Tile, "tile");
  //m_pRenderer->Load(eSprite::TurretSheet, "turret");
  m_pRenderer->Load(eSprite::Spikes, "spikes");
  m_pRenderer->Load(eSprite::Swing, "swing");
  m_pRenderer->Load(eSprite::TestSwing, "testswing");

  // START Potions

  m_pRenderer->Load(eSprite::HealthPotion, "healthpotion");
  m_pRenderer->Load(eSprite::ManaPotion, "manapotion");

  // END Potions

  // START Ending Screen

  m_pRenderer->Load(eSprite::EndingScreen, "endingscreen");

  // END Ending Screen

  // START Titlecard

  m_pRenderer->Load(eSprite::TitlecardSheet, "titlecardsheet");
  m_pRenderer->Load(eSprite::Titlecard, "titlecard");

  // END Titlecard

  // START Gameover

  m_pRenderer->Load(eSprite::GameOver, "gameover");

  // END Gameover

  // START Fadein/Fadeout

  m_pRenderer->Load(eSprite::FadeInSheet, "fadeinsheet");
  m_pRenderer->Load(eSprite::FadeOutSheet, "fadeoutsheet");
  m_pRenderer->Load(eSprite::FadeIn, "fadein");
  m_pRenderer->Load(eSprite::FadeOut, "fadeout");

  // END Fadein/Fadeout

  // START Portal

  m_pRenderer->Load(eSprite::PortalSheet, "portalsheet");
  m_pRenderer->Load(eSprite::NextPortal, "portal");
  m_pRenderer->Load(eSprite::PrevPortal, "portal");


  // END Portal
  
  // START Menu

  m_pRenderer->Load(eSprite::StartSheet, "startsheet");
  m_pRenderer->Load(eSprite::ExitSheet, "exitsheet");
  m_pRenderer->Load(eSprite::BG, "bg");
  m_pRenderer->Load(eSprite::Start, "start");
  m_pRenderer->Load(eSprite::Exit, "exit");


  // END Menu

  // START AttackFX

  m_pRenderer->Load(eSprite::FireballSheet, "fireballsheet");
  m_pRenderer->Load(eSprite::Fireball, "fireball");

  m_pRenderer->Load(eSprite::TurretFireballSheet, "turretfireballsheet");
  m_pRenderer->Load(eSprite::TurretFireball, "turretfireball");

  m_pRenderer->Load(eSprite::ExplosionSheet, "explosionsheet");
  m_pRenderer->Load(eSprite::Explosion, "explosion");

  m_pRenderer->Load(eSprite::Explosion2Sheet, "explosion2sheet");
  m_pRenderer->Load(eSprite::Explosion2, "explosion2");

  m_pRenderer->Load(eSprite::Explosion3Sheet, "explosion3sheet");
  m_pRenderer->Load(eSprite::Explosion3, "explosion3");

  m_pRenderer->Load(eSprite::Explosion4Sheet, "explosion4sheet");
  m_pRenderer->Load(eSprite::Explosion4, "explosion4");

  // END AttackFX

  // START Health
  m_pRenderer->Load(eSprite::HealthBarFull, "healthbarfull");
  m_pRenderer->Load(eSprite::HealthBar5, "healthbar5");
  m_pRenderer->Load(eSprite::HealthBar4, "healthbar4");
  m_pRenderer->Load(eSprite::HealthBar3, "healthbar3");
  m_pRenderer->Load(eSprite::HealthBar2, "healthbar2");
  m_pRenderer->Load(eSprite::HealthBar1, "healthbar1");
  m_pRenderer->Load(eSprite::HealthBarEmpty, "healthbarempty");
  // END Health

  // START Mana

  m_pRenderer->Load(eSprite::ManaBarFull, "manabarfull");
  m_pRenderer->Load(eSprite::ManaBar5, "manabar5");
  m_pRenderer->Load(eSprite::ManaBar4, "manabar4");
  m_pRenderer->Load(eSprite::ManaBar3, "manabar3");
  m_pRenderer->Load(eSprite::ManaBar2, "manabar2");
  m_pRenderer->Load(eSprite::ManaBar1, "manabar1");
  m_pRenderer->Load(eSprite::ManaBarEmpty, "manabarempty");

  // END Mana

  // START Stamina

  m_pRenderer->Load(eSprite::StaminaWheelFull, "Staminawheelfull");
  m_pRenderer->Load(eSprite::StaminaWheel2, "Staminawheel2");
  m_pRenderer->Load(eSprite::StaminaWheel1, "Staminawheel1");
  m_pRenderer->Load(eSprite::StaminaWheelEmpty, "Staminawheelempty");

  // END Stamina

  // START Player
  m_pRenderer->Load(eSprite::PlayerSpriteSheet, "hero_sprite_sheet");
  m_pRenderer->Load(eSprite::Player, "player");

  m_pRenderer->Load(eSprite::PlayerIdleDownSpriteSheet, "idledownsheet");
  m_pRenderer->Load(eSprite::PlayerIdleRightSpriteSheet, "idlerightsheet");
  m_pRenderer->Load(eSprite::PlayerIdleLeftSpriteSheet, "idleleftsheet");
  m_pRenderer->Load(eSprite::PlayerIdleUpSpriteSheet, "idleupsheet");

  m_pRenderer->Load(eSprite::PlayerRunDownSheet, "rundownsheet");
  m_pRenderer->Load(eSprite::PlayerRunRightSheet, "runrightsheet");
  m_pRenderer->Load(eSprite::PlayerRunLeftSheet, "runleftsheet");
  m_pRenderer->Load(eSprite::PlayerRunUpSheet, "runupsheet");

  m_pRenderer->Load(eSprite::PlayerDashDownSheet, "trundownsheet");
  m_pRenderer->Load(eSprite::PlayerDashRightSheet, "trunrightsheet");
  m_pRenderer->Load(eSprite::PlayerDashLeftSheet, "trunleftsheet");
  m_pRenderer->Load(eSprite::PlayerDashUpSheet, "trunupsheet");

  m_pRenderer->Load(eSprite::PlayerAttackDownSheet, "attackdownsheet");
  m_pRenderer->Load(eSprite::PlayerAttackRightSheet, "attackrightsheet");
  m_pRenderer->Load(eSprite::PlayerAttackLeftSheet, "attackleftsheet");
  m_pRenderer->Load(eSprite::PlayerAttackUpSheet, "attackupsheet");

  m_pRenderer->Load(eSprite::PlayerHitDownSheet, "hitdownsheet");
  m_pRenderer->Load(eSprite::PlayerHitRightSheet, "hitrightsheet");
  m_pRenderer->Load(eSprite::PlayerHitLeftSheet, "hitleftsheet");
  m_pRenderer->Load(eSprite::PlayerHitUpSheet, "hitupsheet");

  m_pRenderer->Load(eSprite::PlayerDeathDownSheet, "deathdownsheet");
  m_pRenderer->Load(eSprite::PlayerDeathRightSheet, "deathrightsheet");
  m_pRenderer->Load(eSprite::PlayerDeathLeftSheet, "deathleftsheet");
  m_pRenderer->Load(eSprite::PlayerDeathUpSheet, "deathupsheet");

  m_pRenderer->Load(eSprite::PlayerIdleDown, "idledown");
  m_pRenderer->Load(eSprite::PlayerIdleRight, "idleright");
  m_pRenderer->Load(eSprite::PlayerIdleLeft, "idleleft");
  m_pRenderer->Load(eSprite::PlayerIdleUp, "idleup");

  m_pRenderer->Load(eSprite::PlayerRunDown, "rundown");
  m_pRenderer->Load(eSprite::PlayerRunRight, "runright");
  m_pRenderer->Load(eSprite::PlayerRunLeft, "runleft");
  m_pRenderer->Load(eSprite::PlayerRunUp, "runup");

  m_pRenderer->Load(eSprite::PlayerDashDown, "trundown");
  m_pRenderer->Load(eSprite::PlayerDashRight, "trunright");
  m_pRenderer->Load(eSprite::PlayerDashLeft, "trunleft");
  m_pRenderer->Load(eSprite::PlayerDashUp, "trunup");

  m_pRenderer->Load(eSprite::PlayerStaticDashDown, "sdashdown");
  m_pRenderer->Load(eSprite::PlayerStaticDashRight, "sdashright");
  m_pRenderer->Load(eSprite::PlayerStaticDashLeft, "sdashleft");
  m_pRenderer->Load(eSprite::PlayerStaticDashUp, "sdashup");

  m_pRenderer->Load(eSprite::PlayerAttackDown, "attackdown");
  m_pRenderer->Load(eSprite::PlayerAttackRight, "attackright");
  m_pRenderer->Load(eSprite::PlayerAttackLeft, "attackleft");
  m_pRenderer->Load(eSprite::PlayerAttackUp, "attackup");

  m_pRenderer->Load(eSprite::PlayerHitDown, "hitdown");
  m_pRenderer->Load(eSprite::PlayerHitRight, "hitright");
  m_pRenderer->Load(eSprite::PlayerHitLeft, "hitleft");
  m_pRenderer->Load(eSprite::PlayerHitUp, "hitup");

  m_pRenderer->Load(eSprite::PlayerDeathDown, "deathdown");
  m_pRenderer->Load(eSprite::PlayerDeathRight, "deathright");
  m_pRenderer->Load(eSprite::PlayerDeathLeft, "deathleft");
  m_pRenderer->Load(eSprite::PlayerDeathUp, "deathup");
  // END Player

  //// START Enemies 

  // START Tower

  m_pRenderer->Load(eSprite::TurretSheet, "turretsheet");
  m_pRenderer->Load(eSprite::Turret, "turret");

  // END Tower

  // START Rabite

  m_pRenderer->Load(eSprite::RabiteSheet, "rabitesheet");

  m_pRenderer->Load(eSprite::RabiteHopUpSheet, "rabitehopupsheet");
  m_pRenderer->Load(eSprite::RabiteHopDownSheet, "rabitehopdownsheet");
  m_pRenderer->Load(eSprite::RabiteHopLeftSheet, "rabitehopleftsheet");
  m_pRenderer->Load(eSprite::RabiteHopRightSheet, "rabitehoprightsheet");

  m_pRenderer->Load(eSprite::RabiteBiteUpSheet, "rabitebiteupsheet");
  m_pRenderer->Load(eSprite::RabiteBiteDownSheet, "rabitebitedownsheet");
  m_pRenderer->Load(eSprite::RabiteBiteLeftSheet, "rabitebiteleftsheet");
  m_pRenderer->Load(eSprite::RabiteBiteRightSheet, "rabitebiterightsheet");

  m_pRenderer->Load(eSprite::RabiteIdleUpSheet, "rabiteidleupsheet");
  m_pRenderer->Load(eSprite::RabiteIdleDownSheet, "rabiteidledownsheet");
  m_pRenderer->Load(eSprite::RabiteIdleLeftSheet, "rabiteidleleftsheet");
  m_pRenderer->Load(eSprite::RabiteIdleRightSheet, "rabiteidlerightsheet");

  m_pRenderer->Load(eSprite::RabiteHurtUpSheet, "rabitehurtupsheet");
  m_pRenderer->Load(eSprite::RabiteHurtDownSheet, "rabitehurtdownsheet");
  m_pRenderer->Load(eSprite::RabiteHurtLeftSheet, "rabitehurtleftsheet");
  m_pRenderer->Load(eSprite::RabiteHurtRightSheet, "rabitehurtrightsheet");

  m_pRenderer->Load(eSprite::RabiteDeathSheet, "rabitedeathsheet");

  m_pRenderer->Load(eSprite::RabiteHopUp, "rabitehopup");
  m_pRenderer->Load(eSprite::RabiteHopDown, "rabitehopdown");
  m_pRenderer->Load(eSprite::RabiteHopLeft, "rabitehopleft");
  m_pRenderer->Load(eSprite::RabiteHopRight, "rabitehopright");

  m_pRenderer->Load(eSprite::RabiteBiteUp, "rabitebiteup");
  m_pRenderer->Load(eSprite::RabiteBiteDown, "rabitebitedown");
  m_pRenderer->Load(eSprite::RabiteBiteLeft, "rabitebiteleft");
  m_pRenderer->Load(eSprite::RabiteBiteRight, "rabitebiteright");

  m_pRenderer->Load(eSprite::RabiteIdleUp, "rabiteidleup");
  m_pRenderer->Load(eSprite::RabiteIdleDown, "rabiteidledown");
  m_pRenderer->Load(eSprite::RabiteIdleLeft, "rabiteidleleft");
  m_pRenderer->Load(eSprite::RabiteIdleRight, "rabiteidleright");

  m_pRenderer->Load(eSprite::RabiteHurtUp, "rabitehurtup");
  m_pRenderer->Load(eSprite::RabiteHurtDown, "rabitehurtdown");
  m_pRenderer->Load(eSprite::RabiteHurtLeft, "rabitehurtleft");
  m_pRenderer->Load(eSprite::RabiteHurtRight, "rabitehurtright");

  m_pRenderer->Load(eSprite::RabiteDeath, "rabitedeath");

  // END Rabite

  // START OakSeed

  m_pRenderer->Load(eSprite::OakSeedSheet, "oakseedsheet");

  m_pRenderer->Load(eSprite::OakSeedWalkUpSheet, "oakseedwalkupsheet");
  m_pRenderer->Load(eSprite::OakSeedWalkDownSheet, "oakseedwalkdownsheet");
  m_pRenderer->Load(eSprite::OakSeedWalkLeftSheet, "oakseedwalkleftsheet");
  m_pRenderer->Load(eSprite::OakSeedWalkRightSheet, "oakseedwalkrightsheet");

  m_pRenderer->Load(eSprite::OakSeedIdleUpSheet, "oakseedidleupsheet");
  m_pRenderer->Load(eSprite::OakSeedIdleDownSheet, "oakseedidledownsheet");
  m_pRenderer->Load(eSprite::OakSeedIdleLeftSheet, "oakseedidleleftsheet");
  m_pRenderer->Load(eSprite::OakSeedIdleRightSheet, "oakseedidlerightsheet");

  m_pRenderer->Load(eSprite::OakSeedHurtUpSheet, "oakseedhurtupsheet");
  m_pRenderer->Load(eSprite::OakSeedHurtDownSheet, "oakseedhurtdownsheet");
  m_pRenderer->Load(eSprite::OakSeedHurtLeftSheet, "oakseedhurtleftsheet");
  m_pRenderer->Load(eSprite::OakSeedHurtRightSheet, "oakseedhurtrightsheet");

  m_pRenderer->Load(eSprite::OakSeedDeathSheet, "oakseeddeathsheet");

  m_pRenderer->Load(eSprite::OakSeedWalkUp, "oakseedwalkup");
  m_pRenderer->Load(eSprite::OakSeedWalkDown, "oakseedwalkdown");
  m_pRenderer->Load(eSprite::OakSeedWalkLeft, "oakseedwalkleft");
  m_pRenderer->Load(eSprite::OakSeedWalkRight, "oakseedwalkright");

  m_pRenderer->Load(eSprite::OakSeedIdleUp, "oakseedidleup");
  m_pRenderer->Load(eSprite::OakSeedIdleDown, "oakseedidledown");
  m_pRenderer->Load(eSprite::OakSeedIdleLeft, "oakseedidleleft");
  m_pRenderer->Load(eSprite::OakSeedIdleRight, "oakseedidleright");

  m_pRenderer->Load(eSprite::OakSeedHurtUp, "oakseedhurtup");
  m_pRenderer->Load(eSprite::OakSeedHurtDown, "oakseedhurtdown");
  m_pRenderer->Load(eSprite::OakSeedHurtLeft, "oakseedhurtleft");
  m_pRenderer->Load(eSprite::OakSeedHurtRight, "oakseedhurtright");

  m_pRenderer->Load(eSprite::OakSeedDeath, "oakseeddeath");

  // END OakSeed

  //// END Enemies

  m_pRenderer->Load(eSprite::Bullet, "bullet");
  m_pRenderer->Load(eSprite::Bullet2, "bullet2");
  m_pRenderer->Load(eSprite::Smoke, "smoke");
  m_pRenderer->Load(eSprite::Spark, "spark");
  //m_pRenderer->Load(eSprite::Turret, "turret");

  m_pRenderer->Load(eSprite::Line, "greenline");

  m_pRenderer->Load(eSprite::Slime, "slime");
  m_pRenderer->Load(eSprite::SlimeBig, "slime");
  m_pRenderer->Load(eSprite::SlimeKing, "slime");
  m_pRenderer->Load(eSprite::SlimeIdle, "slimeidle");
  m_pRenderer->Load(eSprite::SlimeHop, "hop");
  m_pRenderer->Load(eSprite::SlimeTent, "tentacle");
  m_pRenderer->Load(eSprite::SlimeHurt, "hurt");
  m_pRenderer->Load(eSprite::SlimeDeath, "death");

  m_pRenderer->Load(eSprite::SlimeSplitSheet, "slimeSplitSheet");
  m_pRenderer->Load(eSprite::SlimeSplit, "slimeSplit");

  m_pRenderer->Load(eSprite::Cross, "cross");
  m_pRenderer->Load(eSprite::Mana, "mana");
  m_pRenderer->Load(eSprite::Circle, "circle");
  m_pRenderer->Load(eSprite::CircleNoBorder, "circlenoborder");
  m_pRenderer->Load(eSprite::SmoothCircle, "smoothcircle");
  m_pRenderer->Load(eSprite::ManaParticlesSheet, "manaparticlessheet");
  m_pRenderer->Load(eSprite::ManaParticles, "manaparticles");
  m_pRenderer->Load(eSprite::SlimeParticleSheet, "slimeparticlesheet");
  m_pRenderer->Load(eSprite::SlimeParticles, "slimeparticles");

  m_pRenderer->Load(eSprite::KeySheet, "keysheet");
  m_pRenderer->Load(eSprite::Key, "key");
  m_pRenderer->Load(eSprite::LockTestSheet, "locktestsheet");
  m_pRenderer->Load(eSprite::LockTest, "locktest");

  ///START mine flavor
  m_pRenderer->Load(eSprite::MineSheet, "minesheet");
  
  //32x32 flavor no hitbox
  m_pRenderer->Load(eSprite::GoldUI, "goldui");
  m_pRenderer->Load(eSprite::GoldItems, "golditems");
  m_pRenderer->Load(eSprite::FlowerUI, "flowerui");
  m_pRenderer->Load(eSprite::FlowerItems, "floweritems");
  m_pRenderer->Load(eSprite::SilverUI, "silverui");
  m_pRenderer->Load(eSprite::SilverItems, "silveritems");
  m_pRenderer->Load(eSprite::GemUI, "gemui");
  m_pRenderer->Load(eSprite::GemItems, "gemitems");
  m_pRenderer->Load(eSprite::Grass1, "grass1");
  m_pRenderer->Load(eSprite::Grass2, "grass2");
  m_pRenderer->Load(eSprite::Grass3, "grass3");
  m_pRenderer->Load(eSprite::Grass4, "grass4");
  m_pRenderer->Load(eSprite::Shrooms, "shrooms");
    //not sure if these 4 should have hitbox
  m_pRenderer->Load(eSprite::BlueShard, "blueshard");   
  m_pRenderer->Load(eSprite::GreyShard, "greyshard");
  m_pRenderer->Load(eSprite::RedShard, "redshard");
  m_pRenderer->Load(eSprite::GreenShard, "greenshard");
  //32x64 flavor no hitbox
  m_pRenderer->Load(eSprite::TallGrass1, "tallgrass1");
  m_pRenderer->Load(eSprite::TallGrass2, "tallgrass2");
  m_pRenderer->Load(eSprite::TallGrass3, "tallgrass3");
  m_pRenderer->Load(eSprite::TallGrass4, "tallgrass4");

  //32x32 flavor with hitbox
  m_pRenderer->Load(eSprite::GoldRock, "goldrock");
  m_pRenderer->Load(eSprite::PurpleRock, "purplerock");
  m_pRenderer->Load(eSprite::SilverRock, "silverrock");
  m_pRenderer->Load(eSprite::BrownRock, "brownrock");
  m_pRenderer->Load(eSprite::BlueRock, "bluerock");
  m_pRenderer->Load(eSprite::GreyRock, "greyrock");
  m_pRenderer->Load(eSprite::RedRock, "redrock");
  m_pRenderer->Load(eSprite::GreenRock, "greenrock");
  //64x64 flavor with hitbox
  m_pRenderer->Load(eSprite::BigRocks, "bigrocks");

  ///END mine flavor

  m_pRenderer->Load(eSprite::MainLev, "mainlev");
  m_pRenderer->Load(eSprite::WallOut1, "wallout1"); 
  m_pRenderer->Load(eSprite::Wall1, "wall1");
  m_pRenderer->Load(eSprite::WallEdgeX1, "walledgex1");
  m_pRenderer->Load(eSprite::WallEdgeY1, "walledgey1");
  m_pRenderer->Load(eSprite::WallCorner1, "wallcorner1");
  m_pRenderer->Load(eSprite::WallBotPoke1, "wallbotpoke1");

  m_pRenderer->Load(eSprite::WallOut2, "wallout2"); 
  m_pRenderer->Load(eSprite::Wall2, "wall2");
  m_pRenderer->Load(eSprite::WallEdgeX2, "walledgex2");
  m_pRenderer->Load(eSprite::WallEdgeY2, "walledgey2");
  m_pRenderer->Load(eSprite::WallCorner2, "wallcorner2");

  m_pRenderer->Load(eSprite::WallOut3, "wallout3"); 
  m_pRenderer->Load(eSprite::Wall3, "wall3");
  m_pRenderer->Load(eSprite::WallEdgeX3, "walledgex3");
  m_pRenderer->Load(eSprite::WallEdgeY3, "walledgey3");
  m_pRenderer->Load(eSprite::WallCorner3, "wallcorner3");
  m_pRenderer->Load(eSprite::WallBotPoke3, "wallbotpoke3");

  m_pRenderer->Load(eSprite::WallOut4, "wallout4"); 
  m_pRenderer->Load(eSprite::Wall4, "wall4");
  m_pRenderer->Load(eSprite::WallEdgeX4, "walledgex4");
  m_pRenderer->Load(eSprite::WallEdgeY4, "walledgey4");
  m_pRenderer->Load(eSprite::WallCorner4, "wallcorner4");
  m_pRenderer->Load(eSprite::WallBotPoke4, "wallbotpoke4");

  m_pRenderer->Load(eSprite::WallOut5, "wallout5"); 
  m_pRenderer->Load(eSprite::Wall5, "wall5");
  m_pRenderer->Load(eSprite::WallEdgeX5, "walledgex5");
  m_pRenderer->Load(eSprite::WallEdgeY5, "walledgey5");
  m_pRenderer->Load(eSprite::WallCorner5, "wallcorner5");

  m_pRenderer->Load(eSprite::WallOut6, "wallout6"); 
  m_pRenderer->Load(eSprite::Wall6, "wall6");
  m_pRenderer->Load(eSprite::WallEdgeX6, "walledgex6");
  m_pRenderer->Load(eSprite::WallEdgeY6, "walledgey6");
  m_pRenderer->Load(eSprite::WallCorner6, "wallcorner6");
  m_pRenderer->Load(eSprite::WallBotPoke6, "wallbotpoke6");

  m_pRenderer->Load(eSprite::WallOut7, "wallout7"); 
  m_pRenderer->Load(eSprite::Wall7, "wall7");
  m_pRenderer->Load(eSprite::WallEdgeX7, "walledgex7");
  m_pRenderer->Load(eSprite::WallEdgeY7, "walledgey7");
  m_pRenderer->Load(eSprite::WallCorner7, "wallcorner7");
  m_pRenderer->Load(eSprite::WallBotPoke7, "wallbotpoke7");

  m_pRenderer->Load(eSprite::WallOut8, "wallout8"); 
  m_pRenderer->Load(eSprite::Wall8, "wall8");
  m_pRenderer->Load(eSprite::WallEdgeX8, "walledgex8");
  m_pRenderer->Load(eSprite::WallEdgeY8, "walledgey8");
  m_pRenderer->Load(eSprite::WallCorner8, "wallcorner8");

  m_pRenderer->Load(eSprite::WallOut9, "wallout9");
  m_pRenderer->Load(eSprite::Wall9, "wall9");
  m_pRenderer->Load(eSprite::WallEdgeX9, "walledgex9");
  m_pRenderer->Load(eSprite::WallEdgeY9, "walledgey9");
  m_pRenderer->Load(eSprite::WallCorner9, "wallcorner9");
  m_pRenderer->Load(eSprite::WallBotPoke9, "wallbotpoke9");

  m_pRenderer->Load(eSprite::Rails, "rails");
  m_pRenderer->Load(eSprite::RailStop, "railstop");

  m_pRenderer->Load(eSprite::Fern, "fern");
  m_pRenderer->Load(eSprite::BigFern, "bigfern");

  m_pRenderer->Load(eSprite::Lantern, "lantern");

  m_pRenderer->Load(eSprite::Support1, "support1");
  m_pRenderer->Load(eSprite::Support2, "support2");

  m_pRenderer->Load(eSprite::BrownCobbleSheet, "browncobblesheet");
  m_pRenderer->Load(eSprite::BrownCobbleFloor, "browncobblefloor");
  m_pRenderer->Load(eSprite::GreenCobbleSheet, "greencobblesheet");
  m_pRenderer->Load(eSprite::GreenCobbleFloor, "greencobblefloor");
  m_pRenderer->Load(eSprite::YellowFloorSheet, "yellowfloorsheet");
  m_pRenderer->Load(eSprite::YellowGroundFloor, "yellowgroundfloor");
  m_pRenderer->Load(eSprite::BrownFloorSheet, "brownfloorsheet");
  m_pRenderer->Load(eSprite::BrownGroundFloor, "browngroundfloor");
  m_pRenderer->Load(eSprite::GrassFloorSheet, "grassfloorsheet");
  m_pRenderer->Load(eSprite::GrassGroundFloor, "grassgroundfloor");
  m_pRenderer->Load(eSprite::GrassFloorSheet2, "grassfloorsheet2");
  m_pRenderer->Load(eSprite::GrassGroundFloor2, "grassgroundfloor2");
  m_pRenderer->Load(eSprite::MarshGrassSheet, "marshgrasssheet");
  m_pRenderer->Load(eSprite::MarshGrassFloor, "marshgrassfloor");

  m_pRenderer->Load(eSprite::PitSheet, "pitsheet");
  m_pRenderer->Load(eSprite::Pit, "pit");
  m_pRenderer->Load(eSprite::Floor1, "floor1");

  m_pRenderer->Load(eSprite::BrownGravelSheet, "browngravelsheet");
  m_pRenderer->Load(eSprite::BrownGravel, "browngravel");
  m_pRenderer->Load(eSprite::GreyGravelSheet, "greygravelsheet");
  m_pRenderer->Load(eSprite::GreyGravel, "greygravel");

  m_pRenderer->Load(eSprite::SlimeKing, "kingslime");
  m_pRenderer->Load(eSprite::KingIdle, "kingidle");
  m_pRenderer->Load(eSprite::KingHop, "kinghop");
  m_pRenderer->Load(eSprite::KingHurt, "kinghurt");
  m_pRenderer->Load(eSprite::KingDeath, "kingdeath");




  m_pRenderer->EndResourceUpload();
} //LoadImages

/// Initialize the audio player and load game sounds.

void CGame::LoadSounds() {
  m_pAudio->Initialize(eSound::Size);

  m_pAudio->Load(eSound::Ow, "ow");
  m_pAudio->Load(eSound::Grunt, "grunt");
  m_pAudio->Load(eSound::Clang, "clang");
  m_pAudio->Load(eSound::Gun, "gun");
  m_pAudio->Load(eSound::Ricochet, "ricochet");
  m_pAudio->Load(eSound::Start, "start");
  m_pAudio->Load(eSound::Boom, "boom");
  m_pAudio->Load(eSound::Swing, "swing");
  m_pAudio->Load(eSound::Impact, "impact");
  m_pAudio->Load(eSound::Thump, "thump");
  m_pAudio->Load(eSound::Achievement, "achievement");
  m_pAudio->Load(eSound::Acquire, "acquire");
  m_pAudio->Load(eSound::BrassMusic, "brassmusic");
  m_pAudio->Load(eSound::Drums, "drums");
  m_pAudio->Load(eSound::SoundTrack, "soundtrack");
  m_pAudio->Load(eSound::MainMenu, "mainmenu");
  m_pAudio->Load(eSound::Exploration, "exploration");
  m_pAudio->Load(eSound::Battle, "battle"); 
  m_pAudio->Load(eSound::Boss, "boss");
  m_pAudio->Load(eSound::BossDefeated, "bossdefeated");
  m_pAudio->Load(eSound::EndingCredits, "endingcredits");
  m_pAudio->Load(eSound::GameOver, "gameover");
  m_pAudio->Load(eSound::LastLevel, "lastlevel");
  m_pAudio->Load(eSound::Story, "story");
  m_pAudio->Load(eSound::Health, "health");
  m_pAudio->Load(eSound::Magic, "magic");


} //LoadSounds

/// Release all of the DirectX12 objects by deleting the renderer.

void CGame::Release() {
  delete m_pRenderer;
  m_pRenderer = nullptr; //for safety
} //Release

/// <summary>
/// Spawns the main menu objects for use by the player
/// </summary>

void CGame::CreateMenu() {
    m_bMainMenu = true;
    m_pObjectManager->clear();
    printf("Making menu\n");
    m_pBackgroundObj = (CMainMenuBG*)m_pObjectManager->create(eSprite::BG, Vector2(512.0, 390.0));

    m_pExitButton = (CMainMenuExit*)m_pObjectManager->create(eSprite::Exit, Vector2(102.0, 58.0));
    m_pStartButton = (CMainMenuStart*)m_pObjectManager->create(eSprite::Start, Vector2(102.0, 88.0));
}

/// <summary>
/// Creates all of the game objects based on their positions in tilemanager.
/// Most objects should be created here.
/// </summary>

void CGame::CreateObjects() {
  //m_pRenderer->GetSize(eSprite::Background, m_vWorldSize.x, m_vWorldSize.y); //init m_vWorldSize
  std::vector<Vector2> turretpos, spikespos, bigPos, slimePos, kingPos, oakPos, rabitePos;
  Vector2 entry_portal;
  Vector2 exit_portal;
  Vector2 playerpos; //player positions
  m_pTileManager->GetObjects(turretpos, playerpos, spikespos, exit_portal, entry_portal, bigPos,
      slimePos, kingPos, oakPos, rabitePos); //get positions
  
  m_pPlayer = (CPlayer*)m_pObjectManager->create(eSprite::Player, playerpos);

  m_pHealthBar = (CHealthBar*)m_pObjectManager->create(eSprite::HealthBarFull, Vector2(playerpos.x - (m_nWinWidth / 2.0f) + 125.0f, playerpos.y - (m_nWinHeight / 2.0f) + 75.0f));
  m_pHealthBar->m_fRoll = 0.0f;
  m_pHealthBar->m_fXScale = m_pHealthBar->m_fYScale = 1.5f;

  m_pManaBar = (CManaBar*)m_pObjectManager->create(eSprite::ManaBarFull, Vector2(playerpos.x + (m_nWinWidth / 2.0f) - 125.0f, playerpos.y - (m_nWinHeight / 2.0f) + 75.0f));
  m_pManaBar->m_fRoll = 0.0f;
  m_pManaBar->m_fXScale = m_pManaBar->m_fYScale = 1.5f;

  m_pStaminaWheel = (CStaminaWheel*)m_pObjectManager->create(eSprite::StaminaWheelFull, Vector2(playerpos.x + (m_nWinWidth / 2.0f) - 125.0f, playerpos.y - (m_nWinHeight / 2.0f) + 50.0f));
  m_pStaminaWheel->m_fRoll = 0.0f;
  m_pStaminaWheel->m_fXScale = m_pStaminaWheel->m_fYScale = 1.5f;

  if (entry_portal.Length() > 0) 
      m_pObjectManager->create(eSprite::PrevPortal, entry_portal);
  if (exit_portal.Length() > 0) {
      m_pObjectManager->create(eSprite::NextPortal, exit_portal);
      //m_pObjectManager->create(eSprite::SlimeKing, exit_portal - Vector2(0.0f, 30.0f));
      //m_pObjectManager->create(eSprite::Rabite, exit_portal - Vector2(0.0f, 150.0f));
      //m_pObjectManager->create(eSprite::OakSeed, exit_portal - Vector2(0.0f, 150.0f));
  }

  for (const Vector2& pos : turretpos)
      m_pObjectManager->create(eSprite::Turret, pos);
  for (const Vector2& pos : bigPos)
      m_pObjectManager->create(eSprite::SlimeBig, pos);
  for (const Vector2& pos : slimePos)
      m_pObjectManager->create(eSprite::Slime, pos);
  for (const Vector2& pos : kingPos)
      m_pObjectManager->create(eSprite::SlimeKing, pos);
  for (const Vector2& pos : oakPos)
      m_pObjectManager->create(eSprite::OakSeed, pos);
  for (const Vector2& pos : rabitePos)
      m_pObjectManager->create(eSprite::Rabite, pos);
  /*std::vector<Vector2> slime_positions;
  slime_positions.push_back(Vector2(680.0f, 515.0f));
  slime_positions.push_back(Vector2(730.0f, 500.0f));

  std::vector<Vector2> health_positions;
  health_positions.push_back(Vector2(800.0f, 580.0f));
  health_positions.push_back(Vector2(700.0f, 580.0f));

  std::vector<Vector2> mana_positions;
  mana_positions.push_back(Vector2(1000.0f, 600.0f));
  mana_positions.push_back(Vector2(900.0f, 600.0f));

  std::vector<Vector2> slime_big_positions;
  slime_big_positions.push_back(Vector2(710.0f, 510.0f));

  SpawnMany(eSprite::Slime, slime_positions);
  SpawnMany(eSprite::HealthPotion, health_positions);
  SpawnMany(eSprite::ManaPotion, mana_positions);
  SpawnMany(eSprite::SlimeBig, slime_big_positions);
  SpawnMany(eSprite::Spikes, spikespos);
  

  //m_pObjectManager->create(eSprite::SlimeBig, Vector2(710.0f, 510.0f));
  //m_pObjectManager->create(eSprite::Turret, Vector2(680.0f, 515.0f));


  //m_pObjectManager->create(eSprite::Turret, Vector2(430.0f, 430.0f));

  //m_pObjectManager->create(eSprite::ManaPotion, Vector2(700.0f, 500.0f));
  */
} //CreateObjects

/// <summary>
/// Takes a vector of positions and the type of object to spawn
/// and spawns one object for each position. 
/// </summary>
/// <param name="type"></param>
/// <param name="positions"></param>

void CGame::SpawnMany(eSprite type, std::vector<Vector2> positions) {
    for (auto i : positions) {
        m_pObjectManager->create(type, i);
    }
}

/// Call this function to start a new game. This should be re-entrant so that
/// you can restart a new game without having to shut down and restart the
/// program. Clear the particle engine to get rid of any existing particles,
/// delete any old objects out of the object manager and create some new ones.

void CGame::BeginGame() {
    printf("BeginGame called\n");
    if (m_pPlayer) {
        printf("Player port: %d\n", m_pPlayer->GetPort());
        printf("Map being loaded: %s\n", maps[m_nCurrentLevelIndex]);
    }
  m_pObjectManager->clear(); //clear old objects

  m_pParticleEngine->clear(); //clear old particles

  m_pTileManager->LoadMap(maps[m_nCurrentLevelIndex], mapmasks[m_nCurrentLevelIndex], flavor[m_nCurrentLevelIndex] ,masks[m_nCurrentLevelIndex], m_nPortDirection);
  m_pRenderer->SetCameraPos(m_vInitCameraPos);
  t = false;
  CreateObjects();
  m_pAudio->BeginFrame();
  if (state != eMusicState::MAINMENU) {
    printf("musicchange\n");
    m_pAudio->stop();
    m_pAudio->loop(eSound::MainMenu); //something like it
    state = eMusicState::MAINMENU;
  }
  m_eGameState = eGameState::Playing; //now playing
  m_bFadeInAnimated = false;
  if (!oneTimeReset) {
      oneTimeReset = true;
      Restart();
  }
} //BeginGame

/// <summary>
/// Restarts the game from the very beginning.
/// Lightweight call to initialize but without
/// the bugs
/// </summary>

void CGame::Restart() {
    m_bTitleSequence = false;
    m_nCurrentLevelIndex = 0;
    m_bInMenu = true;
    m_pObjectManager->clear();
    m_pRenderer->SetCameraPos(m_vInitCameraPos);
    CreateMenu();
    m_pAudio->stop();
    m_pAudio->loop(eSound::MainMenu); //something like it
    state = eMusicState::MAINMENU;
}

void CGame::PlayTitleSequence() {
    printf("PlayTitleSequence()\n");
    m_bTitleSequence = true;
    m_pObjectManager->create(eSprite::Titlecard, Vec3toVec2(m_pRenderer->GetCameraPos()));
}

/// Poll the keyboard state and respond to the key presses that happened since
/// the last frame.


void CGame::KeyboardHandler() {
  m_pKeyboard->GetState(); //get current keyboard state 
  if (m_pKeyboard->TriggerDown(VK_F1)) //help
    ShellExecute(0, 0, "https://www.youtube.com/watch?v=dQw4w9WgXcQ&ab", 0, 0, SW_SHOW);

  if (m_pKeyboard->TriggerDown(VK_F2)) //toggle frame rate
    m_bDrawFrameRate = !m_bDrawFrameRate;

  if (m_pKeyboard->TriggerDown(VK_BACK)) { //start game
      Restart();
  }

  if (m_bAtEndScreen) {
      if (m_pKeyboard->TriggerDown(VK_RETURN)) {
          m_bAtEndScreen = false;
          oneTimeReset = false;
          Restart();
      }
  }

  if (m_bGameOverScreen) {
      printf("In keyboard handle gameover\n");
      printf("m_bInMenu: %d\n", m_bInMenu);
      if (m_pKeyboard->TriggerDown(VK_RETURN)) {
          m_bGameOverScreen = false;
          BeginGame();
      }
      if (m_pKeyboard->TriggerDown(VK_ESCAPE)) {
          exit(0);
      }
  }

  if (m_bTitleSequence) {
      if (m_pKeyboard->TriggerDown(VK_RETURN)) {
          if (m_pObjectManager->GetTitleCard()) {
              m_pObjectManager->GetTitleCard()->NextFrame();
              if (m_pObjectManager->GetTitleCard()->Finished()) {
                  Restart();
              }
          }
      }
  }

  if (!m_bInMenu && !m_bGameOverScreen && !m_bAtEndScreen && !m_bTitleSequence) { // if the player isn't in the main menu
      if (m_pPlayer) { //Safety
        if (m_pKeyboard->TriggerDown(VK_CAPITAL) && m_pPlayer->m_bIsAttacking == false && m_pPlayer->GetHealth() > 0) { //checks to see if player is alive and isn't attacking (no fast attacking)
          if (m_pPlayer->GetMana() > 0) { // Checks to see if player has mana
            m_pPlayer->AttackTrigger();   // Triggers player sword swing
            m_pObjectManager->FireGun(m_pPlayer, eSprite::Bullet);
            m_pPlayer->SetMana(-1);       // reduce player mana by 1
            m_pPlayer->SetFacingVectorAtShot(m_pPlayer->GetFacingVectorRad());  // Orients the explosion correctly
          }
        }
        if (m_pKeyboard->TriggerDown(VK_ESCAPE)) {
          Pause();
        }
        if (m_pKeyboard->TriggerDown(VK_SPACE) && m_pPlayer->m_bIsAttacking == false && m_pPlayer->GetHealth() > 0) { //checks for player alive and isn't mid attack (no fast attacking)
          m_pPlayer->AttackTrigger(); // Triggers player sword swing
          m_pObjectManager->SwingSword(m_pPlayer, eSprite::TestSwing);
        }
        if (m_pKeyboard->Down('W') && !m_pKeyboard->Down('S')) //strafe up
          m_pPlayer->StrafeUp();

        if (m_pKeyboard->Down('D') && !m_pKeyboard->Down('A'))//strafe right
          m_pPlayer->StrafeRight();

        if (m_pKeyboard->Down('A') && !m_pKeyboard->Down('D')) //strafe left
          m_pPlayer->StrafeLeft();

        if (m_pKeyboard->Down('S') && !m_pKeyboard->Down('W')) //strafe down
          m_pPlayer->StrafeDown();

        if (m_pKeyboard->TriggerDown(VK_SHIFT) && m_pPlayer->m_bIsDashing == false && m_pPlayer->GetStamina() > 0) { //Checks to see if player has stamina and isn't mid dash (no fast dashing)
          m_pPlayer->DashTrigger(); // Triggers dash
          m_pPlayer->SetStamina(-1);// reduce player stamina by 1
        }
        if (m_pKeyboard->Down('E')) //interact with things
            m_bInteract = true;
        else
            m_bInteract = false;
      } //if
  } else if (!m_bTitleSequence && m_bInMenu) {
      if (m_pKeyboard->TriggerDown('S') || m_pKeyboard->TriggerDown(VK_DOWN)) {
          m_pExitButton->Highlight();
          m_pStartButton->Normal();
      }
      if (m_pKeyboard->TriggerDown('W') || m_pKeyboard->TriggerDown(VK_UP)) {
          m_pExitButton->Normal();
          m_pStartButton->Highlight();
      }
      if (m_pKeyboard->TriggerDown(VK_RETURN)) {
          if (m_pExitButton->m_bIsSelected) {
              m_pExitButton->Select();
          }
          else if (m_pStartButton->m_bIsSelected) {
              m_eGameState = eGameState::Playing; 
              m_bInMenu = !m_pStartButton->Select();
              BeginGame();
          }
      }
      if (m_pKeyboard->TriggerDown(VK_ESCAPE)) {
          if (m_bMainMenu) {
              m_pStartButton->Highlight();
              m_bInMenu = !m_pStartButton->Select();
              m_bMainMenu = !m_pStartButton->Select();
              m_eGameState = eGameState::Playing;
              BeginGame();
          }
          else {
              Pause();
          }
      } 
  }

} //KeyboardHandler

/// Draw the current frame rate to a hard-coded position in the window.
/// The frame rate will be drawn in a hard-coded position using the font
/// specified in `gamesettings.xml`.

void CGame::DrawFrameRateText() {
  const std::string s = std::to_string(m_pTimer->GetFPS()) + " fps"; //frame rate
  const Vector2 pos(m_nWinWidth - 128.0f, 30.0f); //hard-coded position
  m_pRenderer->DrawScreenText(s.c_str(), pos); //draw to screen
} //DrawFrameRateText

void CGame::DrawGodModeText() {
  const Vector2 pos(64.0f, 30.0f); //hard-coded position
  m_pRenderer->DrawScreenText("God Mode", pos); //draw to screen
} //DrawGodModeText

/// Ask the object manager to draw the game objects. The renderer is notified of
/// the start and end of the frame so that it can let Direct3D do its
/// pipelining jiggery-pokery.

void CGame::RenderFrame() {
  m_pRenderer->BeginFrame(); //required before rendering

  //m_pRenderer->Draw(eSprite::Background, m_vWorldSize / 2.0f); //draw background
  m_pObjectManager->draw(); //draw objects 
  m_pParticleEngine->Draw(); //draw particles
  if (m_bDrawFrameRate)DrawFrameRateText(); //draw frame rate, if required
  if (m_bGodMode)DrawGodModeText(); //draw god mode text, if required

  m_pRenderer->EndFrame(); //required after rendering
} //RenderFrame

/// Draw the god mode text to a hard-coded position in the window using the
/// font specified in `gamesettings.xml`.

/// Make the camera follow the player, but don't let it get too close to the
/// edge unless the world is smaller than the window, in which case we just
/// center everything.

void CGame::FollowCamera() {
    if (!m_bInMenu) {
        if (m_pPlayer == nullptr)return; //safety
        Vector3 vCameraPos(m_pPlayer->GetPos()); //player position

        if (m_vWorldSize.x > m_nWinWidth) { //world wider than screen
            vCameraPos.x = std::max(vCameraPos.x, m_nWinWidth / 2.0f); //stay away from the left edge
            vCameraPos.x = std::min(vCameraPos.x, m_vWorldSize.x - m_nWinWidth / 2.0f);  //stay away from the right edge
        } //if
        else vCameraPos.x = m_vWorldSize.x / 2.0f; //center horizontally.

        if (m_vWorldSize.y > m_nWinHeight) { //world higher than screen
            vCameraPos.y = std::max(vCameraPos.y, m_nWinHeight / 2.0f);  //stay away from the bottom edge
            vCameraPos.y = std::min(vCameraPos.y, m_vWorldSize.y - m_nWinHeight / 2.0f); //stay away from the top edge
        } //if
        else vCameraPos.y = m_vWorldSize.y / 2.0f; //center vertically

        m_pHealthBar->m_vPos = Vector2(vCameraPos.x - (m_nWinWidth / 2.0f) + 125.0f, vCameraPos.y - (m_nWinHeight / 2.0f) + 75.0f);
        m_pManaBar->m_vPos = Vector2(vCameraPos.x + (m_nWinWidth / 2.0f) - 125.0f, vCameraPos.y - (m_nWinHeight / 2.0f) + 75.0f);
        //m_pStaminaWheel->m_vPos = Vector2(vCameraPos.x + (m_nWinWidth / 2.0f) - 50.0f, vCameraPos.y - (m_nWinHeight / 2.0f) + 135.0f); // Above Mana Bar
        m_pStaminaWheel->m_vPos = Vector2(vCameraPos.x, vCameraPos.y - (m_nWinHeight / 2.0f) + 65.0f); // Centered

        m_pRenderer->SetCameraPos(vCameraPos); //camera to player
    }
} //FollowCamera

/// <summary>
/// This function just updates and controls the UI objects
/// to keep their information relevant.
/// </summary>

void CGame::UI() {
  if (m_pPlayer) { // Safety
    if (m_pPlayer->GetHealth() > 5) 
      m_pHealthBar->m_nSpriteIndex = (UINT)eSprite::HealthBarFull;
    else if (m_pPlayer->GetHealth() == 5)
      m_pHealthBar->m_nSpriteIndex = (UINT)eSprite::HealthBar5;
    else if (m_pPlayer->GetHealth() == 4)
      m_pHealthBar->m_nSpriteIndex = (UINT)eSprite::HealthBar4;
    else if (m_pPlayer->GetHealth() == 3)
      m_pHealthBar->m_nSpriteIndex = (UINT)eSprite::HealthBar3;
    else if (m_pPlayer->GetHealth() == 2)
      m_pHealthBar->m_nSpriteIndex = (UINT)eSprite::HealthBar2;
    else if (m_pPlayer->GetHealth() == 1)
      m_pHealthBar->m_nSpriteIndex = (UINT)eSprite::HealthBar1;
    else if (m_pPlayer->GetHealth() == 0)
      m_pHealthBar->m_nSpriteIndex = (UINT)eSprite::HealthBarEmpty;
    else
      m_pHealthBar->m_nSpriteIndex = (UINT)eSprite::HealthBarEmpty;
  }

  if (m_pPlayer) { // Safety
    if (m_pPlayer->GetMana() > 5)
      m_pManaBar->m_nSpriteIndex = (UINT)eSprite::ManaBarFull;
    else if (m_pPlayer->GetMana() == 5)
      m_pManaBar->m_nSpriteIndex = (UINT)eSprite::ManaBar5;
    else if (m_pPlayer->GetMana() == 4)
      m_pManaBar->m_nSpriteIndex = (UINT)eSprite::ManaBar4;
    else if (m_pPlayer->GetMana() == 3)
      m_pManaBar->m_nSpriteIndex = (UINT)eSprite::ManaBar3;
    else if (m_pPlayer->GetMana() == 2)
      m_pManaBar->m_nSpriteIndex = (UINT)eSprite::ManaBar2;
    else if (m_pPlayer->GetMana() == 1)
      m_pManaBar->m_nSpriteIndex = (UINT)eSprite::ManaBar1;
    else if (m_pPlayer->GetMana() == 0)
      m_pManaBar->m_nSpriteIndex = (UINT)eSprite::ManaBarEmpty;
    else
      m_pManaBar->m_nSpriteIndex = (UINT)eSprite::ManaBarEmpty;
  }

  if (m_pPlayer) { // Safety
    if (m_pPlayer->GetStamina() >= 3)
      m_pStaminaWheel->m_nSpriteIndex = (UINT)eSprite::StaminaWheelFull;
    else if (m_pPlayer->GetStamina() == 2)
      m_pStaminaWheel->m_nSpriteIndex = (UINT)eSprite::StaminaWheel2;
    else if (m_pPlayer->GetStamina() == 1)
      m_pStaminaWheel->m_nSpriteIndex = (UINT)eSprite::StaminaWheel1;
    else if (m_pPlayer->GetStamina() == 0)
      m_pStaminaWheel->m_nSpriteIndex = (UINT)eSprite::StaminaWheelEmpty;
    else
      m_pStaminaWheel->m_nSpriteIndex = (UINT)eSprite::StaminaWheelEmpty;
  }
}

/// This function will be called regularly to process and render a frame
/// of animation, which involves the following. Handle keyboard input.
/// Notify the  audio player at the start of each frame so that it can prevent
/// multiple copies of a sound from starting on the same frame.  
/// Move the game objects. Render a frame of animation. 

int frame_n = 0;

void CGame::ProcessFrame() {
  KeyboardHandler(); //handle keyboard input
  //ControllerHandler(); //handle controller input
  m_pAudio->BeginFrame(); //notify audio player that frame has begun
  if (!m_bInMenu) {

    // Music state checks and logic for playing the correct music at the correct time
    if (state != eMusicState::GAMEOVER) {
      if (m_nCurrentLevelIndex == 3) {
        if (m_pObjectManager->GetEnemyCount() > 0) {
          if (state != eMusicState::BOSS) {
            m_pAudio->stop();
            m_pAudio->loop(eSound::Boss);
            state = eMusicState::BOSS;
          }
        }
        else {
          if (state != eMusicState::BOSSDEFEATED) {
            m_pAudio->stop();
            m_pAudio->loop(eSound::BossDefeated);
            state = eMusicState::BOSSDEFEATED;
          }
        }
      }
      else if (m_nCurrentLevelIndex == 2) {
        if (state != eMusicState::LASTLEVEL) {
          m_pAudio->stop();
          m_pAudio->loop(eSound::LastLevel);
          state = eMusicState::LASTLEVEL;
        }
      }
      else {
        if (m_pObjectManager->GetEnemyCount() > 0) {
          if (state != eMusicState::BATTLE) {
            m_pAudio->stop();
            m_pAudio->loop(eSound::Battle);
            state = eMusicState::BATTLE;
          }
        }
        else {
          if (state != eMusicState::EXPLORATION) {
            m_pAudio->stop();
            m_pAudio->loop(eSound::Exploration);
            state = eMusicState::EXPLORATION;
          }
        }
      }
    }
      
    UI();
      
      m_pTimer->Tick([&]() { //all time-dependent function calls should go here
          m_pObjectManager->move(); //move all objects
          
          if (m_pPlayer) {
              
              if (!m_pPlayer->m_bIsTakingDamage)
                  FollowCamera(); //make camera follow player
          }
          else
              FollowCamera(); //make camera follow player
          
          // if there are any particles waiting in min heap, check the trigger time.
          // Create particle and remove CTimedSpawn object from min heap if trigger
          if (!m_pTimedEvents.empty()) {
              /*DEBUG printf("%f\n%f\n%f\n%f\n\n", m_pTimer->GetTime(), (float)(m_pTimedEvents.top().GetTime()), m_pTimedEvents.top().GetPart().m_vPos.x,
                  m_pTimedEvents.top().GetPart().m_vPos.y);*/ 
              while (!m_pTimedEvents.empty() && (m_pTimer->GetTime()) >= (m_pTimedEvents.top().GetTime())) {    //TODO? optimize 
                  m_pParticleEngine->create(m_pTimedEvents.top().GetPart());
                  m_pTimedEvents.pop();
              }
          }

          m_pParticleEngine->step(); //advance particle animation
          });
  }
  else {
      
  }
  RenderFrame(); //render a frame of animation
  ProcessGameState(); //check for end of game
} //ProcessFrame

/// <summary>
/// Updates the state of the game (such as if the player dies) to control game flow logic.
/// </summary>

void CGame::ProcessGameState() {
  static float t = 0; //time at start of game

  switch (m_eGameState) {
  case eGameState::Playing:
    if (m_pPlayer == nullptr) {
      m_eGameState = eGameState::Waiting; //now waiting
      t = m_pTimer->GetTime(); //start wait timer
    } else if (m_pPlayer->GetPort() != 0) {
        /*if (!m_bInMenu) {
            m_pObjectManager->create(eSprite::FadeOut, Vector2(m_pRenderer->GetCameraPos().x, m_pRenderer->GetCameraPos().y));
            m_bInMenu = true;
        }
        if (m_pObjectManager->GetFadeOut()) {
            printf("MPObj FadeOut()\n");
            m_pObjectManager->GetFadeOut()->move();
            if (m_pObjectManager->GetFadeOut()->IsDoneAnimating()) {
                printf("Done animating FadeOut\n");
                m_bInMenu = false;
                m_eGameState == eGameState::Waiting;
                Goto(m_pPlayer->GetPort());
                t = m_pTimer->GetTime();
            }
        }*/
        m_bInMenu = false;
        m_eGameState == eGameState::Waiting;
        Goto(m_pPlayer->GetPort());
        t = m_pTimer->GetTime();
    }
    else if (m_pObjectManager->GetEnemyCount() == 0) {
        // TODO: Add sound effects for clearing a level
        if (m_pObjectManager->GetNextMap()) {
            m_pObjectManager->GetNextMap()->Activate();
        }
        if (m_pObjectManager->GetPreviousMap()) {
            m_pObjectManager->GetPreviousMap()->Activate();
        }
    }
    else {
        if (!m_bInMenu && !m_bFadeInAnimated) {
            m_pObjectManager->create(eSprite::FadeIn, Vec3toVec2(m_pRenderer->GetCameraPos()));
            m_bInMenu = true;
        }
        if (m_pObjectManager->GetFadeIn()) {
            printf("Arrived at GetFadeIn\n");
            m_pObjectManager->GetFadeIn()->move();
            if (m_pObjectManager->GetFadeIn()->IsDoneAnimating() && !m_bFadeInAnimated) {
                printf("Done animating FadeIn\n");
                m_bInMenu = false;
                m_bFadeInAnimated = true;
            }
        }
    }
    break;

  case eGameState::Waiting:
    if (m_pTimer->GetTime() - t > 0.0f) { //0 seconds has elapsed since level end
        if (!m_bGameOverScreen) {
            GameOver();
        }
    } //if
    break;
  case eGameState::Menu:
      break;
  } //switch
} //CheckForEndOfGame