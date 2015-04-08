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
#include "util.h"
#include "Recorder.h"

extern "C" {
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libavutil/imgutils.h>
}


namespace Impact {

  Recorder::Recorder(Game *game)
    : mGame(game)
    , mDoQuit(false)
    , mAudioClient(nullptr)
    , mCaptureClient(nullptr)
    , mRecThread(nullptr)
    , mAudioCtx(nullptr)
    , mAudioCodec(nullptr)
    , mAudioFile(nullptr)
    , mVideoCtx(nullptr)
    , mVideoCodec(nullptr)
    , mVideoFile(nullptr)
    , mSamples(nullptr)
    , mSamplesEnd(nullptr)
    , mCurrentFrame(nullptr)
    , mAudioFrame(nullptr)
    , mBufferSize(0)
    , mNewVideoFrameAvailable(false)
  {
    avcodec_register_all();

    HRESULT hr;
#ifndef NDEBUG
    std::cout << "initRecorder() ..." << std::endl;
#endif
    const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
    const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
    const IID IID_IAudioClient = __uuidof(IAudioClient);
    const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

    mActualDuration = 0;
    UINT32 bufferFrameCount;
    IMMDeviceEnumerator *pEnumerator = nullptr;
    IMMDevice *pDevice = nullptr;

    hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator);
    if (FAILED(hr)) {
      std::cerr << "CoCreateInstance() failed in line " << __LINE__ << std::endl;
      return;
    }

    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    if (FAILED(hr)) {
      std::cerr << "pEnumerator->GetDefaultAudioEndpoint() failed in line " << __LINE__ << std::endl;
      return;
    }

    hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&mAudioClient);
    if (FAILED(hr)) {
      std::cerr << "pDevice->Activate() failed in line " << __LINE__ << std::endl;
      return;
    }

    hr = mAudioClient->GetMixFormat(&mWFX);
    if (FAILED(hr)) {
      std::cerr << "pAudioClient->GetMixFormat() failed in line " << __LINE__ << std::endl;
      return;
    }

    hr = mAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, mActualDuration, 0, mWFX, NULL);
    if (FAILED(hr)) {
      std::cerr << "mAudioClient->Initialize() failed in line " << __LINE__ << std::endl;
      return;
    }

    hr = mAudioClient->GetBufferSize(&bufferFrameCount);
    if (FAILED(hr)) {
      std::cerr << "pAudioClient->GetBufferSize() failed in line " << __LINE__ << std::endl;
      return;
    }

    hr = mAudioClient->GetService(IID_IAudioCaptureClient, (void**)&mCaptureClient);
    if (FAILED(hr)) {
      std::cerr << "pAudioClient->GetService() failed in line " << __LINE__ << std::endl;
      return;
    }

    mActualDuration = 1000 * bufferFrameCount / mWFX->nSamplesPerSec;

    hr = this->start();
    if (FAILED(hr)) {
      std::cerr << "this->start() failed in line " << __LINE__ << std::endl;
      return;
    }

    safeRelease(pEnumerator);
    safeRelease(pDevice);
  }


  Recorder::~Recorder()
  {
#ifndef NDEBUG
    std::cout << "Recorder::~Recorder()" << std::endl;
#endif
    this->stop();
    safeDelete(mRecThread);
#if defined(WIN32)
    CoTaskMemFree(mWFX);
    safeRelease(mAudioClient);
    safeRelease(mCaptureClient);
#endif
  }


  void Recorder::capture(void)
  {
    HRESULT hr = S_OK;
    UINT32 packetLength = 0;
    while (!mDoQuit) {
      hr = mCaptureClient->GetNextPacketSize(&packetLength);
      if (FAILED(hr))
        mDoQuit = true;
      BYTE *pData;
      UINT32 numAudioFramesAvailable;
      DWORD flags;
      while (packetLength != 0 && !mDoQuit) {
        hr = mCaptureClient->GetBuffer(&pData, &numAudioFramesAvailable, &flags, NULL, NULL);
        if (FAILED(hr))
          std::cerr << "mCaptureClient->GetBuffer() failed on line " << __LINE__ << std::endl;
        if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
          pData = nullptr;
        hr = this->copyData(pData, numAudioFramesAvailable, &mDoQuit);
        if (FAILED(hr))
          std::cerr << "this->CopyData() failed on line " << __LINE__ << std::endl;
        hr = mCaptureClient->ReleaseBuffer(numAudioFramesAvailable);
        if (FAILED(hr))
          std::cerr << "mCaptureClient->ReleaseBuffer() failed on line " << __LINE__ << std::endl;
        hr = mCaptureClient->GetNextPacketSize(&packetLength);
        if (FAILED(hr))
          std::cerr << "mCaptureClient->GetNextPacketSize() failed on line " << __LINE__ << std::endl;
      }
      // Sleep(mActualDuration / 2);
      Sleep(1000 / 50);
    }
    hr = mAudioClient->Stop();
    if (FAILED(hr))
      std::cerr << "mAudioClient->Stop() failed on line " << __LINE__ << std::endl;
  }


  HRESULT Recorder::start(void)
  {
    int ret;

    if (mRecThread != nullptr)
      return S_FALSE;

    mDoQuit = false;
    mRecThread = new std::thread(&Recorder::capture, this);

    mAudioCodec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if (mAudioCodec == nullptr) {
      std::cerr << "avcodec_find_encoder() failed in line " << __LINE__ << std::endl;
      return S_FALSE;
    }

    mAudioCtx = avcodec_alloc_context3(mAudioCodec);
    if (!mAudioCtx) {
      std::cerr << "avcodec_alloc_context3() failed in line " << __LINE__ << std::endl;
      return S_FALSE;
    }

    mAudioCtx->bit_rate = 160 * 1000;
    mAudioCtx->sample_fmt = AV_SAMPLE_FMT_S16;
    mAudioCtx->sample_rate = mWFX->nSamplesPerSec;
    mAudioCtx->profile = FF_PROFILE_AAC_MAIN;
    mAudioCtx->channel_layout = AV_CH_LAYOUT_STEREO;
    mAudioCtx->channels = av_get_channel_layout_nb_channels(mAudioCtx->channel_layout);

    ret = avcodec_open2(mAudioCtx, mAudioCodec, NULL);
    if (ret < 0) {
      std::cerr << "avcodec_open2() failed in line " << __LINE__ << std::endl;
      return S_FALSE;
    }

    mAudioFile = fopen("recordings/blah.aac", "wb+");
    if (!mAudioFile) {
      std::cerr << "fopen() failed in line " << __LINE__ << std::endl;
      return S_FALSE;
    }

    mAudioFrame = av_frame_alloc();
    if (!mAudioFrame) {
      std::cerr << "Could not allocate audio frame in line " << __LINE__ << std::endl;
      return S_FALSE;
    }
    mAudioFrame->nb_samples = mAudioCtx->frame_size;
    mAudioFrame->format = mAudioCtx->sample_fmt;
    mAudioFrame->channel_layout = mAudioCtx->channel_layout;

    mBufferSize = av_samples_get_buffer_size(NULL, mAudioCtx->channels, mAudioCtx->frame_size, mAudioCtx->sample_fmt, 0);
    if (mBufferSize < 0) {
      std::cerr << "Could not get sample buffer size in line " << __LINE__ << std::endl;
      return S_FALSE;
    }

    mSamples = (uint8_t*)av_malloc(2 * mBufferSize);
    if (!mSamples) {
      std::cerr << "Could not allocate " << mBufferSize << " bytes for samples buffer in line " << __LINE__ << std::endl;
      return S_FALSE;
    }
    mSamplesEnd = mSamples + mBufferSize;
    mCurrentFrame = mSamples;

    ret = avcodec_fill_audio_frame(mAudioFrame, mAudioCtx->channels, mAudioCtx->sample_fmt, (const uint8_t*)mSamples, mBufferSize, 0);
    if (ret < 0) {
      std::cerr << "Could not setup audio frame in line " << __LINE__ << std::endl;
      return S_FALSE;
    }

    mVideoCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (mVideoCodec == nullptr) {
      std::cerr << "avcodec_find_encoder(AV_CODEC_ID_H264) failed in line " << __LINE__ << std::endl;
      return S_FALSE;
    }

    mVideoCtx = avcodec_alloc_context3(mVideoCodec);
    if (!mVideoCtx) {
      std::cerr << "avcodec_alloc_context3() failed in line " << __LINE__ << std::endl;
      return S_FALSE;
    }

    mVideoCtx->bit_rate = 400000;
    mVideoCtx->width = 640;
    mVideoCtx->height = 480;
    mVideoCtx->time_base = av_make_q(1, 25);
    /* emit one intra frame every ten frames
    * check frame pict_type before passing frame
    * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
    * then gop_size is ignored and the output of encoder
    * will always be I frame irrespective to gop_size
    */
    mVideoCtx->gop_size = 10;
    mVideoCtx->max_b_frames = 1;
    mVideoCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    av_opt_set(mVideoCtx->priv_data, "preset", "slow", 0);

    if (avcodec_open2(mVideoCtx, mVideoCodec, NULL) < 0) {
      std::cerr << "Could not open video codec in line " << __LINE__ << std::endl;
      return S_FALSE;
    }

    mVideoFile = fopen("recordings/blah.mp4", "wb+");
    if (!mVideoFile) {
      std::cerr << "fopen() failed in line " << __LINE__ << std::endl;
      return S_FALSE;
    }

    mVideoFrame = av_frame_alloc();
    if (!mVideoFrame) {
      std::cerr << "Could not allocate video frame in line " << __LINE__ << std::endl;
      return S_FALSE;
    }
    mVideoFrame->format = mVideoCtx->pix_fmt;
    mVideoFrame->width = mVideoCtx->width;
    mVideoFrame->height = mVideoCtx->height;

    /* the image can be allocated by any means and av_image_alloc() is
    * just the most convenient way if av_malloc() is to be used */
    ret = av_image_alloc(mVideoFrame->data, mVideoFrame->linesize, mVideoCtx->width, mVideoCtx->height, mVideoCtx->pix_fmt, 32);
    if (ret < 0) {
      std::cerr << "Could not allocate raw picture buffer in line " << __LINE__ << std::endl;
      return S_FALSE;
    }

    mVideoFrameNumber = 0;

    HRESULT hr;
    hr = mAudioClient->Start();
    if (FAILED(hr)) {
      std::cerr << "mAudioClient->Start() failed on line " << __LINE__ << std::endl;
      return S_FALSE;
    }

#ifndef NDEBUG
    std::cout << "AUDIO INPUT:" << std::endl
      << "mActualDuration: " << mActualDuration << std::endl
      << "wFormatTag:      " << std::showbase << std::internal << std::hex << std::setw(4) << mWFX->wFormatTag << std::endl << std::dec
      << "nChannels:       " << mWFX->nChannels << std::endl
      << "nSamplesPerSec:  " << mWFX->nSamplesPerSec << std::endl
      << "nAvgBytesPerSec: " << mWFX->nAvgBytesPerSec << std::endl
      << "nBlockAlign:     " << mWFX->nBlockAlign << std::endl
      << "wBitsPerSample:  " << mWFX->wBitsPerSample << std::endl
      << "cbSize:          " << mWFX->cbSize << std::endl;
    std::cout << "AUDIO OUTPUT:" << std::endl
      << "mAudioCtx->frame_size:       " << mAudioCtx->frame_size << std::endl
      << "mAudioCtx->channels:         " << mAudioCtx->channels << std::endl
      << "mAudioCtx->channel_layout:   " << mAudioCtx->channel_layout << std::endl
      << "mBufferSize:     " << mBufferSize << std::endl;
#endif

    return S_OK;
  }


  HRESULT Recorder::stop(void)
  {
    if (mRecThread != nullptr) {
#ifndef NDEBUG
      std::cout << "Recorder::stop() ..." << std::endl;
#endif
      mDoQuit = true;
      mRecThread->join();
#ifndef NDEBUG
      std::cout << "mRecThread returned." << std::endl;
#endif
      safeDelete(mRecThread);

      fclose(mAudioFile);

      av_free(mSamples);
      av_frame_free(&mAudioFrame);
      avcodec_close(mAudioCtx);
      av_free(mAudioCtx);

      static const uint8_t endcode[4] = { 0, 0, 1, 0xb7 };
      fwrite(endcode, 1, sizeof(endcode), mVideoFile);
      fclose(mVideoFile);

      avcodec_close(mVideoCtx);
      av_frame_free(&mVideoFrame);
      av_free(mVideoCtx);
    }
    return S_OK;
  }



  HRESULT Recorder::copyData(BYTE *pData, UINT32 nFrames, bool *done)
  {
    /* float -> short
    1b d9 b4 b9 | 1b d9 b4 b9 | da 45 90 3c | da 45 90 3c | 9a ef 11 3d | 9a ef 11 3d | 4d 0e 5c 3d | 4d 0e 5c 3d |
    12 33 93 3d | 12 33 93 3d | 58 59 b8 3d | 58 59 b8 3d | 08 56 dd 3d | 08 56 dd 3d | 6b f0 00 3e | 6b f0 00 3e | ...
    */
    const int nBytesPerOutputFrame = sizeof(sample_t) * mWFX->nChannels;
    const int nOutputBufSize = nFrames * nBytesPerOutputFrame;
    sample_t *dst = reinterpret_cast<sample_t*>(mCurrentFrame);
    if (pData != nullptr) {
      const float32 *src = reinterpret_cast<float32*>(pData);
      for (UINT32 i = 0; i < nFrames; ++i) {
        for (WORD j = 0; j < mWFX->nChannels; ++j) {
          const sample_t sample = int(std::numeric_limits<sample_t>::max() * (*src++));
          *dst++ = sample;
        }
      }
    }
    else {
      memset(dst, 0, nOutputBufSize);
    }

    mCurrentFrame += nOutputBufSize;

    const int overhead = mCurrentFrame - mSamplesEnd;
    if (overhead < 0)
      return S_OK;

    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = nullptr;
    pkt.size = 0;
    int gotOutput = 0;
    int ret = avcodec_encode_audio2(mAudioCtx, &pkt, mAudioFrame, &gotOutput);
    if (ret < 0) {
      std::cerr << "Error encoding frame in line " << __LINE__ << std::endl;
      return S_FALSE;
    }
    if (gotOutput) {
      fwrite(pkt.data, pkt.size, 1, mAudioFile);
      av_free_packet(&pkt);
    }

    memcpy(mSamples, mSamplesEnd, overhead);
    mCurrentFrame = mSamples + overhead;

    do {
      ret = avcodec_encode_audio2(mAudioCtx, &pkt, NULL, &gotOutput);
      if (ret < 0) {
        std::cerr << "Error encoding frame in line " << __LINE__ << std::endl;
        return S_FALSE;
      }
      if (gotOutput) {
        fwrite(pkt.data, pkt.size, 1, mAudioFile);
        av_free_packet(&pkt);
      }

    } while (gotOutput);


    if (mNewVideoFrameAvailable) {
#if 0
      const sf::Image &image = mCurrentVideoFrame;
      if (image.getSize().x > 0 && image.getSize().y > 0) {
        av_init_packet(&pkt);
        pkt.data = nullptr;
        pkt.size = 0;
        const sf::Uint8 *pImage = image.getPixelsPtr();
        for (int y = 0; y < mVideoCtx->height; ++y) {
          // TODO optimize loop and indexing mVideoFrame->data
          for (int x = 0; x < mVideoCtx->width; ++x) {
            const sf::Color &pixel = image.getPixel(x, y);
            mVideoFrame->data[0][y * mVideoFrame->linesize[0] + x] =
              uint8_t((0.257f * pixel.r) + (0.504f * pixel.g) + (0.098f * pixel.b) + 16);
          }
        }
        for (int y = 0; y < mVideoCtx->height / 2; ++y) {
          // TODO optimize loop and indexing mVideoFrame->data
          for (int x = 0; x < mVideoCtx->width / 2; ++x) {
            const sf::Color &pixel = image.getPixel(x, y);
            mVideoFrame->data[1][y * mVideoFrame->linesize[1] + x] =
              uint8_t((0.439f * pixel.r) - (0.368f * pixel.g) - (0.071f * pixel.b) + 128);
            mVideoFrame->data[2][y * mVideoFrame->linesize[2] + x] =
              uint8_t(-(0.148f * pixel.r) - (0.291f * pixel.g) + (0.439f * pixel.b) + 128);
          }
        }

        mVideoFrame->pts = mVideoFrameNumber;

        ret = avcodec_encode_video2(mVideoCtx, &pkt, mVideoFrame, &gotOutput);
        if (ret < 0) {
          std::cerr << "Error encoding frame in line " << __LINE__ << std::endl;
          return S_FALSE;
        }

        if (gotOutput) {
          fwrite(pkt.data, pkt.size, 1, mVideoFile);
          av_free_packet(&pkt);
          ++mVideoFrameNumber;
        }

        do {
          ret = avcodec_encode_video2(mVideoCtx, &pkt, NULL, &gotOutput);
          if (ret < 0) {
            std::cerr << "Error encoding frame in line " << __LINE__ << std::endl;
            return S_FALSE;
          }
          if (gotOutput) {
            fwrite(pkt.data, pkt.size, 1, mVideoFile);
            av_free_packet(&pkt);
            ++mVideoFrameNumber;
          }
        } while (gotOutput);


#ifndef NDEBUG
        std::cout << mVideoFrameNumber << std::endl;
#endif
      }
#endif
      std::ostringstream ssFrameNum;
      ssFrameNum << std::dec << std::setfill('0') << std::setw(8) << mVideoFrameNumber;
      mCurrentVideoFrame.saveToFile(std::string("recordings/snap-") + ssFrameNum.str() + ".jpg");
      ++mVideoFrameNumber;
      mNewVideoFrameAvailable = false;
    }

    return S_OK;
  }


  void Recorder::setFrame(const sf::Image &image)
  {
    mCurrentVideoFrame = image;
    mNewVideoFrameAvailable = true;
  }
}
