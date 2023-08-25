/// \file TileManager.cpp
/// \brief Code for the tile manager CTileManager.

#include "TileManager.h"
#include "SpriteRenderer.h"
#include "Abort.h"

/*Codes for map and mask txts
* Masks are NOT zero indexed - indexing starts at 1
MAP
    'W' = Wall - smart
    'X' = Void
    'P' = Player
    'O' = Exit
    'Y' = Entrance
    'U' = Top spawn point
    'N' = Bottom spawn point
    's' = Spike pit
    ENEMIES
    'T' = Turret
    'K' = OakShield
    'R' = Rabite
    'I' = king slime
    'L' = big slime
    'l' = small slime (lowercase L)
    END ENEMIES
    FLOORS 
    'F' = Base floor - as of now, must be hardcoded
    'B' = Brown cobble - smart
    'M' = Marsh grass - smart
    'G' = Grass2 - smart
    'C' = Green cobble - smart
    END FLOORS

Flavor - Mask Range
    'R' = Normal Rails - smart
    'R' = broken vertical rail - range (5, 6)
    'R' = broken horizontal rail - range (7, 8) //TODO trim sprite if used
    '-' = Rail stop above - range 1
    'G' = gravel brown cobble path - range (1, 4)
    'g' = gravel grey cobble path - range (1, 4)
    'm' = mushroom - range (1,8)
    'f' = fern - range (1,6)
    'F' = big fern - range (1,3)
    'L' = lantern - range (1,9)
    'S' = wall support structure - range (1,2)
    ***The upper half of grass indices have flowers on them***
    'o' = grass (with orange flowers)- range (1,8)
    'O' = tall grass(with orange flowers) - range (1,4)
    'p' = grass (with pink flowers) - range (1,8)
    'P' = tall grass (with pink flowers) - range (1,4)
    'c' = grass (with creme flowers)- range (1,8)
    'C' = grass tall (with creme flowers)- range (1,4)
    'b' = grass (with blue flowers)- range (1,8)
    'B' = grass tall (with blue flowers)- range (1,4)
    
   

END Codes for map and mask txts*/

/// Construct a tile manager using square tiles, given the width and height
/// of each tile.
/// \param n Width and height of square tile in pixels.

CTileManager::CTileManager(size_t n):
  m_fTileSize((float)n){
} //constructor

/// Delete the memory used for storing the map.

CTileManager::~CTileManager(){
    for (size_t i = 0; i < m_nHeight; i++) {
        delete[] m_chMap[i];
        //TODO del mask
    } //for
  delete[] m_chMap;
} //destructor

/// Make the AABBs for the walls. Care is taken to use the longest horizontal
/// and vertical AABBs possible so that there aren't so many of them.

void CTileManager::MakeBoundingBoxes() {
    m_vecWalls.clear(); //no walls yet

    BoundingBox aabb; //current bounding box
    const float t = m_fTileSize; //shorthand for tile width and height
    const Vector3 vTileExtents = 0.5f * t * Vector3::One; //tile extents extended to 3D
    BoundingBox b; //single-tile bounding box
    b.Extents = vTileExtents; //bounding box extents cover a single tile

    //horizontal walls with more than one tile

    const Vector2 vstart(t / 2, t * (m_nHeight - 0.5f)); //start position
    Vector2 pos = vstart; //set current position to start position

    for (size_t i = 0; i < m_nHeight; i++) { //for each row
        size_t j = 0; //column index
        pos.x = vstart.x; //set start position x coordinate
        while (j < m_nWidth) { //for each column
            while (j < m_nWidth && m_chMap[i][j] != 'W' && m_chMap[i][j] != 'X') { //skip over non-wall entries
                j++; //next column
                pos.x += t; //move right by tile width
            } //while

            if (j < m_nWidth) { //found leftmost tile in a wall
                aabb.Center = Vector3(pos.x, pos.y, 0); //bounding box center
                aabb.Extents = vTileExtents; //bounding box extents
                j++; //next column
                pos.x += t; //move right by tile width
            } //if

            bool bSingleTile = true; //as far as we know, this is a single-tile wall

            while (j < m_nWidth && (m_chMap[i][j] == 'W' || m_chMap[i][j] == 'X')) { //for each adjacent wall tile
                b.Center = Vector3(pos.x, pos.y, 0); //bounding box center
                BoundingBox::CreateMerged(aabb, aabb, b); //merge b into aabb
                bSingleTile = false; //the wall now has at least 2 tiles in it
                j++; //next column
                pos.x += t; //move right by tile width
            } //while

            if (!bSingleTile) //skip this wall if it is a single tile
                m_vecWalls.push_back(aabb); //add horizontal wall to the list
        } //while

        pos.y -= t; //next row
    } //for

    //vertical walls, the single tiles get caught here

    pos = vstart; //reset current position to start position

    for (size_t j = 0; j < m_nWidth; j++) { //for each column
        size_t i = 0; //row index
        pos.y = vstart.y; //set start position y coordinate

        while (i < m_nHeight) { //for each row
            while (i < m_nHeight && m_chMap[i][j] != 'W') { //skip over non-wall entries
                i++; //next row
                pos.y -= t; //move down by tile height
            } //while

            if (i < m_nHeight) { //found topmost tile in a wall
                aabb.Center = Vector3(pos.x, pos.y, 0); //bounding box center
                aabb.Extents = vTileExtents; //bounding box extents
                i++; //next row
                pos.y -= t; //move down by tile height
            } //if

            bool bSingleTile = true; //as far as we know, this is a single-tile wall

            while (i < m_nHeight && m_chMap[i][j] == 'W') { //for each adjacent wall tile
                b.Center = Vector3(pos.x, pos.y, 0); //bounding box center
                BoundingBox::CreateMerged(aabb, aabb, b); //merge b into aabb
                bSingleTile = false; //the wall now has at least 2 tiles in it
                i++; //next row
                pos.y -= t; //move down by tile height
            } //while

            if (!bSingleTile) //skip this wall if it is a single tile
                m_vecWalls.push_back(aabb); //add horizontal wall to the list
        } //while

        pos.x += t; //next column
    } //for

  //orphaned single tiles

  pos = vstart; //reset current position to start position
  for(size_t i=0; i<m_nHeight; i++){ //for each row
    for(size_t j=0; j<m_nWidth; j++){ //for each column
        //if (m_chMap[i][j] == 'W') {
        //    b.Center = Vector3(pos.x, pos.y, 0); //bounding box center
        //    m_vecWalls.push_back(b); //add single-tile wall to the list
        //}

      if((m_chMap[i][j] == 'W') && //is a wall tile and
        ((i == 0 || (m_chMap[i - 1][j] != 'W') && //has non-wall tile below or is on edge
         (i == m_nHeight - 1 || (m_chMap[i + 1][j] != 'W' )) && //has non-wall tile above or is on edge
         (j == 0 || (m_chMap[i][j - 1] != 'W')) && //has non-wall tile at left or is on edge
         (j == m_nWidth - 1 || (m_chMap[i][j + 1] != 'W')) //has non-wall tile at right or is on edge
        ))){    
        b.Center = Vector3(pos.x, pos.y, 0); //bounding box center
        m_vecWalls.push_back(b); //add single-tile wall to the list
      } //if

      pos.x += t; //next column
    } //for
    
    pos.x = vstart.x; //first column
    pos.y -= t; //next row
  } //for
} //MakeBoundingBoxes


