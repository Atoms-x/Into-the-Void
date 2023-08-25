/// \file TileManager.h
/// \brief Interface for the tile manager CTileManager.

#ifndef __L4RC_GAME_TILEMANAGER_H__
#define __L4RC_GAME_TILEMANAGER_H__

#include <vector>
#include <random>
#include <map>
#include <utility>

#include "Common.h"
#include "Settings.h"
#include "Sprite.h"
#include "GameDefines.h"

/// \brief The tile manager.
///
/// The tile manager is responsible for the tile-based background.

class CTileManager: 
  public CCommon, 
  public LSettings
{
  private:
    size_t m_nWidth = 0; ///< Number of tiles wide.
    size_t m_nHeight = 0; ///< Number of tiles high.
    UINT wallSpriteSheet = 8; ///< Indicates which wall sprites to draw

    float m_fTileSize = 0.0f; ///< Tile width and height.

    char** m_chMap = nullptr; ///< char arr to store the ground map 
    char** m_chMapMask = nullptr; ///< char arr to store the ground map mask values
    char** m_chFlavor = nullptr; ///< char arr to store the flavor txt
    char** m_chFlavorMask = nullptr; ///< char arr to store the flavor mask

    std::map<UINT, LSpriteDesc2D> groundMap;   ///< associates wall and floor sprites with characters and mapmask values
    std::map<UINT, LSpriteDesc2D> flavorValMap; ///< associates flavor objects with chars and mask vals

    std::vector<LSpriteDesc2D> m_vecDrawOver; ///< Sprites in here are drawn after the floors and walls

    std::vector<BoundingBox> m_vecWalls; ///< AABBs for the walls.

    std::vector<Vector2> m_vecTurrets; ///< Turret positions.
    std::vector<Vector2> m_vecSlimes; ///< Slime positions.
    std::vector<Vector2> m_vecBigSlimes; ///< Big Slime positions.
    std::vector<Vector2> m_vecKingSlimes; ///< King Slime positions.
    std::vector<Vector2> m_vecOakSeeds; ///< Oak Seed positions.
    std::vector<Vector2> m_vecRabites; ///< Rabite positions.

    Vector2 m_vPlayer; ///< Player location.
    Vector2 m_vEntryPortalPosition; ///< This is where the player enters the level
    Vector2 m_vExitPortalPosition; ///< This is where the player leaves the level
    std::vector<Vector2> m_vecSpikes;

    void MakeBoundingBoxes(); ///< Make bounding boxes for walls.
    void SetFloorCode(const int, const int); ///< sets bitmap code for a floor tile
    void SetWallCode(const int, const int); ///< sets bitmap code for a wall tile
    void SetRailCode(const int, const int); ///< sets bitmap code for a rail tile

  public:
    CTileManager(size_t); ///< Constructor.
    ~CTileManager(); ///< Destructor.

    void LoadMap(char*, char*, char*, char*, int); ///< Load a map and map mask

    void InitializeGroundMap(); ///< populates the ground map with valid char - mask associations
    void InitializeFlavorValMap(); ///< populates the flavor map with valid char - mask associations
    void Draw(); ///< Draw the map with a given tile.

    int GetGroundSprite(LSpriteDesc2D*, const char, const char); ///< Finds an associated sprite from groundmap and sets the spritedesc to it
    int GetFlavorSprite(LSpriteDesc2D*, const char, const char); ///< Finds an associated sprite from flavormap and sets the spritedesc to it
    void DrawSupportingWalls(const LSpriteDesc2D*, const char);   ///< draws wall sprites which correspond to a wall's bitmap code

    void DrawOver();    ///< Draws everything in m_vecDrawOver
    void DrawBoundingBoxes(eSprite); ///< Draw the bounding boxes.
    void GetObjects(std::vector<Vector2>&, Vector2&, std::vector<Vector2>&, Vector2&, Vector2&,
        std::vector<Vector2>&, std::vector<Vector2>&, std::vector<Vector2>&, std::vector<Vector2>&,
        std::vector<Vector2>&); ///< Get objects.

    const bool Visible(const Vector2&, const Vector2&, float) const; ///< Check visibility.
    const bool CollideWithWall(BoundingSphere, Vector2&, float&) const; ///< Object-wall collision test.
}; //CTileManager

#endif //__L4RC_GAME_TILEMANAGER_H__