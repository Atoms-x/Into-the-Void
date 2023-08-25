#ifndef __L4RC_GAME_KING_SLIME_H__
#define __L4RC_GAME_KING_SLIME_H__

#include "Object.h"
#include "Common.h"

class KingSlime : public CObject {
	friend class CObjectManager; /// To spawn in slimes
	friend class CPlayer; /// To move towards the player
protected:
	UINT m_nMaxHealth; /// The maximum health the king will have
	UINT m_nHealth; /// The dynamic health of the king

	UINT m_nJigglyWiggly;
	UINT m_nTotalJiggles;
	UINT m_nHopSpawnAmt; /// Amount of slimes to spawn on hop
	UINT m_nDeathSpawnAmt; /// Amount of slimes to spawn on death
	float m_fSpawnChance; /// Chance to spawn slimes on hop
	float m_fEyesight; /// The range that the king slime can see

	LEventTimer* m_pFrameEvent = nullptr;
	LEventTimer* m_pHopEvent = nullptr;

	eSlimeState state{ eSlimeState::NONE };

	virtual void CollisionResponse(const Vector2&, float, CObject* = nullptr);

	void HopTowardsPlayer();
	void HopRandomly();
	void UpdateFrame();
	void HandleFrameLoop(size_t);
	float DistanceToPlayer(); /// The distance between the king and the player
public:
	KingSlime(const Vector2&);
	~KingSlime();

	void idle();
	void splitfx();
	void hurtfx();
	virtual void move();
};

#endif