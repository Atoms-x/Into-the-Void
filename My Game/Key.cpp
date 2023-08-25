#include "Key.h"
#include "ComponentIncludes.h"
#include "Helpers.h"
#include "Particle.h"
#include "ParticleEngine.h"
#include "Player.h"


/// Create and initialize a key object
/// \param p Initial position of key.

CKey::CKey(const Vector2& p) : CObject(eSprite::Key, p) {
    objType = (UINT)eObjectType::HEALTHPOT; //TODO refactor to ITEMDROP
    m_bIsTarget = true;
    m_bStatic = false;
    m_fXScale = m_fYScale = 0.8f;
    m_fRadius = m_fXScale;
    m_fRoll = 0.0f;
} //constructor

CKey::~CKey() {} //destructor

void CKey::CollisionResponse(const Vector2& norm, float d, CObject* pObj) {
    if (m_bDead)return; //already dead, bail out 
    CPlayer* player = (CPlayer*)pObj;
    if (pObj && pObj->isPlayer()) { //Collision with the player
        player->CollectKey();
        m_bDead = true; //mark object for deletion
        m_fRadius = 0.0f;
        m_pAudio->play(eSound::Acquire); //CHANGE OUT
        DeathFX();
    }

} //CollisionResponse

void CKey::DeathFX() {
    // Left for potential use
} //DeathFX

/// Reader function for position.
/// \return Position.

const Vector2& CKey::GetPos() const {
    return m_vPos;
} //GetPos

