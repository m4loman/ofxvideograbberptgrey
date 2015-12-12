# Howto setup an Example project in OSX #

These step-by-step instructions are based on OpenFrameworks 0.06. Starting from a working example project might be the easiest. As a fallback here are the steps to make any oF project a ofxVideoGrabberPtgrey project.

  * take a OpenFrameworks Example and modify it
  * add the ofxVideoGrabberPtgrey folder to the addon section in xcode
    * make sure you add the files "relative to the project"
    * and select the "recursively reate groups for folders" radio button
    * remove any files relating to other OSes
      * FlyCapture folder, ofxVideoGrabberFlyCapture.h/cpp
      * linux and win32 folders
  * add header search path (for all configurations)
    * ../../../addons/ofxVideoGrabberPtgrey/libs/dc1394/include
  * add library search path
    * ../../../addons/ofxVideoGrabberPtgrey/libs/dc1394/lib/osx
  * add existing framework
    * IOKit.framework
  * Change "Base SDK" to 10.5
  * Change Architectures to "Native Arch of Build Machine" effectively setting it to i386