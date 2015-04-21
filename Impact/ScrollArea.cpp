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

#include "ScrollArea.h"


namespace Impact {

  const float ScrollArea::ScrollSpeed = 64.f;
  const float ScrollArea::DefaultScrollbarWidth = 8.f;
  const float ScrollArea::LeftPadding = 16.f;
  const float ScrollArea::TopPadding = 2.f;
  const float ScrollArea::SensitiveScrollAreaRational = .1f;


  ScrollArea::ScrollArea(void)
    : mScrollbarWidth(DefaultScrollbarWidth)
    , mScrollTop(0.f)
    , mScrollbarVisible(false)
    , mElapsedSeconds(0.f)
    , mMouseDown(false)
  {
    mScrollbarTexture.loadFromFile(ImagesDir + "/white-pixel.png");
    mScrollbarSprite.setTexture(mScrollbarTexture);
    mContentsSprite.setScale(1.f, -1.f);
  }


  void ScrollArea::create(unsigned int width, unsigned int height)
  {
    mRenderTexture.create(width, height);
    mRenderView = mRenderTexture.getDefaultView();
    mTotalArea = sf::FloatRect(sf::Vector2f(), sf::Vector2f(mRenderView.getSize().x, mRenderView.getSize().y));
  }


  void ScrollArea::beginUpdate(float elapsedSeconds)
  {
    mElapsedSeconds = elapsedSeconds;
    mRenderTexture.clear(sf::Color(0, 0, 0, 40));
  }


  void ScrollArea::finishUpdate(void)
  {
    mScrollTop = mRenderView.getCenter().y - .5f * mRenderView.getSize().y;
    mScrollBottom = mRenderView.getCenter().y + .5f * mRenderView.getSize().y;
    if (mScrollTop > 0.f && mSensitiveSectionTop.contains(mMousePos)) {
      mRenderView.move(0.f, -ScrollSpeed * mElapsedSeconds);
    }
    else if (mScrollBottom < mTotalArea.height && mSensitiveSectionBottom.contains(mMousePos)) {
      mRenderView.move(0.f, +ScrollSpeed * mElapsedSeconds);
    }
    mScrollTop = mRenderView.getCenter().y - .5f * mRenderView.getSize().y;

    mScrollbarVisible = (mTotalArea.height > mRenderView.getSize().y);
    if (mScrollbarVisible) {
      const float scrollRatio = 1.f - ((mTotalArea.height - mScrollTop - mRenderView.getSize().y) / (mTotalArea.height - mRenderView.getSize().y));
      const float scrollbarLeft = mRenderView.getCenter().x + .5f * mRenderView.getSize().x - mScrollbarWidth;
      const float scrollbarHeight = mRenderView.getSize().y * mRenderView.getSize().y / mTotalArea.height;
      const float scrollbarTop = scrollRatio * (mRenderView.getSize().y - scrollbarHeight);
      sf::FloatRect scrollbarRect(mTotalArea.left + scrollbarLeft, mTotalArea.top + scrollbarTop, mScrollbarWidth, scrollbarHeight);
      bool mouseOverScrollbar = scrollbarRect.contains(mMousePos);
      if (mouseOverScrollbar && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        mMouseDown = true;
        mLastMousePos = mMousePos;
      }
      mScrollbarSprite.setScale(mScrollbarWidth, scrollbarHeight);
      mScrollbarSprite.setPosition(scrollbarRect.left, scrollbarRect.top);
      mScrollbarSprite.setColor(sf::Color(255, 255, 255, mouseOverScrollbar ? 255 : 192));
    }
    mContentsSprite.setTexture(mRenderTexture.getTexture());
  }


  float ScrollArea::scrollTop(void) const
  {
    return mScrollTop;
  }


  void ScrollArea::setTotalHeight(float height)
  {
    mTotalArea.height = height;
  }


  void ScrollArea::setMousePosition(const sf::Vector2f &pos)
  {
    mMousePos = pos;
  }


  void ScrollArea::setPosition(const sf::Vector2f &pos)
  {
    mTotalArea.left = pos.x;
    mTotalArea.top = pos.y;
    mContentsSprite.setPosition(mTotalArea.left, mTotalArea.top + mRenderView.getSize().y);
    mSensitiveSectionTop = mContentsSprite.getGlobalBounds();
    mSensitiveSectionTop.height *= SensitiveScrollAreaRational;
    mSensitiveSectionTop.width -= mScrollbarWidth;
    mSensitiveSectionBottom = mContentsSprite.getGlobalBounds();
    mSensitiveSectionBottom.height *= SensitiveScrollAreaRational;
    mSensitiveSectionBottom.width -= mScrollbarWidth;
    mSensitiveSectionBottom.top += (1.f - SensitiveScrollAreaRational) * mContentsSprite.getGlobalBounds().height;
  }


  void ScrollArea::draw(sf::RenderTarget& target, sf::RenderStates states) const
  {
    UNUSED(states);
    target.draw(mContentsSprite);
    if (mScrollbarVisible) {
      target.draw(mScrollbarSprite);
    }
  }


  void ScrollArea::draw(const sf::Drawable &drawable)
  {
    mRenderTexture.draw(drawable);
  }

}