/// <summary>
/// Delete the old map (if any), allocate the right sized chunk of memory for
/// the new map, and read it from a text file. Read in mask for map the same way.
/// Determine if any chars should be swapped out - for object spawning
/// Call functions to set bitmaps and flavor values
/// </summary>
/// <param name="filename">: map txt name</param>
/// <param name="maskname">: masks txt name</param>
/// <param name="entry_point">: door</param>
//void CTileManager::LoadMap(char* filename, int direction){
void CTileManager::LoadMap(char* filename, char* mapmaskname, char* flavorname, char* maskname, int direction){
  m_vEntryPortalPosition = Vector2::Zero;
  m_vExitPortalPosition = Vector2::Zero;
  
  InitializeGroundMap();    //creates ground map to be referenced during draw fn
  InitializeFlavorValMap(); //creates flavor map to be referenced during draw fn

  //unload any previous maps, flavors, and masks
  if(m_chMap != nullptr){ 
      for (size_t i = 0; i < m_nHeight; i++)
          delete[] m_chMap[i];
      delete [] m_chMap;
  } //if
  if (m_chMapMask != nullptr) {
      for (size_t i = 0; i < m_nHeight; i++)
          delete[] m_chMapMask[i];

      delete[] m_chMapMask;
  } //if
  if (m_chFlavor != nullptr) {
      for (size_t i = 0; i < m_nHeight; i++)
          delete[] m_chFlavor[i];

      delete[] m_chFlavor;
  } //if
  if (m_chFlavorMask != nullptr) {
      for (size_t i = 0; i < m_nHeight; i++)
          delete[] m_chFlavorMask[i];

      delete[] m_chFlavorMask;
  } //if
 
  m_vecTurrets.clear(); //clear out the turret list
  m_vecSlimes.clear();
  m_vecBigSlimes.clear();
  m_vecKingSlimes.clear();
  m_vecOakSeeds.clear();
  m_vecRabites.clear();


  FILE* input;          //ground map file
  FILE* inputMapMask;   //ground map mask file
  FILE* inputFlavor;    //flavor map file
  FILE* inputMask;      //flavor map mask file

  fopen_s(&input, filename, "rb"); //open the map file
  if(input == nullptr) //abort if it's missing
    ABORT("Map &s not found.", filename); //panic

  //read map file into a character buffer 
  fseek(input, 0, SEEK_END); //seek to end of map file
  const size_t n = ftell(input); //get file size in bytes
  rewind(input); //seek to start of file

  char *buffer = new char[n + 1]; //temporary character buffer
  fread(buffer, n, 1, input); //read the whole thing in a chunk
  fclose(input); //close the map file, we're done with it


  fopen_s(&inputMapMask, mapmaskname, "rb"); //open the map file
  if (inputMapMask == nullptr) //abort if it's missing
      ABORT("Mask &s not found.", mapmaskname); //panic

  //read mask file into a character buffer 
  fseek(inputMapMask, 0, SEEK_END); //seek to end of map file
  const size_t nMapMask = ftell(inputMapMask); //get file size in bytes
  rewind(inputMapMask); //seek to start of file

  char* bufferMapMask = new char[nMapMask + 1]; //temporary character buffer
  fread(bufferMapMask, nMapMask, 1, inputMapMask); //read the whole thing in a chunk
  fclose(inputMapMask); //close the map file, we're done with it


  fopen_s(&inputFlavor, flavorname, "rb"); //open the map file
  if (inputFlavor == nullptr) //abort if it's missing
      ABORT("Flavor &s not found.", flavorname); //panic

  //read map file into a character buffer 
  fseek(inputFlavor, 0, SEEK_END); //seek to end of map file
  const size_t nFlav = ftell(inputFlavor); //get file size in bytes
  rewind(inputFlavor); //seek to start of file

  char* bufferFlavor = new char[nFlav + 1]; //temporary character buffer
  fread(bufferFlavor, nFlav, 1, inputFlavor); //read the whole thing in a chunk
  fclose(inputFlavor); //close the flavor file, we're done with it


  fopen_s(&inputMask, maskname, "rb"); //open the map file
  if (inputMask == nullptr) //abort if it's missing
      ABORT("Mask &s not found.", maskname); //panic

  //read mask file into a character buffer 
  fseek(inputMask, 0, SEEK_END); //seek to end of map file
  const size_t nMask = ftell(inputMask); //get file size in bytes
  rewind(inputMask); //seek to start of file

  char* bufferMask = new char[nMask + 1]; //temporary character buffer
  fread(bufferMask, nMask, 1, inputMask); //read the whole thing in a chunk
  fclose(inputMask); //close the map file, we're done with it


  if (n != nMapMask)    //bail if any of the txts are diff sizes
      ABORT("Map %s is not the same size as %s: %d vs %d\n", filename, mapmaskname, n, nMask);
  if (n != nFlav)
      ABORT("Map %s is not the same size as %s: %d vs %d\n", filename, flavorname, n, nMask);
  if (n != nMask)
     ABORT("Map %s is not the same size as %s: %d vs %d\n", filename, maskname, n, nMask);

  //get map width and height into m_nWidth and m_nHeight
  m_nWidth = 0; 
  m_nHeight = 0;
  size_t w = 0; //width of current row
  bool bFirstLine = true;

  for(size_t i=0; i<n; i++){
    if(buffer[i] != '\n')
      w++; //skip characters until the end of line
    else{
      if(w == 0)ABORT("Panic!"); //should never happen

      if(w != m_nWidth && !bFirstLine && w != 0) //not the same length as the previous one
        ABORT("Line %d of map is not the same length as the previous one.", m_nHeight);

      m_nWidth = w; w = 0; m_nHeight++; //next line
      bFirstLine = false; //the next line is not the first
    } //else  
  } //for

  m_nWidth--;

  //allocate space for the map and associated collections
  m_chMap = new char*[m_nHeight];
  m_chMapMask = new char* [m_nHeight];
  m_chFlavor = new char*[m_nHeight];
  m_chFlavorMask = new char* [m_nHeight];

  for (size_t i = 0; i < m_nHeight; i++) {
      m_chMap[i] = new char[m_nWidth];
      m_chMapMask[i] = new char[m_nWidth];
      m_chFlavor[i] = new char[m_nWidth];
      m_chFlavorMask[i] = new char [m_nWidth];
  } //for

  //load the map information from the buffer to the map

  size_t index = 0; //index into character buffer
  
  for(size_t i=0; i<m_nHeight; i++){
    for(size_t j=0; j<m_nWidth; j++){
      const char c = buffer[index];
      const char n = bufferMapMask[index];
      const char f = bufferFlavor[index];
      const char m = bufferMask[index];

      int val = 0;

      //just store the chars for these txts. no substitution needed
      m_chFlavor[i][j] = f;  
      m_chMapMask[i][j] = n;
      m_chFlavorMask[i][j] = m;

      Vector2 pos;
      switch (c) {
      case 'T':
          m_chMap[i][j] = 'F'; //floor tile
          pos = m_fTileSize * Vector2(j + 0.5f, m_nHeight - i - 0.5f);
          m_vecTurrets.push_back(pos);
          break;
      case 's':
          m_chMap[i][j] = 'F'; // spikes
          pos = m_fTileSize * Vector2(j + 0.5f, m_nHeight - i - 0.5f);
          m_vecSpikes.push_back(pos);
          break;
      case 'L':
          m_chMap[i][j] = 'F'; //floor tile
          pos = m_fTileSize * Vector2(j + 0.5f, m_nHeight - i - 0.5f);
          m_vecBigSlimes.push_back(pos);
          break;
      case 'l':
          m_chMap[i][j] = 'F'; //floor tile
          pos = m_fTileSize * Vector2(j + 0.5f, m_nHeight - i - 0.5f);
          m_vecSlimes.push_back(pos);
          break;
      case 'I': 
          m_chMap[i][j] = 'F'; //floor tile
          pos = m_fTileSize * Vector2(j + 0.5f, m_nHeight - i - 0.5f);
          m_vecKingSlimes.push_back(pos);
          break;
      case 'K':
          m_chMap[i][j] = 'F'; //floor tile
          pos = m_fTileSize * Vector2(j + 0.5f, m_nHeight - i - 0.5f);
          m_vecOakSeeds.push_back(pos);
          break;
      case 'R':
          m_chMap[i][j] = 'F'; //floor tile
          pos = m_fTileSize * Vector2(j + 0.5f, m_nHeight - i - 0.5f);
          m_vecRabites.push_back(pos);
          break;
      default:
          m_chMap[i][j] = c;
          break;
      }

      // this is where to spawn the player and the portal(s)
      //
      switch (direction) {
      case 0: { // default, usually just the first map. only need next portal and player
          if (c == 'P') {
              m_chMap[i][j] = 'F';
              m_vPlayer = m_fTileSize * Vector2(j + 0.5f, m_nHeight - i - 0.5f);
          }
          if (c == 'O') {
              m_chMap[i][j] = 'F';
              m_vExitPortalPosition = m_fTileSize * Vector2(j + 0.5f, m_nHeight - i - 0.5f);
          }
          if (c == 'A') {
              m_chMap[i][j] = 'F';
          }
      } break;
      case 1: {
          if (c == 'P') {
              m_chMap[i][j] = 'F';
              m_vPlayer = m_fTileSize * Vector2(j + 0.5f, m_nHeight - i - 0.5f);
          }
          if (c == 'O') {
              m_chMap[i][j] = 'F';
              m_vExitPortalPosition = m_fTileSize * Vector2(j + 0.5f, m_nHeight - i - 0.5f);
          }
          if (c == 'Y') {
              m_chMap[i][j] = 'F';
              m_vEntryPortalPosition = m_fTileSize * Vector2(j + 0.5f, m_nHeight - i - 0.5f);
          }
          if (c == 'A') {
              m_chMap[i][j] = 'F';
          }
      } break;
      case -1: {
          if (c == 'A') {
              m_chMap[i][j] = 'F';
              m_vPlayer = m_fTileSize * Vector2(j + 0.5f, m_nHeight - i - 0.5f);
          }
          if (c == 'O') {
              m_chMap[i][j] = 'F';
              m_vExitPortalPosition = m_fTileSize * Vector2(j + 0.5f, m_nHeight - i - 0.5f);
          }
          if (c == 'Y') {
              m_chMap[i][j] = 'F';
              m_vEntryPortalPosition = m_fTileSize * Vector2(j + 0.5f, m_nHeight - i - 0.5f);
          }
          if (c == 'P') {
              m_chMap[i][j] = 'F';
          }
      } break;

      }

      index++; //next index
    } //for
    index += 2; //skip end of line character
  } //for

  //iterate again now that surrounding chars can be examined
  for (int i = 0; i < m_nHeight; ++i) {
      for (int j = 0; j < m_nWidth; ++j) {
          //set mask values for ground and flavor bit map logic
          //only occurs if one is not provided in mask, ie '0'
          if (m_chMap[i][j] == 'W') //walls
              SetWallCode(i, j);
          else if((m_chMap[i][j] == 'B' || m_chMap[i][j] == 'M' || m_chMap[i][j] == 'G' || 
              m_chMap[i][j] == 'C') && m_chMapMask[i][j] == '0' ) //brown cobble
              SetFloorCode(i, j);
          if (m_chFlavor[i][j] == 'R' && m_chFlavorMask[i][j] == '0')  //rails
              SetRailCode(i, j);
      } //for
  } //for

  m_vWorldSize = Vector2((float)m_nWidth, (float)m_nHeight)*m_fTileSize;
  MakeBoundingBoxes();

  //clean up
  delete [] buffer; 
  delete[] bufferMapMask;
  delete[] bufferMask;
  delete[] bufferFlavor;
} //LoadMap

