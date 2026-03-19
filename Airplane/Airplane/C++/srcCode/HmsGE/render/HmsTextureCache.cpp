#include "render/HmsTextureCache.h"

#include <errno.h>
#include <stack>
#include <cctype>
#include <list>
#include <stdio.h>

#include "render/HmsTexture2D.h"
#include "HmsAviMacros.h"
#include "base/HmsFileUtils.h"
#include "HmsAviDisplayUnit.h"

using namespace std;

NS_HMS_BEGIN


TextureCache::TextureCache()
: _loadingThread(nullptr)
, _needQuit(false)
, _asyncRefCount(0)
{
}

TextureCache::~TextureCache()
{
    CCLOGINFO("deallocing TextureCache: %p", this);

    for( auto it=_textures.begin(); it!=_textures.end(); ++it)
        (it->second)->release();

    HMS_SAFE_DELETE(_loadingThread);
}

std::string TextureCache::getDescription() const
{
    //return StringUtils::format("<TextureCache | Number of textures = %d>", static_cast<int>(_textures.size()));
	return "";
}

struct TextureCache::AsyncStruct
{
public:
    AsyncStruct(const std::string& fn, std::function<void(Texture2D*)> f) : filename(fn), callback(f), loadSuccess(false) {}
    
    std::string filename;
    std::function<void(Texture2D*)> callback;
    Image image;
    bool loadSuccess;
};

/**
 The addImageAsync logic follow the steps:
 - find the image has been add or not, if not add an AsyncStruct to _requestQueue  (GL thread)
 - get AsyncStruct from _requestQueue, load res and fill image data to AsyncStruct.image, then add AsyncStruct to _responseQueue (Load thread)
 - on schedule callback, get AsyncStruct from _responseQueue, convert image to texture, then delete AsyncStruct (GL thread)
 
 the Critical Area include these members:
 - _requestQueue: locked by _requestMutex
 - _responseQueue: locked by _responseMutex
 
 the object's life time:
 - AsyncStruct: construct and destruct in GL thread
 - image data: new in Load thread, delete in GL thread(by Image instance)
 
 Note:
 - all AsyncStruct referenced in _asyncStructQueue, for unbind function use.
 
 How to deal add image many times?
 - At first, this situation is abnormal, we only ensure the logic is correct.
 - If the image has been loaded, the after load image call will return immediately.
 - If the image request is in queue already, there will be more than one request in queue,
 - In addImageAsyncCallback, will deduplicate the request to ensure only create one texture.
 
 Does process all response in addImageAsyncCallback consume more time?
 - Convert image to texture faster than load image from disk, so this isn't a problem.
 */
void TextureCache::addImageAsync(const std::string &path, const std::function<void(Texture2D*)>& callback)
{
    Texture2D *texture = nullptr;

    std::string fullpath = CHmsFileUtils::getInstance()->fullPathForFilename(path);

    auto it = _textures.find(fullpath);
    if( it != _textures.end() )
        texture = it->second;

    if (texture != nullptr)
    {
        if (callback) callback(texture);
        return;
    }

    // check if file exists
    if ( fullpath.empty() || ! CHmsFileUtils::getInstance()->isFileExist( fullpath ) ) {
        if (callback) callback(nullptr);
        return;
    }

    // lazy init
    if (_loadingThread == nullptr)
    {
        // create a new thread to load images
        _loadingThread = new std::thread(&TextureCache::loadImage, this);
        _needQuit = false;
    }

    if (0 == _asyncRefCount)
    {
        //Director::getInstance()->getScheduler()->schedule(HMS_SCHEDULE_SELECTOR(TextureCache::addImageAsyncCallBack), this, 0, false);
		CHmsAviDisplayUnit::GetInstance()->GetScheduler()->AddSchedule(HMS_SCHEDULE_SELECTOR(TextureCache::addImageAsyncCallBack), this, 0, false);
    }

    ++_asyncRefCount;

    // generate async struct
    AsyncStruct *data = new  AsyncStruct(fullpath, callback);
    
    // add async struct into queue
    _asyncStructQueue.push_back(data);
    _requestMutex.lock();
    _requestQueue.push_back(data);
    _requestMutex.unlock();

    _sleepCondition.notify_one();
}

