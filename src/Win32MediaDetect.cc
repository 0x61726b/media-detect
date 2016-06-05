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
#include "Win32MediaDetect.h"
#include "MediaDetect.h"

#include <string>

ThreadId GetThreadIdExx() {
	ThreadId nThreadID;
#ifdef _WIN32
	nThreadID = GetCurrentProcessId();
	nThreadID = (nThreadID << 16) + GetCurrentThreadId();
#else
	nThreadID = getpid();
	nThreadID = (nThreadID << 16) + pthread_self();
#endif
	return nThreadID;
}





namespace MediaDetect
{
	HWND Win32MediaDetect::DetectorHandle;
	UINT Win32MediaDetect::shellHookMessage_;
	Win32MediaDetect* Win32MediaDetect::inst_;


	LRESULT Win32MediaDetect::DetectorWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
	{
		if(msg == Win32MediaDetect::shellHookMessage_)
		{
			char text[256];
			GetWindowTextA((HWND)lParam,text,256);

			if(strlen(text) == 0)
				return 0;

			INT_PTR id;
			id = (INT_PTR)lParam;

			DetectInfo* info = new DetectInfo;
			info->WindowName = text;
			info->Id = id;

			TaskInfo task;

			Win32MediaDetect* instance = reinterpret_cast<Win32MediaDetect*>(GetWindowLongPtr(hwnd,GWLP_USERDATA));

			bool invokeJs = false;

			if(wParam == HSHELL_WINDOWCREATED)
			{
				info->Event = DE_CREATED;
				invokeJs = true;
			}

			if(wParam == HSHELL_WINDOWACTIVATED)
			{
				info->Event = DE_ACTIVATED;
				invokeJs = true;
			}

			if(wParam == HSHELL_WINDOWDESTROYED)
			{
				info->Event = DE_CLOSED;
				invokeJs = true;
			}

			if(wParam == HSHELL_MONITORCHANGED)
			{
				info->Event = DE_MONITORCHANGED;
				invokeJs = true;
			}

			if(instance && invokeJs)
			{
				task.Instance = instance;
				info->Task = task;
				instance->async.data = reinterpret_cast<void*>(info);
				uv_async_send(&instance->async);
			}
		}

		switch(msg)
		{
		case WM_CREATE:
		{
			Win32MediaDetect::shellHookMessage_ = RegisterWindowMessage(TEXT("SHELLHOOK"));

			OutputDebugStringA(std::to_string(Win32MediaDetect::shellHookMessage_).c_str());

			if(!RegisterShellHookWindow(hwnd))
			{
				MessageBox(hwnd,"ShellHook failed.",NULL,MB_OK);
			}
		}
		break;
		case WM_CLOSE:
		{
			OutputDebugStringA("Detector window closing");
		}
		break;
		}
		return DefWindowProc(hwnd,msg,wParam,lParam);
	}

	Win32MediaDetect::Win32MediaDetect()
	{
		loop = uv_default_loop();
		uv_async_init(loop,&async,&Win32MediaDetect::TaskOnMainThread);

		main_thread = GetThreadIdExx();
	}

	Win32MediaDetect::~Win32MediaDetect()
	{
	}

	Win32MediaDetect* Win32MediaDetect::GetInstance()
	{
		return inst_;
	}

	void Win32MediaDetect::TaskOnMainThread(uv_async_t* req)
	{
		DetectInfo* info = reinterpret_cast<DetectInfo*>(req->data);

		info->Task.Instance->owner_->OnMediaDetected(info);
	}

	void Win32MediaDetect::OnWindowDetected(const std::string& name)
	{

	}

	DWORD WINAPI ThreadFunctionStatic(void* data)
	{
		Win32MediaDetect* thread = static_cast<Win32MediaDetect*>(data);
		thread->ThreadRunner();
		return 0;
	}

	void Win32MediaDetect::Init()
	{
		WNDCLASSEX wc ={0};
		wc.cbSize = sizeof(wc);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wc.hCursor = LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW));
		wc.hIcon = LoadIcon(NULL,MAKEINTRESOURCE(IDI_APPLICATION));
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpfnWndProc = DetectorWndProc;
		wc.lpszClassName = "shell_wnd_class";
		wc.style = CS_HREDRAW | CS_VREDRAW;

		if(!RegisterClassEx(&wc))
			return;

		DetectorHandle = CreateWindowEx(0,"shell_wnd_class","Shell Hook Demo",0,
			0,0,0,0,NULL,NULL,GetModuleHandle(NULL),NULL);

		if(!DetectorHandle)
		{
			MessageBox(NULL,"CreateWindowEx Failed!",NULL,MB_OK);
			return;
		}

		UpdateWindow(DetectorHandle);
		ShowWindow(DetectorHandle,SW_HIDE);

		SetWindowLongPtr(DetectorHandle,GWLP_USERDATA,(LONG_PTR)this);

		MSG msg;
		while(GetMessage(&msg,NULL,0,0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void Win32MediaDetect::ThreadRunner()
	{
		Init();
	}

	void Win32MediaDetect::CreateMainThread()
	{
		HANDLE threadHandle = CreateThread(0,0,ThreadFunctionStatic,this,0,0);
	}

	void Win32MediaDetect::SetOwner(MediaDetectWrapper* obj)
	{
		owner_ = obj;
	}
}
