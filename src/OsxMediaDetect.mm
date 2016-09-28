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

// Adapted from github/Atelier-Shiori/detectstream
//
//
//  Created by 高町なのは on 2015/02/09.
//  Copyright (c) 2015年 Chikorita157's Anime Blog. All rights reserved.
//
//  This class parses the title if it's playing an episode.
//  It will find title, episode and season information.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#import "OsxMediaDetect.h"
#import <ScriptingBridge/SBApplication.h>
#import <ScriptingBridge/ScriptingBridge.h>

@implementation OsxMediaDetect

+(NSArray *)GetAllTabs{

	NSMutableArray * tabList = [[NSMutableArray alloc] init];

	// Safari
    SafariApplication* safari = [self GetSafari];
    if(safari)
    {
    	SBElementArray * windows = [safari windows];
	    for (int i = 0; i < [windows count]; i++)
	    {
	        SafariWindow * window = windows[i];
	        SBElementArray * tabs = [window tabs];
	        for (int i = 0 ; i < [tabs count]; i++)
	        {
	        	SafariTab* tab = tabs[i];

	        	NSDictionary* page = [[NSDictionary alloc] initWithObjectsAndKeys:[tab name],@"title",[tab URL], @"url", @"Safari", @"browser",nil];
	        	[tabList addObject:page];
	        }
	    }
    }


    // Chrome
    GoogleChromeApplication* chrome = [self GetChrome];
    if (chrome)
    {
    	SBElementArray * windows = [chrome windows];
	    for (int i = 0; i < [windows count]; i++)
	    {
	        GoogleChromeWindow* window = windows[i];
	        SBElementArray* tabs = [window tabs];
	        for (int i = 0 ; i < [tabs count]; i++)
	        {
	            GoogleChromeTab* tab = tabs[i];

	            NSDictionary* page = [[NSDictionary alloc] initWithObjectsAndKeys:[tab title],@"title",[tab URL], @"url", @"Chrome", @"browser",nil];
	    		[tabList addObject:page];
	        }
		}
    }

        // //
	return tabList;
}


+(GoogleChromeApplication*)GetChrome{
    if ([self CheckIdentifier:@"com.google.Chrome"])
    {
    	return [SBApplication applicationWithBundleIdentifier:@"com.google.Chrome"];
    }
    return nil;
}
+(SafariApplication*)GetSafari{
	if ([self CheckIdentifier:@"com.apple.Safari"])
	{
		return [SBApplication applicationWithBundleIdentifier:@"com.apple.Safari"];
    }
    return nil; 
}

+(BOOL)CheckIdentifier:(NSString*)identifier{
    NSWorkspace * ws = [NSWorkspace sharedWorkspace];
    NSArray *runningApps = [ws runningApplications];
    NSRunningApplication *a;
    for (a in runningApps) {
        if ([[a bundleIdentifier] isEqualToString:identifier]) {
            return true;
        }
    }
    return false;
}

@end