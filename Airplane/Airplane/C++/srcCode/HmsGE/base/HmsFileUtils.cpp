#include "HmsFileUtils.h"
#include <sys/stat.h>
#include "external/unzip/unzip.h"

#if HMS_TARGET_PLATFORM == HMS_PLATFORM_IOS
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

NS_HMS_BEGIN

// Implement FileUtils
CHmsFileUtils* CHmsFileUtils::s_sharedFileUtils = nullptr;


#if HMS_TARGET_PLATFORM != HMS_PLATFORM_ANDROID
CHmsFileUtils* CHmsFileUtils::getInstance()
{
	if (nullptr == s_sharedFileUtils)
	{
		s_sharedFileUtils = new CHmsFileUtils();
	}
	return s_sharedFileUtils;
}
#endif

void CHmsFileUtils::destroyInstance()
{
    HMS_SAFE_DELETE(s_sharedFileUtils);
}


void HmsAviPf::CHmsFileUtils::StringReplace(std::string &strBase, std::string strSrc, std::string strDes)
{
    std::string::size_type pos = 0;
    std::string::size_type srcLen = strSrc.size();
    std::string::size_type desLen = strDes.size();
    pos = strBase.find(strSrc, pos);
    while ((pos != std::string::npos))
    {
        strBase.replace(pos, srcLen, strDes);
        pos = strBase.find(strSrc, (pos + desLen));
    }
}

CHmsFileUtils::CHmsFileUtils()
{
	m_strResourcePath = "../AviRes/";
}

CHmsFileUtils::~CHmsFileUtils()
{
}

bool CHmsFileUtils::writeStringToFile(std::string dataStr, const std::string& fullPath)
{
    Data retData;
    retData.copy((unsigned char*)dataStr.c_str(), dataStr.size());

    return writeDataToFile(retData, fullPath);
}

bool CHmsFileUtils::writeDataToFile(Data retData, const std::string& fullPath)
{
    size_t size = 0;
    const char* mode = "wb";

    HMSASSERT(!fullPath.empty() && retData.getSize() != 0, "Invalid parameters.");

    auto fileutils = CHmsFileUtils::getInstance();
    do
    {
        // Read the file from hardware
        FILE *fp = fopen(fullPath.c_str(), mode);
        HMS_BREAK_IF(!fp);
        size = retData.getSize();

        fwrite(retData.getBytes(), size, 1, fp);

        fclose(fp);

        return true;
    } while (0);

    return false;
}

bool CHmsFileUtils::init()
{
    return true;
}

static Data getData(const std::string& filename, bool forString)
{
    if (filename.empty())
    {
        return Data::Null;
    }

    Data ret;
    unsigned char* buffer = nullptr;
    size_t size = 0;
    size_t readsize;
    const char* mode = nullptr;

    if (forString)
        mode = "rt";
    else
        mode = "rb";

    auto fileutils = CHmsFileUtils::getInstance();
    do
    {
        // Read the file from hardware
        std::string fullPath = fileutils->fullPathForFilename(filename);
        FILE *fp = fopen(fullPath.c_str(), mode);
        
        HMS_BREAK_IF(!fp);
        fseek(fp,0,SEEK_END);
        size = ftell(fp);
        fseek(fp,0,SEEK_SET);

        if (forString)
        {
            buffer = (unsigned char*)malloc(sizeof(unsigned char) * (size + 1));
            buffer[size] = '\0';
        }
        else
        {
            buffer = (unsigned char*)malloc(sizeof(unsigned char) * size);
        }

        readsize = fread(buffer, sizeof(unsigned char), size, fp);
        fclose(fp);

        if (forString && readsize < size)
        {
            buffer[readsize] = '\0';
        }
    } while (0);

    if (nullptr == buffer || 0 == readsize)
    {
        CCLOG("Get data from file %s failed", filename.c_str());
    }
    else
    {
        ret.fastSet(buffer, readsize);
    }

    return ret;
}

std::string CHmsFileUtils::getStringFromFile(const std::string& filename)
{
    Data data = getData(filename, true);
    if (data.isNull())
        return "";

    std::string ret((const char*)data.getBytes());
    return ret;
}


std::string HmsAviPf::CHmsFileUtils::getAviDataPath() const
{
    return m_strAviDataPath;
}


Data CHmsFileUtils::getDataFromFile(const std::string& filename)
{
    return getData(filename, false);
}


