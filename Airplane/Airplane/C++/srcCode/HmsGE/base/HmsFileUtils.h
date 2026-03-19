#ifndef __HMS_FILEUTILS_H__
#define __HMS_FILEUTILS_H__

#include <string>
#include <vector>
#include <unordered_map>

#include "HmsAviMacros.h"
#include "base/HmsTypes.h"
#include "base/HmsData.h"

NS_HMS_BEGIN

class ResizableBuffer {
public:
	virtual ~ResizableBuffer() {}
	virtual void resize(size_t size) = 0;
	virtual void* buffer() const = 0;
};

template<typename T>
class ResizableBufferAdapter { };


template<typename CharT, typename Traits, typename Allocator>
class ResizableBufferAdapter< std::basic_string<CharT, Traits, Allocator> > : public ResizableBuffer{
	typedef std::basic_string<CharT, Traits, Allocator> BufferType;
	BufferType* _buffer;
public:
	explicit ResizableBufferAdapter(BufferType* buffer) : _buffer(buffer) {}
	virtual void resize(size_t size) override {
		_buffer->resize((size + sizeof(CharT) - 1) / sizeof(CharT));
	}
	virtual void* buffer() const override {
		// can not invoke string::front() if it is empty

		if (_buffer->empty())
			return nullptr;
		else
			return &_buffer->front();
	}
};

template<typename T, typename Allocator>
class ResizableBufferAdapter< std::vector<T, Allocator> > : public ResizableBuffer{
	typedef std::vector<T, Allocator> BufferType;
	BufferType* _buffer;
public:
	explicit ResizableBufferAdapter(BufferType* buffer) : _buffer(buffer) {}
	virtual void resize(size_t size) override {
		_buffer->resize((size + sizeof(T) - 1) / sizeof(T));
	}
	virtual void* buffer() const override {
		// can not invoke vector::front() if it is empty

		if (_buffer->empty())
			return nullptr;
		else
			return &_buffer->front();
	}
};


template<>
class ResizableBufferAdapter<Data> : public ResizableBuffer{
	typedef Data BufferType;
	BufferType* _buffer;
public:
	explicit ResizableBufferAdapter(BufferType* buffer) : _buffer(buffer) {}
	virtual void resize(size_t size) override {
		if (static_cast<size_t>(_buffer->getSize()) < size) {
			auto old = _buffer->getBytes();
			void* buffer = realloc(old, size);
			if (buffer)
				_buffer->fastSet((unsigned char*)buffer, size);
		}
	}
	virtual void* buffer() const override {
		return _buffer->getBytes();
	}
};

/** Helper class to handle file operations. */
class HMS_DLL CHmsFileUtils
{
public:
    /**
     *  Gets the instance of FileUtils.
     */
    static CHmsFileUtils* getInstance();

    /**
     *  Destroys the instance of FileUtils.
     */
    static void destroyInstance();

    static void StringReplace(std::string &strBase, std::string strSrc, std::string strDes);


    virtual ~CHmsFileUtils();

    /**
     *  Gets string from a file.
     */
    virtual std::string getStringFromFile(const std::string& filename);

    /**
     *  Creates binary data from a file.
     *  @return A data object.
     */
    virtual Data getDataFromFile(const std::string& filename);
        
    virtual std::string getAviDataPath() const;


	enum Status
	{
		OK = 0,
		NotExists = 1, // File not exists
		OpenFailed = 2, // Open file failed.
		ReadFailed = 3, // Read failed
		NotInitialized = 4, // FileUtils is not initializes
		TooLarge = 5, // The file is too large (great than 2^32-1)
		ObtainSizeFailed = 6 // Failed to obtain the file size.
	};

