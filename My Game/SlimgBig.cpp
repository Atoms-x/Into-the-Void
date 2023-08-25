/// \file Slime.cpp
/// \brief Code for the Slime object class CSlime.

#include "SlimeBig.h"
#include "ComponentIncludes.h"
#include "Particle.h"
#include "ParticleEngine.h"
#include "SimpleMath.h"
#include "ObjectManager.h"
#include "Helpers.h"

/// Create and initialize an Slime object given its initial position.
/// \param pos Initial position of slime
/// \param maxHp starting hp of slime
CSlimeBig::CSlimeBig(const Vector2& pos, const UINT maxHp) :
    CObject(eSprite::SlimeIdle, pos) {
    objType = (UINT)eObjectType::SLIME; //set object type
    state = eSlimeState::IDLE; //initialize animation state to IDLE

    m_nMaxHealth = maxHp; ///< Maximum health.
    m_nHealth = m_nMaxHealth; ///< Current health.
    m_nJiggle = 0; //initialize counter for death animation

    m_fRoll = XM_2PI;   //sprite rotation
    m_vVelocity = Vector2(0.0f, 0.0f); //still until first hop
    m_fXScale = m_fYScale = 3.0f; //scale
    //m_fRadius *= m_fXScale; //scale the bounding circle radius

    m_pFrameEvent = new LEventTimer(0.2f); //period of frame events for big slime
    m_pHopEvent = new LEventTimer(1.2f);   //period of hop event. on trigger, big slime hops

    m_bIsTarget = true; //collides
    m_bStatic = false;  //not static
} //constructor

/// Destructor.
CSlimeBig::~CSlimeBig() {
    delete m_pFrameEvent;
    delete m_pHopEvent;
} //destructor

/// Change state to hop periodically. Advance frame num for current animation
void CSlimeBig::move() {
    if (!m_bDead && !m_bStatic) {
        //call hop fn if hopevent triggered and state is IDLE
        if (state == eSlimeState::IDLE && m_pHopEvent && m_pHopEvent->Triggered())
            hop();
        UpdateFramenumber();

        m_vPos += m_vVelocity * m_pTimer->GetFrameTime(); //update position
    } //if

} //move

void CSlimeBig::idle() {
    state = eSlimeState::IDLE;
    if (m_nSpriteIndex != (UINT)eSprite::SlimeIdle)
        m_nSpriteIndex = (UINT)eSprite::SlimeIdle;
    m_nCurrentFrame = 0;
    m_vVelocity.x = 5.0f;  //set velocity to low value
    m_vVelocity.y = 5.0f;
    m_bIsTarget = true; //disable i-frames if last state is HURT
    SetRandHopDelay(0.0f);
    StrayFromPath();    //randomize movement direction
} //idle

/// Transition to hop animation
void CSlimeBig::hop() {
    state = eSlimeState::HOP;
    if (m_nSpriteIndex != (UINT)eSprite::SlimeHop)
        m_nSpriteIndex = (UINT)eSprite::SlimeHop;
    m_nCurrentFrame = 0;
    m_vVelocity.x = 34.0f;  //set velocity to larger value
    m_vVelocity.y = 34.0f;
    StrayFromPath();    //randomize movement direction
} //hop

/// Transition to hop animation
void CSlimeBig::hurt() {
    state = eSlimeState::HURT;
    if (m_nSpriteIndex != (UINT)eSprite::SlimeHurt)
        m_nSpriteIndex = (UINT)eSprite::SlimeHurt;
    m_nCurrentFrame = 0;
    m_vVelocity.x = 4.0f;
    m_vVelocity.y = 4.0f;
    m_bIsTarget = false;
} //hurt

/// Transition to split animation
void CSlimeBig::split() {
    state = eSlimeState::DEATH;
    if (m_nSpriteIndex != (UINT)eSprite::SlimeSplit)
        m_nSpriteIndex = (UINT)eSprite::SlimeSplit;
    m_nCurrentFrame = 0;
    m_vVelocity.x = 0.0f;
    m_vVelocity.y = 0.0f;
    m_bIsTarget = false;
} //split

