//
// Created by yan.jiang on 2018/6/26.
// for Android platform, redirect the output of std::cout to Logcat
//

/*
    Sample code :

  	AndroidLogcatStreamBuf g_MyStreamBuf;
	std::cout.rdbuf(&g_MyStreamBuf);
	//NOTE: std::endl will call sync() immediately, message will appear on the Logcat. std::endl is not just for return a new line.
	std::cout << "hello " << 123 << std::endl;
	std::cout << "pi = " << 3.14 << std::endl;

 */

#ifndef PROJ_ANDROID_STUDIO_ANDROIDLOGCATSTREAMBUF_H
#define PROJ_ANDROID_STUDIO_ANDROIDLOGCATSTREAMBUF_H

#include <iostream>
#include <streambuf>
#ifdef ANDROID
#include <android/log.h>
#endif

class AndroidLogcatStreamBuf : public std::streambuf
{
	enum
	{
		BUFFER_SIZE = 255,
	};

public:
	AndroidLogcatStreamBuf()
	{
		buffer_[BUFFER_SIZE] = '\0';
		setp(buffer_, buffer_ + BUFFER_SIZE - 1);
	}

	~AndroidLogcatStreamBuf()
	{
		sync();
	}

protected:
	virtual int_type overflow(int_type c)
	{
		if (c != EOF)
		{
			*pptr() = c;
			pbump(1);
		}
		flush_buffer();
		return c;
	}

	virtual int sync()
	{
		flush_buffer();
		return 0;
	}

private:
	int flush_buffer()
	{
		int len = int(pptr() - pbase());
		if (len <= 0)
			return 0;

		if (len <= BUFFER_SIZE)
			buffer_[len] = '\0';

#ifdef ANDROID
		android_LogPriority t = ANDROID_LOG_INFO;
		__android_log_write(t, "mylog", buffer_);
#else
		printf("%s", buffer_);
#endif

		pbump(-len);
		return len;
	}

private:
	char buffer_[BUFFER_SIZE + 1];

};


#endif //PROJ_ANDROID_STUDIO_ANDROIDLOGCATSTREAMBUF_H
