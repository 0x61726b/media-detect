var md = require('.');
var MediaDetect = new md.MediaDetect();
_ = require('lodash');

MediaPlayerList = [
  { name: "MPC" , class: "MediaPlayerClassicW", executables: ['mpc-hc', 'mpc-hc64'] },
  { name: "BSPlayer" , class: "BSPlayer", executables: ['bsplayer'] },
  { name: "Chrome" , class: "Chrome_WidgetWin_1", browser:0, executables: ['chrome'] },
  { name: "Firefox" , class: "MozillaWindowClass", browser: 1, executables: ['firefox'] },
];

_.forEach(MediaPlayerList,function(v,k) {
  var currentWindows = MediaDetect.GetCurrentWindows();
  var currentMp = _.filter(currentWindows.PlayerArray, {windowClass:v.class});
  if(!_.isEmpty(currentMp)) {
    _.forEach( currentMp, function(mv,mk) {
      console.log(mv.windowClass + " - " + mv.Handle + " - " + v.browser + "- " + MediaDetect.GetActiveTabLink({ Handle:mv.Handle, Browser:v.browser }));
    });
  }
})

process.on('exit',function() {
  console.log("Exiting");
  MediaDetect.Release();
})
