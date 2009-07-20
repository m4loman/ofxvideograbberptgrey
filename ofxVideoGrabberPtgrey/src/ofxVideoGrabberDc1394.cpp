#include "ofxVideoGrabberDc1394.h"





//--------------------------------------------------------------------
ofxVideoGrabberPtgrey::ofxVideoGrabberPtgrey(){
	bIsFrameNew				= false;
	bVerbose 				= false;
	bGrabberInited 			= false;
	bUseTexture				= true;
	bChooseDevice			= false;
	deviceID				= 0;
	width 					= 320;	// default setting
	height 					= 240;	// default setting
	pixels					= NULL;

	frame                   = NULL;
	driver                  = NULL;
	deviceList              = NULL;
	camera                  = NULL;
	video_mode              = DC1394_VIDEO_MODE_640x480_MONO8;
	framerate				= DC1394_FRAMERATE_60;
	
    driver = dc1394_new();
    if (!driver) { return; }
    err = dc1394_camera_enumerate(driver, &deviceList);
	if(err){ ofLog(OF_LOG_ERROR, "Failed to enumerate cameras"); }
	
    if( deviceList->num == 0 ) {
		ofLog(OF_LOG_ERROR, "No cameras found");
    }

}


//--------------------------------------------------------------------
ofxVideoGrabberPtgrey::~ofxVideoGrabberPtgrey(){
	close();
    
    //free device enumeration
    dc1394_camera_free_list(deviceList);
    
    //free context
    dc1394_free(driver);

}


//--------------------------------------------------------------------
void ofxVideoGrabberPtgrey::listDevices(){
    ofLog(OF_LOG_NOTICE, "-------------------------------------");
        
	ofLog(OF_LOG_NOTICE, "Listing available IIDC capture devices.\nUse unit number as unique ID for camera...");
	for (uint32_t index = 0; index < deviceList->num; index++) {
		//printf("Using camera with GUID %"PRIx64"\n", camera->guid);
		ofLog(OF_LOG_NOTICE, "Device[%d].unit = %x, GUID = %x", index, deviceList->ids[index].unit, deviceList->ids[index].guid);
	}    
    
    ofLog(OF_LOG_NOTICE, "-------------------------------------");
}


//--------------------------------------------------------------------
void ofxVideoGrabberPtgrey::setVerbose(bool bTalkToMe){
	bVerbose = bTalkToMe;

}


//--------------------------------------------------------------------
void ofxVideoGrabberPtgrey::setDeviceID(int _deviceID){
	deviceID		= _deviceID;
	bChooseDevice	= true;
}

//---------------------------------------------------------------------------
unsigned char * ofxVideoGrabberPtgrey::getPixels(){
	return pixels;
	//return frame->image;
}

//------------------------------------
//for getting a reference to the texture
ofTexture & ofxVideoGrabberPtgrey::getTextureReference(){
	if(!tex.bAllocated() ){
		ofLog(OF_LOG_WARNING, "ofVideoGrabber - getTextureReference - texture is not allocated");
	}
	return tex;
}

//---------------------------------------------------------------------------
bool  ofxVideoGrabberPtgrey::isFrameNew(){
	return bIsFrameNew;
}

//--------------------------------------------------------------------
void ofxVideoGrabberPtgrey::update(){
	grabFrame();
}

//--------------------------------------------------------------------
void ofxVideoGrabberPtgrey::grabFrame(){
    if (camera != NULL) {
		// get a frame
        // The first time you call a DMA capture function dc1394_capture_dequeue() it returns
		// a pointer to the first frame buffer structure (dc1394frame_t). After a successful 
		// capture function call, the capture_buffer pointer and the frame buffer it points 
		// to are available to you for reading and writing. It will not be overwritten with 
		// a newer frame while it is allocated to you (FREE), even if the ring buffer overflows. 
		// Once you have finished with it you should release it as soon as possible with a call 
		// to dc1394_capture_enqueue().
		err = dc1394_capture_dequeue(camera, DC1394_CAPTURE_POLICY_POLL, &frame);
        if (frame == NULL) {
            bIsFrameNew = false;
        } else {
			bIsFrameNew = true;
			
			// copy into pixels
			for( int i = 0; i < height; i++ ) {
				memcpy( pixels + (i*width), frame->image + (i*frame->stride), width );
			}		

			if (bUseTexture) {
				tex.loadData(frame->image, width, height, GL_LUMINANCE);
			}
			
			// make frame available again as part of the 
			// ring buffer receiving images from the cam
			err = dc1394_capture_enqueue(camera, frame);		
		}
	}
}


//--------------------------------------------------------------------
void ofxVideoGrabberPtgrey::close(){
    if (camera != NULL) {
        dc1394_video_set_transmission(camera, DC1394_OFF);
        dc1394_capture_stop(camera);
        dc1394_camera_free(camera);
    }

	if (pixels != NULL){
		delete[] pixels;
		pixels = NULL;
	}

	tex.clear();
}

//--------------------------------------------------------------------
void ofxVideoGrabberPtgrey::videoSettings(void){
    ofLog(OF_LOG_NOTICE, "videoSettings not implemented");
}