/// <summary>
/// Populates ground map with (UINT, SpriteDesc) associations
/// </summary>
void CTileManager::InitializeGroundMap() {
    LSpriteDesc2D spriteDesc;
    spriteDesc = LSpriteDesc2D((UINT)eSprite::Floor1, Vector2::Zero);
    spriteDesc.m_nCurrentFrame = (UINT)0;
    groundMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'F' * (UINT)'F'), spriteDesc));

    spriteDesc = LSpriteDesc2D((UINT)eSprite::BrownCobbleFloor, Vector2::Zero);
    for (UINT i = 0; i < 9; i++) {
        spriteDesc.m_nCurrentFrame = (UINT)i;
        groundMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'B' * (UINT)'B') + i, spriteDesc));
    }

    spriteDesc = LSpriteDesc2D((UINT)eSprite::MarshGrassFloor, Vector2::Zero);
    for (UINT i = 0; i < 9; i++) {
        spriteDesc.m_nCurrentFrame = (UINT)i;
        groundMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'M' * (UINT)'M') + i, spriteDesc));
    }

    spriteDesc = LSpriteDesc2D((UINT)eSprite::GrassGroundFloor2, Vector2::Zero);
    for (UINT i = 0; i < 9; i++) {
        spriteDesc.m_nCurrentFrame = (UINT)i;
        groundMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'G' * (UINT)'G') + i, spriteDesc));
    }

    spriteDesc = LSpriteDesc2D((UINT)eSprite::GreenCobbleFloor, Vector2::Zero);
    for (UINT i = 0; i < 9; i++) {
        spriteDesc.m_nCurrentFrame = (UINT)i;
        groundMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'C' * (UINT)'C') + i, spriteDesc));
    }

    spriteDesc = LSpriteDesc2D((UINT)eSprite::WallOut1, Vector2::Zero);
    for (UINT i = 0; i < 9; i++) {
        spriteDesc.m_nCurrentFrame = (UINT)i;
        groundMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'W' * (UINT)'W') + i, spriteDesc));
    }


    spriteDesc.m_nCurrentFrame = (UINT)9;
    spriteDesc.m_vPos = Vector2(18.0f, 18.0f);
    groundMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'W' * (UINT)'W') + 9, spriteDesc));  //ascii ':'

    spriteDesc.m_nCurrentFrame = (UINT)10;
    spriteDesc.m_vPos = Vector2(-18.0f, 18.0f);
    groundMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'W' * (UINT)'W') + 10, spriteDesc)); //';'

    spriteDesc.m_nCurrentFrame = (UINT)9;
    spriteDesc.m_vPos = Vector2(-18.0f, -18.0f);
    spriteDesc.m_fRoll = -XM_PI;
    groundMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'W' * (UINT)'W') + 11, spriteDesc)); //'<'

    spriteDesc.m_nCurrentFrame = (UINT)10;
    spriteDesc.m_vPos = Vector2(18.0f, -18.0f);
    spriteDesc.m_fRoll = -XM_PI;
    groundMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'W' * (UINT)'W') + 12, spriteDesc)); //'='


}

