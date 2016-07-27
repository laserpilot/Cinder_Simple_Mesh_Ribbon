#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/CinderMath.h"
#include "cinder/Perlin.h"
#include "cinder/gl/Shader.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Simple_Mesh_RibbonApp : public App {
  public:
    void setup() override;
    static void prepareSettings (App::Settings * settings) {
        
        settings->setHighDensityDisplayEnabled();
        settings->setWindowSize(1280, 720);
    };
    void mouseDown( MouseEvent event ) override;
    void mouseUp( MouseEvent event ) override;
    void mouseDrag(MouseEvent event) override;
    void keyDown(KeyEvent event) override;
    void update() override;
    void draw() override;
    
    void drawRibbon();
    
    bool mEditing;
    vector<vec3> mPoints;
    vector<gl::VboMeshRef> mCurves;
    
    float noiseAccum;
    float noiseSpeed;
    
    Perlin mPerlin;
    
    bool autoDraw;
};

void Simple_Mesh_RibbonApp::setup()
{
    noiseAccum = 0;
    noiseSpeed = 0.04;
    autoDraw = true;
}

void Simple_Mesh_RibbonApp::update()
{
    if(autoDraw){
        
        vec3 tempPt;
        tempPt.x =lmap<float>(mPerlin.noise(noiseAccum),-0.5f,0.5f, 0.0f,getWindowWidth());
        tempPt.y =lmap<float>(mPerlin.noise(noiseAccum+4000),-0.5f,0.5f, 0.0f,getWindowHeight());
        tempPt.z =lmap<float>(mPerlin.noise(noiseAccum+700),-0.5f,0.5f, 0.0f,getWindowHeight());
        
        mPoints.push_back(tempPt);
        
        if(mPoints.size()>300){
            mPoints.erase(mPoints.begin(), mPoints.begin()+1);
        }
        
    }
    
    
    noiseAccum += noiseSpeed;
    
    int i = 0;
    for(auto &pt : mPoints){
        
        pt.x = pt.x + 3 * mPerlin.noise(noiseAccum + i*0.06);
        pt.y = pt.y + 3 * mPerlin.noise(noiseAccum + i*0.03);
        pt.z = pt.z + 2 * mPerlin.noise(noiseAccum + i*0.045);
        
        i++;
    }
}

void Simple_Mesh_RibbonApp::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
    gl::enableAlphaBlending();
    gl::enableAdditiveBlending();

    

    gl::begin(GL_LINES);
    drawRibbon();
    gl::end();
    
    //THis isn't necessary yet - still trying to figure out what is happening here
//    auto lambert = gl::ShaderDef().lambert().color();
//    auto shader = gl::getStockShader(lambert);
//    shader->bind();
    
    gl::begin(GL_TRIANGLE_STRIP);
    drawRibbon();
    gl::end();
    
    gl::drawStringCentered(ci::toString(getAverageFps()), vec2(40,20));
    
    
}

void Simple_Mesh_RibbonApp::drawRibbon(){
    vec3 tempPt = vec3(0,0,0);
    vec3 a,b,tangent, normal, mappedA, mappedB;
    
    int i = 0;
    for(auto pt : mPoints){
        
        a = pt;
        b = tempPt;
        tangent = b-a;
        float rotateAngle = 90;
        //float rotateAngle=lmap((float)i, 0.0f, (float)mPoints.size(),0.0f,90.0f);
        //rotateAngle = fmodf((float)timeMan.getSeconds()*5+lmap((float)i, 0.0f, (float)mPoints.size(),0.0f,360.0f),360.0f);

        normal = glm::rotate(tangent, glm::radians(rotateAngle), glm::normalize(tangent));
        //The other option here is to just set the rotation matrix to Z only, and this will work better when doing 2D drawing - such as:
        //normal = glm::rotate(tangent, glm::radians(rotateAngle), vec3(0,0,1));
        normal = glm::normalize(normal);
        
        pt.x = pt.x + lmap<int>(i,0,mPoints.size(), 30,0) * mPerlin.noise(noiseAccum + i*lmap<float>(i,0.0f,mPoints.size(), 0.03f,0.0f));
        pt.y = pt.y + lmap<int>(i,0,mPoints.size(), 30,0)  * mPerlin.noise(noiseAccum + i*lmap<float>(i,0.0f,mPoints.size(), 0.06f,0.0f));
        
        
        if(i>1){
            vec3 corner;
            
            float rotateHue =lmap<float>(i, 0.0f, mPoints.size(),0.0f,1.0f);
            Color tempColor = Color(1.0,0.85,1.0);
            //tempColor = Color( CM_HSV, rotateHue, 1, 1 );
            float thickness =lmap<float>(i, 0.0f,mPoints.size(),0.0f,80.0f);
            //thickness = thickness*(300 * mPerlin.noise(noiseAccum + i*lmap((float)i,0.0f,(float)mPoints.size(), 0.0f,0.02f)));
            corner.x = a.x + normal.x*thickness;
            corner.y = a.y + normal.y*thickness;
            gl::color(tempColor.r, tempColor.g, tempColor.b, lmap<float>(i, 0.0f,mPoints.size(),0.0f,1.0f));
            gl::vertex(corner);
            //rotate back the other way
            //if you rotate this around the normalized tangent as well, you won't see anything
            normal = glm::rotate(tangent, glm::radians(-rotateAngle), vec3(0,0,1));
            normal = glm::normalize(normal);
            corner.x = a.x + normal.x*thickness;
            corner.y = a.y + normal.y*thickness;
            gl::color(tempColor.r, tempColor.g, tempColor.b, lmap<float>(i, 0.0f,mPoints.size(),0.0f,1.0f));
            gl::vertex(corner);
        }
        tempPt = vec3(pt.x, pt.y, 0);
        i++;
    }

}

void Simple_Mesh_RibbonApp::mouseDown( MouseEvent event )
{
    autoDraw = false;
    
    mPoints.push_back(vec3(event.getPos().x, event.getPos().y, 0) );
    
    if(mPoints.size()>300){
        mPoints.erase(mPoints.begin(), mPoints.begin()+1);
    }
}

void Simple_Mesh_RibbonApp::mouseUp( MouseEvent event )
{
    autoDraw = true;
}

void Simple_Mesh_RibbonApp::mouseDrag( MouseEvent event )
{
    mPoints.push_back(vec3(event.getPos().x, event.getPos().y, 0) );
    
    if(mPoints.size()>300){
        mPoints.erase(mPoints.begin(), mPoints.begin()+1);
    }
}
void Simple_Mesh_RibbonApp::keyDown(KeyEvent event){
    
    switch( event.getCode() ) {
        case KeyEvent::KEY_ESCAPE:
            quit();
            break;
        case KeyEvent::KEY_f:
            setFullScreen( !isFullScreen() );
            break;
        case KeyEvent::KEY_c:
            mPoints.clear();
            break;
    }
}

//Compare without the renderer options for msaa 16 - it gets much more pixellated
CINDER_APP( Simple_Mesh_RibbonApp, RendererGl( RendererGl::Options().msaa( 16 ) ), Simple_Mesh_RibbonApp::prepareSettings  )

