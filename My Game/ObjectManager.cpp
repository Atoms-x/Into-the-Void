/// \file ObjectManager.cpp
/// \brief Code for the the object manager class CObjectManager.

#include "ObjectManager.h"
#include "ComponentIncludes.h"

#include "Player.h"
#include "Turret.h"
#include "Bullet.h"
#include "TurretBullet.h"
#include "Explosion.h"
#include "Explosion2.h"
#include "Explosion3.h"
#include "SwordAttack.h"
#include "Slime.h"
#include "LockTest.h"
#include "SlimeBig.h"
#include "ParticleEngine.h"
#include "Helpers.h"
#include "GameDefines.h"
#include "TileManager.h"
#include "HealthBar.h"
#include "HealthPotion.h"
#include "ManaPotion.h"
#include "ManaBar.h"
#include "MainMenuExit.h"
#include "MainMenuBG.h"
#include "MainMenuStart.h"
#include "Obstacle.h"
#include "Key.h"
#include "NextMapPorter.h"
#include "KingSlime.h"
#include "PrevMapPorter.h"
#include "Rabite.h"
#include "OakSeed.h"
#include "FadeInObject.h"
#include "FadeOutObject.h"
#include "GameOverObject.h"
#include "EndingScreen.h"


/// Create an object and put a pointer to it at the back of the object list
/// `m_stdObjectList`, which it inherits from `LBaseObjectManager`.
/// \param t Sprite type.
/// \param pos Initial position.
/// \return Pointer to the object created.

CObject* CObjectManager::create(eSprite t, const Vector2& pos) {
  CObject* pObj = nullptr;

  switch (t) { //create object of type t
    case eSprite::Player:  pObj = new CPlayer(pos); break;
    case eSprite::Turret:  pObj = new CTurret(eSprite::Turret, pos); break;
    case eSprite::Bullet:  pObj = new CBullet(eSprite::Fireball, pos); break;
    case eSprite::Explosion: pObj = new CExplosion(eSprite::Explosion, pos); break;
    case eSprite::Explosion2: pObj = new CExplosion2(eSprite::Explosion2, pos); break;
    case eSprite::Explosion3: pObj = new CExplosion3(eSprite::Explosion4, pos); break;
    case eSprite::Bullet2: pObj = new CTurretBullet(eSprite::TurretFireball, pos); break;
    case eSprite::Slime:   pObj = new CSlime(pos, 1); break;
    case eSprite::SlimeBig:pObj = new CSlimeBig(pos, 3); break;
    case eSprite::SlimeKing: pObj = new KingSlime(pos); break;
    case eSprite::Rabite: pObj = new CRabite(eSprite::RabiteIdleDown, pos); break;
    case eSprite::OakSeed: pObj = new COakSeed(eSprite::OakSeedIdleDown, pos); break;
    case eSprite::HealthPotion:pObj = new CHealthPotion(pos); break;
    case eSprite::ManaPotion:pObj = new CManaPotion(pos); break;
    case eSprite::Exit:pObj = new CMainMenuExit(pos); break;
    case eSprite::BG:pObj = new CMainMenuBG(pos); break;
    case eSprite::Start:pObj = new CMainMenuStart(pos); break;
    case eSprite::Key:pObj = new CKey(pos); break;
    case eSprite::LockTest:pObj = new CLockTest(pos, eSprite::LockTest); break;
    case eSprite::Swing:pObj = new CSwordAttack(eSprite::Swing, pos); break;
    case eSprite::TestSwing:pObj = new CSwordAttack(eSprite::Swing, pos); break;
    case eSprite::NextPortal:pObj = new NextMapPorter(pos); break;
    case eSprite::PrevPortal:pObj = new PrevMapPorter(pos); break;
    case eSprite::FadeIn: pObj = new FadeInObject(pos); break;
    case eSprite::FadeOut: pObj = new FadeOutObject(pos); break;
    case eSprite::GameOver: pObj = new GameOverObject(pos); break;
    case eSprite::EndingScreen: pObj = new EndingScreen(pos); break;
    case eSprite::Titlecard: pObj = new TitleCard(pos); break;
    case eSprite::Spikes: {
        pObj = new Obstacle(pos, 0, eSprite::Spikes);
        Obstacle* p = (Obstacle*)pObj;
        p->FixBoundingBox(0.02f);
    } break;
 /* case eSprite::PlayerIdleRight: pObj = new CPlayer(eSprite::PlayerIdleRight, pos); break;
  case eSprite::PlayerRunRight: pObj = new CPlayer(eSprite::PlayerRunRight, pos); break;*/
  default: pObj = new CObject(t, pos);
  } //switch

  m_stdObjectList.push_back(pObj); //push pointer onto object list
  return pObj; //return pointer to created object
} //create