void TextureCache::unbindImageAsync(const std::string& filename)
{
    if (_asyncStructQueue.empty())
    {
        return;
    }
    std::string fullpath = CHmsFileUtils::getInstance()->fullPathForFilename(filename);
    for (auto it = _asyncStructQueue.begin(); it != _asyncStructQueue.end(); ++it)
    {
        if ((*it)->filename == fullpath)
        {
            (*it)->callback = nullptr;
        }
    }
}

void TextureCache::unbindAllImageAsync()
{
    if (_asyncStructQueue.empty())
    {
        return;

    }
    for (auto it = _asyncStructQueue.begin(); it != _asyncStructQueue.end(); ++it)
    {
        (*it)->callback = nullptr;
    }
}

void TextureCache::loadImage()
{
    AsyncStruct *asyncStruct = nullptr;
    std::mutex signalMutex;
    std::unique_lock<std::mutex> signal(signalMutex);
    while (!_needQuit)
    {
        // pop an AsyncStruct from request queue
        _requestMutex.lock();
        if(_requestQueue.empty())
        {
            asyncStruct = nullptr;
        }else
        {
            asyncStruct = _requestQueue.front();
            _requestQueue.pop_front();
        }
        _requestMutex.unlock();
        
        if (nullptr == asyncStruct) {
            _sleepCondition.wait(signal);
            continue;
        }
        
        // load image
        asyncStruct->loadSuccess = asyncStruct->image.initWithImageFileThreadSafe(asyncStruct->filename);

        // push the asyncStruct to response queue
        _responseMutex.lock();
        _responseQueue.push_back(asyncStruct);
        _responseMutex.unlock();
    }
}

void TextureCache::addImageAsyncCallBack(float dt)
{
    Texture2D *texture = nullptr;
    AsyncStruct *asyncStruct = nullptr;
    while (true)
    {
        // pop an AsyncStruct from response queue
        _responseMutex.lock();
        if(_responseQueue.empty())
        {
            asyncStruct = nullptr;
        }else
        {
            asyncStruct = _responseQueue.front();
            _responseQueue.pop_front();
            
            // the asyncStruct's sequence order in _asyncStructQueue must equal to the order in _responseQueue
            HMS_ASSERT(asyncStruct == _asyncStructQueue.front());
            _asyncStructQueue.pop_front();
        }
        _responseMutex.unlock();
        
        if (nullptr == asyncStruct) {
            break;
        }
        
        // check the image has been convert to texture or not
        auto it = _textures.find(asyncStruct->filename);
        if(it != _textures.end())
        {
            texture = it->second;
        }
        else
        {
            // convert image to texture
            if (asyncStruct->loadSuccess)
            {
                Image* image = &(asyncStruct->image);
                // generate texture in render thread
                texture = new  Texture2D();
                
                texture->initWithImage(image);

                _textures.insert( std::make_pair(asyncStruct->filename, texture) );
                texture->retain();
                
                texture->autorelease();
            } else {
                texture = nullptr;
                CCLOG("HmsAviPf: failed to call TextureCache::addImageAsync(%s)", asyncStruct->filename.c_str());
            }
        }
        
        // call callback function
        if (asyncStruct->callback)
        {
            (asyncStruct->callback)(texture);
        }

        // release the asyncStruct
        delete asyncStruct;
        --_asyncRefCount;
    }

    if (0 == _asyncRefCount)
    {
       // Director::getInstance()->getScheduler()->unschedule(HMS_SCHEDULE_SELECTOR(TextureCache::addImageAsyncCallBack), this);
		CHmsAviDisplayUnit::GetInstance()->GetScheduler()->RemoveSchedule(HMS_SCHEDULE_SELECTOR(TextureCache::addImageAsyncCallBack), this);
    }
}

Texture2D * TextureCache::addImage(const std::string &path)
{
    Texture2D * texture = nullptr;
    Image* image = nullptr;
    // Split up directory and filename
    // MUTEX:
    // Needed since addImageAsync calls this method from a different thread

    std::string fullpath = CHmsFileUtils::getInstance()->fullPathForFilename(path);
    if (fullpath.size() == 0)
    {
        return nullptr;
    }
    auto it = _textures.find(fullpath);
    if( it != _textures.end() )
        texture = it->second;

    if (! texture)
    {
        // all images are handled by UIImage except PVR extension that is handled by our own handler
        do 
        {
//            image = new  Image();
            image = new (std::nothrow) Image();

            HMS_BREAK_IF(nullptr == image);

            bool bRet = image->initWithImageFile(fullpath);
            HMS_BREAK_IF(!bRet);

//            texture = new  Texture2D();
            texture = new (std::nothrow) Texture2D();

            if( texture && texture->initWithImage(image) )
            {
                // texture already retained, no need to re-retain it
                _textures.insert( std::make_pair(fullpath, texture) );
            }
            else
            {
                CCLOG("HmsAviPf: Couldn't create texture for file:%s in TextureCache", path.c_str());
            }
        } while (0);
    }

    HMS_SAFE_RELEASE(image);

    return texture;
}

