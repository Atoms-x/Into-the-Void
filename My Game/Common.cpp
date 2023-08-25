/// \file Common.cpp
/// \brief Code for the class CCommon.
///
/// This file contains declarations and initial values
/// for CCommon's static member variables.

#include "Common.h"

LSpriteRenderer* CCommon::m_pRenderer = nullptr;
CObjectManager* CCommon::m_pObjectManager = nullptr;
LParticleEngine2D* CCommon::m_pParticleEngine = nullptr;

std::priority_queue< CTimedSpawn, std::vector<CTimedSpawn>, std::greater<std::vector<CTimedSpawn>::value_type>> CCommon::m_pTimedEvents;
//std::priority_queue< CTimedSpawn, 
//	std::vector<CTimedSpawn>, 
//	std::less<std::vector<CTimedSpawn>::value_type>>* CCommon::m_pTimedEvents;

CTileManager* CCommon::m_pTileManager = nullptr;
bool CCommon::m_bDrawAABBs = false;
bool CCommon::m_bGodMode = false;
bool CCommon::m_bInteract = false;

Vector2 CCommon::m_vWorldSize = Vector2::Zero;
CPlayer* CCommon::m_pPlayer = nullptr;
CMainMenuExit* CCommon::m_pExitButton = nullptr;
CMainMenuStart* CCommon::m_pStartButton = nullptr;
CMainMenuBG* CCommon::m_pBackgroundObj = nullptr;
CHealthBar* CCommon::m_pHealthBar = nullptr;
CManaBar* CCommon::m_pManaBar = nullptr;
CStaminaWheel* CCommon::m_pStaminaWheel = nullptr;
CHealthPotion* CCommon::m_pHealthPotion = nullptr;
CManaPotion* CCommon::m_pManaPotion = nullptr;
CExplosion* CCommon::m_pExplosion = nullptr;
CExplosion2* CCommon::m_pExplosion2 = nullptr;
