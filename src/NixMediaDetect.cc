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
#include "NixMediaDetect.h"
#include "MediaDetect.h"

#include <iostream>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

namespace XUtil
{
	Window* GetWindowList(Display* disp, unsigned long* len) 
	{
	    Atom prop = XInternAtom(disp,"_NET_CLIENT_LIST",False), type;
	    int form;
	    unsigned long remain;
	    unsigned char *list;
	 
	    errno = 0;
	    if (XGetWindowProperty(disp,XDefaultRootWindow(disp),prop,0,1024,False,XA_WINDOW,
	                &type,&form,len,&remain,&list) != Success) {
	        perror("winlist() -- GetWinProp");
	        return 0;
	    }
     
    	return (Window*)list;
	}

	char* GetX11Property(char* propType,Display *disp, Window win)
	{
	    Atom prop = XInternAtom(disp,propType,False), type;
	    int form;
	    unsigned long remain, len;
	    unsigned char *list;
	 
	    errno = 0;
	    if (XGetWindowProperty(disp,win,prop,0,1024,False,XA_STRING,
	                &type,&form,&len,&remain,&list) != Success) {
	        perror("winlist() -- GetWinProp");
	        return NULL;
	    }
	 
	    return (char*)list;
	}
}

namespace MediaDetect
{
	NixMediaDetect::NixMediaDetect()
	{
		
	}

	NixMediaDetect::~NixMediaDetect()
	{

	}
	std::vector<WndInfo> NixMediaDetect::GetWindows()
	{
		std::vector<WndInfo> windows;



		return windows;
	}
	std::vector<WndInfo> NixMediaDetect::GetCurrentWindows()
	{
		std::vector<WndInfo> windows;

		Display* display = XOpenDisplay(NULL);

		Window* windowList;

		if(!display)
			return windows;

		unsigned long len;
		windowList = (Window*)XUtil::GetWindowList(display,&len);

		char* name;
		char* pid;
		char* machine;
 
	    for (int i=0;i<(int)len;i++) {
	    	name = XUtil::GetX11Property("WM_NAME",display,windowList[i]);
	    	pid = XUtil::GetX11Property("_NET_WM_PID",display,windowList[i]);
	    	machine = XUtil::GetX11Property("WM_CLIENT_MACHINE",display,windowList[i]);
//	    	name = XUtil::winame(display,windowList[i]);
	        std::cout << name << " (" << pid << ")" << " (" << machine << ")" << std::endl;
	        free(name);
	    }

		XFree(windowList);
		XCloseDisplay(display);
		return windows;
	}
}