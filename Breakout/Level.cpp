// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#include "stdafx.h"


namespace Breakout {

  Level::Level(int level)
    : mMapData(nullptr)
    , mMapDataSize(0)
    , mNumTilesX(0)
    , mNumTilesY(0)
    , mTileWidth(0)
    , mTileHeight(0)
    , mNum(0)
  {
    setLevel(level);
  }


  Level::~Level()
  {
    freeMapData();
  }


  void Level::gotoNext(void)
  {
    setLevel(mNum + 1);
  }


  void Level::setLevel(int level)
  {
    mNum = level;
    if (mNum > 0)
      load();
  }


#pragma warning(disable : 4503)
  void Level::load(void)
  {
    safeFree(mMapData);
    std::ostringstream buf;
    buf << "resources/levels/" << std::setw(4) << std::setfill('0') << (1) << ".tmx";
    const std::string &filename = buf.str();
    bool ok = true;
    boost::property_tree::ptree pt;
    try {
      boost::property_tree::xml_parser::read_xml(filename, pt);
    }
    catch (const boost::property_tree::xml_parser::xml_parser_error &ex) {
      sf::err() << "XML parser error: " << ex.what() << " (line " << ex.line() << ")" << std::endl;
      ok = false;
    }
    if (ok) {
      const std::string &mapDataB64 = pt.get<std::string>("map.layer.data");
      mTileWidth = pt.get<int>("map.<xmlattr>.tilewidth");
      mTileHeight = pt.get<int>("map.<xmlattr>.tileheight");
      mNumTilesX = pt.get<int>("map.<xmlattr>.width");
      mNumTilesY = pt.get<int>("map.<xmlattr>.height");
#ifndef NDEBUG
      std::cout << "Map size: " << mNumTilesX << "x" << mNumTilesY << std::endl;
#endif
      uint8_t *compressed = nullptr;
      uLong compressedSize = 0UL;
      base64_decode(mapDataB64, compressed, compressedSize);
      if (compressed != nullptr && compressedSize > 0) {
        static const int CHUNKSIZE = 1*1024*1024;
        mMapData = (uint32_t*)std::malloc(CHUNKSIZE);
        mMapDataSize = CHUNKSIZE;
        int rc = uncompress((Bytef*)mMapData, &mMapDataSize, (Bytef*)compressed, compressedSize);
        if (rc == Z_OK) {
          mMapData = reinterpret_cast<uint32_t*>(std::realloc(mMapData, mMapDataSize));
#ifndef NDEBUG
          std::cout << "map data contains " << (mMapDataSize / sizeof(uint32_t)) << " elements." << std::endl;
#endif
        }
        else sf::err() << "Inflating map data failed with code " << rc << "\n";
        delete [] compressed;
      }
    }
  }


  uint32_t *const Level::scanLine(int y) const
  {
    return mMapData + y * mNumTilesX;
  }


  uint32_t Level::mapData(int x, int y) const
  {
    return this->scanLine(y)[x];
  }


  int Level::num(void) const
  {
    return mNum;
  }


  int Level::width(void) const
  {
    return mNumTilesX * mTileWidth;
  }


  int Level::height(void) const
  {
    return mNumTilesY * mTileHeight;
  }


  int Level::tileWidth(void) const
  {
    return mTileWidth;
  }


  int Level::tileHeight(void) const
  {
    return mTileHeight;
  }


  int Level::cols(void) const
  {
    return mNumTilesX;
  }


  int Level::rows(void) const
  {
    return mNumTilesY;
  }

}
