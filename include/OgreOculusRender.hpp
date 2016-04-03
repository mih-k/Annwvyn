/**
 * \file OgreOculusRenderer.hpp
 * \brief Initialize rendering for the rift with SDK post traitement (OpenGL ONLY)
 * \author A. Brainville (Ybalrid)
 */

///huge thanks to Germanunkol (aka ueczz on Oculus Forums) https://github.com/Germanunkol/OgreOculusSample
///(even if now I'm not using code from him anymore)
///Shout out to Kojack too for his post of an OgreOculus class a short time after DK1 was out.

#ifndef OGRE_OCULUS_RENDERER
#define OGRE_OCULUS_RENDERER

//Oculus Rift Lib
//#include <OVR.h>
#include <OVR_CAPI_GL.h>

//C++ SDL Includes
#include <iostream>
#include <sstream>

//Ogre
#include <Ogre.h>

//Accessing Oculus Rift through a class : 
#include "OculusInterface.hpp"

//OS Specific build macro 
#include "systemMacro.h"
#ifdef _WIN32
#include <Windows.h>
#include <glew.h>
#elif __gnu_linux__ 
#include <unistd.h>
#endif

#include "AnnErrorCode.hpp"

using namespace std;
//using namespace OVR;

///A pose refer to the combinaison of a position and an orientation. 
///It permit to define the placement of an object with 6DOF
struct OgrePose
{
	///A 3D vector representing a position
	Ogre::Vector3 position;
	///A quaternion representiong an orientation
	Ogre::Quaternion orientation;
};

///Do the initialization and graphical rendering for the Oculus Rift using Ogre
class DLL OgreOculusRender
{
    public:
		
		///Construct the OgreOculusRender. Can only be called once
        OgreOculusRender(std::string windowName = "OgreOculusRender");

		///Class destructor
        ~OgreOculusRender();

		///Cycle through all Oculus Performance HUD available
		void cycleOculusHUD();
		
		///Get the timing and tracking state form the oculus runtime and moves the cameras according to the reference and the tracked data
		void updateTracking();

		///Render each texture buffer, copy contne to the oculus swap texture, get mirror texture data to the OgreMirror texture and update debug window
		void renderAndSubmitFrame();

		///Set the near Z clipping plane distance from the POV. Used to calculate Projections matricies
		void setCamerasNearClippingDistance(float distance);

		///Automatic initialization of the renderer.
        void initialize();

		///Start Oculus and Ogre libraries.
        void initLibraries(std::string = "Ogre.log");

		///Load the given 'resource.cfg' file. See Ogre Help for referene here.
        void loadReseourceFile(const char path[]);

		//Init all resource groups loaded on resource group manager. 
        void initAllResources();
        
		///Get Configuration from ogre.cfg or display a Dialog. The Resolution and FullScreen settings will be ignored. 
		void getOgreConfig();
        
		///Create the RenderWindow
        void createWindow();

		///Initialize the SceneManager for the aplication. 
        void initScene();

		///Initialise the camera for Stereo Render
        void initCameras();

		///Initialise the RTT Rendering System. Create two textures, two viewports and bind cameras on them.
        void initRttRendering();
        
		///Init the Rift rendering. Configure Oculus SDK to use the two RTT textures created.
		void initOculus();

		///Get the scene manager.
        Ogre::SceneManager* getSceneManager();

		///Get the RenderWindow
        Ogre::RenderWindow* getWindow();

		///Print various informations about the cameras
        void debugPrint();

		///Get a node representing the camera. NOTE: Camera isn"t attached.
        Ogre::SceneNode* getCameraInformationNode();

		///Get the timer
        Ogre::Timer* getTimer();

		///Get time between frames
        double getUpdateTime();

		///Recenter rift to default position.
		void recenter();

		///Compute from OVR the correct projection matrix for the given clipping distance
		void calculateProjectionMatrix();

