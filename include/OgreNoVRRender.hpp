#ifndef OGRE_NO_VR_RENDER
#define OGRE_NO_VR_RENDER

#include "systemMacro.h"

#include "OgreVRRender.hpp"

///Dummy VR renderer, just render the scene to a window like a non vr game. Doesn't need special hardware or runtime to run
class DLL OgreNoVRRender : public OgreVRRender
{
public:

	///Create the no vr renderer. This will not output to a VR system but can be useful for trying stuff anyway.
	OgreNoVRRender(std::string winName = "OgreVRNoVRRender");

	///Something is buggy here.
	~OgreNoVRRender();

	///Dummy mandatory method : there's no HMD to initialize
	void initVrHmd() override;

	///Create the default scene manager
	void initScene() override;

	///There's no RenderToTexture to do here, so this attach the mono camera to the window with a viewport
	void initRttRendering() override;

	///No client HMD
	void initClientHmdRendering() override;

	///Return true if window is closed
	bool shouldQuit() override;

	///Will allays be false
	bool shouldRecenter() override;

	///Will allays be true
	bool isVisibleInHmd() override;

	///This will just apply a simple calculation with the base position and the body orientaiton + the height (supposed) of an human user standing up
	void getTrackingPoseAndVRTiming() override;

	///Update the window
	void renderAndSubmitFrame() override;

	///Will do nothing
	void recenter() override;

	///Change the clear color on the window
	void changeViewportBackgroundColor(Ogre::ColourValue) override;

	///Nothing to do, just here to not make this class abstract
	void showDebug(DebugMode mode) override;

	///This just apply the near/far clip distances to the mono camera
	void updateProjectionMatrix() override;

	///No stereo 3D so inter-pupilarry distance is useless.
	void handleIPDChange() override;

private:
	static OgreNoVRRender* noVRself;
	Ogre::Viewport* noVRViewport;
	bool running;
};

#endif //OGRE_NO_VR_RENDER