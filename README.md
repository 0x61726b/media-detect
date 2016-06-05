#Media Detect

```npm install media-detect```


Media-Detect is a tool that listens global window creation/destruction and fires callbacks accordingly.
Only works on Windows for *now*. On Windows the library hooks into shell using **RegisterShellHookWindow** then listens shell messages and fires javascript callbacks.

![](http://i.imgur.com/L9U8OOK.png)

#Usage

Require the module and simply create an object then register the callback you'd like to listen.

```

var NativeMD = require('media-detect').MediaDetect;

var MediaDetect = new NativeMD();

//Called when a new window created,i.e., you launch a program/explorer
MediaDetect.SetWindowCreateCallback( function(arg) { //Arg will be [WindowName,WindowId]
  //...
});

//Called when a window is activated
MediaDetect.SetWindowActivateCallback( function(arg) { //Arg will be [WindowName,WindowId]
  //...
});

//Called when a window closed/destroyed
MediaDetect.SetWindowCloseCallback( function(arg) { //Arg will be [WindowName,WindowId]
  //...
});

//Called when a window changes monitor
MediaDetect.SetWindowMonitorChangeCallback( function(arg) { //Arg will be [WindowName,WindowId]
  //...
});

```

#Tests

Install ```npm i jasmine-node``` then run ```npm test```. Then launch some windows/apps and see the output.

![](http://i.imgur.com/q28a5GV.png)


#Installing

``` npm install media-detect ```

You'll need

- MSVC for Windows (Visual Studio 2013)
- node-gyp
- NaN

or just type ```npm install``` and NPM will build it for you.
