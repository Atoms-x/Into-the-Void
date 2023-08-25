/// \file Slime.cpp
/// \brief Code for the Slime object class CSlime.

#include "Slime.h"
#include "ComponentIncludes.h"
#include "Particle.h"
#include "ParticleEngine.h"
#include "SimpleMath.h"
#include "Helpers.h"
#include "ObjectManager.h"

/// Create and initialize an Slime object given its initial position.
/// \param pos Initial position of slime.
/// \param maxHp starting hp of slime

CSlime::CSlime(const Vector2& pos, const UINT maxHp) : 
    CObject(eSprite::SlimeIdle, pos)  {
    objType = (UINT)eObjectType::SLIME; //set object type
    state = eSlimeState::IDLE; //initialize animation state to IDLE

    m_nMaxHealth = maxHp; ///< Maximum health.
    m_nHealth = m_nMaxHealth;///< Current health.

    m_fRoll = XM_2PI; //sprite rotation
    m_vVelocity = Vector2(0.0f, 0.0f); //still until first hop
    m_fXScale = m_fYScale = 2.0f; //scale
    //m_fRadius *= m_fXScale; //scale the bounding circle radius

    m_pFrameEvent = new LEventTimer(0.2f); //period of frame events for slime
    m_pHopEvent = new LEventTimer(1.6f); //period of hop event. on trigger, slime hops

    m_bIsTarget = true; //collides
    m_bStatic = false;  //not static
} //constructor

/// Destructor.
CSlime::~CSlime() {
    delete m_pFrameEvent;
    delete m_pHopEvent;
} //destructor


/// Change state to hop periodically. Advance frame num for current animation
void CSlime::move() {
    if (!m_bDead && !m_bStatic) {
        //call hop fn if hopevent triggered and state is IDLE
        if (state == eSlimeState::IDLE && m_pHopEvent && m_pHopEvent->Triggered())
            hop();
        UpdateFramenumber();
        m_vPos += m_vVelocity * m_pTimer->GetFrameTime(); //update position
    } //if

} //move


/// Transition to idle animation
void CSlime::idle() {
    state = eSlimeState::IDLE;
    if (m_nSpriteIndex != (UINT)eSprite::SlimeIdle)
        m_nSpriteIndex = (UINT)eSprite::SlimeIdle;
    m_nCurrentFrame = 0;
    m_vVelocity.x = 2.0f; //set velocity to low value
    m_vVelocity.y = 2.0f;
    m_bIsTarget = true; //disable i-frames if last state is HURT
    SetRandHopDelay(0.0f);
} //idle

/// Transition to hop animation
void CSlime::hop() {
    state = eSlimeState::HOP;
    if (m_nSpriteIndex != (UINT)eSprite::SlimeHop)
        m_nSpriteIndex = (UINT)eSprite::SlimeHop;
    m_nCurrentFrame = 0;
    m_vVelocity.x = 34.0f;  //set velocity to larger value
    m_vVelocity.y = 34.0f;
    StrayFromPath();    //randomize movement direction
} //hop

/// Transition to hop animation
void CSlime::hurt() {
    state = eSlimeState::HURT;
    if (m_nSpriteIndex != (UINT)eSprite::SlimeHurt)
        m_nSpriteIndex = (UINT)eSprite::SlimeHurt;
    m_nCurrentFrame = 0;
    m_vVelocity.x = 4.0f; //set velocity to low value
    m_vVelocity.y = 4.0f;
    m_bIsTarget = false;
} //hurt

/// Transition to death animation
void CSlime::death() {
    state = eSlimeState::DEATH;
    if (m_nSpriteIndex != (UINT)eSprite::SlimeDeath)
        m_nSpriteIndex = (UINT)eSprite::SlimeDeath;
    m_nCurrentFrame = 0;
    m_vVelocity.x = 3.0f; //set velocity to value that complements animation
    m_vVelocity.y = 0.0f;
    m_bIsTarget = false;
} //death

/// <summary>
/// Create slime blood particles as slime disintegrates. lots of randomness. 
/// </summary>
void CSlime::deathFx() {
    const int count = m_pRandom->randn(8, 16); //gen random num blood parts
    UINT randColor, randSize; //random vars
    LParticleDesc2D blood;
    for (int i = 0; i < count; i++) {
        blood.m_nSpriteIndex = (UINT)eSprite::CircleNoBorder;

        //fade properties
        blood.m_fFadeOutFrac = 0.8f;
        blood.m_fFadeInFrac = 0.2f;
        blood.m_fLifeSpan = 4.0f;

        //get random position within the slime sprite
        blood.m_vPos = GenRandomPosition(0.3f);

        //offset position to match sprite, account for scale
        blood.m_vPos.x -= m_fRadius * 0.25f;
        blood.m_vPos.y -= m_fRadius;

        randColor = m_pRandom->randn(0, 2);    //random color 
        randSize = m_pRandom->randn(0, 2);     //random size

        //select random color
        if (randColor == 0) 
            blood.m_f4Tint = XMFLOAT4(Colors::Black);
        else if(randColor == 1)
            blood.m_f4Tint = XMFLOAT4(Colors::DarkRed);
        else 
            blood.m_f4Tint = XMFLOAT4(Colors::Red);

        blood.m_f4Tint.w = 0.6f; //const alpha for all particles
        
        //select random size
        if (!randSize)
            blood.m_fMaxScale = 0.2f;
        else if (randSize == 1)
            blood.m_fMaxScale = 0.25f;
        else
            blood.m_fMaxScale = 0.3f;

        //spawn particle
        m_pParticleEngine->create(blood);
    } //for

} //deathFx