/// Set velocity to random direction
void CSlimeBig::StrayFromPath() {
    //TODO make slime move towards player when within a certain distance
    const float t = m_pTimer->GetTime(); //current time

    UINT multiplier = m_pRandom->randn(1, 16);
    const float delta = multiplier * (m_bStrayParity ? -1.0f : 1.0f) * 0.4f; //angle delta

    m_vVelocity = RotateVector(m_vVelocity, delta); //change direction by delta

    m_bStrayParity = m_pRandom->randf() < 0.5f; //next stray is randomly left or right 
} //StrayFromPath

/// Set random time to hop. This time is a multiple of the idle animation - as a gameplay mechanic -
/// to make it more predictable. A time offset is taken as parameter. If no offset, pass in 0.0f
void CSlimeBig::SetRandHopDelay(float offset) {
    UINT wait = m_pRandom->randn(1, 5);
    m_pHopEvent->SetDelay((float)(wait * 0.8f) + offset);
} //SetRandHopDelay

/// Update the frame number in the animation sequence.
void CSlimeBig::UpdateFramenumber() {
    const UINT n = (UINT)m_pRenderer->GetNumFrames(m_nSpriteIndex); //number of frames
    if (n > 1 && m_pFrameEvent && m_pFrameEvent->Triggered()) { //if frame event and sprite is anim
        if ((m_nCurrentFrame + 1) >= n)
        {
            HandleFrameLoop(n); //call fn when animation reaches last frame
        } //if
        else
        {
            m_nCurrentFrame = (m_nCurrentFrame + 1);
            //if slime state DEATH and in split animation
            if(state == eSlimeState::DEATH && m_nSpriteIndex == (UINT)eSprite::SlimeSplit) {
                //create split particles during this porion of the animation
                if (m_nCurrentFrame > 2 && m_nCurrentFrame <= 7)
                    splitFx();

                //gradually change tint back to normal from reddened
                const float f = (0.5f + (0.5f  * (float)(m_nCurrentFrame)/(n-1))); 
                m_f4Tint = XMFLOAT4(1.0f, f, f, 0); 

                //scale down as split animation plays to match scale of small slimes
                m_fXScale -= 0.2f *(float)m_nCurrentFrame / (n - 1);
                m_fYScale -= 0.2f * (float)m_nCurrentFrame / (n - 1);
            } //if

        } //else

    } //if

} //UpdateFramenumber

/// spawn moving slime fragments on split. lots of randomness.
/// ending position of moving particles is calculated with the lifetime and vel.
/// adds particles to m_pTimedEvents for delayed spawn. 
void CSlimeBig::splitFx() {
    const int count = m_pRandom->randn(6, 12); //gen random num slime fragments
    UINT randSpeed, randSize; //random vars
    float endingX, endingY;
    LParticleDesc2D movingPart, endPart; //moving and delayed particles
    for (int i = 0; i < count; i++) {
        movingPart.m_nSpriteIndex = (UINT)eSprite::SlimeParticles;
        movingPart.m_nCurrentFrame = m_pRandom->randn(0, 8); //random sprite index

        //set tint to big slime tint
        movingPart.m_f4Tint = m_f4Tint;

        //fade properties
        movingPart.m_fFadeOutFrac = 0.05f;
        movingPart.m_fFadeInFrac = 0.4f;

        //random start position on slime sprite
        movingPart.m_vPos = GenRandomPosition(0.05f); //TODO update multiplier after hurtbox is scaled down
        movingPart.m_vPos.y -= m_fRadius * 0.75f; //offset

        //set velocity to random direction
        movingPart.m_vVel = m_pRandom->randv();

        randSpeed = m_pRandom->randn(60, 300);  //random speed
        randSize = m_pRandom->randn(6, 14);     //random size

        //apply random speed
        movingPart.m_vVel.x *= randSpeed;
        movingPart.m_vVel.y *= randSpeed;

        //apply random scale
        movingPart.m_fMaxScale = 1.0f;
        movingPart.m_fMaxScale *= 0.13f * randSize;

        //scale life span with speed, faster particles last longer
        movingPart.m_fLifeSpan = 0.4f - (0.0005f * (float)randSpeed);
 
        //store calculated final position in vector splitPositions 
        endingX = movingPart.m_vPos.x + (movingPart.m_vVel.x * movingPart.m_fLifeSpan);
        endingY = movingPart.m_vPos.y + (movingPart.m_vVel.y * movingPart.m_fLifeSpan);
        
        //copy moving particle properties for the particle after it "lands" on the ground
        endPart = movingPart;

        //tweak end particle properties
        endPart.m_vPos = Vector2(endingX, endingY);
        endPart.m_vVel.x *= 0.05f;
        endPart.m_vVel.y *= 0.05f;
        endPart.m_fLifeSpan = 3.5f;
        endPart.m_fFadeOutFrac = 0.95f;
        endPart.m_fFadeInFrac = 0.00f;

        //store ending particle in Common vector to be created later
        m_pTimedEvents.emplace(CTimedSpawn::CTimedSpawn(m_pTimer->GetTime() + movingPart.m_fLifeSpan - 0.05f, endPart));

        //spawn moving particle
        m_pParticleEngine->create(movingPart);
    } //for

} // splitFx

