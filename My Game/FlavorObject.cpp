#include "FlavorObject.h"

CFlavorObject::CFlavorObject(const Vector2& pos, eSprite sprite) : CObject(sprite, pos) {
	objType = (UINT)eObjectType::UNDEFINED; //TODO give type
	m_bStatic = true;
}	//TODO have constructor handle random gen

CFlavorObject::~CFlavorObject() {} ///< Destructor.

