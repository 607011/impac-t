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

#include <cmath>

extern "C" {
#include <libavcodec/avcodec.h>
}

namespace Impact {

  static int check_sample_fmt(AVCodec *codec, enum AVSampleFormat sample_fmt)
  {
    const enum AVSampleFormat *p = codec->sample_fmts;
    while (*p != AV_SAMPLE_FMT_NONE) {
      if (*p == sample_fmt)
        return 1;
      p++;
    }
    return 0;
  }

  static int select_sample_rate(AVCodec *codec)
  {
    const int *p;
    int best_samplerate = 0;
    if (!codec->supported_samplerates)
      return 44100;
    p = codec->supported_samplerates;
    while (*p) {
      best_samplerate = FFMAX(*p, best_samplerate);
      p++;
    }
    return best_samplerate;
  }

  /* select layout with the highest channel count */
  static uint64_t select_channel_layout(AVCodec *codec)
  {
    const uint64_t *p;
    uint64_t best_ch_layout = 0;
    int best_nb_channels = 0;
    if (!codec->channel_layouts)
      return AV_CH_LAYOUT_STEREO;
    p = codec->channel_layouts;
    while (*p) {
      int nb_channels = av_get_channel_layout_nb_channels(*p);
      if (nb_channels > best_nb_channels) {
        best_ch_layout = *p;
        best_nb_channels = nb_channels;
      }
      p++;
    }
    return best_ch_layout;
  }


