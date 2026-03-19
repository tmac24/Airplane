#pragma once
#include "HmsGL.h"
#include <stdlib.h>

#define _HMS_LINUX_USE_WAYLAND

#ifdef __cplusplus

extern "C" {
#endif


///
//  Macros
//
#ifdef WIN32
#define ESUTIL_API  __cdecl
#define ESCALLBACK  __cdecl
#else
#define ESUTIL_API
#define ESCALLBACK
#endif


	/// esCreateWindow flag - RGB color buffer
#define ES_WINDOW_RGB           0
	/// esCreateWindow flag - ALPHA color buffer
#define ES_WINDOW_ALPHA         1
	/// esCreateWindow flag - depth buffer
#define ES_WINDOW_DEPTH         2
	/// esCreateWindow flag - stencil buffer
#define ES_WINDOW_STENCIL       4
	/// esCreateWindow flat - multi-sample buffer
#define ES_WINDOW_MULTISAMPLE   8

#define ES_MOUSE_BTN_LEFT		1
#define ES_MOUSE_BTN_RIGHT		2
#define ES_MOUSE_BTN_MIDDLE		3


	///
	// Types
	//
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

	typedef struct ESContext ESContext;

	struct EsTouchPoint
	{
		int nId;
		float fX;
		float fY;
	};

	struct ESContext
	{
		void		* platformData;			//Put platform specific data here
		void		* userData;				//Put your user data here...
		GLint		surfXPos;
		GLint		surfYPos;
		GLint		surfWidth;				//windows width
		GLint		surfHeight;				//windows height
		GLint		viewportWidth;
		GLint		viewportHeight;
		GLint 		surfBuffer;				//surface render buffer
		GLint		supportTouch;			//is supprot touch

		float		fUpdateInterval;		//the update time interval

		EGLNativeDisplayType eglNativeDisplay;	// Display handle
		EGLNativeWindowType  eglNativeWindow;	// Window handle
		EGLDisplay  eglDisplay;					// EGL display
		EGLContext  eglContext;					// EGL context
		EGLSurface  eglSurface;					// EGL surface

		/// Callbacks
		void (ESCALLBACK *drawFunc) (ESContext *, float fDelta);
		void (ESCALLBACK *shutdownFunc) (ESContext *);
		void (ESCALLBACK *keyFunc) (ESContext *, unsigned char, int, int);
		void (ESCALLBACK *updateFunc) (ESContext *, float deltaTime);
		void (ESCALLBACK *touchBeginFunc)(ESContext *, unsigned int, struct EsTouchPoint *);
		void (ESCALLBACK *touchMoveFunc)(ESContext *, unsigned int, struct EsTouchPoint *);
		void (ESCALLBACK *touchEndFunc)(ESContext *, unsigned int, struct EsTouchPoint *);
		void (ESCALLBACK *windowMove)(ESContext *, int, int);
		void (ESCALLBACK *mouseDownFunc)(ESContext *, unsigned int, unsigned int, unsigned int);					//mouse id, x, y
		void (ESCALLBACK *mouseUpFunc)(ESContext *, unsigned int, unsigned int, unsigned int);						//mouse id, x, y
		void (ESCALLBACK *mouseMoveFunc)(ESContext *, unsigned int, unsigned int);									//x, y
		void (ESCALLBACK *mouseWheelFunc)(ESContext *, unsigned int, unsigned int, unsigned int);					//x,y,wheelDelta
		void (ESCALLBACK *mouseWheelHorFunc)(ESContext *, unsigned int, unsigned int, unsigned int);				//x,y,wheelDelta
	};

	/**
	* Create a window with the specified parameters
	*param	esContext:Application context
	*param	title:Name for title bar of window
	*param	flags:Bitfield for the window creation flags
	*			ES_WINDOW_RGB     - specifies that the color buffer should have R,G,B channels
	*			ES_WINDOW_ALPHA   - specifies that the color buffer should have alpha
	*			ES_WINDOW_DEPTH   - specifies that a depth buffer should be created
	*			ES_WINDOW_STENCIL - specifies that a stencil buffer should be created
	*			ES_WINDOW_MULTISAMPLE - specifies that a multi-sample buffer should be created
	*returns GL_TRUE if window creation is succesful, GL_FALSE otherwise
	*by [3/23/2017 song.zhang079] 
	*/
	GLboolean ESUTIL_API esCreateWindow(ESContext *esContext, const char *title, GLuint flags);

	/**
	* Set the window position, this is only used in windows
	*param	esContext:Application context
	*param	xpos		- the x position
	*		ypos		- the y position
	*
	*by [3/23/2017 song.zhang079]
	*/
	void ESUTIL_API esSetWindowPos(ESContext * esContext, int xpos, int ypos);

	/**
	* Release a windows's context
	*param   esContext:Application context
	*returns 
	*by [3/23/2017 song.zhang079] 
	*/
	void ESUTIL_API esRelaseWindowContext(ESContext *esContext);

	/**
	* Swap the openGL es buffers
	*param   esContext:Application context
	*returns 
	*by [3/23/2017 song.zhang079] 
	*/
	void ESUTIL_API esSwapBuffers(ESContext * esContext);

	//
	/// \brief Register a draw callback function to be used to render each frame
	/// \param esContext Application context
	/// \param drawFunc Draw callback function that will be used to render the scene
	//
	void ESUTIL_API esRegisterDrawFunc(ESContext *esContext, void (ESCALLBACK *drawFunc) (ESContext *, float));

	//
	/// \brief Register a callback function to be called on shutdown
	/// \param esContext Application context
	/// \param shutdownFunc Shutdown callback function
	//
	void ESUTIL_API esRegisterShutdownFunc(ESContext *esContext, void (ESCALLBACK *shutdownFunc) (ESContext *));

	//
	/// \brief Register an update callback function to be used to update on each time step
	/// \param esContext Application context
	/// \param updateFunc Update callback function that will be used to render the scene
	//
	void ESUTIL_API esRegisterUpdateFunc(ESContext *esContext, void (ESCALLBACK *updateFunc) (ESContext *, float));

	//
	/// \brief Register an keyboard input processing callback function
	/// \param esContext Application context
	/// \param keyFunc Key callback function for application processing of keyboard input
	//
	void ESUTIL_API esRegisterKeyFunc(ESContext *esContext,
		void (ESCALLBACK *drawFunc) (ESContext *, unsigned char, int, int));
	//
	/// \brief Log a message to the debug output for the platform
	/// \param formatStr Format string for error log.
	//
	void ESUTIL_API esLogMessage(const char *formatStr, ...);

    GLboolean esUserInterrupt(ESContext * esContext);



#ifdef __cplusplus
}
#endif