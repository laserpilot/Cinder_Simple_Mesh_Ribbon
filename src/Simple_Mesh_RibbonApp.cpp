#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Simple_Mesh_RibbonApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void Simple_Mesh_RibbonApp::setup()
{
}

void Simple_Mesh_RibbonApp::mouseDown( MouseEvent event )
{
}

void Simple_Mesh_RibbonApp::update()
{
}

void Simple_Mesh_RibbonApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( Simple_Mesh_RibbonApp, RendererGl )