CHmsFileUtils::Status CHmsFileUtils::getContents(const std::string& filename, ResizableBuffer* buffer)
{
	if (filename.empty())
		return Status::NotExists;

	auto fs = CHmsFileUtils::getInstance();

	std::string fullPath = fs->fullPathForFilename(filename);
	if (fullPath.empty())
		return Status::NotExists;

	FILE *fp = fopen(fullPath.c_str(), "rb");
	if (!fp)
		return Status::OpenFailed;

#if defined(_MSC_VER)
	auto descriptor = _fileno(fp);
#else
	auto descriptor = fileno(fp);
#endif
	struct stat statBuf;
	if (fstat(descriptor, &statBuf) == -1) {
		fclose(fp);
		return Status::ReadFailed;
	}
	size_t size = statBuf.st_size;

	buffer->resize(size);
	size_t readsize = fread(buffer->buffer(), 1, size, fp);
	fclose(fp);

	if (readsize < size) {
		buffer->resize(readsize);
		return Status::ReadFailed;
	}

	return Status::OK;
}

unsigned char* CHmsFileUtils::getFileDataFromZip(const std::string& zipFilePath, const std::string& filename, ssize_t *size)
{
#if 1
    unsigned char * buffer = nullptr;
    unzFile file = nullptr;
    *size = 0;

    do
    {
        HMS_BREAK_IF(zipFilePath.empty());

        file = unzOpen(zipFilePath.c_str());
        HMS_BREAK_IF(!file);

        // FIXME: Other platforms should use upstream minizip like mingw-w64
        #ifdef MINIZIP_FROM_SYSTEM
        int ret = unzLocateFile(file, filename.c_str(), NULL);
        #else
        int ret = unzLocateFile(file, filename.c_str(), 1);
        #endif
        HMS_BREAK_IF(UNZ_OK != ret);

        char filePathA[260];
        unz_file_info fileInfo;
        ret = unzGetCurrentFileInfo(file, &fileInfo, filePathA, sizeof(filePathA), nullptr, 0, nullptr, 0);
        HMS_BREAK_IF(UNZ_OK != ret);

        ret = unzOpenCurrentFile(file);
        HMS_BREAK_IF(UNZ_OK != ret);

        buffer = (unsigned char*)malloc(fileInfo.uncompressed_size);
        int HMS_UNUSED readedSize = unzReadCurrentFile(file, buffer, static_cast<unsigned>(fileInfo.uncompressed_size));
        HMSASSERT(readedSize == 0 || readedSize == (int)fileInfo.uncompressed_size, "the file size is wrong");

        *size = fileInfo.uncompressed_size;
        unzCloseCurrentFile(file);
    } while (0);

	
    if (file)
    {
        unzClose(file);
    }

    return buffer;
#else
	return nullptr;
#endif
}

#if __vxworks
std::string CHmsFileUtils::fullPathForFilename(const std::string &filename) const
{
	if (filename.empty())
	{
		return "";
	}

	if (isAbsolutePath(filename))
	{
		return filename;
	}
	else
	{
		//return "/ata1:1/AviRes/" + filename;
		return m_strResourcePath + filename;
	}

	return filename;
}
#elif __WIN32

std::string GetExtPath()
{
	char strPath[MAX_PATH];
	DWORD nRet = GetModuleFileNameA(NULL, strPath, MAX_PATH);
	while (nRet >= 0)
	{
		if (strPath[nRet] == '\\')
		{
			strPath[nRet+1] = 0;
			break;
		}
		else
		{
			nRet--;
		}
	}

	std::string path = strPath;
	return path;
}

std::string CHmsFileUtils::fullPathForFilename(const std::string &filename) const
{
	if (filename.empty())
	{
		return "";
	}

	if (isAbsolutePath(filename))
	{
		return filename;
	}
	else
	{
		static std::string s_strFilePath = GetExtPath();
		//return "E:/HermesProject/AvionicsSoft/WindowsProject/CommonSrc/HmsAvionicsIntegrate/bin/" + filename;
		return s_strFilePath + filename;
	}

	return filename;
}
#else

std::string CHmsFileUtils::fullPathForFilename(const std::string &filename) const
{
	if (filename.empty())
	{
		return "";
	}

	if (isAbsolutePath(filename))
	{
		return filename;
	}
	else
	{
        //return "/ata1:1/AviRes/" + filename;
        return m_strResourcePath + filename;
	}

	return filename;
}

#endif

bool CHmsFileUtils::isFileExist(const std::string& filename) const
{
    if (isAbsolutePath(filename))
    {
        return isFileExistInternal(filename);
    }
    else
    {
        std::string fullpath = fullPathForFilename(filename);
        if (fullpath.empty())
        {
            return false;
        }
        else
        {
            return isFileExistInternal(fullpath);
        }
    }
}

