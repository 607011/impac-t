// Code taken from https://github.com/jesusgollonet/ofpennereasing

#ifndef __EASINGS_H_
#define __EASINGS_H_

#include <cmath>

namespace Impact {


  /*
  * @t is the current time (or position) of the tween.
  *    This can be seconds or frames, steps, seconds, ms, ... –
  *    as long as the unit is the same as is used for the total time [3].
  * @b is the beginning value of the property.
  * @c is the change between the beginning and destination value of the property.
  * @d is the total time of the tween.
  */
  template <typename T>
  class Easing {
  public:
    static T quadEaseInForthAndBack(T t, T b, T c, T d) {
      float dt = t / d;
      dt = dt < .5f ? dt * 2 : 1 - 2 * (dt - .5f);
      return c * ((t = dt - 1) * t * t + 1) + b;
    }
    static T quadEaseIn(T t, T b, T c, T d)
    {
      return c * ( t /= d) * t + b;
    }
    static T quadEaseOut(T t, T b, T c, T d)
    {
      return -c * (t /= d) * (t - 2) + b;
    }
    static T quadEaseInOut(T t, T b, T c, T d)
    {
      if ((t/=d/2) < 1)
        return ((c/2)*(t*t)) + b;
      return -c/2 * (((t-2)*(--t)) - 1) + b;
    }
    static T bounceEaseIn(T t, T b, T c, T d)
    {
      return c - bounceEaseOut(d-t, 0, c, d) + b;
    }
    static T bounceEaseOut(T t, T b, T c, T d)
    {
      if ((t/=d) < (1/2.75f)) {
        return c*(7.5625f*t*t) + b;
      }
      else if (t < (2/2.75f)) {
        T postFix = t-=(1.5f/2.75f);
        return c*(7.5625f*(postFix)*t + .75f) + b;
      }
      else if (t < (2.5/2.75)) {
        T postFix = t-=(2.25f/2.75f);
        return c*(7.5625f*(postFix)*t + .9375f) + b;
      }
      else {
        T postFix = t-=(2.625f/2.75f);
        return c*(7.5625f*(postFix)*t + .984375f) + b;
      }
    }
    static T bounceEaseInOut(T t, T b, T c, T d) {
      if (t < d/2)
        return bounceEaseIn (t*2, 0, c, d) * .5f + b;
      return bounceEaseOut (t*2-d, 0, c, d) * .5f + c*.5f + b;
    }
    static T sineEaseIn (T t, T b, T c, T d) {
      return -c * std::cos(t/d * (3.14159265358979f/2)) + c + b;
    }
    static T sineEaseOut(T t, T b, T c, T d) {	
      return c * std::sin(t/d * (3.14159265358979f/2)) + b;	
    }
    static T sineEaseInOut(T t, T b, T c, T d) {
      return -c/2 * (std::cos(3.14159265358979f*t/d) - 1) + b;
    }
  };

}

#endif // __EASINGS_H_