/// Test whether an object's left, right, top or bottom edge has crossed the 
/// left, right, top, bottom edge of the world, respectively. If so, then the
/// object's position is corrected. This function assumes that the bottom left
/// corner of the world is at the origin.
/// \param p Pointer to an object.
/// \param norm [out] Collision normal.
/// \param d [out] Overlap distance.
/// \return true if the object overlaps the edge of the world.

//bool CObjectManager::AtWorldEdge(CObject* p, Vector2& norm, float& d) const{ 
//  d = 0; //safety
//
//  float w, h; //for sprite width and height
//  m_pRenderer->GetSize(p->m_nSpriteIndex, w, h); //get sprite width and height
//  w *= p->m_fXScale/2; //scaled half width
//  h *= p->m_fYScale/2; //scale half height
//        
//  if(p->m_vPos.x < w){ //left edge
//    norm = Vector2::UnitX;
//    d = w - p->m_vPos.x;
//  } //if
//
//  else if(p->m_vPos.x > m_vWorldSize.x - w){ //right edge
//    norm = -Vector2::UnitX;
//    d = p->m_vPos.x - m_vWorldSize.x + w;
//  } //else if
//
//  else if(p->m_vPos.y < h){ //bottom edge
//    norm = Vector2::UnitY;
//    d = h - p->m_vPos.y;
//  } //else if
//
//  else if(p->m_vPos.y > m_vWorldSize.y - h){ //top edge
//    norm = -Vector2::UnitY;
//    d = p->m_vPos.y - m_vWorldSize.y + h;
//  } //else if
//
//  return d > 0;
//} //AtWorldEdge

/// Draw the tiled background and the objects in the object list.

void CObjectManager::draw() {
  m_pTileManager->Draw(); //draw tiled background
  m_pTileManager->DrawOver();
  m_bDrawAABBs = false;
  if (m_bDrawAABBs)
    m_pTileManager->DrawBoundingBoxes(eSprite::Line); //draw AABBs

  LBaseObjectManager::draw();
} //draw

/// Perform collision detection and response for each object with the world
/// edges and for all objects with another object, making sure that each pair
/// of objects is processed only once.

//void CObjectManager::BroadPhase(){
//  LBaseObjectManager::BroadPhase(); //collide with other objects
//
//  //collide with walls
//
//  for(CObject* pObj: m_stdObjectList) //for each object
//    if(!pObj->m_bDead){ //for each non-dead object, that is
//      for(int i=0; i<2; i++){ //can collide with 2 edges simultaneously
//        Vector2 norm; //collision normal
//        float d = 0; //overlap distance
//
//        if(AtWorldEdge(pObj, norm, d)) //collide with world edge
//          pObj->CollisionResponse(norm, d); //respond 
//      } //for
//  } //for
//} //BroadPhase