	/**
	*  Gets whole file contents as string from a file.
	*
	*  Unlike getStringFromFile, these getContents methods:
	*      - read file in binary mode (does not convert CRLF to LF).
	*      - does not truncate the string when '\0' is found (returned string of getContents may have '\0' in the middle.).
	*
	*  The template version of can accept cocos2d::Data, std::basic_string and std::vector.
	*
	*  @code
	*  std::string sbuf;
	*  FileUtils::getInstance()->getContents("path/to/file", &sbuf);
	*
	*  std::vector<int> vbuf;
	*  FileUtils::getInstance()->getContents("path/to/file", &vbuf);
	*
	*  Data dbuf;
	*  FileUtils::getInstance()->getContents("path/to/file", &dbuf);
	*  @endcode
	*
	*  Note: if you read to std::vector<T> and std::basic_string<T> where T is not 8 bit type,
	*  you may get 0 ~ sizeof(T)-1 bytes padding.
	*
	*  - To write a new buffer class works with getContents, just extend ResizableBuffer.
	*  - To write a adapter for existing class, write a specialized ResizableBufferAdapter for that class, see follow code.
	*
	*  @code
	*  NS_CC_BEGIN // ResizableBufferAdapter needed in cocos2d namespace.
	*  template<>
	*  class ResizableBufferAdapter<AlreadyExistsBuffer> : public ResizableBuffer {
	*  public:
	*      ResizableBufferAdapter(AlreadyExistsBuffer* buffer)  {
	*          // your code here
	*      }
	*      virtual void resize(size_t size) override  {
	*          // your code here
	*      }
	*      virtual void* buffer() const override {
	*          // your code here
	*      }
	*  };
	*  NS_CC_END
	*  @endcode
	*
	*  @param[in]  filename The resource file name which contains the path.
	*  @param[out] buffer The buffer where the file contents are store to.
	*  @return Returns:
	*      - Status::OK when there is no error, the buffer is filled with the contents of file.
	*      - Status::NotExists when file not exists, the buffer will not changed.
	*      - Status::OpenFailed when cannot open file, the buffer will not changed.
	*      - Status::ReadFailed when read end up before read whole, the buffer will fill with already read bytes.
	*      - Status::NotInitialized when FileUtils is not initializes, the buffer will not changed.
	*      - Status::TooLarge when there file to be read is too large (> 2^32-1), the buffer will not changed.
	*      - Status::ObtainSizeFailed when failed to obtain the file size, the buffer will not changed.
	*/
	template <
		typename T,
		typename Enable = typename std::enable_if<
		std::is_base_of< ResizableBuffer, ResizableBufferAdapter<T> >::value
		>::type
	>
	Status getContents(const std::string& filename, T* buffer) {
		ResizableBufferAdapter<T> buf(buffer);
		return getContents(filename, &buf);
	}
	virtual Status getContents(const std::string& filename, ResizableBuffer* buffer);


    /**
     *  Gets resource file data from a zip file.
     *
     *  @param[in]  filename The resource file name which contains the relative path of the zip file.
     *  @param[out] size If the file read operation succeeds, it will be the data size, otherwise 0.
     *  @return Upon success, a pointer to the data is returned, otherwise nullptr.
     *  @warning Recall: you are responsible for calling free() on any Non-nullptr pointer returned.
     */
    virtual unsigned char* getFileDataFromZip(const std::string& zipFilePath, const std::string& filename, ssize_t *size);

    virtual std::string fullPathForFilename(const std::string &filename) const;

    /**
     *  write a string into a file
     *
     * @param dataStr the string want to save
     * @param fullPath The full path to the file you want to save a string
     * @return bool True if write success
     */
    virtual bool writeStringToFile(std::string dataStr, const std::string& fullPath);


    /**
     * write Data into a file
     *
     *@param retData the data want to save
     *@param fullPath The full path to the file you want to save a string
     *@return bool
     */
    virtual bool writeDataToFile(Data retData, const std::string& fullPath);

    /**
     *  Checks whether a file exists.
     *
     *  @note If a relative path was passed in, it will be inserted a default root path at the beginning.
     *  @param filename The path of the file, it could be a relative or absolute path.
     *  @return True if the file exists, false if not.
     */
    virtual bool isFileExist(const std::string& filename) const;

