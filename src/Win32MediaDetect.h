//----------------------------------------------------------------------------
//Chiika
//
//This program is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along
//with this program; if not, write to the Free Software Foundation, Inc.,
//51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.*/
//----------------------------------------------------------------------------
#ifndef __Win32MediaDetect_h__
#define __Win32MediaDetect_h__
//----------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <nan.h>

#ifdef _WIN32
#include <windows.h>
typedef DWORD ThreadId;
#else
#include <unistd.h>
#include <pthread.h>
typedef unsigned int ThreadId;
#endif


//----------------------------------------------------------------------------

namespace MediaDetect
{
	class MediaDetectWrapper;

	class Win32MediaDetect
	{
	public:
		Win32MediaDetect();
		virtual ~Win32MediaDetect();

		static Win32MediaDetect* GetInstance();

		void CreateMainThread();
		void Init();

		void ThreadRunner();

		void SetOwner(MediaDetectWrapper* obj);

		static LRESULT CALLBACK DetectorWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

		void OnWindowDetected(const std::string& name);

		static void TaskOnMainThread(uv_async_t* req);

		uv_async_t async;
		uv_loop_t *loop;

		ThreadId main_thread;
	public:
		static HWND DetectorHandle;
		static UINT shellHookMessage_;

		static Win32MediaDetect* inst_;
		MediaDetectWrapper* owner_;
	private:

		
	};

	struct TaskInfo
	{
		MediaDetect::Win32MediaDetect* Instance;
	};

	enum DetectEvent
	{
		DE_CREATED,
		DE_ACTIVATED,
		DE_CLOSED,
		DE_MONITORCHANGED
	};

	struct DetectInfo
	{
		std::string WindowName;
		INT_PTR Id;
		DetectEvent Event;

		TaskInfo Task;
	};
}

#endif