// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#include "stdafx.h"


namespace Breakout {

  const float Game::ShotSpeed = 9.f;
  const float Game::Scale = 16.f;
  const std::string Game::LevelsRootDir =  "resources/levels/";

  Game::Game(void)
    : mWindow(sf::VideoMode(Game::DefaultWindowWidth, Game::DefaultWindowHeight, Game::ColorDepth), "Bangout")
    , mWorld(nullptr)
    , mBall(nullptr)
    , mGround(nullptr)
    , mPointCount(0)
    , mScore(0)
    , mTotalScore(0)
    , mLives(3)
    , mPaused(false)
    , mState(State::Initialization)
    , mKeyMapping(Action::LastAction)
    , mFirstGID(1)
    , mMapData(nullptr)
    , mMapDataSize(0)
    , mNumTilesX(0)
    , mNumTilesY(0)
    , mTileWidth(0)
    , mTileHeight(0)
    , mLevelNum(0)
  {
    bool ok;


    mWindow.setVerticalSyncEnabled(true);
    glewInit();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_TEXTURE_2D);
    mWindow.setActive();
    resize();

    //sf::Image icon;
    //icon.loadFromFile("resources/gui/app-icon.png");
    //mWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    ok = mDecorationFont.loadFromFile("resources/fonts/planetkosmos.ttf");
    if (!ok)
      sf::err() << "resources/fonts/planetkosmos.ttf failed to load." << std::endl;
    ok = mFixedFont.loadFromFile("resources/fonts/emulogic.ttf");
    if (!ok)
      sf::err() << "resources/fonts/emulogic.ttf failed to load." << std::endl;

    mKeyMapping[Action::MoveLeft] = sf::Keyboard::Left;
    mKeyMapping[Action::MoveRight] = sf::Keyboard::Right;
    mKeyMapping[Action::SpecialAction] = sf::Keyboard::Y;
    mKeyMapping[Action::BackAction] = sf::Keyboard::Escape;
    mKeyMapping[Action::KickLeft] = sf::Keyboard::V;
    mKeyMapping[Action::KickRight] = sf::Keyboard::X;

