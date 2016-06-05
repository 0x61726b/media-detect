if( typeof MediaDetect === 'undefined' ) {
  var MediaDetect = require('../.').MediaDetect;
}

describe('WindowCreate', function(){
  var waitForWindowDetect,windowDetected;

  it('WindowCreate', function(){
    runs(function() {
      var D = new MediaDetect();

      D.SetWindowCreateCallback( function(arg) {
        windowDetected = true;
        waitForWindowDetect = true;
        console.log("Window Create: ( " + arg.WindowId + " ) " + arg.WindowName);
      });
    });

    waitsFor(function() {
      return waitForWindowDetect;
    }, 5000);

    runs(function() {
      expect( windowDetected ).toBe( true );
    });
  });
})

describe('WindowClose', function(){
  var waitForWindowDetect,windowDetected;

  it('WindowClose', function(){
    runs(function() {
      var D = new MediaDetect();

      D.SetWindowCloseCallback( function(arg) {
        windowDetected = true;
        waitForWindowDetect = true;
        console.log("Window Close: ( " + arg.WindowId + " ) " + arg.WindowName);
      });
    });

    waitsFor(function() {
      return waitForWindowDetect;
    }, 5000);

    runs(function() {
      expect( windowDetected ).toBe( true );
    });
  });
})

describe('WindowActivate', function(){
  var waitForWindowDetect,windowDetected;

  it('WindowActivate', function(){
    runs(function() {
      var D = new MediaDetect();

      D.SetWindowCloseCallback( function(arg) {
        windowDetected = true;
        waitForWindowDetect = true;
        console.log("Window Activate: ( " + arg.WindowId + " ) " + arg.WindowName);
      });
    });

    waitsFor(function() {
      return waitForWindowDetect;
    }, 5000);

    runs(function() {
      expect( windowDetected ).toBe( true );
    });
  });
})

describe('WindowMonitorChange', function(){
  var waitForWindowDetect,windowDetected;

  it('WindowMonitorChange', function(){
    runs(function() {
      var D = new MediaDetect();

      D.SetWindowMonitorChangeCallback( function(arg) {
        windowDetected = true;
        waitForWindowDetect = true;
        console.log("Window Change Monitor: ( " + arg.WindowId + " ) " + arg.WindowName);
      });
    });

    waitsFor(function() {
      return waitForWindowDetect;
    }, 5000);

    runs(function() {
      expect( windowDetected ).toBe( true );
    });
  });
})