bool CHmsFileUtils::isFileExistInternal(const std::string& strFilePath) const
{
	if (strFilePath.empty())
	{
		return false;
	}

#if _WIN32
	std::string strPath = strFilePath;

	DWORD attr = GetFileAttributesA(strPath.c_str());
	if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY))
		return false;   //  not a file
	return true;
#else
	struct stat sts;
	return (stat(strFilePath.c_str(), &sts) != -1) ? true : false;
#endif
}

bool CHmsFileUtils::isAbsolutePath(const std::string& path) const
{
#if _WIN32
	auto & strPath = path;
	if ((strPath.length() > 2
		&& ( (strPath[0] >= 'a' && strPath[0] <= 'z') || (strPath[0] >= 'A' && strPath[0] <= 'Z') )
		&& strPath[1] == ':') || (strPath[0] == '/' && strPath[1] == '/'))
	{
		return true;
	}
	return false;
#else
    return (path[0] == '/') || (path[0] == '.');
#endif
}

bool CHmsFileUtils::isDirectoryExist(const std::string& dirPath) const
{
	HMSASSERT(!dirPath.empty(), "Invalid path");

	if (isAbsolutePath(dirPath))
	{
		return isDirectoryExistInternal(dirPath);
	}
#if _WIN32
	return isDirectoryExistInternal(dirPath);
#endif
	return false;
}

#if _WIN32
// windows os implement should override in platform specific FileUtiles class
#include "io.h"

bool CHmsFileUtils::isDirectoryExistInternal(const std::string& dirPath) const
{
	if (_access(dirPath.c_str(), 0) == 0)
    {
		return true;
    }
    return false;
}

#include "direct.h"

bool CHmsFileUtils::createDirectory(const std::string& dirPath)
{
	if (isDirectoryExist(dirPath))
		return true;

	// Split the path
	size_t start = 0;
	size_t found = dirPath.find_first_of("/\\", start);
	std::string subpath;
	std::vector<std::string> dirs;

	if (found != std::string::npos)
	{
		while (true)
		{
			subpath = dirPath.substr(start, found - start + 1);
			if (!subpath.empty())
				dirs.push_back(subpath);
			start = found + 1;
			found = dirPath.find_first_of("/\\", start);
			if (found == std::string::npos)
			{
				if (start < dirPath.length())
				{
					dirs.push_back(dirPath.substr(start));
				}
				break;
			}
		}
	}

	// Create path recursively
	subpath = "";
	for (std::vector<std::string>::size_type i = 0; i < dirs.size(); ++i)
	{
		subpath += dirs[i]; 
		if (!isDirectoryExist(subpath))
		{
			if (-1 == _mkdir(subpath.c_str()))
			{
				return false;
			}
		}
	}
	return true;
}

bool CHmsFileUtils::removeDirectory(const std::string& path)
{
    HMSASSERT(false, "FileUtils not support removeDirectory");
    return false;
}

bool CHmsFileUtils::removeFile(const std::string &path)
{
    HMSASSERT(false, "FileUtils not support removeFile");
    return false;
}

bool CHmsFileUtils::renameFile(const std::string &oldfullpath, const std::string& newfullpath)
{
    HMSASSERT(false, "FileUtils not support renameFile");
    return false;
}

bool CHmsFileUtils::renameFile(const std::string &path, const std::string &oldname, const std::string &name)
{
    HMSASSERT(false, "FileUtils not support renameFile");
    return false;
}

long CHmsFileUtils::getFileSize(const std::string &filepath)
{
    HMSASSERT(false, "getFileSize should be override by platform FileUtils");
    return 0;
}

#else
// default implements for unix like os
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>

bool CHmsFileUtils::isDirectoryExistInternal(const std::string& dirPath) const
{
    struct stat st;
    if (stat(dirPath.c_str(), &st) == 0)
    {
        return S_ISDIR(st.st_mode);
    }
    return false;
}