Texture2D* TextureCache::addImage(Image *image, const std::string &key)
{
    HMSASSERT(image != nullptr, "TextureCache: image MUST not be nil");

    Texture2D * texture = nullptr;

    do
    {
        auto it = _textures.find(key);
        if( it != _textures.end() ) {
            texture = it->second;
            break;
        }

        // prevents overloading the autorelease pool
        texture = new  Texture2D();
        texture->initWithImage(image);

        if(texture)
        {
            _textures.insert( std::make_pair(key, texture) );
            texture->retain();

            texture->autorelease();
        }
        else
        {
            CCLOG("HmsAviPf: Couldn't add UIImage in TextureCache");
        }

    } while (0);
    
    return texture;
}

bool TextureCache::reloadTexture(const std::string& fileName)
{
    Texture2D * texture = nullptr;
    Image * image = nullptr;

    std::string fullpath = CHmsFileUtils::getInstance()->fullPathForFilename(fileName);
    if (fullpath.size() == 0)
    {
        return false;
    }

    auto it = _textures.find(fullpath);
    if (it != _textures.end()) {
        texture = it->second;
    }

    bool ret = false;
    if (! texture) {
        texture = this->addImage(fullpath);
        ret = (texture != nullptr);
    }
    else
    {
        do {
            image = new  Image();
            HMS_BREAK_IF(nullptr == image);

            bool bRet = image->initWithImageFile(fullpath);
            HMS_BREAK_IF(!bRet);
            
            ret = texture->initWithImage(image);
        } while (0);
    }
    
    HMS_SAFE_RELEASE(image);

    return ret;
}

// TextureCache - Remove

void TextureCache::removeAllTextures()
{
    for( auto it=_textures.begin(); it!=_textures.end(); ++it ) {
        (it->second)->release();
    }
    _textures.clear();
}

void TextureCache::removeUnusedTextures()
{
    for( auto it=_textures.cbegin(); it!=_textures.cend(); /* nothing */) {
        Texture2D *tex = it->second;
        if( tex->getReferenceCount() == 1 ) {
            CCLOG("HmsAviPf: TextureCache: removing unused texture: %s", it->first.c_str());

            tex->release();
            _textures.erase(it++);
        } else {
            ++it;
        }

    }
}

void TextureCache::removeTexture(Texture2D* texture)
{
    if( ! texture )
    {
        return;
    }

    for( auto it=_textures.cbegin(); it!=_textures.cend(); /* nothing */ ) {
        if( it->second == texture ) {
            texture->release();
            _textures.erase(it++);
            break;
        } else
            ++it;
    }
}

void TextureCache::removeTextureForKey(const std::string &textureKeyName)
{
    std::string key = textureKeyName;
    auto it = _textures.find(key);

    if( it == _textures.end() ) {
        key = CHmsFileUtils::getInstance()->fullPathForFilename(textureKeyName);
        it = _textures.find(key);
    }

    if( it != _textures.end() ) {
        (it->second)->release();
        _textures.erase(it);
    }
}

Texture2D* TextureCache::getTextureForKey(const std::string &textureKeyName) const
{
    std::string key = textureKeyName;
    auto it = _textures.find(key);

    if( it == _textures.end() ) {
        key = CHmsFileUtils::getInstance()->fullPathForFilename(textureKeyName);
        it = _textures.find(key);
    }

    if( it != _textures.end() )
        return it->second;
    return nullptr;
}

const std::string TextureCache::getTextureFilePath( HmsAviPf::Texture2D *texture )const
{
    for(auto& item : _textures)
    {
        if(item.second == texture)
        {
            return item.first;
            break;
        }
    }
    return "";
}

void TextureCache::waitForQuit()
{
    // notify sub thread to quick
    _needQuit = true;
    _sleepCondition.notify_one();
    if (_loadingThread) _loadingThread->join();
}

NS_HMS_END

