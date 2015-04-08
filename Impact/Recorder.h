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

#ifndef __RECORDER_H_
#define __RECORDER_H_

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#if defined(WIN32)
#include <Mmdeviceapi.h>
#include <MMSystem.h>
#include <Audioclient.h>
#endif

#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Time.hpp>

#include <thread>

namespace Impact {

  class Recorder {
  public:
    Recorder(Game *game);
    ~Recorder();

    HRESULT start(void);
    HRESULT stop(void);

    void setFrame(const sf::Image &frame, const sf::Time &dt);

    AVRational timeBase(void) const;

  private:
    HRESULT copyAudioData(float32 *pData, UINT32 nFrames);
    void capture(void);

  private:
    Game *mGame;

    typedef int16_t sample_t;

    FILE *mAudioFile;
    AVCodecContext *mAudioCtx;
    AVCodec *mAudioCodec;
    uint8_t *mSamples;
    uint8_t *mSamplesEnd;
    uint8_t *mCurrentFrame;
    AVFrame *mAudioFrame;
    int mBufferSize;

    AVStream *mVideoOutStream;
    AVFormatContext* mVideoOutContainer;
    AVCodecContext *mVideoCtx;
    AVCodec *mVideoCodec;
    AVFrame *mVideoFrame;
    AVFrame *mRGBFrame;
    SwsContext *mSwsCtx;
    int64_t mVideoFrameNumber;
    sf::Image mCurrentVideoFrame;
    sf::Time mFrameTime;
    bool mNewVideoFrameAvailable;

    IAudioClient *mAudioClient;
    IAudioCaptureClient *mCaptureClient;
    WAVEFORMATEX *mWFX;
    DWORD mActualDuration;

    std::thread *mRecThread;
    bool mDoQuit;
  };

}

#endif
