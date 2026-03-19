#ifndef __HMS_IMAGE_H__
#define __HMS_IMAGE_H__

#include "base/Ref.h"
#include "render/HmsTexture2D.h"


// premultiply alpha, or the effect will wrong when want to use other pixel format in Texture2D,
// such as RGB888, RGB5A1
#define HMS_RGB_PREMULTIPLY_ALPHA(vr, vg, vb, va) \
    (unsigned)(((unsigned)((unsigned char)(vr) * ((unsigned char)(va) + 1)) >> 8) | \
    ((unsigned)((unsigned char)(vg) * ((unsigned char)(va) + 1) >> 8) << 8) | \
    ((unsigned)((unsigned char)(vb) * ((unsigned char)(va) + 1) >> 8) << 16) | \
    ((unsigned)(unsigned char)(va) << 24))

NS_HMS_BEGIN

/**
 @brief Structure which can tell where mipmap begins and how long is it
 */
typedef struct _MipmapInfo
{
    unsigned char* address;
    int len;
    _MipmapInfo():address(NULL),len(0){}
}MipmapInfo;

class HMS_DLL Image : public Ref
{
public:
    friend class TextureCache;

    Image();
    virtual ~Image();

    /** Supported formats for Image */
    enum class Format
    {
        JPG,
        PNG,
        RAW_DATA,
        UNKNOWN
    };

    /**
    @brief Load the image from the specified path.
    @param path   the absolute file path.
    @return true if loaded correctly.
    */
    bool initWithImageFile(const std::string& path);

    /**
    @brief Load image from stream buffer.
    @param data  stream buffer which holds the image data.
    @param dataLen  data length expressed in (number of) bytes.
    @return true if loaded correctly.
    * @js NA
    * @lua NA
    */
    bool initWithImageData(const unsigned char * data, ssize_t dataLen);

    // @warning kFmtRawData only support RGBA8888
    bool initWithRawData(const unsigned char * data, ssize_t dataLen, int width, int height, int bitsPerComponent, bool preMulti = false);

	bool initWithRawDataRGB888(const unsigned char * data, ssize_t dataLen, int width, int height, int bitsPerComponent, bool preMulti = false);
	bool UpdateWidthRawData(const unsigned char * data, int x, int y, int width, int height, int bytesPerComponent);

    // Getters
    inline unsigned char *   getData()               { return _data; }
    inline ssize_t           getDataLen()            { return _dataLen; }
    inline Format            getFileType()           {return _fileType; }
    inline Texture2D::PixelFormat getRenderFormat()  { return _renderFormat; }
    inline int               getWidth()              { return _width; }
    inline int               getHeight()             { return _height; }
    inline int               getNumberOfMipmaps()    { return _numberOfMipmaps; }
    inline MipmapInfo*       getMipmaps()            { return _mipmaps; }
    inline bool              hasPremultipliedAlpha() { return _hasPremultipliedAlpha; }

    int                      getBitPerPixel();
    bool                     hasAlpha();
    bool                     isCompressed();


    /**
     @brief    Save Image data to the specified file, with specified format.
     @param    filePath        the file's absolute path, including file suffix.
     @param    isToRGB        whether the image is saved as RGB format.
     */
    bool saveToFile(const std::string &filename, bool isToRGB = true);
    
    
    /** treats (or not) PVR files as if they have alpha premultiplied.
     Since it is impossible to know at runtime if the PVR images have the alpha channel premultiplied, it is
     possible load them as if they have (or not) the alpha channel premultiplied.
     
     By default it is disabled.
     */
    static void setPVRImagesHavePremultipliedAlpha(bool haveAlphaPremultiplied);

	//=====================
	GLboolean GenMipMap2D(GLubyte *src, GLubyte **dst, int srcWidth, int srcHeight, int *dstWidth, int *dstHeight);
	void GenerateMipmaps();

protected:
    bool initWithJpgData(const unsigned char *  data, ssize_t dataLen);
    bool initWithPngData(const unsigned char * data, ssize_t dataLen);

    bool saveImageToPNG(const std::string& filePath, bool isToRGB = true);
    bool saveImageToJPG(const std::string& filePath);
    
    void premultipliedAlpha();
    
protected:
    /**
     @brief Determine how many mipmaps can we have.
     It's same as define but it respects namespaces
     */
    static const int MIPMAP_MAX = 16;
    unsigned char *_data;
    ssize_t _dataLen;
    int _width;
    int _height;
    bool _unpack;
    Format _fileType;
    Texture2D::PixelFormat _renderFormat;
    MipmapInfo _mipmaps[MIPMAP_MAX];   // pointer to mipmap images
    int _numberOfMipmaps;
    // false if we can't auto detect the image is premultiplied or not.
    bool _hasPremultipliedAlpha;
    std::string _filePath;


protected:
    // noncopyable
    Image(const Image&    rImg);
    Image & operator=(const Image&);
    
    /*
     @brief The same result as with initWithImageFile, but thread safe. It is caused by
     loadImage() in TextureCache.cpp.
     @param fullpath  full path of the file.
     @param imageType the type of image, currently only supporting two types.
     @return  true if loaded correctly.
     */
    bool initWithImageFileThreadSafe(const std::string& fullpath);
    
    Format detectFormat(const unsigned char * data, ssize_t dataLen);
    bool isPng(const unsigned char * data, ssize_t dataLen);
    bool isJpg(const unsigned char * data, ssize_t dataLen);
};


NS_HMS_END

#endif    // __HMS_IMAGE_H__