/// Handle what happens when an animation reaches the last frame
void CSlimeBig::HandleFrameLoop(int n) {
    switch (state) {
    case eSlimeState::IDLE:  //loop idle state
        m_nCurrentFrame = 0;
        break;
    case eSlimeState::HOP:   //when hop or hurt anim complete, transition to idle state
    case eSlimeState::HURT:
        idle();
        break;
    case eSlimeState::DEATH: //how to handle animation end when DEATH
        m_nJiggle++;    //increment jiggle counter
        if (m_nJiggle < 2) { //jiggles a couple times before splitting
            hurt(); //play hurt animation
            state = eSlimeState::DEATH; //override hurt fn state change
            return; //come back when you have more jiggle
        }

        //done jiggling, now split
        if (m_nSpriteIndex != (UINT)eSprite::SlimeSplit) { 
            split();    //transition to split anim
            return;
        }

        //could be useful
        state = eSlimeState::NONE;

        //done with jiggle and split, now spawn bb slimes and mark the big boi dead
        m_bDead = true;
        //create bb slimes
        m_pObjectManager->create(eSprite::Slime, Vector2(m_vPos.x + 25.0f, m_vPos.y - 2.0f));
        m_pObjectManager->create(eSprite::Slime, Vector2(m_vPos.x - 25.0f, m_vPos.y - 2.0f));
        m_nCurrentFrame = n - 1;    //set to last frame of death anim
        break;
    } //switch

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

void CSlimeBig::CollisionResponse(const Vector2& norm, float d, CObject* pObj) {
    if (m_bDead) return; //already dead, bail out 

    if (pObj && (pObj->GetType() == (UINT)eObjectType::BULLET || pObj->GetType() == (UINT)eObjectType::SWORD) && m_bIsTarget) { //collision with bullet
        if (--m_nHealth == 0) { //health decrements to zero means death 
            m_pAudio->play(eSound::Thump); //explosion
            hurt(); //jiggle
            state = eSlimeState::DEATH; //hp is 0, slime state is now DEATH
            DeathFX(); //particle effects
        } //if

        else { //not a death blow
            /* Big slime has less invincibility frames than small slimes */
            m_pAudio->play(eSound::Thump); //hurt sound
            hurt();
        } //else
        const float f = 0.5f + 0.5f * (float)m_nHealth / (float)m_nMaxHealth; //health fraction
        m_f4Tint = XMFLOAT4(1.0f, f, f, 0); //redden the sprite to indicate damage
    } //if
    else if (!pObj)
        CObject::CollisionResponse(norm, d, pObj); //wall collision
} //CollisionResponse

void CSlimeBig::DeathFX() {

    //LParticleDesc2D d; //particle descriptor
    //d.m_vPos = m_vPos; //center particle at turret center

    //d.m_nSpriteIndex = (UINT)eSprite::Smoke;
    //d.m_fLifeSpan = 2.0f;
    //d.m_fMaxScale = 4.0f;
    //d.m_fScaleInFrac = 0.5f;
    //d.m_fFadeOutFrac = 0.8f;
    //d.m_fScaleOutFrac = 0;
    //m_pParticleEngine->create(d);

    //d.m_nSpriteIndex = (UINT)eSprite::Spark;
    //d.m_fLifeSpan = 0.5f;
    //d.m_fMaxScale = 1.5f;
    //d.m_fScaleInFrac = 0.4f;
    //d.m_fScaleOutFrac = 0.3f;
    //d.m_fFadeOutFrac = 0.5f;
    //d.m_f4Tint = XMFLOAT4(Colors::Orange);
    //m_pParticleEngine->create(d);
} //DeathFX
