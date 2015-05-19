#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Capture.h"

#define NORM 0
#define CONFIG 1
using namespace ci;
using namespace ci::app;
using namespace std;

class projections_balletApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );
  void keyDown(KeyEvent event);
	void update();
	void draw();
  int mode = 0;

  
  CaptureRef			mCapture;
  gl::TextureRef		mTexture;
  vector<Vec2i>   window_coords;
};

void projections_balletApp::setup(){
  
  // print the devices
  for( auto device = Capture::getDevices().begin(); device != Capture::getDevices().end(); ++device ) {
    console() << "Device: " << (*device)->getName() << " " << std::endl;
    
    try {
      mCapture = Capture::create( 640, 480 );
      mCapture->start();
    }
    catch( ... ) {
      console() << "Failed to initialize capture" << std::endl;
    }
  }
  
}

void projections_balletApp::keyDown( KeyEvent event )
{
  if( event.getChar() == 'f' )
    setFullScreen( ! isFullScreen() );
  else if( event.getChar() == ' ' )
    ( mCapture && mCapture->isCapturing() ) ? mCapture->stop() : mCapture->start();
  else if(event.getChar() == 'c'){
    mode = CONFIG;
    window_coords.clear();
  }
}

void projections_balletApp::mouseDown( MouseEvent event ){
  if(mode==CONFIG){
    
    Vec2i mMouseLoc = event.getPos();
    window_coords.push_back(mMouseLoc);
    console()<< mMouseLoc;
    
    //exit config mode once you have four vertices
    if(window_coords.size()>=2){
      mode=NORM;
    }
  }

  
}

void projections_balletApp::update(){
  
  if( mCapture && mCapture->checkNewFrame() ) {
    mTexture = gl::Texture::create( mCapture->getSurface() );
  }
  
}

void projections_balletApp::draw(){
  gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
  gl::setMatricesWindow( getWindowWidth(), getWindowHeight() );
  
  if( mTexture ) {
    glPushMatrix();
//#if defined( CINDER_COCOA_TOUCH )
//    //change iphone to landscape orientation
//    gl::rotate( 90.0f );
//    gl::translate( 0.0f, -getWindowWidth() );
//    
//    Rectf flippedBounds( 0.0f, 0.0f, getWindowHeight(), getWindowWidth() );
//    gl::draw( mTexture, flippedBounds );
//#else
    if(window_coords.size()>1){
      Rectf rect = Rectf(0,0, window_coords[1].x-window_coords[0].x, window_coords[1].y-window_coords[0].y);
      gl::draw( mTexture, Area(window_coords[0], window_coords[1]), Rectf(0,0,getWindowWidth(), getWindowHeight()));
    }
    else
      gl::draw(mTexture);
    gl::drawStringCentered(to_string(mode), Vec2i(getWindowWidth()/2, 50));
    if(mode==CONFIG){
      for(int i = 0; i<window_coords.size(); i++){
        gl::drawSolidCircle(window_coords[i], 5);
      }
    }
//#endif
    glPopMatrix();
  }

}

CINDER_APP_NATIVE( projections_balletApp, RendererGl )
