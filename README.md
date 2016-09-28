#Media Detect

This repository is being developed for [Chiika](https://github.com/arkenthera/Chiika).

## Synopsis
Purpose of this library is to detect what the user is currently watching. Be it via media players or using a browser such as Chrome,Firefox etc.
This is achieved by either native APIs or browser extensions like [chiika-chrome](https://github.com/arkenthera/chiika-chrome) or [chiika-firefox](https://github.com/arkenthera/chiika-firefox).
Check out the platform sections to see what this library does in each platform.

## Windows

Supports detecting media players,web browsers.

## Osx

Supports detecting web browsers.

## Linux

Soon ™

# Usage - Win32

## MediaDetect.GetCurrentWindows().PlayerArray

Lists every possible window
```

//Get all windows
var md = require('.').MediaDetect();
var currentWindows = md.GetCurrentWindows().PlayerArray;
console.log(currentWindows);

// Output will be every window in the format of
{
  windowTitle,
  windowClass,
  processName,
  PID,
  Handle
}
```

![](http://i.imgur.com/EDbBdby.png)

## MediaDetect.GetVideoFileOpenByPlayer({ pid })

Returns a video file open by a process.

```
var md = require('.').MediaDetect();

// Find a video player's PID

var videoFile = md.GetVideoFileOpenByPlayer({ pid: pid });
```

![](http://i.imgur.com/WUVV5jp.png)

## MediaDetect.CheckIfTabIsOpen({ Handle, Browser,Title })

Returns if a tab with a given title is still open.

### Parameters
- **Handle** : Hwnd of the window
- **Browser Index** - (0: chrome, 1: Firefox, 2: IE )
- **Title**: Tab Title


## MediaDetect.GetActiveTabLink({ Handle, Browser })

Returns URL of an active tab of a given browser.

### Parameters
- **Handle** : Hwnd of the window
- **Browser Index** - (0: chrome, 1: Firefox, 2: IE )

[Here](https://github.com/arkenthera/Chiika/blob/master/src/main_process/media-detect-win32-process.coffee#L84) is how this library is being used in Chiika.


# Usage - Osx

## MediaDetect.GetAllTabsMacOsx()

Returns all tabs currently open across Safari and Chrome.

Example output

```
[ { tabTitle: 'Google',
    tabUrl: 'https://www.google.com.tr/?client=safari&channel=mac_bm&gws_rd=cr&ei=OOrrV5m9N8T9UobIhcgK',
    browser: 'Safari' },
  { tabTitle: 'Chrome',
    tabUrl: 'chrome://chrome-signin/?access_point=0&reason=0',
    browser: 'Chrome' } ]
```

# Usage - Linux

Coming soon™