/// <summary>
/// Populates flavor map with (UINT, SpriteDesc) associations
/// </summary>
void CTileManager::InitializeFlavorValMap() {
    LSpriteDesc2D spriteDesc = LSpriteDesc2D((UINT)eSprite::Shrooms, Vector2::Zero);
    for (UINT i = 0; i < 8; i++) {  //mushrooms
        spriteDesc.m_nCurrentFrame = (UINT)i;
        flavorValMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'m' * (UINT)'m') + i, spriteDesc));
    }

    spriteDesc = LSpriteDesc2D((UINT)eSprite::Fern, Vector2::Zero);
    for (UINT i = 0; i < 6; i++) {  //little fern 
        spriteDesc.m_nCurrentFrame = (UINT)i;
        flavorValMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'f' * (UINT)'f') + i, spriteDesc));
    }

    spriteDesc = LSpriteDesc2D((UINT)eSprite::BigFern, Vector2::Zero);
    for (UINT i = 0; i < 3; i++) {  //big fern 
        spriteDesc.m_nCurrentFrame = (UINT)i;
        flavorValMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'F' * (UINT)'F') + i, spriteDesc));
    }

    spriteDesc = LSpriteDesc2D((UINT)eSprite::Rails, Vector2::Zero);
    spriteDesc.m_fXScale = 2.24f;
    spriteDesc.m_fYScale = 2.24f;
    for (UINT i = 0; i < 12; i++) {  //smart rails 
        spriteDesc.m_nCurrentFrame = (UINT)i;
        flavorValMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'R' * (UINT)'R') + i, spriteDesc));
    }
    spriteDesc.m_fXScale = spriteDesc.m_fYScale = 1.0f;

    spriteDesc = LSpriteDesc2D((UINT)eSprite::Grass1, Vector2::Zero);
    for (UINT i = 0; i < 8; i++) {   
        spriteDesc.m_nCurrentFrame = (UINT)i;
        flavorValMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'o' * (UINT)'o') + i, spriteDesc));
    }
    spriteDesc = LSpriteDesc2D((UINT)eSprite::TallGrass1, Vector2::Zero);
    for (UINT i = 0; i < 4; i++) {
        spriteDesc.m_nCurrentFrame = (UINT)i;
        flavorValMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'O' * (UINT)'O') + i, spriteDesc));
    }

    spriteDesc = LSpriteDesc2D((UINT)eSprite::Grass2, Vector2::Zero);
    for (UINT i = 0; i < 8; i++) {   
        spriteDesc.m_nCurrentFrame = (UINT)i;
        flavorValMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'p' * (UINT)'p') + i, spriteDesc));
    }
    spriteDesc = LSpriteDesc2D((UINT)eSprite::TallGrass2, Vector2::Zero);
    for (UINT i = 0; i < 4; i++) {
        spriteDesc.m_nCurrentFrame = (UINT)i;
        flavorValMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'P' * (UINT)'P') + i, spriteDesc));
    }

    spriteDesc = LSpriteDesc2D((UINT)eSprite::Grass3, Vector2::Zero);
    for (UINT i = 0; i < 8; i++) {  
        spriteDesc.m_nCurrentFrame = (UINT)i;
        flavorValMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'c' * (UINT)'c') + i, spriteDesc));
    }
    spriteDesc = LSpriteDesc2D((UINT)eSprite::TallGrass3, Vector2::Zero);
    for (UINT i = 0; i < 4; i++) {
        spriteDesc.m_nCurrentFrame = (UINT)i;
        flavorValMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'C' * (UINT)'C') + i, spriteDesc));
    }

    spriteDesc = LSpriteDesc2D((UINT)eSprite::Grass4, Vector2::Zero);
    for (UINT i = 0; i < 8; i++) {   
        spriteDesc.m_nCurrentFrame = (UINT)i;
        flavorValMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'b' * (UINT)'b') + i, spriteDesc));
    }

    spriteDesc = LSpriteDesc2D((UINT)eSprite::TallGrass4, Vector2::Zero);
    for (UINT i = 0; i < 4; i++) {
        spriteDesc.m_nCurrentFrame = (UINT)i;
        flavorValMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'B' * (UINT)'B') + i, spriteDesc));
    }

    spriteDesc = LSpriteDesc2D((UINT)eSprite::BrownGravel, Vector2::Zero);
    for (UINT i = 0; i < 4; i++) {
        spriteDesc.m_nCurrentFrame = (UINT)i;
        flavorValMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'G' * (UINT)'G') + i, spriteDesc));
    }

    spriteDesc = LSpriteDesc2D((UINT)eSprite::GreyGravel, Vector2::Zero);
    for (UINT i = 0; i < 4; i++) {
        spriteDesc.m_nCurrentFrame = (UINT)i;
        flavorValMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'g' * (UINT)'g') + i, spriteDesc));
    }

    spriteDesc = LSpriteDesc2D((UINT)eSprite::Lantern, Vector2::Zero);
    spriteDesc.m_fXScale = spriteDesc.m_fYScale = 1.3f;
    spriteDesc.m_vPos.y += 8.0f;
    for (UINT i = 0; i < 9; i++) {
        spriteDesc.m_nCurrentFrame = (UINT)i;
        flavorValMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'L' * (UINT)'L') + i, spriteDesc));
    }



    spriteDesc = LSpriteDesc2D((UINT)eSprite::Support1, Vector2::Zero);
    spriteDesc.m_fXScale = spriteDesc.m_fYScale = 1.2f;
    spriteDesc.m_vPos.y -= 5.0f;
    flavorValMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'S' * (UINT)'S'), spriteDesc));

    spriteDesc = LSpriteDesc2D((UINT)eSprite::Support2, Vector2::Zero);
    spriteDesc.m_fXScale = spriteDesc.m_fYScale = 1.2f;
    spriteDesc.m_vPos.y -= 5.0f;
    flavorValMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'S' * (UINT)'S') + 1, spriteDesc));

    spriteDesc = LSpriteDesc2D((UINT)eSprite::RailStop, Vector2::Zero);
    spriteDesc.m_nCurrentFrame = (UINT)0;
    spriteDesc.m_fXScale = 2.24f;
    spriteDesc.m_fYScale = 2.24f;
    flavorValMap.insert(std::pair<UINT, LSpriteDesc2D>(UINT((UINT)'-' * (UINT)'-'), spriteDesc));


}


