/*
 * Copyright (c) 2009, The Mozilla Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Mozilla Foundation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY The Mozilla Foundation ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL The Mozilla Foundation BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Ted Mielczarek <ted.mielczarek@gmail.com>
 */
/*
 * screenshot.cpp : Save a screenshot of the Windows desktop in .png format.
 *  If a filename is specified as the first argument on the commandline,
 *  then the image will be saved to that filename. Otherwise, the image will
 *  be saved as "screenshot.png" in the current working directory.
 *
 *  Requires GDI+. All linker dependencies are specified explicitly in this
 *  file, so you can compile screenshot.exe by simply running:
 *    cl screenshot.cpp
 */

#include <windows.h>
#include <gdiplus.h>
#include <stdio.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

// From http://msdn.microsoft.com/en-us/library/ms533843%28VS.85%29.aspx
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
  UINT  num = 0;          // number of image encoders
  UINT  size = 0;         // size of the image encoder array in bytes

  ImageCodecInfo* pImageCodecInfo = NULL;

  GetImageEncodersSize(&num, &size);
  if(size == 0)
    return -1;  // Failure

  pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
  if(pImageCodecInfo == NULL)
    return -1;  // Failure

  GetImageEncoders(num, size, pImageCodecInfo);

  for(UINT j = 0; j < num; ++j)
    {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
        {
          *pClsid = pImageCodecInfo[j].Clsid;
          free(pImageCodecInfo);
          return j;  // Success
        }    
    }

  free(pImageCodecInfo);
  return -1;  // Failure
}

int _tmain(int argc, wchar_t* argv[])
{
  GdiplusStartupInput gdiplusStartupInput;
  ULONG_PTR gdiplusToken;
  GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
  
  if (argc != 4) {
    wprintf(L"Usage: %s outfile.jpg(or - for stdout) WxH JPEG_quality(0-100)\n", argv[0]);
    return 1;
  }
  int outWidth = 0, outHeight = 0;
  swscanf(argv[2], L"%dx%d", &outWidth, &outHeight);
  ULONG jpegQuality = 0;
  swscanf(argv[3], L"%d", &jpegQuality);
  BOOL useStdout = wcscmp(L"-", argv[1]) == 0;

  HWND desktop = GetDesktopWindow();
  HDC desktopdc = GetDC(desktop);
  HDC mydc = CreateCompatibleDC(desktopdc);
  int width = GetSystemMetrics(SM_CXSCREEN);
  int height = GetSystemMetrics(SM_CYSCREEN);
  if (!outWidth || !outHeight) {
    outWidth = width;
    outHeight = height;
  }
  
  HBITMAP mybmp = CreateCompatibleBitmap(desktopdc, outWidth, outHeight);
  HBITMAP oldbmp = (HBITMAP)SelectObject(mydc, mybmp);
  //BitBlt(mydc,0,0,width,height,desktopdc,0,0, SRCCOPY|CAPTUREBLT);
  StretchBlt(mydc, 0, 0, outWidth, outHeight, desktopdc, 0, 0, width, height, SRCCOPY|CAPTUREBLT);
  SelectObject(mydc, oldbmp);
  
  const wchar_t* filename = argv[1];
  EncoderParameters encoderParameters;
  encoderParameters.Count = 1;
  encoderParameters.Parameter[0].Guid = EncoderQuality;
  encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
  encoderParameters.Parameter[0].NumberOfValues = 1;
  encoderParameters.Parameter[0].Value = &jpegQuality;

  LPSTREAM lpStream = NULL;
  CreateStreamOnHGlobal(NULL, TRUE, &lpStream);

  Bitmap* b = Bitmap::FromHBITMAP(mybmp, NULL);
  CLSID  encoderClsid;
  Status stat = GenericError;
  if (b && GetEncoderClsid(L"image/jpeg", &encoderClsid) != -1) {
    if (useStdout)
      stat = b->Save(lpStream, &encoderClsid, &encoderParameters);
    else
      stat = b->Save(filename, &encoderClsid, &encoderParameters);
  }
  if (useStdout) {
    ULARGE_INTEGER pos, newpos;
    LARGE_INTEGER seek;
    ULONG read;
    seek.QuadPart = 0L;
    lpStream->Seek(seek, STREAM_SEEK_CUR, &pos);
    lpStream->Seek(seek, STREAM_SEEK_SET, &newpos);
    void *buf = malloc(pos.QuadPart);
    lpStream->Read(buf, pos.QuadPart, &read);
    _setmode(_fileno(stdout), _O_BINARY);
    fwrite(buf, 1, pos.QuadPart, stdout);
  }
  if (b)
    delete b;
  
  // cleanup
  GdiplusShutdown(gdiplusToken);
  ReleaseDC(desktop, desktopdc);
  DeleteObject(mybmp);
  DeleteDC(mydc);
  return !(stat == Ok);
}

