//
//  Particle.h
//  ParticleEngine
//
//  Created by Anthony Scavarelli on 2013-01-20.
//  http://www.anthony-scavarelli.com
//  http://www.luminartists.ca
//

#ifndef __FloatingCubes__PARTICLE__
#define __FloatingCubes__PARTICLE__

#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Perlin.h"
#include "cinder/Rect.h"
#include "cinder/Camera.h"

#include <vector>

using namespace ci;
using namespace ci::app;
using namespace std;

class Particle
{
public:
    //viewing properties
    ColorA          mCol;
    float           mInitAlpha;
    
    //movement properties
	Vec3f           mLoc;
    Vec3f           mPrevLoc;
	Vec3f           mVel;
	Vec3f           mAcc;
    float           mSpeed;
    
    //rotation properties
    Vec3f           mRotation;
    Vec3f           mInitRotation; //want variation in texture placements
    Vec3f           mCurrDir;
    Vec3f           mDestRot;
    
    //inertial properties
	float           mScale;
	
    //lifetime properties
	float           mBirthTime;
    float           mDecay;
	float           mLifespan;
	float			mAge;
	bool            mIsDead;
    bool            mIsInPurgatory;

	Perlin          mPerlin;
    unsigned int    mCounter;

	//billboard vectors
	Vec3f			mCamRight;
	Vec3f			mCamUp;
	Vec3f			mBillboardNormal;
    
protected:
    float           mInitScale;
    bool            mImmortal;
    
public:
    Particle(){}
    ~Particle(){}
    
    void setup(     const Vec3f       &loc,
                    const float       speed,
                    const float       scale,
                    const ColorA      &color,
                    const float       lifespan,
                    const float       decay           )
    {
        mLoc            = loc;
        mSpeed          = speed;
        mPrevLoc        = mLoc;
        mVel            = Rand::randVec3f() * mSpeed;
        mAcc			= Vec3f::zero();
        mScale          = scale;
        mInitScale      = mScale;
        mCol            = color;
        mInitAlpha      = mCol.a;
        mLifespan		= lifespan;
        mImmortal       = false;
        mDecay			= decay;
        mIsDead			= false;
        mIsInPurgatory  = false;
        mInitRotation   = Vec3f( 0.0f,0.0f, Rand::randFloat(0.0f, 360.0f) );
        mRotation       = Vec3f::zero();
        mDestRot        = Vec3f::zero();
        mCurrDir        = Vec3f::zero();
        
        mBirthTime      = getElapsedSeconds();
		mAge			= 0.0f;
        
        mPerlin = Perlin(3);
        mPerlin.setSeed(getElapsedFrames() * randInt(1000));
        mCounter = 0;

		mCamRight			= Vec3f::zero();
		mCamUp				= Vec3f::zero();
		mBillboardNormal	= Vec3f::zero();
    }
    
    void applyPerlin( const float turbulance, const Vec3f* perlinVec = NULL )
    {
        if (perlinVec == NULL) {
            Vec3f noiseVector;
            noiseVector = mPerlin.dfBm( Vec3f(0.0f, 0.0f, mCounter) * turbulance ) * turbulance * 0.1f;
            mAcc += noiseVector;
            mCounter++;
        }
        else {
            //noiseVector = perlin->dfBm( Vec3f( mLoc.x, mLoc.y, mLoc.z ) + Vec3f(0.0f, 0.0f, (float)(*globalCounter)) * turbulance ) * turbulance * 0.1f;
            mAcc += (*perlinVec);
        }
    }
    
    void update( const Vec3f &force = Vec3f(0,0,0) )
    {
		mAge = getElapsedSeconds() - mBirthTime;

        mAcc += force;
        
        mVel += mAcc;
        mPrevLoc = mLoc;
        mLoc += mVel;
        mVel *= mDecay;
        
        mScale = mInitScale * (1.0f - mAge/mLifespan);
        //mCol.a = 1.0f * (1.0f - mAge/mLifespan);
        
        if ( !mImmortal ) {
            if( mAge > mLifespan ) {
                mIsDead = true;
            }
        }
        
        mAcc.set(0.0f, 0.0f, 0.0f);
    }
    
    void render( const CameraPersp *cam, const int renderType )
    {
		cam->getBillboardVectors(&mCamRight, &mCamUp);
		mBillboardNormal = mCamRight.cross(mCamUp);

        gl::color( mCol );
        
        if(renderType == 0) {
            glPushMatrix();
            gl::translate( Vec3f(mLoc.x, mLoc.y, mLoc.z) );
            glRotatef( (360.0f * mAge/mLifespan), mBillboardNormal);
            gl::scale(mScale, mScale, mScale);
            gl::drawBillboard(Vec3f::zero(), Vec2f::one(), 90.0f, mCamRight, mCamUp);
            glPopMatrix();
        }
        else if(renderType == 1) {
            glPushMatrix();
            gl::translate( Vec3f(mLoc.x, mLoc.y, mLoc.z) );
            gl::rotate( Vec3f((360.0f * mAge/mLifespan), mInitRotation.z + (360.0f * mAge/mLifespan), mInitRotation.z + (360.0f * mAge/mLifespan) ) );
            gl::drawSolidRect(Rectf(0.0f, 0.0f, mScale, mScale));
            glPopMatrix();
        }
        else if(renderType == 2) {
            
        }
        else if(renderType == 3) {
            
        }
    }
    
    void makeImmortal( const bool decision = true )
    { 
        mImmortal = decision;
    }
};

#endif /* defined(__FloatingCubes__Particle__) */