/* Bitmapping with booleans
    * Middle '-' is self
    * Each surrounding tile is tested to be true or false
    * If the adjacent tile is wall or out of bounds, it is true
    * Else, false
        -------------
        | C | E | H |
        | B | - | G |
        | A | D | F |
        -------------
*/
/// <summary>
/// stores the walls corresponding bitmap code in chMapMask arr
/// </summary>
void CTileManager::SetWallCode(const int i, const int j) {
    char value = '0';
    LSpriteDesc2D sprite;
    if (m_chMap[i][j] == 'W')
    {
        int offset;
        int x, y;
        bool A, B, C, D, E, F, G, H;
        A = false; B = false; C = false; D = false;
        E = false; F = false; G = false; H = false;
        offset = 0;
        //check if edge of map
        if (i == (m_nHeight - 1) || j == (m_nWidth - 1) || i == 0 || j == 0)
            return;

        else {
            //check surrounding tiles for walls and generate code
            for (int k = 0; k < 9; ++k) {
                x = (k % 3) - 1;  //mod to get relative row
                y = (k / 3) - 1;  //int div to get relative col

                if (x == 0 && y == 0)    //skip self
                {
                    offset = 1;
                }

                else {  //not self
                    if (m_chMap[i + x][j + y] == 'W' || m_chMap[i + x][j + y] == 'X')
                        switch (k - offset) {
                            //didn't use a bool Arr to improve readability and usability
                        case 2: A = true; break;
                        case 1: B = true; break;
                        case 0: C = true; break;
                        case 4: D = true; break;
                        case 3: E = true; break;
                        case 7: F = true; break;
                        case 6: G = true; break;
                        case 5: H = true; break;
                        }
                }
            }
            //Top middle
            if (B && D && !E && G)
                value = '2';

            //Mid left 
            else if (!B && D && E && G)
                value = '4';

            //Mid right 
            else if (B && D && E && !G)
                value = '6';

            //Bottom mid 
            else if (B && !D && E && G)
                value = '8';

            //Bottom right corner
            else if (B && C && !D && E && !F && !G)
                value = '9';

            //Top right corner
            else if (A && B && D && !E && !G && !H)
                value = '3';

            //top left corner
            else if (!B && !C && D && !E && F && G)
                value = '1';

            //Bottom left corner
            else if (!A && !B && !D && E && G && H)
                value = '7';

            //Inner corner bot right
            else if (B && !C && D && E && G)
                value = ';';

            //Inner corner bot left
            else if (B && D && E && G && !H)
                value = ':';

            //Top corner - rotated bot corner
            else if (!A && B && D && E && G)
                value = '<';
            //Top corner - rotated bot corner
            else if (B && D && E && !F && G)
                value = '=';

            //void
            else if (B && D && E && G)
                value = '5';

        } //else

    } //if
    m_chMapMask[i][j] = value;
} //SetWallCode