    /**
    *  Gets filename extension is a suffix (separated from the base filename by a dot) in lower case.
    *  Examples of filename extensions are .png, .jpeg, .exe, .dmg and .txt.
    *  @param filePath The path of the file, it could be a relative or absolute path.
    *  @return suffix for filename in lower case or empty if a dot not found.
    */
    virtual std::string getFileExtension(const std::string& filePath) const;

    /**
     *  Checks whether the path is an absolute path.
     *
     *  @note On Android, if the parameter passed in is relative to "assets/", this method will treat it as an absolute path.
     *        Also on Blackberry, path starts with "app/native/Resources/" is treated as an absolute path.
     *
     *  @param path The path that needs to be checked.
     *  @return True if it's an absolute path, false if not.
     */
    virtual bool isAbsolutePath(const std::string& path) const;

	/**
	*  Checks whether the path is a directory.
	*
	*  @param dirPath The path of the directory, it could be a relative or an absolute path.
	*  @return True if the directory exists, false if not.
	*/
	virtual bool isDirectoryExist(const std::string& dirPath) const;

    /**
     *  Creates a directory.
     *
     *  @param dirPath The path of the directory, it must be an absolute path.
     *  @return True if the directory have been created successfully, false if not.
     */
    virtual bool createDirectory(const std::string& dirPath);

    /**
     *  Removes a directory.
     *
     *  @param dirPath  The full path of the directory, it must be an absolute path.
     *  @return True if the directory have been removed successfully, false if not.
     */
    virtual bool removeDirectory(const std::string& dirPath);

    /**
     *  Removes a file.
     *
     *  @param filepath The full path of the file, it must be an absolute path.
     *  @return True if the file have been removed successfully, false if not.
     */
    virtual bool removeFile(const std::string &filepath);

    /**
     *  Renames a file under the given directory.
     *
     *  @param path     The parent directory path of the file, it must be an absolute path.
     *  @param oldname  The current name of the file.
     *  @param name     The new name of the file.
     *  @return True if the file have been renamed successfully, false if not.
     */
    virtual bool renameFile(const std::string &path, const std::string &oldname, const std::string &name);

    /**
     *  Renames a file under the given directory.
     *
     *  @param oldfullpath  The current fullpath of the file. Includes path and name.
     *  @param newfullpath  The new fullpath of the file. Includes path and name.
     *  @return True if the file have been renamed successfully, false if not.
     */
    virtual bool renameFile(const std::string &oldfullpath, const std::string &newfullpath);

    /**
     *  Retrieve the file size.
     *
     *  @note If a relative path was passed in, it will be inserted a default root path at the beginning.
     *  @param filepath The path of the file, it could be a relative or absolute path.
     *  @return The file size.
     */
    virtual long getFileSize(const std::string &filepath);

	virtual std::string getWritablePath() const;

	/*
	* Set the ResourcePath Used In vxworks
	*/
	void SetResourcePath(const std::string & strPath);
    
    void SetAviDataPath(const std::string & strPath);

	/**
	* Check the strFile Names whether Exist in ResourcePath
	*/
	bool CheckResFileIsExist(const std::string & strFile);

protected:
    /**
     *  The default constructor.
     */
    CHmsFileUtils();

    /**
     *  Initializes the instance of FileUtils. It will set _searchPathArray and _searchResolutionsOrderArray to default values.
     *
     *  @note When you are porting HmsAviPf-x to a new platform, you may need to take care of this method.
     *        You could assign a default value to _defaultResRootPath in the subclass of FileUtils(e.g. FileUtilsAndroid). Then invoke the FileUtils::init().
     *  @return true if succeed, otherwise it returns false.
     *
     */
    virtual bool init();

	virtual bool isFileExistInternal(const std::string& strFilePath) const;
	virtual bool isDirectoryExistInternal(const std::string& dirPath) const;

    static CHmsFileUtils* s_sharedFileUtils;

private:
	std::string			m_strResourcePath;
    std::string         m_strAviDataPath;
};

NS_HMS_END

#endif    // __HMS_FILEUTILS_H__
