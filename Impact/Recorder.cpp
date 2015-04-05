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
#ifndef NDEBUG
    std::cout
      << "mActualDuration: " << mActualDuration << std::endl
      << "wFormatTag:      " << std::showbase << std::internal << std::hex << std::setw(4) << mWFX->wFormatTag << std::endl << std::dec
      << "nChannels:       " << mWFX->nChannels << std::endl
      << "nSamplesPerSec:  " << mWFX->nSamplesPerSec << std::endl
      << "nAvgBytesPerSec: " << mWFX->nAvgBytesPerSec << std::endl
      << "nBlockAlign:     " << mWFX->nBlockAlign << std::endl
      << "wBitsPerSample:  " << mWFX->wBitsPerSample << std::endl
      << "cbSize:          " << mWFX->cbSize << std::endl;
#endif

    hr = this->start();
    if (FAILED(hr)) {
      std::cerr << "this->start() failed in line " << __LINE__ << std::endl;
      return;
    }

    mBuf = new BYTE[RecordBufSize];
    mBufPointer = mBuf;
  }


  Recorder::~Recorder()
  {
    this->stop();
    safeDelete(mRecThread);
    safeDeleteArray(mBuf);
  }


  void Recorder::capture(void)
  {
    HRESULT hr = S_OK;
    UINT32 packetLength = 0;
    while (!mDoQuit) {
      Sleep((DWORD) mActualDuration / 2);
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
    mRecThread = new std::thread(&Recorder::capture, this);
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
      FILE *file = fopen("blah.wav", "wb+");

      WaveHeader header;
      header.ChunkSize = sizeof(header) + mBufPointer - mBuf - 8;
      header.wFormatTag = WAVE_FORMAT_PCM;
      header.wChannels = mWFX->nChannels;
      header.dwSamplesPerSec = mWFX->nSamplesPerSec;
      header.dwAvgBytesPerSec = mWFX->nAvgBytesPerSec;
      header.wBlockAlign = mWFX->nChannels * mWFX->wBitsPerSample / 8;
      header.wBitsPerSample = mWFX->wBitsPerSample;

#ifndef NDEBUG
      std::cout << "sizeof(WaveHeader): " << sizeof(WaveHeader) << std::endl
        << "header.wFormatTag = " << std::showbase << std::hex << std::setw(4) << std::setfill('0') << header.wFormatTag << std::endl
        << std::dec
        << "header.wChannels = " << header.wChannels << std::endl
        << "header.dwSamplesPerSec = " << header.dwSamplesPerSec << std::endl
        << "header.dwAvgBytesPerSec = " << header.dwAvgBytesPerSec << std::endl
        << "header.wBlockAlign = " << header.wBlockAlign << std::endl
        << "header.wBitsPerSample = " << header.wBitsPerSample << std::endl;
#endif
      fwrite(&header, sizeof(header), 1, file);
      fwrite("data", 4, 1, file);
      uint32_t dataLen = mBufPointer - mBuf - 44;
      fwrite(&dataLen, 4, 1, file);
      fwrite(mBuf, mBufPointer - mBuf, 1, file);
      fclose(file);

      safeDelete(mRecThread);
      safeDeleteArray(mBuf);
    }
    return S_OK;
  }



  HRESULT Recorder::copyData(BYTE *pData, UINT32 numFramesAvailable, bool *done)
  {
    const int nBytes = numFramesAvailable * mWFX->nBlockAlign;
    if (mBufPointer - mBuf + nBytes >= RecordBufSize) {
      *done = true;
    }
    else {
      if (pData == nullptr) {
        ZeroMemory(mBufPointer, nBytes);
      }
      else {
        memcpy_s(mBufPointer, RecordBufSize - (mBufPointer - mBuf), pData, nBytes);
      }
      mBufPointer += nBytes;
    }
    return S_OK;
  }

}