/////// Adjust direction randomly at random intervals.
void CSlime::StrayFromPath() {
    //TODO make slime move towards player
    const float t = m_pTimer->GetTime(); //current time

    UINT multiplier = m_pRandom->randn(1, 16);
    const float delta = multiplier * (m_bStrayParity ? -1.0f : 1.0f) * 0.4f; //angle delta

    m_vVelocity = RotateVector(m_vVelocity, delta); //change direction by delta
    //m_fRoll = XM_2PI;
    //m_fRoll += delta; //rotate to face that direction

    m_bStrayParity = m_pRandom->randf() < 0.5f; //next stray is randomly left or right 
} //StrayFromPath

///Set random time to hop. This time is a multiple of the idle animation to make it more predictable 
///an offset is taken as arg. If no offset, pass in 0.0f
void CSlime::SetRandHopDelay(float offset) {
    UINT wait = m_pRandom->randn(1, 4);
    m_pHopEvent->SetDelay((float)(wait * 0.8f) + offset);
} //SetRandHopDelay

/// Update the frame number in the animation sequence.
void CSlime::UpdateFramenumber() {
    const size_t n = m_pRenderer->GetNumFrames(m_nSpriteIndex); //number of frames
    if (n > 1 && m_pFrameEvent && m_pFrameEvent->Triggered()){
        if (m_nCurrentFrame + 1 >= n)
        {
            HandleFrameLoop(n); //call fn when animation reaches last frame
        } //if
        else
        {
            m_nCurrentFrame++;
            //if state death and sprite slimedeath
            if (state == eSlimeState::DEATH && m_nSpriteIndex == (UINT)eSprite::SlimeDeath) {
              m_bDeadCheck = true;
                if (m_nCurrentFrame > 2)
                    deathFx();  //spawn blood particles around corpse
                const float col = (0.5f - (0.5f * (float)(m_nCurrentFrame) / (float)(n - 1))); //gradually change color to a darker red as death plays
                m_f4Tint = XMFLOAT4((1.0f - 0.2f * col), col, col, 0);
                m_fXScale -= 0.2f * (float)m_nCurrentFrame / (float)(n - 1);   //slightly lower scale as death plays
                m_fYScale -= 0.2f * (float)m_nCurrentFrame / (float)(n - 1);
                //printf("%f\n%f\n\n", m_fXScale, m_fYScale); DEBUG
            } //if

        } //else

    } //if

} //UpdateFramenumber

/// Handle what happens when an animation reaches the last frame
void CSlime::HandleFrameLoop(size_t n) {
    switch (state) {
    case eSlimeState::IDLE:  //loop idle state
        m_nCurrentFrame = 0; 
        break;
    case eSlimeState::HOP:   //when hop or hurt anim complete, transition to idle state
    case eSlimeState::HURT:
        idle();
        break;
    case eSlimeState::DEATH: //when death animation completes, set sprite to last frame of death anim. mark dead and static
        m_bStatic = true;
        m_bDead = true;         //flag for destruction
        m_nCurrentFrame = (UINT)(n - 1);    //last frame of death anim
        const UINT chance = m_pRandom->randn(1, 3);
        if (chance == 1)
            m_pObjectManager->create(eSprite::HealthPotion, m_vPos);
        else if(chance == 2)
            m_pObjectManager->create(eSprite::ManaPotion, m_vPos);
        break;
    }
} //HandleFrameLoop

/// Response to collision. If the Slime is facing the object that is colliding
/// with, then it rotates in its preferred direction and then calls 
/// CObject::CollisionResponse. Note that the only reason that colliding
/// Slimes won't be facing each other is in the case of multiple Slimes colliding
/// and pushing one of them in some direction other than the one in which
/// it is facing. Howerver, faster objects such as the player objact can
/// collide from behind.
/// \param norm Collision normal.
/// \param d Overlap distance.
/// \param pObj Pointer to object being collided with (defaults to nullptr,
/// which means collision with a wall).
void CSlime::CollisionResponse(const Vector2& norm, float d, CObject* pObj) {
    if (m_bDead) return; //already dead, bail out 

    if (pObj && (pObj->isBullet() || pObj->GetType() == (UINT)eObjectType::SWORD) && m_bIsTarget) { //collision with bullet
        m_bIsTarget = false; //intangibility while in damaged animation
        if (--m_nHealth == 0) { //health decrements to zero means death 
            m_pAudio->play(eSound::Thump); //hurt sound
            death();
        } //if

        else { //not a death blow
            m_pAudio->play(eSound::Thump); //hurt sound
            hurt();
        } //else
        const float f = 0.5f + 0.5f * (float)m_nHealth / m_nMaxHealth; //health fraction
        m_f4Tint = XMFLOAT4(1.0f, f, f, 0); //redden the sprite to indicate damage
    } //if
    else if(!pObj)
        CObject::CollisionResponse(norm, d, pObj); //wall collision


} //CollisionResponse
