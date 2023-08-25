#include "KingSlime.h"
#include "Object.h"
#include "ComponentIncludes.h"
#include "Player.h"
#include "Particle.h"
#include "ParticleEngine.h"
#include "ObjectManager.h"
#include "Helpers.h"

/// <summary>
/// Collision detection and logic for the KingSlime class
/// </summary>
/// <param name="norm"></param>
/// <param name="d"></param>
/// <param name="pObj"></param>

void KingSlime::CollisionResponse(const Vector2& norm, float d, CObject* pObj) {
	if (m_bDead) return;

	if (pObj && (pObj->GetType() == (UINT)eObjectType::BULLET || pObj->GetType() == (UINT)eObjectType::SWORD) && m_bIsTarget) {
		if (--m_nHealth == 0) {
			m_pAudio->play(eSound::Thump);
			hurtfx();
			state = eSlimeState::DEATH;
		}
		else {
			if (m_nHealth == 5) {
				Vector2 temp;
				for (int i = 0; i < m_nDeathSpawnAmt; i++) {
					splitfx();
					temp = GenRandomPosition(0.6);
					temp.y -= 96.0f;
					m_pObjectManager->create(eSprite::SlimeBig, temp);
				}
			}
			m_pAudio->play(eSound::Thump);
			hurtfx();
		}
		const float tint = 0.5f + 0.5f * (float)m_nHealth / (float)m_nMaxHealth;
		m_f4Tint = XMFLOAT4(1.0f, tint, tint, 0);
	}
	else if (!pObj) {
		CObject::CollisionResponse(norm, d, pObj);
	}
}

/// <summary>
/// This function updates the velocity of
/// the KingSlime object based on where the Player
/// is in the game world. It then normalizes the
/// distance vector and applies a speed to give it
/// a constant uniform speed.
/// </summary>

void KingSlime::HopTowardsPlayer() {
	Vector2 temp;
	if (!m_pPlayer)
		return;
	printf("Hopping towards player\n");
	state = eSlimeState::HOP;
	if (m_nSpriteIndex != (UINT)eSprite::KingHop) {
		m_nSpriteIndex = (UINT)eSprite::KingHop;
	}
	m_nCurrentFrame = 0;
	m_vVelocity.x = m_pPlayer->GetPos().x - m_vPos.x;
	m_vVelocity.y = m_pPlayer->GetPos().y - m_vPos.y;
	m_vVelocity.Normalize();
	m_vVelocity *= 70.0f;
	if (m_nHealth <= 5)
		m_vVelocity *= 2.0f;
	if (m_pRandom->randf() < m_fSpawnChance) {
		for (int i = 0; i < m_nHopSpawnAmt; i++) {
			splitfx();
			temp = GenRandomPosition(0.6);
			temp.y -= 96.0f;
			m_pObjectManager->create(eSprite::SlimeBig, temp);
		}
	}
}

/// <summary>
/// If its not around the player, it just hops in
/// any damn direction it wants.
/// </summary>

void KingSlime::HopRandomly() {
	printf("Hopping randomly\n");
	state = eSlimeState::HOP;
	if (m_nSpriteIndex != (UINT)eSprite::KingHop) {
		m_nSpriteIndex = (UINT)eSprite::KingHop;
	}
	m_nCurrentFrame = 0;
	m_vVelocity.x = 34.0f;
	m_vVelocity.y = 34.0f;
	if (m_nHealth <= 5)
		m_vVelocity *= 2.0f;
	if (m_pRandom->randn(0, 1) == 1) {
		m_vVelocity = RotateVector(m_vVelocity, m_pRandom->randf() * 5.0f);
	}
}

/// <summary>
/// Updates the animation frame and applies
/// the tint effect based on how damaged it is.
/// </summary>

void KingSlime::UpdateFrame(){
	const UINT n_frames = (UINT)m_pRenderer->GetNumFrames(m_nSpriteIndex);

	if (n_frames > 1 && m_pFrameEvent && m_pFrameEvent->Triggered()) {
		if ((m_nCurrentFrame + 1) >= n_frames) {
			HandleFrameLoop(n_frames);
		}
		else {
			m_nCurrentFrame = (m_nCurrentFrame + 1);
			if (state == eSlimeState::DEATH && m_nSpriteIndex == (UINT)eSprite::SlimeSplit) {
				if (m_nCurrentFrame > 2 && m_nCurrentFrame <= 7) {
					splitfx();
				}
				const float tint = (0.5f + (0.5f * (float)(m_nCurrentFrame) / (n_frames - 1)));
				m_f4Tint = XMFLOAT4(1.0f, tint, tint, 0);
				m_fXScale -= 0.2f * (float)m_nCurrentFrame / (n_frames - 1);
				m_fYScale -= 0.2f * (float)m_nCurrentFrame / (n_frames - 1);
			}
		}
	}
}


/// <summary>
/// Logic for when the Slime splits 
/// into smaller slimes.
/// </summary>