    restart();
  }


  Game::~Game(void)
  {
    // ...
  }


  b2World *Game::world(void)
  {
    return mWorld;
  }


  b2Body *Game::ground(void) const
  {
    return mGround;
  }


  void Game::restart(void)
  {
    safeRenew(mWorld, new b2World(b2Vec2(0.f, 9.81f)));
    mWorld->SetDestructionListener(&mDestructionListener);
	  mWorld->SetContactListener(this);
    mWorld->SetAllowSleeping(false);
    mWorld->SetWarmStarting(true);
    mWorld->SetContinuousPhysics(false);
    mWorld->SetSubStepping(true);

#ifndef NDEBUG
    mLives = 1;
#else
    mLives = DefaultLives;
#endif
    mTotalScore = 0;
    setLevel(1);
    setState(State::Playing);
  }


  void Game::resize(void)
  {
    mDefaultView = sf::View(sf::FloatRect(0.f, 0.f, float(mWindow.getSize().x), float(mWindow.getSize().y)));
    mDefaultView.setCenter(0.5f * sf::Vector2f(mWindow.getSize()));
    mPlayView = sf::View(sf::FloatRect(0.f, 0.f, float(mWindow.getSize().x), float(mWindow.getSize().y)));
  }
    


  void Game::setState(State state)
  {
    mState = state;
#ifndef NDEBUG
    std::cout << "Game::setState(" << mState << ")\n";
#endif
  }


  void Game::enterLoop(void)
  {
    while (mWindow.isOpen()) {

      switch (mState) {
      case State::Playing:
        onPlaying();
        break;

      case State::WelcomeScreen:
        onWelcomeScreen();
        break;

      case State::CountdownBeforeLevelStarts:
        //onCountdownBeforeLevelStarts();
        break;

      case State::PlayerKilled:
        //onPlayerKilled();
        break;

      case State::PauseAfterLevelCompleted:
        //onPauseAfterLevelCompleted();
        break;

      case State::LevelCompleted:
        //onLevelCompleted();
        break;

      case State::Pausing:
        //onPausing();
        break;

      case State::CreditsScreen:
        //onCreditsScreen();
        break;

      default:
        break;
      }

      mWindow.display();
    }
  }


  void Game::clearWindow(void)
  {
    mWindow.clear();
  }


  void Game::clearWorld(void)
  {
    // safeDelete(mPlayer);
    mBodies.clear();
  }


  void Game::handleEvents(void)
  {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
      switch (event.type)
      {
      case sf::Event::Closed:
        mWindow.close();
        break;
      case sf::Event::Resized:
        resize();
        break;
      case sf::Event::LostFocus:
        //pause();
        break;
      case sf::Event::GainedFocus:
        //resume();
        break;
      case sf::Event::KeyReleased:
        //...
        break;
      case sf::Event::KeyPressed:
        if (event.key.code == mKeyMapping[Action::BackAction]) {
          mWindow.close();
        }
        else if (event.key.code == mKeyMapping[Action::KickLeft]) {
				  mPad->kickLeft();
        }
        else if (event.key.code == mKeyMapping[Action::KickRight]) {
				  mPad->kickRight();
        }
        else if (event.key.code == mKeyMapping[Action::SpecialAction]) {
          if (mBall)
            mBall->kill();
          mBall = new Ball(this);
          mBall->setPosition(float(mNumTilesX / 2), float(mNumTilesY - 2));
          addBody(mBall);
        }
        break;
      }
    }
  }


  void Game::handlePlayerInteraction(void)
  {
    if (!mPaused) {
      mPad->stopMotion();
      if (sf::Keyboard::isKeyPressed(mKeyMapping[Action::KickLeft]))
        mPad->kickLeft();
      if (sf::Keyboard::isKeyPressed(mKeyMapping[Action::KickRight]))
        mPad->kickRight();
      if (sf::Keyboard::isKeyPressed(mKeyMapping[Action::MoveLeft]))
        mPad->moveLeft();
      if (sf::Keyboard::isKeyPressed(mKeyMapping[Action::MoveRight]))
        mPad->moveRight();
    }
  }


  void Game::onPlaying(void)
  {
    const sf::Time &elapsed = mClock.restart();
    clearWindow();
    mWindow.draw(mBackgroundSprite);
    handleEvents();
    handlePlayerInteraction();
    if (!mPaused)
      update(elapsed.asSeconds());
    drawWorld(mPlayView);
  }


  void Game::onWelcomeScreen(void)
  {
    const sf::Time &elapsed = mClock.restart();
    clearWindow();
    drawWorld(mDefaultView);
  }


  void Game::drawWorld(const sf::View &view)
  {
    mWindow.setView(view);
    const BodyList &bodies = mBodies;
    for (BodyList::const_iterator b = bodies.cbegin(); b != bodies.cend(); ++b) {
      const Body *body = *b;
      if (body->isAlive()) {
        mWindow.draw(*body);
      }
    }
  }


  void Game::update(float elapsedSeconds)
  {
    mPointCount = 0;
    mWorld->Step(elapsedSeconds, VelocityIterations, PositionIterations);
    for (BodyList::iterator b = mBodies.begin(); b != mBodies.end(); ++b) {
      Body *body = *b;
      if (body->isAlive()) {
        body->update(elapsedSeconds);
      }
      else {
        mDeadBodies.push_back(*b);
      }
    }
    for (BodyList::iterator b = mDeadBodies.begin(); b != mDeadBodies.end(); ++b) {
      Body *deadBody = *b;
      mBodies.remove(deadBody);
      safeDelete(deadBody);
    }
    mDeadBodies.clear();
  }


  void Game::PreSolve(b2Contact *contact, const b2Manifold *oldManifold)
  {
    const b2Manifold *manifold = contact->GetManifold();
    if (manifold->pointCount == 0)
      return;

    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    b2PointState state1[b2_maxManifoldPoints];
    b2PointState state2[b2_maxManifoldPoints];
    b2GetPointStates(state1, state2, oldManifold, manifold);

    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);

    for (int32 i = 0; i < manifold->pointCount && mPointCount < MaxContactPoints; ++i) {
      ContactPoint *cp = mPoints + mPointCount;
      cp->fixtureA = fixtureA;
      cp->fixtureB = fixtureB;
      cp->position = worldManifold.points[i];
      cp->normal = worldManifold.normal;
      cp->state = state2[i];
      cp->normalImpulse = manifold->points[i].normalImpulse;
      cp->tangentImpulse = manifold->points[i].tangentImpulse;
      cp->separation = worldManifold.separations[i];
      ++mPointCount;
    }
  }


  void Game::BeginContact(b2Contact *contact)
  {
    B2_NOT_USED(contact);
  }


  void Game::EndContact(b2Contact *contact)
  {
    B2_NOT_USED(contact);
  }


  void Game::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse)
  {
    B2_NOT_USED(contact);
    B2_NOT_USED(impulse);
  }


  // level functions

  void Game::setLevel(int level)
  {
    mLevelNum = level;
    if (mLevelNum > 0) {
      loadLevel();
      buildLevel();
    }
  }


  #pragma warning(disable : 4503)
  void Game::loadLevel(void)
  {
    safeFree(mMapData);
    mBackgroundImageOpacity = 1.f;

    std::ostringstream buf;
    buf << LevelsRootDir << std::setw(4) << std::setfill('0') << (1) << ".tmx";
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
        else
          sf::err() << "Inflating map data failed with code " << rc << "\n";
        delete [] compressed;
      }
      const std::string &backgroundTextureFilename = LevelsRootDir + pt.get<std::string>("map.imagelayer.image.<xmlattr>.source");
      mBackgroundTexture.loadFromFile(backgroundTextureFilename);
      mBackgroundSprite.setTexture(mBackgroundTexture);
      mBackgroundImageOpacity = pt.get<float>("map.imagelayer.<xmlattr>.opacity");
      mBackgroundSprite.setColor(sf::Color(255, 255, 255, sf::Uint8(mBackgroundImageOpacity * 0xff)));

      mTextures.clear();
      const boost::property_tree::ptree &tileset = pt.get_child("map.tileset");
      mFirstGID = tileset.get<uint32_t>("<xmlattr>.firstgid");
      boost::property_tree::ptree::const_iterator ti;
      std::string tileName;
      for (ti = tileset.begin(); ti != tileset.end(); ++ti) {
        boost::property_tree::ptree tile = ti->second;
        if (ti->first == "tile") {
          const int id = mFirstGID + tile.get<int>("<xmlattr>.id");
          const std::string &filename = LevelsRootDir + tile.get<std::string>("image.<xmlattr>.source");
          const boost::property_tree::ptree &properties = tile.get_child("properties");
          boost::property_tree::ptree::const_iterator pi;
          for (pi = properties.begin(); pi != properties.end(); ++pi) {
            boost::property_tree::ptree property = pi->second;
            if (pi->first == "property") {
              if (property.get<std::string>("<xmlattr>.name") == "Name" ) {
                tileName = property.get<std::string>("<xmlattr>.value");
                break;
              }
            }
          }
          mTextures.add(filename, id, tileName);
        }
      }
    }
  }


  void Game::buildLevel(void)
  {
    mCurrentBodyId = 0;
    clearWorld();

    // create boundaries
    { 
      float32 W = float32(mNumTilesX);
      float32 H = float32(mNumTilesY) + 2;
      b2BodyDef bd;
      b2Body *boundaries = mWorld->CreateBody(&bd);
      b2EdgeShape topBoundary;
      topBoundary.Set(b2Vec2(0, 0), b2Vec2(W, 0));
			boundaries->CreateFixture(&topBoundary, 0);
      b2EdgeShape rightBoundary;
      rightBoundary.Set(b2Vec2(W, 0), b2Vec2(W, H));
      boundaries->CreateFixture(&rightBoundary, 0.f);
      b2EdgeShape bottomBoundary;
      bottomBoundary.Set(b2Vec2(0.f, H), b2Vec2(W, H));
      boundaries->CreateFixture(&bottomBoundary, 0.f);
      b2EdgeShape leftBoundary;
      leftBoundary.Set(b2Vec2(0, 0), b2Vec2(0, H));
			boundaries->CreateFixture(&leftBoundary, 0.f);
    }

    // create virtual ground
    {
      b2BodyDef bd;
      bd.position.Set(0.f, float32(mNumTilesY - 1));
      mGround = mWorld->CreateBody(&bd);
    }

    // create pad
    mPad = new Pad(this);
    mPad->setPosition(float(mNumTilesX / 2), float(mNumTilesY - 1.5f));
    addBody(mPad);

    // create blocks
    for (int y = 0; y < mNumTilesY; ++y) {
      const uint32_t *mapRow = mapDataScanLine(y);
      for (int x = 0; x < mNumTilesX; ++x) {
        const uint32_t tileId = mapRow[x];
        if (tileId >= mFirstGID) {
          Block *block = new Block(tileId, this);
          block->setPosition(float(x), float(y));
          addBody(block);
        }
      }
    }

  }


  void Game::pause(void)
  {
    mPaused = true;
  }


  void Game::resume(void)
  {
    mPaused = false;
  }


  uint32_t *const Game::mapDataScanLine(int y) const
  {
    return mMapData + y * mNumTilesX;
  }


  uint32_t Game::mapData(int x, int y) const
  {
    return mapDataScanLine(y)[x];
  }


  int Game::width(void) const
  {
    return mNumTilesX;
  }


  int Game::height(void) const
  {
    return mNumTilesY;
  }


  int Game::tileWidth(void) const
  {
    return mTileWidth;
  }


  int Game::tileHeight(void) const
  {
    return mTileHeight;
  }


  int Game::levelWidth(void) const
  {
    return mNumTilesX * mTileWidth;
  }


  int Game::levelHeight(void) const
  {
    return mNumTilesY * mTileHeight;
  }


  void Game::addBody(Body *body)
  {
    body->setId(mCurrentBodyId++);
    mBodies.push_back(body);
  }


  void Game::onBodyKilled(Body *killedBody)
  {
    UNUSED(killedBody);
  }
}
