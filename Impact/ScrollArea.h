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

#ifndef __SCROLLAREA_H_
#define __SCROLLAREA_H_

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

namespace Impact {

  class ScrollArea : public sf::Drawable {
  public:
    ScrollArea(void);

    void create(unsigned int width, unsigned int height);

    virtual void beginUpdate(float elapsedSeconds);
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
    virtual void finishUpdate(void);
    void draw(const sf::Drawable&);

    void setPosition(const sf::Vector2f &);
    void setTotalHeight(float);
    void setMousePosition(const sf::Vector2f &);
    float scrollTop(void) const;

  private:
    void scrollArea(const float);

    float mScrollTop;
    float mScrollBottom;
    float mScrollbarWidth;
    sf::View mRenderView;
    sf::RenderTexture mRenderTexture;
    sf::Sprite mContentsSprite;
    sf::Texture mScrollbarTexture;
    sf::Sprite mScrollbarSprite;
    bool mScrollbarVisible;
    sf::FloatRect mTotalArea;
    sf::FloatRect mSensitiveSectionTop;
    sf::FloatRect mSensitiveSectionBottom;
    sf::Vector2f mMousePos;
    sf::Vector2f mLastMousePos;
    bool mMouseDown;
    float mElapsedSeconds;

    static const float ScrollSpeed;
    static const float DefaultScrollbarWidth;
    static const float LeftPadding;
    static const float TopPadding;

    static const float SensitiveScrollAreaRational;
  };


}

#endif // __VERTICALSCROLLAREA_H_
