# Howto setup an Example project in Windows VS2008 Express #

These step-by-step instructions are based on OpenFrameworks 0.06. Starting from a working example project might be the easiest. As a fallback here are the steps to make any oF project a ofxVideoGrabberPtgrey project.

  * take a OpenFrameworks Example and modify it

  * add header search path under C/C++/General
    * `..\..\..\addons\ofxVideoGrabberPtgrey\libs\FlyCapture\include\FlyCapture`
    * `..\..\..\addons\ofxVideoGrabberPtgrey\src`
  * add library under Linker/Input
    * `..\..\..\addons\ofxVideoGrabberPtgrey\libs\FlyCapture\lib\win32\FlyCapture2.lib`