#ifndef TESTLEVEL
#define TESTLEVEL
 
#include <Annwvyn.h>
using namespace Annwvyn;

//Custom object:
class Sinbad : public AnnGameObject
{
public:
	void postInit()
	{
		setPosition(0,0,-5);
		setScale(0.2f,0.2f,0.2f);
		setAnimation("Dance");
		playAnimation(true);
		loopAnimation(true);
		setUpPhysics(40, phyShapeType::boxShape);
	}

	void atRefresh()
	{
		//AnnDebug() << "Sinbad position is : " << getPosition();
	}
};

class TestLevel : LEVEL
{
public:
	///Construct the Level : 
	void load()
	{
		//Set some ambiant light
		AnnGetSceneryManager()->setAmbiantLight(AnnColor(.6f,.6f,.6f));


		//We add our brand new 3D object
		auto MyObject = addGameObject("MyObject.mesh");
		MyObject->setPosition(5, 1, 0);//We put it 5 meters to the right, and 1 metter up...
		//MyObject->setUpPhysics(); // <---- This activate the physics for the object as static geommetry
		MyObject->setUpPhysics(100, convexShape); // <------- this activate the physics as a dynamic object. We need to tell the shape aproximation to use. and a mass in Kg

		//The shape aproximation is put at the Object CENTER POINT. The CENTER POINT should be at the object's bounding box CENTER before exporting from blender.
		
		text = std::make_shared<Ann3DTextPlane>(1.0f, 0.5f, 96.f / 0.0254f, "Hello, Virtual World!\nthis is one line only this is one line only this is one line only this is one line only");
		text->setTextAlign(text->ALIGN_LEFT);
		text->setPosition({ 0, 0.5f, 0 });

		//Add other source of light
		auto Sun = addLightObject();
		Sun->setType(AnnLightObject::ANN_LIGHT_DIRECTIONAL);
		Sun->setDirection(AnnVect3::NEGATIVE_UNIT_Y + 1.5f* AnnVect3::NEGATIVE_UNIT_Z);

		//Create objects and register them as content of the level
		auto S = AnnGetGameObjectManager()->createGameObject("Sinbad.mesh", std::make_shared<Sinbad>());
		levelContent.push_back(S);
		S->playSound("media/monster.wav", true, 1);

		//Add water
		auto Water = addGameObject("environement/Water.mesh");	

		//Add the island
		auto Island = addGameObject("environement/Island.mesh");
		Island->setUpPhysics();

		//Add the sign
		auto Sign(addGameObject("environement/Sign.mesh"));
		Sign->setPosition(1,-0,-2);
		Sign->setUpPhysics(0, phyShapeType::staticShape);
		Sign->setOrientation(Ogre::Quaternion(Ogre::Degree(-45), Ogre::Vector3::UNIT_Y));

		auto t(AnnGetGameObjectManager()->createTriggerObject(std::make_shared<AnnAlignedBoxTriggerObject>()));
		dynamic_cast<AnnAlignedBoxTriggerObject*>(t.get())->setBoundaries(-1, 1, -1, 1, -1, 1);
		levelTrigger.push_back(t);

		//Put some music here
		AnnGetAudioEngine()->playBGM("media/bgm/bensound-happyrock.ogg", 0.4);

		//Place the starting point 
		AnnGetPlayer()->setPosition(AnnVect3::ZERO);
		AnnGetPlayer()->setOrientation(Ogre::Euler(0));
		AnnGetPlayer()->resetPlayerPhysics();
	}

	void unload()
	{
		text.reset();
		AnnLevel::unload();
	}

	void runLogic()
	{
		//AnnDebug() << "Player position is : " << AnnGetPlayer()->getPosition();
	}

private:

	std::shared_ptr<Ann3DTextPlane> text;
};

class PhysicsDebugLevel : LEVEL
{
	void load()
	{
		AnnGetSceneryManager()->setWorldBackgroundColor(AnnColor(0, 0, 0));
		AnnGetPhysicsEngine()->getWorld()->setGravity(btVector3(0, 0, 0));
		AnnGetPlayer()->setPosition(AnnVect3::ZERO);
		AnnGetPlayer()->resetPlayerPhysics();
	}

	void runLogic()
	{
		AnnDebug() << "Player position is : " << AnnGetPlayer()->getPosition();
	}
};

#endif //TESTLEVEL