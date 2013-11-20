//
//  Emitter.h
//  ParticleEngine
//
//  Created by Anthony Scavarelli on 2013-01-20.
//  http://www.anthony-scavarelli.com
//  http://www.luminartists.ca
//

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/MayaCamUI.h"

#include "Emitter.h"
#include "Particle.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ParticleEngineApp : public AppBasic
{
    public:
    Emitter<Particle> mParticles1;
	Emitter<Particle> mParticles2;
	Emitter<Particle> mParticles3;
    
    //debug
    params::InterfaceGl mParams;
    bool                mShowParams;
    float               mCurrFramerate;
	MayaCamUI			mMayaCam;
	int					mNumLiveParticles;
    
	gl::Texture			mGlowRedTex;
	gl::Texture			mGlowBlueTex;

    public:
    void prepareSettings(Settings *settings);
	void setup();
	void mouseDown( MouseEvent event );	
	void mouseDrag( MouseEvent event );
	void update();
	void draw();
};

void ParticleEngineApp::prepareSettings(Settings *settings)
{
    settings->setFrameRate(120.0f);
    settings->setWindowSize(1024, 768);
}

void ParticleEngineApp::setup()
{
	gl::disableVerticalSync();
	glEnable(GL_TEXTURE_2D);

	CameraPersp initialCam;
	initialCam.setPerspective( 45.0f, getWindowAspectRatio(), 0.1, 10000 );
	initialCam.lookAt(Vec3f(0.0f,0.0f,1000.0f), Vec3f(0.0f,0.0f,0.0f), Vec3f(0,1,0));
	initialCam.setCenterOfInterestPoint( Vec3f(0.0f,0.0f,0.0f) );
	mMayaCam.setCurrentCam( initialCam );

	//red fire
    EmitterFormat format1;
	format1.emitterPosition = Vec3f( 0.0f, -62.0f, 0.0f );
    format1.particlePerlinType = EmitterFormat::PERLIN_TYPE_SYNCHRONIZED;
    format1.particlesPerSecond = 300;
	format1.particleSize = 40.0f;
    format1.particleGravity = Vec3f(-0.01f, -0.03f, 0.0f) * 1.2f;
    format1.particleSpeed = 0.5f;
    format1.particleLifespanSeconds = 5.0f;
    format1.particleTurbulance = 2.7f;
    mParticles1.setup(format1);

	//blue flame
	EmitterFormat format2;
	format2.emitterPosition = Vec3f( 10.0f, -33.0f, 0.0f );
    format2.particlesPerSecond = 300;
	format2.particleSize = 60.0f;
    format2.particleGravity = Vec3f(0.01f, 0.03f, 0.0f) * 1.2f;
    format2.particleSpeed = 0.5f;
    format2.particleLifespanSeconds = 5.0f;
    format2.particleTurbulance = 0.3f;
    mParticles2.setup(format2);

	//cinders
	EmitterFormat format3;
	format3.emitterPosition = Vec3f( 0.0f, -50.0f, 0.0f );
    format3.particleRenderType = EmitterFormat::PARTICLE_RENDER_TYPE_QUAD;
    format3.particlesPerSecond = 75;
	format3.particleSize = 30.0f;
    format3.particleGravity = Vec3f(-0.01f, -0.03f, 0.0f) * 2.2f;
    format3.particleSpeed = 2.0f;
    format3.particleLifespanSeconds = 3.0f;
    format3.particleTurbulance = 0.5f;
    mParticles3.setup(format3);

	mGlowRedTex		= gl::Texture( loadImage( loadResource( RES_GLOW_RED_TEX ) ) );
	mGlowBlueTex	= gl::Texture( loadImage( loadResource( RES_GLOW_BLUE_TEX ) ) );
    
	//debug
    mCurrFramerate = 0.0f;
    mShowParams = true;
	mNumLiveParticles = 0;
    mParams = params::InterfaceGl( "Particle Engine", Vec2i( 225, 600 ) );
	mParams.addParam( "Framerate", &mCurrFramerate, "", true );
	mParams.addParam( "Live Particles", &mNumLiveParticles, "", true );
	mParams.addParam( "Show/Hide Params", &mShowParams, "key=x");
	mParams.addSeparator();
	mParams.addText("Emitter Properties");
	mParams.addParam("Gravity", &mParticles1.mFormat.particleGravity );
	mParams.addParam("Particles/s", &mParticles1.mFormat.particlesPerSecond, "min=1 max=10000 step=1" );
	mParams.addParam("Max Particles", &mParticles1.mMaxParticles, "min=1 max=30000 step=1" );
	mParams.addParam("Turbulance", &mParticles1.mFormat.particleTurbulance, "min=0 max=50 step=0.1");
	mParams.addSeparator();
	mParams.addText("Particle Properties");
	mParams.addParam("Color", &mParticles1.mFormat.particleColorA);
    mParams.addParam("Speed", &mParticles1.mFormat.particleSpeed, "min=0 max=100 step=0.1");
    mParams.addParam("Lifespan", &mParticles1.mFormat.particleLifespanSeconds, "min=0 max=10 step=0.1");
    mParams.addParam("Size", &mParticles1.mFormat.particleSize, "min=0 max=100 step=0.1");
    mParams.addParam("Decay", &mParticles1.mFormat.particleDecay, "min=0.90 max=0.99 step=0.01");
}

void ParticleEngineApp::mouseDown( MouseEvent event )
{
	if( event.isAltDown() ) {
		mMayaCam.mouseDown( event.getPos() );
	}
}

void ParticleEngineApp::mouseDrag( MouseEvent event )
{
	if( event.isAltDown() ) {
		mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
	}
}

void ParticleEngineApp::update()
{
    //debug
    mCurrFramerate = getAverageFps();
	mNumLiveParticles = mParticles1.getNumLiveParticles()
                            + mParticles2.getNumLiveParticles()
                            + mParticles3.getNumLiveParticles()
                            + mParticles3.getNumLiveParticles();
}

void ParticleEngineApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    
	gl::setMatrices( mMayaCam.getCamera() );

	//red flame
	gl::enableAdditiveBlending();
	mGlowRedTex.bind();
    mParticles1.render(mMayaCam.getCamera());
    mParticles3.render(mMayaCam.getCamera());
    mGlowRedTex.unbind();
    
	//blue flame
	mGlowBlueTex.bind();
    mParticles2.render(mMayaCam.getCamera());
	mGlowBlueTex.unbind();

    //debug
    if (mShowParams) {
		mParams.draw();
    }
}

CINDER_APP_BASIC( ParticleEngineApp, RendererGl )
