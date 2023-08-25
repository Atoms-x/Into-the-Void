#include "TransferObject.h"

TransferObjectExit::TransferObjectExit(const Vector2& pos) : CObject(eSprite::Portal, pos) {
    m_nDirection = 0;
    objType = (UINT)eObjectType::TRANSPORT;
}

TransferObjectExit::~TransferObjectExit() {
}

void TransferObjectExit::SetDirection(int direction) {
    m_nDirection = direction;
}

int TransferObjectExit::TeleportPlayer() {
    return m_nDirection;
}

int TransferObjectExit::GetDirection() {
    return m_nDirection;
}

void TransferObjectExit::CollisionResponse(const Vector2& norm, float d, CObject* pObj) {
    if (m_bDead) return;
}
