#include "PngLoader.hpp"

#include <stdexcept>

 
HBITMAP PngLoader::operator()(std::string filePath)
{
    HBITMAP hbmp = NULL;
 
    IStream * ipImageStream = createStreamOnFile(filePath.c_str());
    try 
    {
        IWICBitmapSource * ipBitmap = loadBitmapFromStream(ipImageStream);
        hbmp = createHBITMAP(ipBitmap);
        ipBitmap->Release();
 
    }
    catch(...)
    {
        ipImageStream->Release();
        throw;
    }

    return hbmp;
}

IStream *PngLoader::createStreamOnFile(LPCTSTR lpName)
{
    IStream * ipStream = nullptr;
    HGLOBAL hgblResourceData;
    /* auto hrsrc = FindResource(NULL, lpName, lpType);
    // if (hrsrc == NULL)
    //     throw std::runtime_error("Failed to find resource!");

    // auto dwResourceSize = SizeofResource(NULL, hrsrc);
    // auto hglbImage = LoadResource(NULL, hrsrc);
    // if (hglbImage == NULL)
    //     throw std::runtime_error("Failed to load resource!");

    // auto pvSourceResourceData = LockResource(hglbImage);
    // if (pvSourceResourceData == NULL)
    //     throw std::runtime_error("Failed to lock resource!");*/

    auto fileHandle = CreateFile(lpName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(fileHandle == INVALID_HANDLE_VALUE)
        throw std::runtime_error("Failed to open file!");
    
    auto fileSize = GetFileSize(fileHandle, nullptr);
    if(fileSize == INVALID_FILE_SIZE)
        throw std::runtime_error("Failed to get file size!");

    try
    {
        hgblResourceData = GlobalAlloc(GMEM_MOVEABLE, fileSize);
        if (hgblResourceData == NULL)
            throw std::runtime_error("Failed to alloc memmory!");

        auto pvResourceData = GlobalLock(hgblResourceData);
        if (pvResourceData == NULL)
            throw std::runtime_error("Failed to lock memory!");
    
        if(!ReadFile(fileHandle, pvResourceData, fileSize, &fileSize, nullptr))
            throw std::runtime_error("Failed to read file!");

        GlobalUnlock(hgblResourceData);

        if(FAILED(CreateStreamOnHGlobal(hgblResourceData, TRUE, &ipStream)))
            throw std::runtime_error("Failed to create stream!");
    }
    catch(...)
    {
        GlobalFree(hgblResourceData);
        throw;
    }

    return ipStream;

}

IWICBitmapSource *PngLoader::loadBitmapFromStream(IStream *ipImageStream)
{
    IWICBitmapSource * ipBitmap = nullptr;
    IWICBitmapDecoder * ipDecoder = nullptr;
    CoInitialize(NULL);
    auto res = CoCreateInstance(CLSID_WICPngDecoder, NULL, CLSCTX_INPROC_SERVER, __uuidof(ipDecoder), reinterpret_cast<void**>(&ipDecoder));
    if (FAILED(res))
        throw std::runtime_error("Failed to create decoder!");

    try
    {
        if (FAILED(ipDecoder->Initialize(ipImageStream, WICDecodeMetadataCacheOnLoad)))
            throw std::runtime_error("Failed to init decoder!");

        UINT nFrameCount = 0;
        if (FAILED(ipDecoder->GetFrameCount(&nFrameCount)) || nFrameCount != 1)
            throw std::runtime_error("Failed to get frame count!");

        IWICBitmapFrameDecode * ipFrame = nullptr;
        if (FAILED(ipDecoder->GetFrame(0, &ipFrame)))
            throw std::runtime_error("Failed to get first frame!");

        WICConvertBitmapSource(GUID_WICPixelFormat32bppPBGRA, ipFrame, &ipBitmap);
        ipFrame->Release();
    }
    catch(...)
    {
        ipDecoder->Release();
        throw;
    }

    return ipBitmap;
}

HBITMAP PngLoader::createHBITMAP(IWICBitmapSource *ipBitmap)
{
    HBITMAP hbmp = NULL;

    UINT width = 0;
    UINT height = 0;
    if (FAILED(ipBitmap->GetSize(&width, &height)) || width == 0 || height == 0)
        throw std::runtime_error("Failed to get bitmap size!");

    BITMAPINFO bminfo;
    ZeroMemory(&bminfo, sizeof(bminfo));
    bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bminfo.bmiHeader.biWidth = width;
    bminfo.bmiHeader.biHeight = -((LONG) height);
    bminfo.bmiHeader.biPlanes = 1;
    bminfo.bmiHeader.biBitCount = 32;
    bminfo.bmiHeader.biCompression = BI_RGB;

    void * pvImageBits = NULL;
    HDC hdcScreen = GetDC(NULL);
    hbmp = CreateDIBSection(hdcScreen, &bminfo, DIB_RGB_COLORS, &pvImageBits, NULL, 0);
    ReleaseDC(NULL, hdcScreen);
    if (hbmp == NULL)
        throw std::runtime_error("Failed to create Bitmap!");

    const UINT cbStride = width * 4;
    const UINT cbImage = cbStride * height;
    if (FAILED(ipBitmap->CopyPixels(NULL, cbStride, cbImage, static_cast<BYTE *>(pvImageBits))))
    {
        DeleteObject(hbmp);
        hbmp = NULL;
        throw std::runtime_error("Failed to extract image to bitmap!");
    }
    
    return hbmp;
}
