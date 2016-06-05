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
#ifndef __MediaDetect_h__
#define __MediaDetect_h__
//----------------------------------------------------------------------------
#include <nan.h>
#include "Win32MediaDetect.h"
#include "Common.h"

#include <map>
//----------------------------------------------------------------------------
namespace MediaDetect
{
	typedef std::map<std::string,
		std::pair<Nan::Persistent<v8::Object,v8::CopyablePersistentTraits<v8::Object>>,
		Nan::Persistent<v8::Function,v8::CopyablePersistentTraits<v8::Function>> >> CallbackMap;

	class MediaDetectWrapper : public Nan::ObjectWrap
	{
	public:
		static void Init(Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target);

	public:
		explicit MediaDetectWrapper();
		~MediaDetectWrapper();

		static NAN_METHOD(New);

		static NAN_METHOD(SetWindowCreateCallback);
		static NAN_METHOD(SetWindowActivateCallback);
		static NAN_METHOD(SetWindowCloseCallback);
		static NAN_METHOD(SetWindowMonitorChangeCallback);

		static Nan::Persistent<v8::Function> constructor;

		static NAN_PROPERTY_GETTER(MediaDetectWrapperGetter);

		void OnMediaDetected(DetectInfo* detected);

		Win32MediaDetect mediaDetector_;
	};

}
//----------------------------------------------------------------------------
#endif