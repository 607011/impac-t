// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#include "stdafx.h"


namespace Breakout {

  const float Game::ShotSpeed = 9.f;
  const float Game::Scale = 16.f;

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
  {
    bool ok;


    mWindow.setVerticalSyncEnabled(true);
    // glewInit();
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

    mScoreMsg.setFont(mFixedFont);
    mScoreMsg.setCharacterSize(22);
    mScoreMsg.setColor(sf::Color(255, 255, 255));

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
    mLevel.set(1);
    buildLevel();
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


  void Game::evaluateCollisions(void)
  {
    for (int i = 0; i < mPointCount; ++i) {
      ContactPoint &cp = mPoints[i];

    }
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
        pause();
        break;
      case sf::Event::GainedFocus:
        resume();
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
          mBall->setPosition(float(mLevel.width() / 2), float(mLevel.height() - 2.5f));
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
      mPad->stopKick();
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
    mWindow.draw(mLevel.backgroundSprite());
    handleEvents();
    handlePlayerInteraction();
    if (!mPaused)
      update(elapsed.asSeconds());
    drawWorld(mPlayView);
    mScoreMsg.setString(std::to_string(mScore));
    mScoreMsg.setPosition(mPlayView.getCenter().x + mPlayView.getSize().x / 2 - mScoreMsg.getLocalBounds().width - 20, 20);
    mWindow.draw(mScoreMsg);
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
    evaluateCollisions();

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
      deadBody->remove(),
      mBodies.remove(deadBody);
      safeDelete(deadBody); // removes b2Body from world, see Body::~Body()
    }
    mDeadBodies.clear();
  }


  void Game::PreSolve(b2Contact *contact, const b2Manifold *oldManifold)
  {
    //const b2Manifold *manifold = contact->GetManifold();
    //if (manifold->pointCount == 0)
    //  return;

    //b2Fixture* fixtureA = contact->GetFixtureA();
    //b2Fixture* fixtureB = contact->GetFixtureB();

    //b2PointState state1[b2_maxManifoldPoints];
    //b2PointState state2[b2_maxManifoldPoints];
    //b2GetPointStates(state1, state2, oldManifold, manifold);

    //b2WorldManifold worldManifold;
    //contact->GetWorldManifold(&worldManifold);

    //for (int32 i = 0; i < manifold->pointCount && mPointCount < MaxContactPoints; ++i) {
    //  ContactPoint *cp = mPoints + mPointCount;
    //  cp->fixtureA = fixtureA;
    //  cp->fixtureB = fixtureB;
    //  cp->position = worldManifold.points[i];
    //  cp->normal = worldManifold.normal;
    //  cp->state = state2[i];
    //  cp->normalImpulse = manifold->points[i].normalImpulse;
    //  cp->tangentImpulse = manifold->points[i].tangentImpulse;
    //  cp->separation = worldManifold.separations[i];
    //  ++mPointCount;
    //}
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
    void *dA = contact->GetFixtureA()->GetBody()->GetUserData();
    void *dB = contact->GetFixtureB()->GetBody()->GetUserData();

    if (dA == nullptr || dB == nullptr)
      return;

    Body *a = reinterpret_cast<Body *>(dA);
    Body *b = reinterpret_cast<Body *>(dB);

    if (a->type() == Body::BodyType::Block || b->type() == Body::BodyType::Block) {
      if (a->type() == Body::BodyType::Ball || b->type() == Body::BodyType::Ball) {
        Block *block = reinterpret_cast<Block*>(a->type() == Body::BodyType::Block ? a : b);
        Ball *ball = reinterpret_cast<Ball*>(a->type() == Body::BodyType::Ball ? a : b);
        block->hit(impulse->normalImpulses[0]);
      }

    }
  }


  void Game::buildLevel(void)
  {
    mCurrentBodyId = 0;
    clearWorld();

    // create boundaries
    { 
      float32 W = float32(mLevel.width());
      float32 H = float32(mLevel.height());
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
      bd.position.Set(0.f, float32(mLevel.height() + 0.5f));
      mGround = mWorld->CreateBody(&bd);
    }

    // create pad
    {
      mPad = new Pad(this);
      mPad->setPosition(float(mLevel.width() / 2), float(mLevel.height() - 0.5f));
      addBody(mPad);
    }

    // create blocks
    {
      for (int y = 0; y < mLevel.height(); ++y) {
        const uint32_t *mapRow = mLevel.mapDataScanLine(y);
        for (int x = 0; x < mLevel.width(); ++x) {
          const uint32_t tileId = mapRow[x];
          if (tileId >= mLevel.firstGID()) {
            Block *block = new Block(tileId, this);
            block->setScore(mLevel.score(tileId));
            block->setPosition(float(x), float(y));
            addBody(block);
          }
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


  void Game::addBody(Body *body)
  {
    body->setId(mCurrentBodyId++);
    mBodies.push_back(body);
  }


  void Game::onBodyKilled(Body *killedBody)
  {
    mScore += killedBody->getScore();
  }


  const Level *Game::level(void) const
  {
    return &mLevel;
  }


  b2World *Game::world(void)
  {
    return mWorld;
  }


  b2Body *Game::ground(void) const
  {
    return mGround;
  }



  int Game::tileWidth(void) const
  {
    return mLevel.tileWidth();
  }


  int Game::tileHeight(void) const
  {
    return mLevel.tileHeight();
  }


}