/// <summary>
/// Determines bitmap code - corresponding to frame num - and stores it in mapMask arr
/// </summary>
/// <param name="i">: x index for mask data and flavor arr</param>
/// <param name="j">: y index for mask data and flavor arr</param>
void CTileManager::SetFloorCode(const int i, const int j) {
    char value = '0';
    const char c = m_chMap[i][j];
    int offset;
    int x, y;
    bool B, D, E, G;

    B = false; D = false; E = false; G = false;
    offset = 0;
    //check if edge of map
    if (i == (m_nHeight - 1) || j == (m_nWidth - 1) || i == 0 || j == 0)
        m_chMap[i][j] = value;

    else {
        //check surrounding tiles for walls and generate code
        for (int k = 0; k < 9; ++k) {
            x = (k % 3) - 1;  //mod to get relative row
            y = (k / 3) - 1;  //int div to get relative col

            if (x == 0 && y == 0)    //skip self
                offset = 1;

            else {  //not self
                if (m_chMap[i + x][j + y] == c) //check if adj char is this floor 
                    switch (k - offset) {
                    case 1: B = true; break;
                    case 3: E = true; break;
                    case 4: D = true; break;
                    case 6: G = true; break;
                    } //switch
            } //else
        } //for

        if (!B && D && !E && G)
            value = '1'; //top left corner
        else if (B && D && !E && G)
            value = '2';    //top edge
        else if (B && D && !E && !G)
            value = '3';    //top right corner
        else if (!B && D && E && G)
            value = '4';    //left edge
        else if (B && D && E && G)
            value = '5';    //middle surrounded
        else if (B && D && E && !G)
            value = '6';    //right edge
        else if (!B && !D && E && G)
            value = '7'; //bot left corner
        else if (B && !D && E && G)
            value = '8';    //bottom edge
        else if (B && !D && E && !G)
            value = '9';
    } //else
    m_chMapMask[i][j] = value;   //assign floor 'index'
} //SetFloorCode

/// <summary>
/// Determines bit map code for rails and stores it in flavorMask arr
/// </summary>
/// <param name="i">: x index for mask data and flavor arr</param>
/// <param name="j">: y index for mask data and flavor arr</param>
void CTileManager::SetRailCode(const int i, const int j) {
    char value = '0';
    int offset;
    int x, y;
    bool B, D, E, G;

    B = false; D = false; E = false; G = false;
    offset = 0;
    //check if edge of map
    if (i == (m_nHeight - 1) || j == (m_nWidth - 1) || i == 0 || j == 0)
        m_chFlavorMask[i][j] = value;

    else {
        //check surrounding tiles for walls and generate code
        for (int k = 0; k < 9; ++k) {
            x = (k % 3) - 1;  //mod to get relative row
            y = (k / 3) - 1;  //int div to get relative col

            if (x == 0 && y == 0)    //skip self
                offset = 1;

            else {  //not self
              if (m_chFlavor[i + x][j + y] == 'R') //check if adj char is rail 
                switch (k - offset) {
                    case 1: B = true; break;
                    case 3: E = true; break;
                    case 4: D = true; break;
                    case 6: G = true; break;
                } //switch
            } //else
        } //for

        if (!D && E && !B && !G) 
            value = '4'; //9
        else if (B && E)
            value = ':';    //ascii val that comes after '9'
        else if (E && G)
            value = '9';
        else if (B && D)
            value = '3';
        else if (B && G)
            value = '2';
        else if (D && G)
            value = '1';
        else if (D || E)
            value = '4';
    } //else
    m_chFlavorMask[i][j] = value;   //assign flavor code
} //SetRailCode


/// Get positions of objects listed on map.
/// \param turrets [out] Vector of turret positions
/// \param player [out] Player position.

void CTileManager::GetObjects(std::vector<Vector2>& turrets, Vector2& player, 
    std::vector<Vector2>& spikes, Vector2& exit_portal, Vector2& entry_portal,
    std::vector<Vector2>& bigSlimes, std::vector<Vector2>& slimes, 
    std::vector<Vector2>& kingSlimes, std::vector<Vector2>& oakSeeds,
    std::vector<Vector2>& rabites){
  turrets = m_vecTurrets;
  player = m_vPlayer;
  spikes = m_vecSpikes;
  entry_portal = m_vEntryPortalPosition;
  exit_portal = m_vExitPortalPosition;
  bigSlimes = m_vecBigSlimes;
  slimes = m_vecSlimes;
  kingSlimes = m_vecKingSlimes;
  oakSeeds = m_vecOakSeeds;
  rabites = m_vecRabites;
} //GetObjects

/// This is for debug purposes so that you can verify that
/// the collision shapes are in the right places.
/// \param t Line sprite to be stretched to draw the line.

void CTileManager::DrawBoundingBoxes(eSprite t){
  for(auto& p: m_vecWalls)
    m_pRenderer->DrawBoundingBox(t, p);
} //DrawBoundingBoxes