void CObjectManager::BroadPhase() {
  LBaseObjectManager::BroadPhase(); //collide with other objects

  //collide with walls

  for (CObject* pObj : m_stdObjectList) //for each object
    if (!pObj->m_bDead) { //for each non-dead object, that is
      for (int i = 0; i < 2; i++) { //can collide with 2 edges simultaneously
        Vector2 norm; //collision normal
        float d = 0; //overlap distance
        BoundingSphere s(Vector3(pObj->m_vPos), pObj->m_fRadius);

        if (m_pTileManager->CollideWithWall(s, norm, d)) //collide with wall
          pObj->CollisionResponse(norm, d); //respond 
      } //for
    } //for
} //BroadPhase


/// Perform collision detection and response for a pair of objects. Makes
/// use of the helper function Identify() because this function may be called
/// with the objects in an arbitrary order.
/// \param p0 Pointer to the first object.
/// \param p1 Pointer to the second object.

void CObjectManager::NarrowPhase(CObject* p0, CObject* p1) {
  Vector2 vSep = p0->m_vPos - p1->m_vPos; //vector from *p1 to *p0
  const float d = p0->m_fRadius + p1->m_fRadius - vSep.Length(); //overlap

  if (d > 0.0f) { //bounding circles overlap
    vSep.Normalize(); //vSep is now the collision normal

    p0->CollisionResponse(vSep, d, p1); //this changes separation of objects
    p1->CollisionResponse(-vSep, d, p0); //same separation and opposite normal
  } //if
} //NarrowPhase

/// Create a bullet object and a flash particle effect. It is assumed that the
/// object is round and that the bullet appears at the edge of the object in
/// the direction that it is facing and continues moving in that direction.
/// \param pObj Pointer to an object.
/// \param bullet Sprite type of bullet.

void CObjectManager::FireGun(CObject* pObj, eSprite bullet) {
  m_pAudio->play(eSound::Magic);

  //const Vector2 view = pObj->GetViewVector(); //firing object view vector
  const Vector2 view = pObj->GetFacingVector(); //firing object in facing vector
  const float w0 = 0.5f * m_pRenderer->GetWidth(pObj->m_nSpriteIndex); //firing object width
  const float w1 = m_pRenderer->GetWidth(bullet); //bullet width
  const Vector2 pos = pObj->m_vPos + (w0 + w1 + 10) * view; //bullet initial position. ARBITRARY 5 FOR NOW

  //create bullet object

  CObject* pBullet = create(bullet, pos); //create bullet

  const Vector2 norm = VectorNormalCC(view); //normal to view direction
  const float m = 2.0f * m_pRandom->randf() - 1.0f; //random deflection magnitude
  const Vector2 deflection = 0.01f * m * norm; //random deflection

  pBullet->m_vVelocity = pObj->m_vVelocity + 500.0f * (view + deflection);
  //pBullet->m_fRoll = pObj->m_fRoll; 
  pBullet->m_fRoll = pObj->m_fFacingVector;

  //particle effect for gun fire

  //LParticleDesc2D d;

  //d.m_nSpriteIndex = (UINT)eSprite::Spark;
  //d.m_fRoll = pObj->m_fFacingVector;
  //d.m_vPos = pos;
  //d.m_vVel = pObj->m_fSpeed * view;
  //d.m_fLifeSpan = 0.25f;
  //d.m_fScaleInFrac = 0.4f;
  //d.m_fFadeOutFrac = 0.5f;
  //d.m_fMaxScale = 0.5f;
  //d.m_f4Tint = XMFLOAT4(Colors::Yellow);

  //m_pParticleEngine->create(d);
} //FireGun