//--------------------------------------------------------------------
bool ofxVideoGrabberPtgrey::initGrabber(int w, int h, bool setUseTexture){
    width = w;
    height = h;
    bUseTexture = setUseTexture;
	
    //first choose a device to use
    if( bChooseDevice ) {
        bool bFound = false;
        for (uint32_t index = 0; index < deviceList->num; index++) {
            if (deviceList->ids[index].unit == deviceID) {
                bFound = true;
                deviceID = deviceList->ids[index].guid;
            }
        }
        if (!bFound) {
            printf("initGrabber warning: Device ID for unit %x not found, using first device\n", deviceID);
            deviceID = deviceList->ids[0].guid;
        }
    } else if(deviceList->num > 0) {
        deviceID = deviceList->ids[0].guid;
    } else {
        ofLog(OF_LOG_ERROR, "in initGrabber, No cameras found");
    }
	
    	
    camera = dc1394_camera_new(driver, deviceID);
	if (!camera) { 
		ofLog(OF_LOG_ERROR, "Failed to initialize camera with guid %x", deviceID);
		return 1;
	}
	
    /*-----------------------------------------------------------------------
     *  setup capture
     *-----------------------------------------------------------------------*/
	
    err=dc1394_video_set_iso_speed(camera, DC1394_ISO_SPEED_400);
    //DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not set iso speed");
	
    err=dc1394_video_set_mode(camera, video_mode);
	//DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not set video mode");
	
    err=dc1394_video_set_framerate(camera, framerate);
	//DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not set framerate");
	
    err=dc1394_capture_setup(camera,4, DC1394_CAPTURE_FLAGS_DEFAULT);
    //DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not setup camera-\nmake sure that the video mode and framerate are\nsupported by your camera");
	

	/*-----------------------------------------------------------------------
     *  set camera's features
     *-----------------------------------------------------------------------*/

	err = dc1394_feature_set_mode(camera, DC1394_FEATURE_BRIGHTNESS, DC1394_FEATURE_MODE_MANUAL);
	err = dc1394_feature_set_value(camera, DC1394_FEATURE_BRIGHTNESS, 200);

	err = dc1394_feature_set_mode(camera, DC1394_FEATURE_EXPOSURE, DC1394_FEATURE_MODE_MANUAL);
	err = dc1394_feature_set_value(camera, DC1394_FEATURE_EXPOSURE, 62);

	err = dc1394_feature_set_mode(camera, DC1394_FEATURE_SHUTTER, DC1394_FEATURE_MODE_MANUAL);
	err = dc1394_feature_set_value(camera, DC1394_FEATURE_SHUTTER, 400);
	
	err = dc1394_feature_set_mode(camera, DC1394_FEATURE_GAIN, DC1394_FEATURE_MODE_MANUAL);
	err = dc1394_feature_set_value(camera, DC1394_FEATURE_GAIN, 50);

	//err = dc1394_feature_set_mode(camera, DC1394_FEATURE_PAN, DC1394_FEATURE_MODE_MANUAL);
	//err = dc1394_feature_set_value(camera, DC1394_FEATURE_PAN, 56);
	
	
    /*-----------------------------------------------------------------------
     *  report camera's features
     *-----------------------------------------------------------------------*/
    err=dc1394_feature_get_all(camera,&features);
    if (err!=DC1394_SUCCESS) {
        dc1394_log_warning("Could not get feature set");
    }
    else {
        dc1394_feature_print_all(&features, stdout);
    }
	
    /*-----------------------------------------------------------------------
     *  have the camera start sending us data
     *-----------------------------------------------------------------------*/
    err=dc1394_video_set_transmission(camera, DC1394_ON);
    //DC1394_ERR_CLN_RTN(err,cleanup_and_exit(camera),"Could not start camera iso transmission");
	
	//create pixel buffer
    pixels	= new unsigned char[width * height];
    
    //create texture
    if (bUseTexture) {
        tex.allocate(width,height,GL_LUMINANCE);
        memset(pixels, 0, width*height);
        tex.loadData(pixels, width, height, GL_LUMINANCE);
    }
	
}


//------------------------------------
void ofxVideoGrabberPtgrey::setUseTexture(bool bUse){
	bUseTexture = bUse;
}

//----------------------------------------------------------
void ofxVideoGrabberPtgrey::setAnchorPercent(float xPct, float yPct){
    if (bUseTexture)tex.setAnchorPercent(xPct, yPct);
}

//----------------------------------------------------------
void ofxVideoGrabberPtgrey::setAnchorPoint(int x, int y){
    if (bUseTexture)tex.setAnchorPoint(x, y);
}

//----------------------------------------------------------
void ofxVideoGrabberPtgrey::resetAnchor(){
   	if (bUseTexture)tex.resetAnchor();
}

//------------------------------------
void ofxVideoGrabberPtgrey::draw(float _x, float _y, float _w, float _h){
	if(bUseTexture) {
		tex.draw(_x, _y, _w, _h);
	}
}

//------------------------------------
void ofxVideoGrabberPtgrey::draw(float _x, float _y){
	draw(_x, _y, (float)width, (float)height);
}


//----------------------------------------------------------
float ofxVideoGrabberPtgrey::getHeight(){
	return (float)height;
}

//----------------------------------------------------------
float ofxVideoGrabberPtgrey::getWidth(){
	return (float)width;
}


