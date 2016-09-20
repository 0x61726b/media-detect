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
#include <vector>
#include <map>
#include <Windows.h>

namespace MediaDetect
{
	class MediaDetectWrapper;

	struct WndInfo
	{
		unsigned long ProcessID;
		intptr_t Handle;
		std::string ClassName;
		std::string ProcessName;
		std::string Title;
	};

	class Win32MediaDetect
	{
	public:
		Win32MediaDetect();
		virtual ~Win32MediaDetect();

		std::vector<std::string> GetFilesOpenByProcess(unsigned PID);
		std::vector<WndInfo> GetWindows();

		std::string GetActiveTabLink(intptr_t handle,unsigned browser);
		bool CheckIfTabWithTitleIsOpen(const std::string& title,intptr_t handle,unsigned browser);

		std::vector<std::string> videoFormats;
		std::vector<int> webBrowsers;
	};
}

#endif