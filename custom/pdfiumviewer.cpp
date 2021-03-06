// Copyright (c) 2015 Pieter van Ginkel. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "public/fpdfview.h"
#if PDF_ENABLE_V8
#include "v8/include/v8.h"
#include "v8/include/libplatform/libplatform.h"
#endif // PDF_ENABLE_V8

extern "C"
{
	FPDF_EXPORT void FPDF_CALLCONV FPDF_AddRef();
	FPDF_EXPORT void FPDF_CALLCONV FPDF_Release();
}

class RefCounter
{
private:
	CRITICAL_SECTION cs;
	int refCount;
#if PDF_ENABLE_V8
	v8::Platform* platform;
#endif // PDF_ENABLE_V8

public:
	RefCounter()
	{
		::InitializeCriticalSection(&cs);
		refCount = 0;
#if PDF_ENABLE_V8
		platform = NULL;
#endif // PDF_ENABLE_V8
	}

	~RefCounter()
	{
		::DeleteCriticalSection(&cs);
	}

	void Enter()
	{
		::EnterCriticalSection(&cs);
	}

	void Leave()
	{
		::LeaveCriticalSection(&cs);
	}

	void AddRef()
	{
		::EnterCriticalSection(&cs);

		if (refCount == 0)
		{
#if PDF_ENABLE_V8
			v8::V8::InitializeICU();
			platform = v8::platform::CreateDefaultPlatform();
			v8::V8::InitializePlatform(platform);
			v8::V8::Initialize();
#endif // PDF_ENABLE_V8

			FPDF_InitLibrary();
		}

		refCount++;

		::LeaveCriticalSection(&cs);
	}

	void Release()
	{
		::EnterCriticalSection(&cs);

		refCount--;

		if (refCount == 0)
		{
			FPDF_DestroyLibrary();
#if PDF_ENABLE_V8
			v8::V8::ShutdownPlatform();
			delete platform;
#endif // PDF_ENABLE_V8
		}

		::LeaveCriticalSection(&cs);
	}
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
static RefCounter refCounter;
#pragma clang diagnostic pop

FPDF_EXPORT void FPDF_CALLCONV FPDF_AddRef()
{
	refCounter.AddRef();
}

FPDF_EXPORT void FPDF_CALLCONV FPDF_Release()
{
	refCounter.Release();
}