bool CHmsFileUtils::createDirectory(const std::string& path)
{
    HMSASSERT(!path.empty(), "Invalid path");

    if (isDirectoryExist(path))
        return true;

    // Split the path
    size_t start = 0;
    size_t found = path.find_first_of("/\\", start);
    std::string subpath;
    std::vector<std::string> dirs;

    if (found != std::string::npos)
    {
        while (true)
        {
            subpath = path.substr(start, found - start + 1);
            if (!subpath.empty())
                dirs.push_back(subpath);
            start = found+1;
            found = path.find_first_of("/\\", start);
            if (found == std::string::npos)
            {
                if (start < path.length())
                {
                    dirs.push_back(path.substr(start));
                }
                break;
            }
        }
    }

    DIR *dir = NULL;

    // Create path recursively
    subpath = "";
    for (int i = 0; i < dirs.size(); ++i)
    {
        subpath += dirs[i];
        dir = opendir(subpath.c_str());

        if (!dir)
        {
            // directory doesn't exist, should create a new one

            int ret = mkdir(subpath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
            if (ret != 0 && (errno != EEXIST))
            {
                // current directory can not be created, sub directories can not be created too
                // should return
                return false;
            }
        }
        else
        {
            // directory exists, should close opened dir
            closedir(dir);
        }
    }
    return true;
}

bool CHmsFileUtils::removeDirectory(const std::string& path)
{
    DIR* dir = opendir(path.c_str());
    if (!dir)
        return false;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        // 跳过 . 和 ..
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;

        std::string fullPath = path + "/" + entry->d_name;

        struct stat st;
        if (lstat(fullPath.c_str(), &st) == 0)
        {
            if (S_ISDIR(st.st_mode))
            {
                // 递归删除子目录
                removeDirectory(fullPath);
            }
            else
            {
                // 删除文件
                unlink(fullPath.c_str());
            }
        }
    }

    closedir(dir);

    // 删除空目录本身
    return rmdir(path.c_str()) == 0;
}

bool CHmsFileUtils::removeFile(const std::string &path)
{
    if (remove(path.c_str())) {
        return false;
    } else {
        return true;
    }
}

bool CHmsFileUtils::renameFile(const std::string &oldfullpath, const std::string &newfullpath)
{
    HMSASSERT(!oldfullpath.empty(), "Invalid path");
    HMSASSERT(!newfullpath.empty(), "Invalid path");

    int errorCode = rename(oldfullpath.c_str(), newfullpath.c_str());

    if (0 != errorCode)
    {
        CCLOGERROR("Fail to rename file %s to %s !Error code is %d", oldfullpath.c_str(), newfullpath.c_str(), errorCode);
        return false;
    }
    return true;
}

bool CHmsFileUtils::renameFile(const std::string &path, const std::string &oldname, const std::string &name)
{
    HMSASSERT(!path.empty(), "Invalid path");
    std::string oldPath = path + oldname;
    std::string newPath = path + name;

    return this->renameFile(oldPath, newPath);
}


long CHmsFileUtils::getFileSize(const std::string &filepath)
{
    HMSASSERT(!filepath.empty(), "Invalid path");

    std::string fullpath = filepath;
    if (!isAbsolutePath(filepath))
    {
        fullpath = fullPathForFilename(filepath);
        if (fullpath.empty())
            return 0;
    }

    struct stat info;
    // Get data associated with "crt_stat.c":
    int result = stat(fullpath.c_str(), &info);

    // Check if statistics are valid:
    if (result != 0)
    {
        // Failed
        return -1;
    }
    else
    {
        return (long)(info.st_size);
    }
}
#endif

#include <stdlib.h>

static char ToLowChar(char c)
{
	if(c >= 'A' && c <= 'Z')
		return c - ('A' - 'a');
	return c;
}

std::string CHmsFileUtils::getFileExtension(const std::string& filePath) const
{
    std::string fileExtension;
    size_t pos = filePath.find_last_of('.');
    if (pos != std::string::npos)
    {
        fileExtension = filePath.substr(pos, filePath.length());

        std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ToLowChar);
    }

    return fileExtension;
}

void CHmsFileUtils::SetResourcePath(const std::string & strPath)
{
#if 0
	m_strResourcePath = strPath;
	auto it = m_strResourcePath.rbegin();
	if (it != m_strResourcePath.rend())
	{
		if ((*it != '/') || (*it != '\\'))
		{
			m_strResourcePath += '/';
		}
	}
#else
	m_strResourcePath = strPath;

	if (strPath.size())
	{
		auto c = strPath.at(strPath.length() - 1);
		if ((c == '/') || (c == '\\'))
		{

		}
		else
		{
			m_strResourcePath += '/';
		}
	}
#endif
}

void CHmsFileUtils::SetAviDataPath(const std::string & strPath)
{
    m_strAviDataPath = strPath;
}



bool HmsAviPf::CHmsFileUtils::CheckResFileIsExist(const std::string & strFile)
{
	std::string fullpath = strFile;
	if (!isAbsolutePath(strFile))
	{
		fullpath = fullPathForFilename(strFile);
	}
	
	if (fullpath.empty())
		return false;

	if (isFileExistInternal(fullpath))
	{
		return true;
	}
	return false;
}

std::string HmsAviPf::CHmsFileUtils::getWritablePath() const
{
	return "";
}

NS_HMS_END



