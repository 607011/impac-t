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
#include <Audioclient.h>
#endif

#include <thread>

namespace Impact {

  class Recorder {
  public:
    Recorder(void);
    ~Recorder();

    HRESULT start(void);
    HRESULT stop(void);

  private:
    HRESULT CopyData(BYTE *pData, UINT32 numFramesAvailable, bool *done);
    HRESULT SetFormat(WAVEFORMATEX *pwfx);
    void Capture(void);

  private:
    static const REFERENCE_TIME REFTIMES_PER_SEC = 10000000;
    static const REFERENCE_TIME REFTIMES_PER_MILLISEC = 10000;

    IAudioClient *mAudioClient;
    IAudioCaptureClient *mCaptureClient;
    WAVEFORMATEX *mWFX;
    REFERENCE_TIME mActualDuration;

    std::thread *mRecThread;
    BYTE *mBuf;
    BYTE *mBufPointer;
    bool mDoQuit;
  };

}

#endif