		///change main viewport background color
		void changeViewportBackgroundColor(Ogre::ColourValue color);

		///Show in debug window what the camera are seeing
		static void showRawView();

		///Show in the debug window what the Oculus service send as mirrored view
		static void showMirrorView();

		///Show in the debug window a monoscopic view with a default Fov of 90�
		static void showMonscopicView();

    private://Methods
		///Set the Fov for the monoscopic view
		void setMonoFov(float degreeFov);

	private://Attributes
		///Instance for the private singleton
		static OgreOculusRender* self;

		///Everything that is an array of 2 will use theses indexes in code. 
        enum 
        {
            left = 0,
            right = 1,
        };

		///Save content of the RenderTexture to the specified file. Please use a valid extentsion of a format handeled by FreeImage
		///Note that this is SLOW AS HELL!
        void debugSaveToFile(const char path[]);
		
		///Object for getting informations from the Oculus Rift
        OculusInterface* Oculus; 

		///background color of viewports
		Ogre::ColourValue backgroundColor;

        ///Name of the Window
        string name; 

        ///Ogre Root instance
        Ogre::Root* root; 

		///Ogre Render Window for debuging out
        Ogre::RenderWindow* window; 

		///Ogre Scene Manager
        Ogre::SceneManager* smgr, * debugSmgr;	

		///Stereoscopic camera array. Indexes are "left" and "right" + debug view cam
        Ogre::Camera* cams[2], * debugCam, * monoCam; 
		
		///Nodes for the debug scene
		Ogre::SceneNode* debugCamNode, * debugPlaneNode; 

		///Node that store camera position/orientation
        Ogre::SceneNode* CameraNode; 

		///Vewports on textures. Textures are separated. One vieport for each textures
        Ogre::Viewport* vpts[2], *debugViewport;

		///The Z axis clipping planes distances
        Ogre::Real nearClippingDistance, farClippingDistance;

		///Position of the camera.
        Ogre::Vector3 cameraPosition;

		///Orientation of the camera.
        Ogre::Quaternion cameraOrientation;

		///Timing in seconds 
		double currentFrameDisplayTime, lastFrameDisplayTime;

		///Render descriptor for each eye. Indexes are "left" and "right"
        ovrEyeRenderDesc EyeRenderDesc[2];

		///Size of left eye texture
        ovrSizei bufferSize;

		///Mirror texture 
		ovrMirrorTexture mirrorTexture;

		///OpenGL Texture ID of the render buffers
		GLuint oculusMirrorTextureID, ogreMirrorTextureID, oculusRenderTextureID, renderTextureID;

		///Time between two frames in seconds
        double updateTime;

		///Pointer to the debug plane manual material
		Ogre::MaterialPtr DebugPlaneMaterial;

		///If true, need to copy the mirrored buffer from Oculus to Ogre
		static bool mirror;

		///Compositing layer for the rendered scene
		ovrLayerEyeFov layer;

		///GL texture set for the rendering
		ovrTextureSwapChain textureSwapChain; 

		///offset between render center and camera (for IPD variation)
		ovrVector3f offset[2];

		///Pose (position+orientation) 
		ovrPosef pose;

		///Tracking state
		ovrTrackingState ts;

		///Orientation of the headset
		ovrQuatf oculusOrient;

		///Position of the headset
		ovrVector3f oculusPos;

		///Pointer to the layer to be submited
		ovrLayerHeader* layers;

		///State of the performance HUD
		int perfHudMode;

		///Index of the current texture in the textureSwapChain
		int currentIndex;

    public:
		///Position of the rift at the last frame
        Ogre::Vector3 lastOculusPosition;

		///Orientation of the rift at the last frame
        Ogre::Quaternion lastOculusOrientation;

		///Pose to be returned
		OgrePose returnPose;

		static Ogre::TextureUnitState* debugTexturePlane;

};
#endif //OGRE_OCULUS_RENDERER
