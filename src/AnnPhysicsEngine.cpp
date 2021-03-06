#include "stdafx.h"
#include "AnnPhysicsEngine.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"
using namespace Annwvyn;

AnnPhysicsEngine::AnnPhysicsEngine(Ogre::SceneNode * rootNode,
								   std::shared_ptr<AnnPlayer> player,
								   AnnGameObjectList & objects,
								   AnnTriggerObjectList & triggers) : AnnSubSystem("PhysicsEngie"),
	Broadphase(nullptr),
	CollisionConfiguration(nullptr),
	Solver(nullptr),
	DynamicsWorld(nullptr),
	debugDrawer(nullptr),
	playerRigidBodyState(nullptr),
	gameObjects(objects),
	triggerObjects(triggers),
	playerObject(player),
	defaultGravity(0, -9.81f, 0)
{
	//Initialize the Bullet world
	Broadphase = std::make_unique<btDbvtBroadphase>();
	CollisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
	Dispatcher = std::make_unique<btCollisionDispatcher>(CollisionConfiguration.get());
	Solver = std::make_unique<btSequentialImpulseConstraintSolver>();
	DynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(Dispatcher.get(), Broadphase.get(), Solver.get(), CollisionConfiguration.get());
	AnnDebug() << "btDiscreteDynamicsWorld instantiated";

	//Set gravity vector
	DynamicsWorld->setGravity(defaultGravity.getBtVector());
	AnnDebug() << "Gravity vector " << defaultGravity;

	debugPhysics = false;//by default
	debugDrawer = new BtOgre::DebugDrawer(rootNode, DynamicsWorld.get());
	DynamicsWorld->setDebugDrawer(debugDrawer);
}

AnnPhysicsEngine::~AnnPhysicsEngine()
{
}

void AnnPhysicsEngine::addPlayerPhysicalBodyToDynamicsWorld()
{
	// TODO define name for the bullet's collision masks
	DynamicsWorld->addRigidBody(playerObject->getBody(), MASK(0), MASK(1));
}

void AnnPhysicsEngine::createPlayerPhysicalVirtualBody(Ogre::SceneNode* node)
{
	AnnDebug() << "createPlayerPhysicalVirtualBody";

	//Create (new) a rigid body state through BtOgre
	if (playerRigidBodyState) delete playerRigidBodyState;
	playerRigidBodyState = new BtOgre::RigidBodyState(node);

	//Get inertia vector
	btVector3 inertia;
	playerObject->getShape()->calculateLocalInertia(playerObject->getMass(), inertia);

	//Set the body to the player
	auto body = new btRigidBody(
		playerObject->getMass(),
		playerRigidBodyState,
		playerObject->getShape(),
		inertia);
	playerObject->setBody(body);
}

void AnnPhysicsEngine::createVirtualBodyShape(float radius)
{
	//remove the diameter of the two half sphere on top and bottom of the capsule
	playerObject->setShape(new btCapsuleShape(radius, playerObject->getEyesHeight() - 2 * radius));
}

btDiscreteDynamicsWorld* AnnPhysicsEngine::getWorld()
{
	return DynamicsWorld.get();
}

void AnnPhysicsEngine::step(float delta)
{
	DynamicsWorld->stepSimulation(delta, 10, 1.0f / 240.0f);
}

void AnnPhysicsEngine::stepDebugDrawer()
{
	if (debugPhysics)
		debugDrawer->step();
}

void AnnPhysicsEngine::processCollisionTesting()
{
	auto nbManifold = DynamicsWorld->getDispatcher()->getNumManifolds();
	for (auto i{ 0 }; i < nbManifold; ++i)
	{
		auto contactManifold = DynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		if (contactManifold->getNumContacts() > 0)
			AnnGetEventManager()->detectedCollision(contactManifold->getBody0()->getUserPointer(),
													contactManifold->getBody1()->getUserPointer());
	}
}

void AnnPhysicsEngine::removeRigidBody(btRigidBody* body)
{
	AnnDebug() << "Removing " << body << " Form physics simulation";
	if (body)
		DynamicsWorld->removeRigidBody(body);
}

void AnnPhysicsEngine::setDebugPhysics(bool state)
{
	debugPhysics = state;
	debugDrawer->setDebugMode(int(state));
	debugDrawer->step();
}

void AnnPhysicsEngine::processTriggersContacts()
{
	for (auto trigger : triggerObjects)
	{
		trigger->setContactInformation(trigger->computeVolumetricTest(playerObject));
		if (!trigger->lastFrameContactWithPlayer && trigger->contactWithPlayer ||
			trigger->lastFrameContactWithPlayer && !trigger->contactWithPlayer)
			AnnGetEventManager()->spatialTrigger(trigger);
	}
}

void AnnPhysicsEngine::changeGravity(AnnVect3 gravity)
{
	DynamicsWorld->setGravity(gravity.getBtVector());
}

void AnnPhysicsEngine::resetGravity()
{
	changeGravity(defaultGravity);
}

void AnnPhysicsEngine::update()
{
	processTriggersContacts();
	stepDebugDrawer();
	step(AnnGetEngine()->getFrameTime());
	processCollisionTesting();
}

void AnnPhysicsEngine::toggleDebugPhysics()
{
	setDebugPhysics(!debugPhysics);
}

void AnnPhysicsEngine::initPlayerRoomscalePhysics(Ogre::SceneNode* playerAnchorNode)
{
	playerObject->setMode(ROOMSCALE);
	AnnDebug() << "Initializing player's physics in roomscale mode";
	AnnDebug() << "Player can walk around";
	playerObject->setRoomRefNode(playerAnchorNode);
}

void AnnPhysicsEngine::initPlayerStandingPhysics(Ogre::SceneNode* node)
{
	playerObject->setMode(STANDING);
	AnnDebug() << "Initializing player's physics  in standing mode";
	AnnDebug() << "Capsule rigidbody : " << playerObject->getMass() << "Kg" << playerObject->getEyesHeight();
	createVirtualBodyShape();
	createPlayerPhysicalVirtualBody(node);
	addPlayerPhysicalBodyToDynamicsWorld();
}