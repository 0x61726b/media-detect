#Media Detect

Media-Detect is a tool that listens global window creation/destruction and fires callbacks accordingly.
Only works on Windows for *now*.

![](http://i.imgur.com/L9U8OOK.png)

#Usage

Require the module and simply create an object then register the callback you'd like to listen.

```

var NativeMD = require('media-detect').MediaDetect;

var MediaDetect = new NativeMD();

//Called when a new window created,i.e., you launch a program/explorer
D.SetWindowCreateCallback( function(arg) { //Arg will be [WindowName,WindowId]
  //...
});

//Called when a window is activated
D.SetWindowActivateCallback( function(arg) { //Arg will be [WindowName,WindowId]
  //...
});

//Called when a window closed/destroyed
D.SetWindowCloseCallback( function(arg) { //Arg will be [WindowName,WindowId]
  //...
});

//Called when a window changes monitor
D.SetWindowMonitorChangeCallback( function(arg) { //Arg will be [WindowName,WindowId]
  //...
});

```

#Tests

Install ```npm i jasmine-node``` then run ```npm test```. Then launch some windows/apps and see the output.

![](http://i.imgur.com/q28a5GV.png)
