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
#include "../MediaDetect.h"
#include <iostream>
//----------------------------------------------------------------------------

Nan::Persistent<v8::Function> MediaDetect::MediaDetectWrapper::constructor;


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
		Nan::SetPrototypeMethod(tpl,"GetAllTabsMacOsx",MediaDetectWrapper::GetAllTabsMacOsx);




		constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
		Nan::Set(target,Nan::New("MediaDetect").ToLocalChecked(),Nan::GetFunction(tpl).ToLocalChecked());
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

	#ifdef CHIIKA_MACOSX
		NAN_METHOD(MediaDetectWrapper::GetAllTabsMacOsx)
		{
			NSArray * pages = [OsxMediaDetect GetAllTabs];

			v8::Local<v8::Array> tabArray = Nan::New<v8::Array>((int)[pages count]);

			unsigned counter = 0;
			for (NSDictionary* dict in pages)
			{
				v8::Local<v8::Object> tab = Nan::New<v8::Object>();

				NSString* title = [NSString stringWithFormat: @"%@", dict[@"title"]];
				NSString* url = [NSString stringWithFormat: @"%@", dict[@"url"]];
				NSString* browser = [NSString stringWithFormat: @"%@", dict[@"browser"]];

				std::string titleToStd = std::string([title UTF8String]);
				std::string urlToStd = std::string([url UTF8String]);
				std::string browserToStd = std::string([browser UTF8String]);

				Nan::Set(tab,Nan::New("tabTitle").ToLocalChecked(),Nan::New(titleToStd).ToLocalChecked());
				Nan::Set(tab,Nan::New("tabUrl").ToLocalChecked(),Nan::New(urlToStd).ToLocalChecked());
				Nan::Set(tab,Nan::New("browser").ToLocalChecked(),Nan::New(browserToStd).ToLocalChecked());

				Nan::Set(tabArray,counter,tab);
				counter++;
			}

			info.GetReturnValue().Set(tabArray);
		}
	#endif
}
