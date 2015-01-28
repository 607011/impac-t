/*  

    Copyright (c) 2015 Oliver Lau <ola@ct.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


#include "stdafx.h"
#include <sys/stat.h>


namespace Breakout {

  Level::Level(void)
    : mBackgroundColor(sf::Color::Black)
    , mFirstGID(0)
    , mMapData(nullptr)
    , mNumTilesX(0)
    , mNumTilesY(0)
    , mTileWidth(0)
    , mTileHeight(0)
    , mLevelNum(0)
  {
    // ...
  }


  Level::~Level()
  {
    clear();
  }


  bool Level::set(int level)
  {
    bool valid = false;
    mLevelNum = level;
    if (mLevelNum > 0)
      valid = load();
    return valid;
  }


  bool Level::gotoNext(void)
  {
    return set(mLevelNum + 1);
  }


  static bool fileExists(const std::string &filename)
  {
    struct stat buffer;
    return stat(filename.c_str(), &buffer) == 0;
  }


  static inline int digit2Int(char c) {
    return int(c - '0');
  }


#pragma warning(disable : 4503)
  bool Level::load(void)
  {
    bool ok = true;
    safeFree(mMapData);
    mBackgroundImageOpacity = 1.f;

    std::ostringstream buf;
    // mLevelNum = 4;
    buf << gLevelsRootDir << std::setw(4) << std::setfill('0') << mLevelNum << ".tmx";
    const std::string &filename = buf.str();

    ok = fileExists(filename.c_str());
    if (!ok)
      return false;

#ifndef NDEBUG
    std::cout << "Loading level " << filename << " ..." << std::endl;
#endif
    boost::property_tree::ptree pt;
    try {
      boost::property_tree::xml_parser::read_xml(filename, pt);
    }
    catch (const boost::property_tree::xml_parser::xml_parser_error &ex) {
      std::cerr << "XML parser error: " << ex.what() << " (line " << ex.line() << ")" << std::endl;
      ok = false;
    }

    if (!ok)
      return false;

    try {
      const std::string &mapDataB64 = pt.get<std::string>("map.layer.data");
      mTileWidth = pt.get<int>("map.<xmlattr>.tilewidth");
      mTileHeight = pt.get<int>("map.<xmlattr>.tileheight");
      mNumTilesX = pt.get<int>("map.<xmlattr>.width");
      mNumTilesY = pt.get<int>("map.<xmlattr>.height");
      try {
        std::string bgColor = pt.get<std::string>("map.<xmlattr>.backgroundcolor");
        int r = 0, g = 0, b = 0;
        if (bgColor.size() == 7 && bgColor[0] == '#') {
          r = digit2Int(bgColor[1]) << 8 | digit2Int(bgColor[2]);
          g = digit2Int(bgColor[3]) << 8 | digit2Int(bgColor[4]);
          b = digit2Int(bgColor[5]) << 8 | digit2Int(bgColor[6]);
        }
      } catch (boost::property_tree::ptree_error &e) { UNUSED(e); }


#ifndef NDEBUG
      std::cout << "Map size: " << mNumTilesX << "x" << mNumTilesY << std::endl;
#endif
      uint8_t *compressed = nullptr;
      uLong compressedSize = 0UL;
      base64_decode(mapDataB64, compressed, compressedSize);
      if (compressed != nullptr && compressedSize > 0) {
        static const int CHUNKSIZE = 1*1024*1024;
        mMapData = (uint32_t*)std::malloc(CHUNKSIZE);
        uLongf mapDataSize = CHUNKSIZE;
        int rc = uncompress((Bytef*)mMapData, &mapDataSize, (Bytef*)compressed, compressedSize);
        if (rc == Z_OK) {
          mMapData = reinterpret_cast<uint32_t*>(std::realloc(mMapData, mapDataSize));
#ifndef NDEBUG
          std::cout << "map data contains " << (mapDataSize / sizeof(uint32_t)) << " elements." << std::endl;
#endif
        }
        else {
          std::cerr << "Inflating map data failed with code " << rc << std::endl;
          ok = false;
        }
        delete [] compressed;
      }

      if (!ok)
        return false;

      const std::string &backgroundTextureFilename = gLevelsRootDir + pt.get<std::string>("map.imagelayer.image.<xmlattr>.source");
      mBackgroundTexture.loadFromFile(backgroundTextureFilename);
      mBackgroundSprite.setTexture(mBackgroundTexture);
      try {
        mBackgroundImageOpacity = pt.get<float>("map.imagelayer.<xmlattr>.opacity");
      } catch (boost::property_tree::ptree_error &e) { UNUSED(e); }
      mBackgroundSprite.setColor(sf::Color(255, 255, 255, sf::Uint8(mBackgroundImageOpacity * 0xff)));

      mTextures.clear();
      const boost::property_tree::ptree &tileset = pt.get_child("map.tileset");
      mFirstGID = tileset.get<uint32_t>("<xmlattr>.firstgid");
      boost::property_tree::ptree::const_iterator ti;
      for (ti = tileset.begin(); ti != tileset.end(); ++ti) {
        boost::property_tree::ptree tile = ti->second;
        if (ti->first == "tile") {
          std::string tileName;
          int score = -1;
          const int id = mFirstGID + tile.get<int>("<xmlattr>.id");
          const std::string &filename = gLevelsRootDir + tile.get<std::string>("image.<xmlattr>.source");
          const boost::property_tree::ptree &properties = tile.get_child("properties");
          boost::property_tree::ptree::const_iterator pi;
          for (pi = properties.begin(); pi != properties.end(); ++pi) {
            boost::property_tree::ptree property = pi->second;
            if (pi->first == "property") {
              const std::string &propName = property.get<std::string>("<xmlattr>.name"); 
              if (propName == "Name") {
                tileName = property.get<std::string>("<xmlattr>.value");
              }
              else if (propName == "Points") {
                score = property.get<int>("<xmlattr>.value");
              }
            }
          }
          addTexture(filename, id, tileName);
          mScores[id] = score;
        }
      }
    }
    catch (boost::property_tree::ptree_error &e) {
      std::cerr << "Error parsing TMX file: " << e.what() << std::endl;
      ok = false;
    }
    return ok;
  }


  void Level::clear(void)
  {
    mTextures.clear();
    mTileMapping.clear();
    safeFree(mMapData);
  }


  void Level::addTexture(const std::string &filename, int index, const std::string &tileName)
  {
    mTextures[index].loadFromFile(filename);
    mTextureNames[index] = tileName;
  }


  int Level::bodyIndexByTextureName(const std::string &name) const
  {
    int result = -1;
    std::map<int, std::string>::const_iterator i;
    for (i = mTextureNames.cbegin(); i != mTextureNames.cend(); ++i) {
      if (i->second == name)
        return i->first;
    }
    return result;
  }


  const sf::Texture &Level::texture(int index) const
  {
    return mTextures.at(index);
  }


  const sf::Texture &Level::texture(const std::string &name) const
  {
    return texture(bodyIndexByTextureName(name));
  }


  const std::string &Level::textureName(int index)
  {
    return mTextureNames.at(index);
  }


  uint32_t *const Level::mapDataScanLine(int y) const
  {
    return mMapData + y * mNumTilesX;
  }


  uint32_t Level::mapData(int x, int y) const
  {
    return mapDataScanLine(y)[x];
  }


  int Level::score(int index) const
  {
    return mScores.at(index);
  }

}
