#ifndef __INC_LINUX_AMD64
#define __INC_LINUX_AMD64

#include <stdint.h>
#include <stdio.h>
#include <time.h>
#define MAX_PATH FILENAME_MAX
typedef uint32_t DWORD;
typedef uint32_t UINT32;
typedef union _ULARGE_INTEGER {
  struct {
    DWORD LowPart;
    DWORD HighPart;
  };
  struct {
    DWORD LowPart;
    DWORD HighPart;
  } u;
  uint64_t QuadPart;
} ULARGE_INTEGER, *PULARGE_INTEGER;
typedef char TCHAR;
typedef void* HANDLE;
typedef int HRESULT;
#define DECLARE_HANDLE(a) \
	typedef HANDLE a; \
	typedef a *P##a; \
	typedef a *LP##a
typedef bool BOOL;
#define TRUE 1

#endif