void CObjectManager::SwingSword(CObject* pObj, eSprite sword) {
  m_pAudio->play(eSound::Swing);
  const Vector2 view = pObj->GetFacingVector(); //firing object in facing vector
  const float w0 = 0.5f * m_pRenderer->GetWidth(pObj->m_nSpriteIndex); //firing object width
  const float w1 = m_pRenderer->GetWidth(sword); //bullet width
  const Vector2 pos = pObj->m_vPos + (w0 + w1 + 20) * view; //bullet initial position. ARBITRARY 5 FOR NOW

  CObject* pSword = create(sword, pos); //create sword

  const Vector2 norm = VectorNormalCC(view); //normal to view direction
  const float m = 2.0f * m_pRandom->randf() - 1.0f; //random deflection magnitude
  const Vector2 deflection = 0.01f * m * norm; //random deflection

  pSword->m_fRoll = pObj->m_fFacingVector;
  //pSword->m_vVelocity = Vector2(10.0f * cos(pObj->m_fFacingVector), 10.0f * sin(pObj->m_fFacingVector)); //Can drift it forward slightly
}

/// Find target closest to a position.
/// \param pos [in] Position.
/// \param pObj [out] Pointer to closest target, `NULL` if not found.
/// \param dsq Distance to closest object squared.

//void CObjectManager::FindClosest(const Vector2& pos, CObject*& pObj, float& dsq){
//  pObj = nullptr; //for closest object
//  dsq = FLT_MAX; //for closest distance squared
//
//  for(auto const& p: m_stdObjectList) { //for each object
//    const float dsq2 = (pos - p->m_vPos).LengthSquared(); //distance squared
//
//    if(p->m_bIsTarget && dsq2 < dsq){ //is closer target
//      pObj = p; //new closest target
//      dsq = dsq2; //new closest distance squared
//    } //if
//  } //for
//} //FindClose

const size_t CObjectManager::GetNumTurrets() const {
  size_t n = 0; //number of turrets

  for (CObject* pObj : m_stdObjectList) //for each object
    if (pObj->m_nSpriteIndex == (UINT)eSprite::Turret)
      n++;

  return n;
} //GetNumTurrets

const size_t CObjectManager::GetEnemyCount() const {
    size_t n = 0; //number of enemies

    for (CObject* pObj : m_stdObjectList) //for each object
        if (pObj->objType == (UINT)eObjectType::TURRET ||
            pObj->objType == (UINT)eObjectType::SLIME ||
            pObj->objType == (UINT)eObjectType::OAKSEED ||
            pObj->objType == (UINT)eObjectType::RABITE)
            n++;

    return n;
} //GetEnemyCount

NextMapPorter* CObjectManager::GetNextMap() {
    NextMapPorter* next_map;
    for (CObject* pObj : m_stdObjectList) {
        if (pObj->m_nSpriteIndex == (UINT)eSprite::NextPortal) {
            next_map = (NextMapPorter*)pObj;
            return next_map;
        }
    }
    return nullptr;
}

PrevMapPorter* CObjectManager::GetPreviousMap() {
    PrevMapPorter* prev_map;
    for (CObject* pObj : m_stdObjectList) {
        if (pObj->m_nSpriteIndex == (UINT)eSprite::PrevPortal) {
            prev_map = (PrevMapPorter*)pObj;
            return prev_map;
        }
    }
    return nullptr;
}

FadeOutObject* CObjectManager::GetFadeOut()
{
    FadeOutObject* fade_out;
    for (CObject* pObj : m_stdObjectList) {
        if (pObj->m_nSpriteIndex == (UINT)eSprite::FadeOut) {
            fade_out = (FadeOutObject*)pObj;
            return fade_out;
        }
    }
    return nullptr;
}

FadeInObject* CObjectManager::GetFadeIn()
{
    FadeInObject* fade_in;
    for (CObject* pObj : m_stdObjectList) {
        if (pObj->m_nSpriteIndex == (UINT)eSprite::FadeIn) {
            fade_in = (FadeInObject*)pObj;
            return fade_in;
        }
    }
    return nullptr;
}

TitleCard* CObjectManager::GetTitleCard() {
    TitleCard* title;
    for (CObject* pObj : m_stdObjectList) {
        if (pObj->m_nSpriteIndex == (UINT)eSprite::Titlecard) {
            title = (TitleCard*)pObj;
            return title;
        }
    }
    return nullptr;
}