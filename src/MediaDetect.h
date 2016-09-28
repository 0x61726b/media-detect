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

#include "Common.h"

#ifdef CHIIKA_WIN32
#include "Win32MediaDetect.h"
#elif CHIIKA_LINUX
#include "NixMediaDetect.h"
#elif CHIIKA_MACOSX
#include "OsxMediaDetect.h"
#endif


#include <map>
//----------------------------------------------------------------------------
namespace MediaDetect
{
	class MediaDetectWrapper : public Nan::ObjectWrap
	{
	public:
		static void Init(Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target);

	public:
		explicit MediaDetectWrapper();
		~MediaDetectWrapper();

		static NAN_METHOD(New);


		static NAN_METHOD(GetRunningPlayers);
		static NAN_METHOD(GetActiveTabLink);
		static NAN_METHOD(CheckIfTabIsOpen);
		static NAN_METHOD(GetVideoFileOpenByPlayer);
		static NAN_METHOD(Release);

		#ifdef CHIIKA_MACOSX
		static NAN_METHOD(GetAllTabsMacOsx);
		#endif

		static Nan::Persistent<v8::Function> constructor;
	};

}
//----------------------------------------------------------------------------
#endif
