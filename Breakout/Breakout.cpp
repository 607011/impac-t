// Copyright (c) 2015 Oliver Lau <oliver@ersatzworld.net>
// All rights reserved.

#include "stdafx.h"


namespace Breakout {

  const float Game::ShotSpeed = 9.f;

  Game::Game(void)
    : mWindow(sf::VideoMode(Game::DefaultWindowWidth, Game::DefaultWindowHeight, Game::ColorDepth), "Bangout")
    , mPointCount(0)
    , mScore(0)
    , mLevel(0)
    , mTotalScore(0)
    , mLives(3)
    , mState(State::Initialization)
    , mKeyMapping(Action::LastAction)
  {
    bool ok;

    mWorld = new b2World(b2Vec2(0.f, -10.f));
    mWorld->SetDestructionListener(&mDestructionListener);
	  mWorld->SetContactListener(this);

    mWindow.setVerticalSyncEnabled(true);
    // glewInit();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_TEXTURE_2D);
    mWindow.setActive();

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

    mClock.restart();
    restart();
  }


  Game::~Game(void)
  {
  }


  void Game::restart(void)
  {

#ifndef NDEBUG
    mLives = 1;
#else
    mLives = DefaultLives;
#endif

    mTotalScore = 0;
    mLevel.setLevel(1);
    setState(State::Playing);
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
    mWindow.clear(sf::Color(26, 13, 6));
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
        //resize();
        break;
      case sf::Event::LostFocus:
        //pause();
        break;
      case sf::Event::GainedFocus:
        //resume();
        break;
      case sf::Event::KeyPressed:
        if (event.key.code == mKeyMapping[Action::BackAction]) {
          mWindow.close();
        }
        break;
      }
    }
  }

  void Game::onPlaying(void)
  {
    const sf::Time &elapsed = mClock.restart();

    clearWindow();
    handleEvents();
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
    // draw all bodies ...
  }


  void Game::update(float elapsedSeconds)
  {
    mWorld->SetAllowSleeping(false);
    mWorld->SetWarmStarting(true);
    mWorld->SetContinuousPhysics(true);
    mWorld->SetSubStepping(false);

    mWorld->Step((float32)elapsedSeconds, VelocityIterations, PositionIterations);

    mPointCount = 0;
  
  }


  void Game::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
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



}
