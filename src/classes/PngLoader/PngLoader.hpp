#ifndef PNGLOADER_HPP
#define PNGLOADER_HPP

#include <Windows.h>
#include <wincodec.h>
#include <string>

class PngLoader
{
public:
    HBITMAP operator()(std::string filePath);

private:

    static inline IStream* createStreamOnFile(LPCTSTR lpName);
    static inline IWICBitmapSource * loadBitmapFromStream(IStream * ipImageStream);
    static inline HBITMAP createHBITMAP(IWICBitmapSource * ipBitmap);
};

#endif