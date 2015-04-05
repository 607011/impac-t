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

namespace Impact {

  Recorder::Recorder(void)
    : mDoQuit(false)
    , mAudioClient(nullptr)
    , mCaptureClient(nullptr)
    , mRecThread(nullptr)
    , mBuf(nullptr)
    , mBufPointer(nullptr)
  {
    HRESULT hr;
#ifndef NDEBUG
    std::cout << "initRecorder() ..." << std::endl;
#endif
    const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
    const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
    const IID IID_IAudioClient = __uuidof(IAudioClient);
    const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

    mActualDuration = REFTIMES_PER_SEC;
    UINT32 bufferFrameCount;
    IMMDeviceEnumerator *pEnumerator = nullptr;
    IMMDevice *pDevice = nullptr;
    WAVEFORMATEX *pwfx = nullptr;

    hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator);
    if (FAILED(hr)) {
      std::cerr << "CoCreateInstance() failed in line " << __LINE__ << std::endl;
      return;
    }

    hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice);
    if (FAILED(hr)) {
      std::cerr << "pEnumerator->GetDefaultAudioEndpoint() failed in line " << __LINE__ << std::endl;
      return;
    }

    hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&mAudioClient);
    if (FAILED(hr)) {
      std::cerr << "pDevice->Activate() failed in line " << __LINE__ << std::endl;
      return;
    }

    hr = mAudioClient->GetMixFormat(&pwfx);
    if (FAILED(hr)) {
      std::cerr << "pAudioClient->GetMixFormat() failed in line " << __LINE__ << std::endl;
      return;
    }

    hr = mAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, mActualDuration, 0, pwfx, NULL);
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

    hr = this->SetFormat(pwfx);
    if (FAILED(hr)) {
      std::cerr << "this->SetFormat() failed in line " << __LINE__ << std::endl;
      return;
    }

    mActualDuration = (REFERENCE_TIME)((double)REFTIMES_PER_SEC * bufferFrameCount / pwfx->nSamplesPerSec);
#ifndef NDEBUG
    std::cout << "samples per second: " << pwfx->nSamplesPerSec << std::endl;
    std::cout << "mActualDuration: " << mActualDuration << " ms" << std::endl;
#endif

    hr = this->start();
    if (FAILED(hr)) {
      std::cerr << "this->start() failed in line " << __LINE__ << std::endl;
      return;
    }

    mBuf = new BYTE[100 * 1024 * 1024];
    mBufPointer = mBuf;
  }


  Recorder::~Recorder()
  {
    this->stop();
    safeDelete(mRecThread);
    safeDeleteArray(mBuf);
  }


  void Recorder::Capture(void)
  {
    HRESULT hr = S_OK;
    UINT32 packetLength = 0;
    while (!mDoQuit) {
      Sleep((DWORD)mActualDuration / REFTIMES_PER_MILLISEC / 2);
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
        hr = this->CopyData(pData, numFramesAvailable, &mDoQuit);
        if (FAILED(hr))
          std::cerr << "this->CopyData() failed on line " << __LINE__ << std::endl;
        hr = mCaptureClient->ReleaseBuffer(numFramesAvailable);
        if (FAILED(hr))
          std::cerr << "mCaptureClient->ReleaseBuffer() failed on line " << __LINE__ << std::endl;
        hr = mCaptureClient->GetNextPacketSize(&packetLength);
        if (FAILED(hr))
          std::cerr << "mCaptureClient->GetNextPacketSize() failed on line " << __LINE__ << std::endl;
      }
    }
    hr = mAudioClient->Stop();
    if (FAILED(hr))
      std::cerr << "mAudioClient->Stop() failed on line " << __LINE__ << std::endl;
  }


  HRESULT Recorder::start(void)
  {
    if (mRecThread != nullptr)
      return S_FALSE;
    HRESULT hr;
    hr = mAudioClient->Start();
    mDoQuit = false;
    if (FAILED(hr)) {
      std::cerr << "mAudioClient->Start() failed on line " << __LINE__ << std::endl;
      return S_FALSE;
    }
    mRecThread = new std::thread(&Recorder::Capture, this);
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
      HANDLE hFile;
      hFile = CreateFile("blah.wav", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
      DWORD bytesWritten = 0;
      WriteFile(hFile, mBuf, mBufPointer - mBuf, &bytesWritten, NULL);
#ifndef NDEBUG
      std::cout << "bytesWritten: " << bytesWritten << std::endl;
#endif
      CloseHandle(hFile);
      safeDelete(mRecThread);
      safeDeleteArray(mBuf);
    }
    return S_OK;
  }



  HRESULT Recorder::CopyData(BYTE *pData, UINT32 numFramesAvailable, bool *done)
  {
    CopyMemory(mBufPointer, pData, numFramesAvailable);
    mBufPointer += numFramesAvailable;
    return S_OK;
  }


  HRESULT Recorder::SetFormat(WAVEFORMATEX *pwfx)
  {
    mWFX = pwfx;
    return S_OK;
  }

}