/// Draw order is top-down, left-to-right so that the image
/// agrees with the map text file viewed in NotePad.
void CTileManager::Draw(){
    std::string s;
  LSpriteDesc2D desc; //sprite descriptors for tiles

  const int w = (int)ceil(m_nWinWidth/m_fTileSize) + 2; //width of window in tiles, with 2 extra
  const int h = (int)ceil(m_nWinHeight/m_fTileSize) + 2; //height of window in tiles, with 2 extra

  const Vector2 campos = m_pRenderer->GetCameraPos(); //camera position
  const Vector2 origin = campos + 0.5f*m_nWinWidth*Vector2(-1.0f, 1.0f); //position of top left corner of window

  const int top = std::max(0, (int)m_nHeight - (int)round(origin.y/m_fTileSize) + 1); //index of top tile
  const int bottom = std::min(top + h + 1, (int)m_nHeight - 1); //index of bottom tile

  const int left = std::max(0, (int)round(origin.x/m_fTileSize) - 1); //index of left tile
  const int right = std::min(left + w, (int)m_nWidth - 1); //index of right tile

  LSpriteDesc2D floor = LSpriteDesc2D((UINT)eSprite::GrassGroundFloor, Vector2::Zero); //floor tile
  floor.m_nCurrentFrame = (UINT)4; //TODO make it ez to replace base floor

  LSpriteDesc2D darkspace = LSpriteDesc2D((UINT)eSprite::WallOut1, Vector2::Zero); //dark tile
  darkspace.m_nCurrentFrame = (UINT)4;

  for (int i = top; i <= bottom; i++) {//for each column
      for (int j = left; j <= right; j++) { //for each row    
          desc.m_nSpriteIndex = ((UINT)eSprite::WallOut1/* + (wallSpriteSheet * 6)*/); //sprite index for tile
          desc.m_nCurrentFrame = 0;
          desc.m_vPos.x = (j + 0.5f) * m_fTileSize; //horizontal component of tile position
          desc.m_vPos.y = (m_nHeight - 1 - i + 0.5f) * m_fTileSize; //vertical component of tile position

          floor.m_vPos = desc.m_vPos;
          darkspace.m_vPos = desc.m_vPos;

          if (i == (m_nHeight - 1) || j == (m_nWidth - 1) || i == 0 || j == 0)
              desc.m_nCurrentFrame = (UINT)4; //if on edge tile is void
          else {
              switch (m_chMap[i][j]) { //select which frame of the tile sprite is to be drawn
              case 'B': //draw base floor under top-layer //TODO other floors
              case 'M':
              case 'G':
              case 'C':
              case 'F': //draw base floor at F
                  m_pRenderer->Draw(&floor);
                  break; //floor
              case 'X': m_pRenderer->Draw(&darkspace); break; //BLACKNESS
              default:
                  desc.m_nCurrentFrame = (UINT)4; break; //error tile
              } //switch
          } //else 

          /*Check for char - mask matches in groundMap. Handle cases where extra sprites are
          needed. Draw match if one is found */
          if (m_chMapMask[i][j] != '0' &&
              GetGroundSprite(&desc, m_chMap[i][j], m_chMapMask[i][j]) != '0') {
                m_pRenderer->Draw(&floor);    //draw floor under walls

              if (m_chMap[i][j] == 'W') {
                  if (m_chMapMask[i][j] > '9')  //check for corners - need dark below them (rather than floor)
                      m_pRenderer->Draw(&darkspace);  
                  else if (m_chMapMask[i][j] > '6')   //check for bottom walls - need surface below
                      DrawSupportingWalls(&desc, m_chMapMask[i][j]); //draw wall surfaces
              } //if
              m_pRenderer->Draw(&desc); //draw floor/wall tile

          } //if

          /*Check for char - mask matches in flavorMap. Draw match if one is found*/
          if (m_chFlavorMask[i][j] != '0' && 
            GetFlavorSprite(&desc, m_chFlavor[i][j], m_chFlavorMask[i][j]) != -1)
              m_vecDrawOver.push_back(desc);    //store flavor stuff to be drawn later

      } //for
  } //for
} //Draw

/// <summary>
/// 
/// </summary>
/// <param name="wallDesc"></param>
/// <param name="maskVal"></param>
void CTileManager::DrawSupportingWalls(const LSpriteDesc2D* wallDesc, const char maskVal) {
    LSpriteDesc2D support = *wallDesc;
    support.m_vPos += Vector2(0.0f, -46.0f);
    support.m_fXScale = 1.125f;
    support.m_nSpriteIndex = (UINT)eSprite::Wall1;
    switch (maskVal) {
    case '7':
        support.m_nCurrentFrame = 0;
        break;
    case '8':
        support.m_nCurrentFrame = 1;
        break;
    case '9':
        support.m_nCurrentFrame = 2;
        break;
    default:
        printf("return\n");
        return;
        break;
    }
    m_vecDrawOver.push_back(support);
}

/// <summary>
/// Get sprite from ground map if one exists.
/// </summary>
/// <param name="desc"></param>
/// <param name="c">flavor char</param>
/// <param name="m">mask char</param>
/// <returns>0 on success. -1 on no match</returns>
int CTileManager::GetGroundSprite(LSpriteDesc2D* desc, const char c, const char m) {
    UINT n = m - (UINT)49;   //convert ascii char to int
    auto it = groundMap.find((UINT((UINT)c * (UINT)c) + n)); //check for matching sprite in map
    if (it != groundMap.end()) { //match found
        //copy everything from mapped sprite - forgot why I did it this way. but I don't wanna break it
        desc->m_nSpriteIndex = groundMap.at((UINT((UINT)c * (UINT)c) + n)).m_nSpriteIndex;
        desc->m_vPos += groundMap.at((UINT((UINT)c * (UINT)c) + n)).m_vPos; //add offset, don't copy
        desc->m_nCurrentFrame = groundMap.at((UINT((UINT)c * (UINT)c) + n)).m_nCurrentFrame;
        desc->m_fXScale = groundMap.at((UINT((UINT)c * (UINT)c) + n)).m_fXScale;
        desc->m_fYScale = groundMap.at((UINT((UINT)c * (UINT)c) + n)).m_fYScale;
        desc->m_fRoll = groundMap.at((UINT((UINT)c * (UINT)c) + n)).m_fRoll;
        return 0;
    }
    else //match not found
        return -1; //check for -1 in caller - desc not set to sprite
}

