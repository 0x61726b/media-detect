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
#include "MediaDetect.h"
#include <iostream>
//----------------------------------------------------------------------------

Nan::Persistent<v8::Function> MediaDetect::MediaDetectWrapper::constructor;


using v8::Function;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Null;
using v8::Object;
using v8::String;
using v8::Value;
using v8::Handle;
using v8::ObjectTemplate;
using v8::Context;
using v8::Persistent;

namespace MediaDetect
{
	MediaDetectWrapper::MediaDetectWrapper()
	{
	}

	MediaDetectWrapper::~MediaDetectWrapper()
	{
	}

	void MediaDetectWrapper::Init(Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target)
	{
		v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
		tpl->SetClassName(Nan::New("MediaDetect").ToLocalChecked());
		tpl->InstanceTemplate()->SetInternalFieldCount(1);

		v8::Local<v8::ObjectTemplate> inst = tpl->InstanceTemplate();


		Nan::SetPrototypeMethod(tpl,"GetCurrentWindows",MediaDetectWrapper::GetRunningPlayers);
		Nan::SetPrototypeMethod(tpl,"GetVideoFileOpenByPlayer",MediaDetectWrapper::GetVideoFileOpenByPlayer);
		Nan::SetPrototypeMethod(tpl,"GetActiveTabLink",MediaDetectWrapper::GetActiveTabLink);
		Nan::SetPrototypeMethod(tpl,"CheckIfTabIsOpen",MediaDetectWrapper::CheckIfTabIsOpen);
		Nan::SetPrototypeMethod(tpl,"Release",MediaDetectWrapper::Release);


		constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
		Nan::Set(target,Nan::New("MediaDetect").ToLocalChecked(),Nan::GetFunction(tpl).ToLocalChecked());

#ifdef _WIN32
		CoInitialize(NULL);
#endif
	}

	NAN_METHOD(MediaDetectWrapper::Release)
	{
#ifdef _WIN32
		CoUninitialize();
#endif
	}

	NAN_METHOD(MediaDetectWrapper::GetVideoFileOpenByPlayer)
	{
		v8::Local<v8::Array> args = info[0].As<v8::Array>();
		v8::Isolate* isolate = info.GetIsolate();

		V8Value argPid = args->Get(v8::String::NewFromUtf8(isolate,"pid"));


		int32_t pid = argPid->Int32Value();

#ifdef _WIN32

		Win32MediaDetect win32;
		std::vector<std::string> files = win32.GetFilesOpenByProcess(pid);

		if(files.size())
		{
			std::string videoFile = files[0];
			info.GetReturnValue().Set(Nan::New(videoFile).ToLocalChecked());
		}
		else
			info.GetReturnValue().Set(Nan::Null());
#endif
	}

	NAN_METHOD(MediaDetectWrapper::GetRunningPlayers)
	{
		/*	PersistentFunction windowCreated;
			windowCreated.Reset((info[0].As<Function>()));

			PersistentObject caller;
			caller.Reset(info.This());*/
		typedef std::vector<WndInfo> WindowVector;
#ifdef _WIN32
		Win32MediaDetect win32;
		WindowVector players = win32.GetWindows();
		WindowVector::iterator playersIt = players.begin();

		Local<Object> returnVal = Nan::New<v8::Object>();

		Local<v8::Array> playerArray = Nan::New<v8::Array>((int)players.size());

		unsigned counter = 0;
		for(playersIt; playersIt != players.end(); ++playersIt,++counter)
		{
			Local<Object> player = Nan::New<v8::Object>();
			Nan::Set(player,Nan::New("windowTitle").ToLocalChecked(),Nan::New((*playersIt).Title).ToLocalChecked());
			Nan::Set(player,Nan::New("windowClass").ToLocalChecked(),Nan::New((*playersIt).ClassName).ToLocalChecked());
			Nan::Set(player,Nan::New("processName").ToLocalChecked(),Nan::New((*playersIt).ProcessName).ToLocalChecked());
			Nan::Set(player,Nan::New("PID").ToLocalChecked(),Nan::New((unsigned)(*playersIt).ProcessID));
			Nan::Set(player,Nan::New("Handle").ToLocalChecked(),Nan::New((uint32_t)(*playersIt).Handle));

			Nan::Set(playerArray,counter,player);
		}

		Nan::Set(returnVal,Nan::New("PlayerArray").ToLocalChecked(),playerArray);

		info.GetReturnValue().Set(returnVal);
#endif

#ifdef __linux__
		NixMediaDetect nix;
		WindowVector windows = nix.GetWindows();

		info.GetReturnValue().Set(Nan::Null());
#endif
	}


	NAN_METHOD(MediaDetectWrapper::New)
	{
		if(info.IsConstructCall())
		{
			v8::Isolate* isolate = info.GetIsolate();
			MediaDetectWrapper *obj = new MediaDetectWrapper;
			obj->Wrap(info.This());


			info.GetReturnValue().Set(info.This());
		}
		else
		{
			const int argc = 1;
			v8::Local<v8::Value> argv[argc] ={info[0]};
			v8::Local<v8::Function> cons = Nan::New(constructor);
			info.GetReturnValue().Set(cons->NewInstance(argc,argv));
		}
	}
	NAN_METHOD(MediaDetectWrapper::GetActiveTabLink)
	{
		v8::Local<v8::Array> args = info[0].As<v8::Array>();
		v8::Isolate* isolate = info.GetIsolate();

		V8Value argHandle = args->Get(v8::String::NewFromUtf8(isolate,"Handle"));
		V8Value argBrowser = args->Get(v8::String::NewFromUtf8(isolate,"Browser"));

		int32_t pid = argHandle->Int32Value();
		int32_t browser = argBrowser->Int32Value();

#ifdef _WIN32
		Win32MediaDetect win32;
		std::string link = win32.GetActiveTabLink((intptr_t)pid,browser);

		if(!link.empty())
			info.GetReturnValue().Set(Nan::New(link.c_str()).ToLocalChecked());
		else
			info.GetReturnValue().Set(Nan::Null());
#endif
	}

	NAN_METHOD(MediaDetectWrapper::CheckIfTabIsOpen)
	{
		v8::Local<v8::Array> args = info[0].As<v8::Array>();
		v8::Isolate* isolate = info.GetIsolate();

		V8Value argHandle = args->Get(v8::String::NewFromUtf8(isolate,"Handle"));
		V8Value argBrowser = args->Get(v8::String::NewFromUtf8(isolate,"Browser"));
		V8Value argTitle = args->Get(v8::String::NewFromUtf8(isolate,"Title"));

		int32_t pid = argHandle->Int32Value();
		int32_t browser = argBrowser->Int32Value();
		std::string title = std::string(*v8::String::Utf8Value(argTitle));

#ifdef _WIN32
		Win32MediaDetect win32;
		bool isOpen = win32.CheckIfTabWithTitleIsOpen(title,(intptr_t)pid,browser);
		info.GetReturnValue().Set(isOpen);
#endif
	}
}