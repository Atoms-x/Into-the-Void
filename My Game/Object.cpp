/// \file Object.cpp
/// \brief Code for the game object class CObject.

#include "Object.h"
#include "ComponentIncludes.h"
#include "GameDefines.h"
#include "Particle.h"
#include "ParticleEngine.h"
#include "Helpers.h"

/// Create and initialize an object given its sprite type and initial position.
/// \param t Type of sprite.
/// \param p Initial position of object.

CObject::CObject(eSprite t, const Vector2& p):
  LBaseObject(t, p)
{ 
  m_fRoll = XM_PIDIV2; //facing upwards
  m_bIsTarget = false; //not a target

  const float w = m_pRenderer->GetWidth(t); //sprite width
  const float h = m_pRenderer->GetHeight(t); //sprite height
  m_fRadius = std::max(w, h) / 2; //bounding circle radius

  m_pGunFireEvent = new LEventTimer(1.0f); //timer for firing gun
} //constructor

/// Destructor.
CObject::~CObject(){
  delete m_pGunFireEvent;
} //destructor

/// Move object an amount that depends on its velocity and the frame time.

void CObject::move(){
  if(!m_bDead && !m_bStatic)
    m_vPos += m_vVelocity*m_pTimer->GetFrameTime();
} //move

/// Ask the renderer to draw the sprite described in the sprite descriptor.
/// Note that `CObject` is derived from `LBaseObject` which is inherited from
/// `LSpriteDesc2D`. Therefore `LSpriteRenderer::Draw(const LSpriteDesc2D*)`
/// will accept `this` as a parameter, automatically down-casting it from
/// `CObject*` to `LSpriteDesc2D*`, effectively drawing the object from its
/// sprite descriptor.

void CObject::draw(){ 
  m_pRenderer->Draw(this);
} //draw

/// Response to collision. Move back the overlap distance along the collision
/// normal. 
/// \param norm Collision normal.
/// \param d Overlap distance.
/// \param pObj Pointer to object being collided with (defaults to `nullptr`,
/// which means collision with a wall).

void CObject::CollisionResponse(const Vector2& norm, float d, CObject* pObj){
  if(m_bDead)return; //dead, bail out
  if (pObj) {
      if (pObj->GetType() == (UINT)eObjectType::UI) return;
      if (pObj->GetType() == (UINT)eObjectType::MANAPOT) return;
      if (pObj->GetType() == (UINT)eObjectType::HEALTHPOT) return;
  }

  const Vector2 vOverlap = d*norm; //overlap in direction of this
  const bool bStatic = !pObj || pObj->m_bStatic; //whether other object is static

  if(!m_bStatic && !bStatic) //both objects are dynamic
    m_vPos += vOverlap/2; //back off this object by half

  else if(!m_bStatic && bStatic) //only this object is dynamic
    m_vPos += vOverlap; //back off this object
  
} //CollisionResponse

/// Create a particle effect to mark the death of the object.
/// This function is a stub intended to be overridden by various object classes
/// derived from this class.

void CObject::DeathFX(){
 //stub
} //DeathFX

/// Compute the view vector from the object orientation.
/// \return The view vector.

const Vector2 CObject::GetViewVector() const{
  return AngleToVector(m_fRoll);
} //ViewVector

const Vector2 CObject::GetFacingVector() const {
  return AngleToVector(m_fFacingVector);
}

const float CObject::GetFacingVectorRad() const {
  return m_fFacingVector;
}


/// <summary> 
/// Generates a random position within the callers radius.
/// takes float as parameter, which can shrink or grow the position area
/// </summary>
/// <returns> Vector2 position </returns>
const Vector2 CObject::GenRandomPosition(float radiusMultiplier) const {
    UINT randMult; 
    Vector2 randomPosition;
    randMult = m_pRandom->randn(0, 31); //gen random value
    //generate random x offset on object
    if (randMult < 16)  //offset positive
        randomPosition.x = this->m_vPos.x + (this->m_fRadius * radiusMultiplier) * (float)randMult / (float)16;
    else                //offset negative
        randomPosition.x = this->m_vPos.x - (this->m_fRadius * radiusMultiplier) * (float)randMult / (float)16;
    
    //generate random y offset on player
    randMult = m_pRandom->randn(0, 31);
    if (randMult < 16)  //offset positive
        randomPosition.y = this->m_vPos.y + (this->m_fRadius * radiusMultiplier) * (float)randMult / (float)16;
    else                //offset negative
        randomPosition.y = this->m_vPos.y - (this->m_fRadius * radiusMultiplier) * (float)randMult / (float)16;

    return randomPosition;
} //GenRandomPosition

/// <summary>
///  type enum accessor
/// </summary>
/// <returns>object type as UINT</returns>
const UINT CObject::GetType() const {
    return objType;
} //GetType

/// Reader function for bullet flag.
/// \return true if a bullet.

const bool CObject::isBullet() const{
    return (objType == (UINT)eObjectType::BULLET);
} //isBullet

/// Reader function for Sword attack flag
/// \return true if a sword attack

const bool CObject::isSword() const {
    return (objType == (UINT)eObjectType::SWORD);
} //isSword

const bool CObject::isPlayer() const {
    return (objType == (UINT)eObjectType::PLAYER);
} //isPlayer

const bool CObject::isHealthPotion() const {
    return (objType == (UINT)eObjectType::HEALTHPOT);
} //isHealthPotion

const bool CObject::isManaPotion() const {
    return (objType == (UINT)eObjectType::MANAPOT);
} //isManaPotion

const bool CObject::isSlime() const {
    return (objType == (UINT)eObjectType::SLIME);
} //isSlime

const bool CObject::GetIsDead() const {
  return m_bDeadCheck;
}