/// <summary>
/// Get sprite from flavor map if one exists.
/// </summary>
/// <param name="desc"></param>
/// <param name="c">flavor char</param>
/// <param name="m">mask char</param>
/// <returns>0 on success. -1 on no match</returns>
int CTileManager::GetFlavorSprite(LSpriteDesc2D *desc, const char c, const char m) {
    UINT n = m - (UINT)49;   //convert ascii char to frame number: '1' => 0
    auto it = flavorValMap.find((UINT((UINT)c * (UINT)c) + n)); //check for matching sprite in map
    if (it != flavorValMap.end()) { //match found
        //copy everything from mapped sprite - forgot why I did it this way. but I don't wanna break it
        desc->m_nSpriteIndex = flavorValMap.at((UINT((UINT)c * (UINT)c) + n)).m_nSpriteIndex;   
        desc->m_vPos += flavorValMap.at((UINT((UINT)c * (UINT)c) + n)).m_vPos;  //add offset, don't copy
        desc->m_nCurrentFrame = flavorValMap.at((UINT((UINT)c * (UINT)c) + n)).m_nCurrentFrame;
        desc->m_fXScale = flavorValMap.at((UINT((UINT)c * (UINT)c) + n)).m_fXScale;
        desc->m_fYScale = flavorValMap.at((UINT((UINT)c * (UINT)c) + n)).m_fYScale;
        desc->m_fRoll = flavorValMap.at((UINT((UINT)c * (UINT)c) + n)).m_fRoll;
        return 0;
    }
    else //match not found
        return -1;  //check for -1 in caller - desc not set to sprite
}

/// <summary>
/// Draws everything in m_vecDrawOver then clears it. Called after main draw fn
/// Draws things on top of the floor and walls. 
/// </summary>
void CTileManager::DrawOver() {
    for (auto i : m_vecDrawOver) {
        m_pRenderer->Draw(&i);
    }
    m_vecDrawOver.erase(m_vecDrawOver.begin(), m_vecDrawOver.end());

} //DrawOver


/// Check whether a circle is visible from a point, that is, either the left
/// or the right side of the object (from the perspective of the point)
/// has no walls between it and the point. This gives some weird behavior
/// when the circle is partially hidden by a block, but it doesn't seem
/// particularly unnatural in practice. It'll do.
/// \param p0 A point.
/// \param p1 Center of circle.
/// \param r Radius of circle.
/// \return true If the circle is visible from the point.

const bool CTileManager::Visible(const Vector2& p0, const Vector2& p1, float r) const{
  bool visible = true;

  for(auto i=m_vecWalls.begin(); i!=m_vecWalls.end() && visible; i++){
    Vector2 direction = p0 - p1;
    direction.Normalize();
    const Vector2 norm = Vector2(-direction.y, direction.x);

    const float delta = std::min(r, 16.0f);

    //left-hand triangle
    const Vector3 v0(p0);
    const Vector3 v1(p1 + r*norm);
    const Vector3 v2(p1 + (r - delta)*norm);
    
    //right-hand triangle
    const Vector3 v3(p1 - r*norm);
    const Vector3 v4(p1 - (r - delta)*norm);

    visible = !(*i).Intersects(v0, v1, v2) || !(*i).Intersects(v0, v3, v4);
  } //for

  return visible;
} //Visible

/// Check whether a bounding sphere collides with one of the wall bounding boxes.
/// If so, compute the collision normal and the overlap distance. 
/// \param s Bounding sphere of object.
/// \param norm [out] Collision normal.
/// \param d [out] Overlap distance.
/// \return true if the bounding sphere overlaps a wall.

const bool CTileManager::CollideWithWall(
  BoundingSphere s, Vector2& norm, float& d) const
{
  bool hit = false; //return result, true if there is a collision with a wall

  for(auto i=m_vecWalls.begin(); i!=m_vecWalls.end() && !hit; i++){
    const BoundingBox& aabb = *i; //shorthand

    Vector3 corner[8]; //for corners of aabb
    aabb.GetCorners(corner);  //get corners of aabb
    s.Center.z = corner[0].z; //make sure they are at the same depth

    //the first 4 corners of aabb are the same as the last 4 but with different z

    hit = s.Intersects(aabb); //includes when they are touching

    if(hit){ //collision with either a point or an edge
      bool bPointCollide = false; //true if colliding with corner of bounding box

      for(UINT i=0; i<4 && !bPointCollide; i++) //check first 4 corners
        if(s.Contains(corner[i])){ //collision of bounding sphere with corner
          bPointCollide = true;
          Vector3 norm3 = s.Center - corner[i]; //vector from corner to sphere center
          norm = (Vector2)norm3; //cast to 2D
          d = s.Radius - norm.Length(); //overlap distance
          norm.Normalize(); //norm needs to be a unit vector
        } //if

      if(!bPointCollide){ //edge collide
        const float fLeft   = corner[0].x; //left of wall
        const float fRight  = corner[1].x; //right of wall
        const float fBottom = corner[1].y; //bottom of wall
        const float fTop    = corner[2].y; //top of wall

        const float epsilon = 0.01f; //small amount of separation

        if(s.Center.x <= fLeft){ //collide with left edge
          norm = -Vector2::UnitX; //normal
          d = s.Center.x - fLeft + s.Radius + epsilon; //overlap
        } //if

        else if(fRight <= s.Center.x){ //collide with right edge
          norm = Vector2::UnitX; //normal
          d = fRight - s.Center.x + s.Radius + epsilon; //overlap
        } //if
   
        else if(s.Center.y <= fBottom){ //collide with bottom edge
          norm = -Vector2::UnitY; //normal
          d = s.Center.y - fBottom + s.Radius + epsilon; //overlap
        } //if

        else if(fTop <= s.Center.y){ //collide with top edge
          norm = Vector2::UnitY; //normal
          d =  fTop - s.Center.y + s.Radius + epsilon; //overlap
        } //if 
      } //if
    } //if
  } //for

  return hit;
} //CollideWithWall
