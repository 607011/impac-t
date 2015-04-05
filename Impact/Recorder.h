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

#if defined(WIN32)
#include <Mmdeviceapi.h>
#include <MMSystem.h>
#include <Audioclient.h>
#endif

#include <thread>

namespace Impact {

  __declspec(align(1))
  struct WaveHeader {
    // RIFF part
    __declspec(align(1)) char chunkID[4];
    __declspec(align(1)) uint32_t ChunkSize;
    __declspec(align(1)) char riffType[4];
    // Format part
    __declspec(align(1)) char fmt[4];
    __declspec(align(1)) uint32_t fmtLen;
    __declspec(align(1)) uint16_t wFormatTag;
    __declspec(align(1)) uint16_t wChannels;
    __declspec(align(1)) uint32_t dwSamplesPerSec;
    __declspec(align(1)) uint32_t dwAvgBytesPerSec;
    __declspec(align(1)) uint16_t wBlockAlign;
    __declspec(align(1)) uint16_t wBitsPerSample;
    WaveHeader(void)
      : fmtLen(16)
    {
      memcpy_s(chunkID, 4, "RIFF", 4);
      memcpy_s(riffType, 4, "WAVE", 4);
      memcpy_s(fmt, 4, "fmt ", 4);
    }
  };

  class Recorder {
  public:
    Recorder(void);
    ~Recorder();

    HRESULT start(void);
    HRESULT stop(void);

  private:
    HRESULT copyData(BYTE *pData, UINT32 numFramesAvailable, bool *done);
    void capture(void);

  private:
    static const int RecordBufSize = 10 * 1024 * 1024;

    IAudioClient *mAudioClient;
    IAudioCaptureClient *mCaptureClient;
    WAVEFORMATEX *mWFX;
    REFERENCE_TIME mActualDuration;
    HWAVEIN mMicroHandle;
    WAVEHDR mWaveHeader;

    std::thread *mRecThread;
    BYTE *mBuf;
    BYTE *mBufPointer;
    bool mDoQuit;
  };

}

#endif