void KingSlime::splitfx() {
	const int count = m_pRandom->randn(6, 12);
	UINT randSpeed, randSize;
	float x, y;
	LParticleDesc2D m_part, e_part;
	for (int i = 0; i < count; i++) {
		m_part.m_nSpriteIndex = (UINT)eSprite::SlimeParticles;
		m_part.m_nCurrentFrame = m_pRandom->randn(0, 8);

		m_part.m_f4Tint = m_f4Tint;

		m_part.m_fFadeOutFrac = 0.05f;
		m_part.m_fFadeInFrac = 0.4f;

		m_part.m_vPos = GenRandomPosition(0.05f);
		m_part.m_vPos.y -= m_fRadius * 0.75f;

		m_part.m_vVel = m_pRandom->randv();

		randSpeed = m_pRandom->randn(60, 300);
		randSize = m_pRandom->randn(6, 14);

		m_part.m_vVel.x *= randSpeed;
		m_part.m_vVel.y *= randSpeed;

		m_part.m_fMaxScale = 3.0f;
		m_part.m_fMaxScale *= 0.13f * randSize;

		m_part.m_fLifeSpan = 0.8f - (0.0006f * (float)randSpeed);

		x = m_part.m_vPos.x + (m_part.m_vVel.x * m_part.m_fLifeSpan);
		y = m_part.m_vPos.y + (m_part.m_vVel.y * m_part.m_fLifeSpan);

		e_part = m_part;

		e_part.m_vPos = Vector2(x, y);
		e_part.m_vVel.x *= 0.05f;
		e_part.m_vVel.y *= 0.05f;
		e_part.m_fLifeSpan = 3.5f;
		e_part.m_fFadeInFrac = 0.00f;
		e_part.m_fFadeOutFrac = 0.95f;
		m_pTimedEvents.emplace(CTimedSpawn::CTimedSpawn(m_pTimer->GetTime() + m_part.m_fLifeSpan - 0.05f, e_part));
		m_pParticleEngine->create(m_part);
	}
}

/// <summary>
/// Handles the damage frames
/// </summary>

void KingSlime::hurtfx() {
	state = eSlimeState::HURT;
	if (m_nSpriteIndex != (UINT)eSprite::KingHurt) {
		m_nSpriteIndex = (UINT)eSprite::KingHurt;
	}
	m_nCurrentFrame = 0;
	m_vVelocity.x = 4.0f;
	m_vVelocity.y = 4.0f;
	m_bIsTarget = false;
}

void KingSlime::HandleFrameLoop(size_t n) {
	Vector2 temp;
	switch (state) {
	case eSlimeState::IDLE:
		m_nCurrentFrame = 0;
		break;
	case eSlimeState::HOP:
	case eSlimeState::HURT:
		idle();
		break;
	case eSlimeState::DEATH:
		m_nJigglyWiggly++;
		if (m_nJigglyWiggly < m_nTotalJiggles) {
			hurtfx();
			splitfx();

			state = eSlimeState::DEATH;
			return;
		}

		if (state == eSlimeState::NONE) {
			splitfx();
			return;
		}
		
		state = eSlimeState::NONE;
		m_bDead = true;

		for (int i = 0; i < m_nDeathSpawnAmt; i++) {
			temp = GenRandomPosition(0.6);
			temp.y -= 96.0f;
			m_pObjectManager->create(eSprite::SlimeBig, temp);
		}
		m_nCurrentFrame = n - 1;
		break;
	}
}

/// <summary>
/// Gets the distance from the king slime to the player
/// </summary>
/// <returns></returns>

float KingSlime::DistanceToPlayer()
{
	if (m_pPlayer) {
		printf("Distance to player: %f\n", (m_vPos - m_pPlayer->GetPos()).Length());
		return (m_vPos - m_pPlayer->GetPos()).Length();
	}
	else {
		return 0.0f;
	}
}

KingSlime::KingSlime(const Vector2& pos) : CObject(eSprite::KingIdle, pos) {
	objType = (UINT)eObjectType::SLIME;
	state = eSlimeState::IDLE;
	m_nJigglyWiggly = 0; // the number of jigglies per wiggly
	m_nTotalJiggles = 5;
	m_fRoll = XM_2PI;
	m_vVelocity = Vector2(0.0f, 0.0f);
	//m_fRadius *= 1.8;
	m_fXScale = m_fYScale = 4.0f;
	m_fRadius *= 0.8f;
	m_pFrameEvent = new LEventTimer(0.2f);
	m_pHopEvent = new LEventTimer(1.2f);
	m_bIsTarget = true;
	m_bStatic = false;
	m_nMaxHealth = 10;
	m_nHealth = m_nMaxHealth;
	m_nHopSpawnAmt = 2;
	m_nDeathSpawnAmt = 2;
	m_fSpawnChance = 0.1;
	m_fEyesight = 700.0f;
}

KingSlime::~KingSlime() {
	delete m_pFrameEvent;
	delete m_pHopEvent;
}

void KingSlime::idle() {
	state = eSlimeState::IDLE;
	if (m_nSpriteIndex != (UINT)eSprite::KingIdle) {
		m_nSpriteIndex = (UINT)eSprite::KingIdle;
	}
	m_nCurrentFrame = 0;
	m_vVelocity = Vector2(5.0f, 5.0f);
	m_bIsTarget = true;
	m_pHopEvent->SetDelay((float)(5.0f * 0.8f));
	if(m_nHealth <= 5)
		m_pHopEvent->SetDelay((float)(3.5f * 0.8f));
}

void KingSlime::move() {
	printf("Can be hit? %d\n", m_bIsTarget);
	if (!m_bDead && !m_bStatic) {
		if (state == eSlimeState::IDLE && m_pHopEvent && m_pHopEvent->Triggered()) {
			if (DistanceToPlayer() <= m_fEyesight) {
				// the king can see the player
				HopTowardsPlayer();
			}
			else {
				// the king cannot see the player
				HopRandomly();
			}
		}
		UpdateFrame();
		m_vPos += m_vVelocity * m_pTimer->GetFrameTime();
	}
}
