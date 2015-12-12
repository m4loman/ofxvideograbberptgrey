# Howto setup an Example project in Linux code::blocks #

These step-by-step instructions are based on OpenFrameworks 0.06. Starting from a working example project might be the easiest. As a fallback here are the steps to make any oF project a ofxVideoGrabberPtgrey project.

  * take a OpenFrameworks Example and modify it
  * open .cbp project file in a text editor
  * under the general `<Compiler>` add
    * `<Add directory="../../../addons/ofxVideoGrabberPtgrey/libs/dc1394/include" />`
    * `<Add directory="../../../addons/ofxVideoGrabberPtgrey/src" />`
  * under the general `<Linker>` add
    * `<Add library="../../../addons/ofxVideoGrabberPtgrey/libs/dc1394/lib/linux/libdc1394.a" />`
  * under '<Option virtualFolders=' add
    * 'addons/ofxVideoGrabberPtgrey/src/'
  * In the `<Unit>` list add
    * `<Unit filename="../../../addons/ofxVideoGrabberPtgrey/src/ofxVideoGrabberDc1394.cpp"> <Option virtualFolder="addons/ofxVideoGrabberPtgrey/src" /> </Unit>`
    * `<Unit filename="../../../addons/ofxVideoGrabberPtgrey/src/ofxVideoGrabberDc1394.h"> <Option virtualFolder="addons/ofxVideoGrabberPtgrey/src" /> </Unit>`
    * `<Unit filename="../../../addons/ofxVideoGrabberPtgrey/src/ofxVideoGrabberPtgrey.h"> <Option virtualFolder="addons/ofxVideoGrabberPtgrey/src" /> </Unit>`