  Recorder::Recorder(void)
    : mDoQuit(false)
    , mAudioClient(nullptr)
    , mCaptureClient(nullptr)
    , mRecThread(nullptr)
    , mAudioCodec(nullptr)
    , mCodec(nullptr)
    , mSamples(nullptr)
    , mCurrentFrame(nullptr)
    , mFrame(nullptr)
    , mBufferSize(0)
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
      UINT32 numFramesAvailable;
      DWORD flags;
      while (packetLength != 0) {
        hr = mCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, NULL, NULL);
        if (FAILED(hr))
          std::cerr << "mCaptureClient->GetBuffer() failed on line " << __LINE__ << std::endl;
        if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
          pData = nullptr;
        hr = this->copyData(pData, numFramesAvailable, &mDoQuit);
        if (FAILED(hr))
          std::cerr << "this->CopyData() failed on line " << __LINE__ << std::endl;
        hr = mCaptureClient->ReleaseBuffer(numFramesAvailable);
        if (FAILED(hr))
          std::cerr << "mCaptureClient->ReleaseBuffer() failed on line " << __LINE__ << std::endl;
        hr = mCaptureClient->GetNextPacketSize(&packetLength);
        if (FAILED(hr))
          std::cerr << "mCaptureClient->GetNextPacketSize() failed on line " << __LINE__ << std::endl;
      }
      // Sleep((DWORD)mActualDuration / 2);
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

    mCodec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if (mCodec == nullptr) {
      std::cerr << "avcodec_find_encoder() failed in line " << __LINE__ << std::endl;
      return S_FALSE;
    }

    mAudioCodec = avcodec_alloc_context3(mCodec);
    if (!mAudioCodec) {
      std::cerr << "avcodec_alloc_context3() failed in line " << __LINE__ << std::endl;
      return S_FALSE;
    }

    mAudioCodec->bit_rate = 160 * 1000;

    mAudioCodec->sample_fmt = AV_SAMPLE_FMT_S16;
    if (!check_sample_fmt(mCodec, mAudioCodec->sample_fmt)) {
      std::cerr << "Encoder does not support sample format " << av_get_sample_fmt_name(mAudioCodec->sample_fmt) << " in line " << __LINE__ << std::endl;
      return S_FALSE;
    }

    mAudioCodec->sample_rate = mWFX->nSamplesPerSec;
    // mAudioCodec->profile = FF_PROFILE_AAC_MAIN;
    mAudioCodec->channel_layout = AV_CH_FRONT_LEFT | AV_CH_FRONT_RIGHT; // select_channel_layout(mCodec);
    mAudioCodec->channels = 2; // av_get_channel_layout_nb_channels(mAudioCodec->channel_layout);

    ret = avcodec_open2(mAudioCodec, mCodec, NULL);
    if (ret < 0) {
      std::cerr << "avcodec_open2() failed in line " << __LINE__ << std::endl;
      return S_FALSE;
    }

    mFile = fopen("blah.aac", "wb+");
    if (!mFile) {
      std::cerr << "fopen() failed in line " << __LINE__ << std::endl;
      return S_FALSE;
    }

    mFrame = av_frame_alloc();
    if (!mFrame) {
      std::cerr << "Could not allocate audio frame in line " << __LINE__ << std::endl;
      return S_FALSE;
    }
    mFrame->nb_samples = mAudioCodec->frame_size;
    mFrame->format = mAudioCodec->sample_fmt;
    mFrame->channel_layout = mAudioCodec->channel_layout;

    mBufferSize = av_samples_get_buffer_size(NULL, mAudioCodec->channels, mAudioCodec->frame_size, mAudioCodec->sample_fmt, 0);
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

    ret = avcodec_fill_audio_frame(mFrame, mAudioCodec->channels, mAudioCodec->sample_fmt, (const uint8_t*)mSamples, mBufferSize, 0);
    if (ret < 0) {
      std::cerr << "Could not setup audio frame in line " << __LINE__ << std::endl;
      return S_FALSE;
    }

    HRESULT hr;
    hr = mAudioClient->Start();
    mDoQuit = false;
    if (FAILED(hr)) {
      std::cerr << "mAudioClient->Start() failed on line " << __LINE__ << std::endl;
      return S_FALSE;
    }
    mRecThread = new std::thread(&Recorder::capture, this);

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
      << "mAudioCodec->frame_size:       " << mAudioCodec->frame_size << std::endl
      << "mAudioCodec->channels:         " << mAudioCodec->channels << std::endl
      << "mAudioCodec->channel_layout:   " << mAudioCodec->channel_layout << std::endl
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

      fclose(mFile);

      av_free(mSamples);
      av_frame_free(&mFrame);
      avcodec_close(mAudioCodec);
      av_free(mAudioCodec);
    }
    return S_OK;
  }

  HRESULT Recorder::copyData(BYTE *pData, UINT32 numFramesAvailable, bool *done)
  {
    if (pData == nullptr)
      return S_OK;
    //float t = 0.f;
    //const float tincr = 2 * float(M_PI) * 46.5f / mAudioCodec->sample_rate;
    //for (int j = 0; j < mAudioCodec->frame_size; ++j) {
    //  mSamples[2 * j] = (int)(sin(t) * 18000);
    //  for (int k = 1; k < mAudioCodec->channels; ++k)
    //    mSamples[2 * j + k] = mSamples[2 * j];
    //  t += tincr;
    //}

    /* float -> short
    1b d9 b4 b9 | 1b d9 b4 b9 | da 45 90 3c | da 45 90 3c | 9a ef 11 3d | 9a ef 11 3d | 4d 0e 5c 3d | 4d 0e 5c 3d |
    12 33 93 3d | 12 33 93 3d | 58 59 b8 3d | 58 59 b8 3d | 08 56 dd 3d | 08 56 dd 3d | 6b f0 00 3e | 6b f0 00 3e | ...
    */

    int16_t *dst = reinterpret_cast<int16_t*>(mCurrentFrame);
    const int16_t *const dst0 = dst;
    if (pData != nullptr) {
      const float32 *src = reinterpret_cast<float32*>(pData);
      for (UINT32 i = 0; i < numFramesAvailable; ++i) {
        for (WORD j = 0; j < mWFX->nChannels; ++j) {
          *dst = int16_t((2 << 15) * (*src));
          ++dst;
          ++src;
        }
      }
    }
    else {
      memset(dst, 0, numFramesAvailable * mWFX->nBlockAlign);
    }
    mCurrentFrame += numFramesAvailable * mWFX->nBlockAlign;

    const int overhead = mCurrentFrame - mSamplesEnd;

#ifndef NDEBUG
    std::cout << std::dec << numFramesAvailable << " " << mBufferSize << " "
      << std::showbase << std::internal << std::hex << std::setfill('0')
      << std::setw(8) << (void*)mCurrentFrame << " - " << (void*)mSamplesEnd << " = " << std::dec << overhead
      << std::endl;
#endif

    if (overhead < 0)
      return S_OK;

#ifndef NDEBUG
    std::cout << "****ENCODING****" << std::endl;
#endif

    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    int gotOutput = 0;
    int ret = avcodec_encode_audio2(mAudioCodec, &pkt, mFrame, &gotOutput);
    if (ret < 0) {
      std::cerr << "Error encoding frame in line " << __LINE__ << std::endl;
      return S_FALSE;
    }
    if (gotOutput) {
      fwrite(pkt.data, pkt.size, 1, mFile);
      av_free_packet(&pkt);
    }

    memcpy(mSamples, mSamplesEnd, overhead);
    mCurrentFrame = mSamples + overhead;

    do {
      ret = avcodec_encode_audio2(mAudioCodec, &pkt, NULL, &gotOutput);
      if (ret < 0) {
        std::cerr << "Error encoding frame in line " << __LINE__ << std::endl;
        return S_FALSE;
      }
      if (gotOutput) {
        fwrite(pkt.data, pkt.size, 1, mFile);
        av_free_packet(&pkt);
#ifndef NDEBUG
        std::cout << "*************delayed frame" << std::endl;
#endif
      }

    } while (gotOutput);

    return S_OK;
  }

}
