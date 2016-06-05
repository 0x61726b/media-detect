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
//----------------------------------------------------------------------------

Nan::Persistent<v8::Function> MediaDetect::MediaDetectWrapper::constructor;
MediaDetect::CallbackMap g_CallbackMap;

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


		Nan::SetPrototypeMethod(tpl,"SetWindowCreateCallback",MediaDetectWrapper::SetWindowCreateCallback);
		Nan::SetPrototypeMethod(tpl,"SetWindowActivateCallback",MediaDetectWrapper::SetWindowActivateCallback);
		Nan::SetPrototypeMethod(tpl,"SetWindowCloseCallback",MediaDetectWrapper::SetWindowCloseCallback);
		Nan::SetPrototypeMethod(tpl,"SetWindowMonitorChangeCallback",MediaDetectWrapper::SetWindowCloseCallback);


		//tpl->Set(DEFINE_PROPERTY(kArgsLogLevel,Nan::New(1)));

		//tpl->Set(DEFINE_PROPERTY(kArgsUserName,Nan::New("Not_Defined").ToLocalChecked()));
		//tpl->Set(DEFINE_PROPERTY(kArgsPass,Nan::New("Not_Defined").ToLocalChecked()));
		//tpl->Set(DEFINE_PROPERTY(kArgsModulePath,Nan::New("Not_Defined").ToLocalChecked()));


		constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
		Nan::Set(target,Nan::New("MediaDetect").ToLocalChecked(),Nan::GetFunction(tpl).ToLocalChecked());
	}

	NAN_METHOD(MediaDetectWrapper::SetWindowCreateCallback)
	{
		PersistentFunction windowCreated;
		windowCreated.Reset((info[0].As<Function>()));

		PersistentObject caller;
		caller.Reset(info.This());

		g_CallbackMap.insert(std::make_pair("WindowCreated",
			std::make_pair(caller,windowCreated)));

	}

	NAN_METHOD(MediaDetectWrapper::SetWindowActivateCallback)
	{
		PersistentFunction windowActivated;
		windowActivated.Reset((info[0].As<Function>()));

		PersistentObject caller;
		caller.Reset(info.This());

		g_CallbackMap.insert(std::make_pair("WindowActivated",
			std::make_pair(caller,windowActivated)));
	}

	NAN_METHOD(MediaDetectWrapper::SetWindowCloseCallback)
	{
		PersistentFunction windowClosed;
		windowClosed.Reset((info[0].As<Function>()));

		PersistentObject caller;
		caller.Reset(info.This());

		g_CallbackMap.insert(std::make_pair("WindowClosed",
			std::make_pair(caller,windowClosed)));
	}

	NAN_METHOD(MediaDetectWrapper::SetWindowMonitorChangeCallback)
	{
		PersistentFunction windowClosed;
		windowClosed.Reset((info[0].As<Function>()));

		PersistentObject caller;
		caller.Reset(info.This());

		g_CallbackMap.insert(std::make_pair("WindowMonitorChanged",
			std::make_pair(caller,windowClosed)));
	}

	NAN_METHOD(MediaDetectWrapper::New)
	{
		if(info.IsConstructCall())
		{
			v8::Isolate* isolate = info.GetIsolate();
			MediaDetectWrapper *obj = new MediaDetectWrapper;
			obj->Wrap(info.This());

#ifdef _WIN32
			obj->mediaDetector_.CreateMainThread();
			obj->mediaDetector_.SetOwner(obj);
#endif

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

	NAN_PROPERTY_GETTER(MediaDetectWrapper::MediaDetectWrapperGetter)
	{
		MediaDetectWrapper* obj = Nan::ObjectWrap::Unwrap<MediaDetectWrapper>(info.This());
	}

	void MediaDetectWrapper::OnMediaDetected(DetectInfo* detected)
	{
		if(!detected)
			return;

		Nan::HandleScope scope;

		Local<Object> val = Nan::New<v8::Object>();

		Nan::Set(val,Nan::New("WindowName").ToLocalChecked(),Nan::New(detected->WindowName).ToLocalChecked());
		Nan::Set(val,Nan::New("WindowId").ToLocalChecked(),Nan::New(std::to_string(detected->Id)).ToLocalChecked());

		PersistentValue value;
		value.Reset(val);

		if(detected->Event == DE_CREATED)
		{
			auto c = g_CallbackMap.find("WindowCreated");

			if(c != g_CallbackMap.end())
			{
				Local<Function> local = Nan::New(c->second.second);

				Local<Value> ret[1] ={Nan::New(value)};
				local->Call(Null(Isolate::GetCurrent()),1,ret);
			}
		}

		if(detected->Event == DE_ACTIVATED)
		{
			auto c = g_CallbackMap.find("WindowActivated");

			if(c != g_CallbackMap.end())
			{
				Local<Function> local = Nan::New(c->second.second);

				Local<Value> ret[1] ={Nan::New(value)};
				local->Call(Null(Isolate::GetCurrent()),1,ret);
			}
		}

		if(detected->Event == DE_CLOSED)
		{
			auto c = g_CallbackMap.find("WindowClosed");

			if(c != g_CallbackMap.end())
			{
				Local<Function> local = Nan::New(c->second.second);

				Local<Value> ret[1] ={Nan::New(value)};
				local->Call(Null(Isolate::GetCurrent()),1,ret);
			}
		}

		if(detected->Event == DE_MONITORCHANGED)
		{
			auto c = g_CallbackMap.find("WindowMonitorChanged");

			if(c != g_CallbackMap.end())
			{
				Local<Function> local = Nan::New(c->second.second);

				Local<Value> ret[1] ={Nan::New(value)};
				local->Call(Null(Isolate::GetCurrent()),1,ret);
			}
		}
	}
}