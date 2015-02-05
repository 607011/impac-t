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


namespace Impact {

  const float32 Level::DefaultGravity = 9.81f;

  Level::Level(void)
    : mBackgroundColor(sf::Color::Black)
    , mFirstGID(0)
    , mMapData(nullptr)
    , mNumTilesX(0)
    , mNumTilesY(0)
    , mTileWidth(0)
    , mTileHeight(0)
    , mLevelNum(0)
    , mGravity(9.81f)
    , mExplosionParticlesCollideWithBall(false)
    , mKillingsPerKillingSpree(Game::DefaultKillingsPerKillingSpree)
    , mKillingSpreeBonus(Game::DefaultKillingSpreeBonus)
    , mKillingSpreeInterval(Game::DefaultKillingSpreeInterval)
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


  static inline int hexDigit2Int(char c) {
    if (c < 'A')
      return int(c - '0');
    else if (c < 'a')
      return int(c - 'A');
    return int(c - 'a');
  }


#pragma warning(disable : 4503)
  bool Level::load(void)
  {
    bool ok = true;
    safeFree(mMapData);
    mBackgroundImageOpacity = 1.f;

    std::ostringstream buf;
#ifndef NDEBUG
    // mLevelNum = 8;
#endif
    buf << gLevelsDir << "/" << std::setw(4) << std::setfill('0') << mLevelNum << ".tmx";
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

    try { // evaluate level properties
      mGravity = DefaultGravity;
      mKillingsPerKillingSpree = Game::DefaultKillingsPerKillingSpree;
      mKillingSpreeBonus = Game::DefaultKillingSpreeBonus;
      mKillingSpreeInterval = Game::DefaultKillingSpreeInterval;
      mExplosionParticlesCollideWithBall = false;
      const boost::property_tree::ptree &layerProperties = pt.get_child("map.layer.properties");
      boost::property_tree::ptree::const_iterator pi;
      for (pi = layerProperties.begin(); pi != layerProperties.end(); ++pi) {
        boost::property_tree::ptree property = pi->second;
        if (pi->first == "property") {
          std::string propName = property.get<std::string>("<xmlattr>.name");
          boost::algorithm::to_lower(propName);
          if (propName == "Gravity") {
            mGravity = property.get<float32>("<xmlattr>.value");
          }
          else if (propName == "explosionparticlescollidewithball") {
            mExplosionParticlesCollideWithBall = property.get<int>("<xmlattr>.value") > 0;
#ifndef NDEBUG
            std::cout << "mExplosionParticlesCollideWithBall = " << mExplosionParticlesCollideWithBall << std::endl;
#endif
          }
          else if (propName == "killingspreebonus") {
            mKillingSpreeBonus = property.get<int>("<xmlattr>.value");
#ifndef NDEBUG
            std::cout << "mKillingSpreeBonus = " << mKillingSpreeBonus << std::endl;
#endif
          }
          else if (propName == "killingspreeinterval") {
            mKillingSpreeInterval = sf::milliseconds(property.get<int>("<xmlattr>.value"));
#ifndef NDEBUG
            std::cout << "mKillingSpreeInterval = " << mKillingSpreeInterval.asMilliseconds() << std::endl;
#endif
          }
          else if (propName == "killingsperkillingspree") {
            mKillingsPerKillingSpree = property.get<int>("<xmlattr>.value");
#ifndef NDEBUG
            std::cout << "mKillingsPerKillingSpree = " << mKillingsPerKillingSpree << std::endl;
#endif
          }
        }
      }
    } catch (boost::property_tree::ptree_error &e) { UNUSED(e); }

    try {
      const std::string &mapDataB64 = pt.get<std::string>("map.layer.data");
      mTileWidth = pt.get<int>("map.<xmlattr>.tilewidth");
      mTileHeight = pt.get<int>("map.<xmlattr>.tileheight");
      mNumTilesX = pt.get<int>("map.<xmlattr>.width");
      mNumTilesY = pt.get<int>("map.<xmlattr>.height");
      try {
        const std::string &bgColor = pt.get<std::string>("map.<xmlattr>.backgroundcolor");
        int r = 0, g = 0, b = 0;
        if (bgColor.size() == 7 && bgColor[0] == '#') {
          r = hexDigit2Int(bgColor[1]) << 8 | hexDigit2Int(bgColor[2]);
          g = hexDigit2Int(bgColor[3]) << 8 | hexDigit2Int(bgColor[4]);
          b = hexDigit2Int(bgColor[5]) << 8 | hexDigit2Int(bgColor[6]);
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

      const std::string &backgroundTextureFilename = gLevelsDir + "/" + pt.get<std::string>("map.imagelayer.image.<xmlattr>.source");
      mBackgroundTexture.loadFromFile(backgroundTextureFilename);
      mBackgroundSprite.setTexture(mBackgroundTexture);
      try {
        mBackgroundImageOpacity = pt.get<float>("map.imagelayer.<xmlattr>.opacity");
      } catch (boost::property_tree::ptree_error &e) { UNUSED(e); }
      mBackgroundSprite.setColor(sf::Color(255, 255, 255, sf::Uint8(mBackgroundImageOpacity * 0xff)));

      mBoundary = Boundary();
      try {
        const boost::property_tree::ptree &object = pt.get_child("map.objectgroup.object");
        int x = object.get<int>("<xmlattr>.x");
        int y = object.get<int>("<xmlattr>.y");
        int w = object.get<int>("<xmlattr>.width");
        int h = object.get<int>("<xmlattr>.height");
        mBoundary.left = x;
        mBoundary.top = y;
        mBoundary.right = x + w;
        mBoundary.bottom = y + h;
        mBoundary.valid = true;
      } catch (boost::property_tree::ptree_error &e) { UNUSED(e); }

      mTiles.clear();
      const boost::property_tree::ptree &tileset = pt.get_child("map.tileset");
      mFirstGID = tileset.get<uint32_t>("<xmlattr>.firstgid");
      boost::property_tree::ptree::const_iterator ti;
      for (ti = tileset.begin(); ti != tileset.end(); ++ti) {
        boost::property_tree::ptree tile = ti->second;
        if (ti->first == "tile") {
          const int id = mFirstGID + tile.get<int>("<xmlattr>.id");
          const std::string &filename = gLevelsDir + "/" + tile.get<std::string>("image.<xmlattr>.source");
          ok = mTiles[id].texture.loadFromFile(filename);
          if (!ok)
            return false;
          const boost::property_tree::ptree &tileProperties = tile.get_child("properties");
          boost::property_tree::ptree::const_iterator pi;
          for (pi = tileProperties.begin(); pi != tileProperties.end(); ++pi) {
            boost::property_tree::ptree property = pi->second;
            if (pi->first == "property") {
              try {
                std::string propName = property.get<std::string>("<xmlattr>.name");
                boost::algorithm::to_lower(propName);
                if (propName == "name") {
                  mTiles[id].textureName = property.get<std::string>("<xmlattr>.value");
                }
                else if (propName == "points") {
                  mTiles[id].score = property.get<int>("<xmlattr>.value");
                }
                else if (propName == "fixed") {
                  mTiles[id].fixed = property.get<int>("<xmlattr>.value") > 0;
                }
                else if (propName == "restitution") {
                  mTiles[id].restitution = property.get<float32>("<xmlattr>.value");
                }
                else if (propName == "density") {
                  mTiles[id].density = property.get<float32>("<xmlattr>.value");
                }
                else if (propName == "gravityscale") {
                  mTiles[id].gravityScale = property.get<float32>("<xmlattr>.value");
                }
              } catch (boost::property_tree::ptree_error &e) { UNUSED(e); }
            }
          }
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
    mTiles.clear();
    safeFree(mMapData);
  }


  int Level::bodyIndexByTextureName(const std::string &name) const
  {
    int result = -1;
    std::map<int, Tile>::const_iterator i;
    for (i = mTiles.cbegin(); i != mTiles.cend(); ++i) {
      if (i->second.textureName == name)
        return i->first;
    }
    return result;
  }


  const sf::Texture &Level::texture(const std::string &name) const
  {
    int index = bodyIndexByTextureName(name);
    if (index < 0)
      throw "Bad texture name: '" + name + "'";
    return mTiles.at(index).texture;
  }


  uint32_t *const Level::mapDataScanLine(int y) const
  {
    return mMapData + y * mNumTilesX;
  }


  uint32_t Level::mapData(int x, int y) const
  {
    return mapDataScanLine(y)[x];
  }


  const Tile &Level::tile(int index) const
  {
    return mTiles.at(index);
  }

}