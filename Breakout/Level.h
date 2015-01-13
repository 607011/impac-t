// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#ifndef __LEVEL_H_
#define __LEVEL_H_

#include <vector>
#include <cstdint>

#include <zlib.h>

namespace Breakout {


  struct LevelOptions {
  };

  typedef std::vector<LevelOptions> LevelOptionsList;


  class Level {
  public:
    explicit Level(int level = 0);
    ~Level();

    void gotoNext(void);
    void setLevel(int);
    void load(void);
    uint32_t mapData(int x, int y) const;
    uint32_t *const scanLine(int y) const;
    int num(void) const;
    int width(void) const;
    int height(void) const;
    int tileWidth(void) const;
    int tileHeight(void) const;
    int rows(void) const;
    int cols(void) const;

  private:
    int mNum;
    uint32_t *mMapData;
    uLongf mMapDataSize;
    int mNumTilesX;
    int mNumTilesY;
    int mTileWidth;
    int mTileHeight;
  };

}


#endif 