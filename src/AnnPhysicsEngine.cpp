#include "AnnPhysicsEngine.hpp"

using namespace Annwvyn;

AnnPhysicsEngine::AnnPhysicsEngine(Ogre::SceneNode* rootNode)
{    
	AnnEngine::log("Init Bullet physics");

	//Initialize the Bullet world
	m_Broadphase = new btDbvtBroadphase();
	m_CollisionConfiguration = new btDefaultCollisionConfiguration();
	m_Dispatcher = new btCollisionDispatcher(m_CollisionConfiguration);
	m_Solver = new btSequentialImpulseConstraintSolver();
	m_ghostPairCallback = new btGhostPairCallback();

	m_DynamicsWorld = new btDiscreteDynamicsWorld(m_Dispatcher, m_Broadphase, m_Solver, m_CollisionConfiguration);

	AnnEngine::log("Gravity vector = (0,-9.81f,0)");
	m_DynamicsWorld->setGravity(btVector3(0,-9.81f,0));
	m_DynamicsWorld->getPairCache()->setInternalGhostPairCallback(m_ghostPairCallback);

	debugPhysics = false;//by default
	m_debugDrawer = new BtOgre::DebugDrawer(rootNode, m_DynamicsWorld);
	m_DynamicsWorld->setDebugDrawer(m_debugDrawer);
}

AnnPhysicsEngine::~AnnPhysicsEngine()
{
	//Bullet
	delete m_DynamicsWorld;
	delete m_Broadphase;
	delete m_CollisionConfiguration;
	delete m_Dispatcher;
	delete m_Solver;
	delete m_debugDrawer;
}

void AnnPhysicsEngine::addPlayerPhysicalBodyToDynamicsWorld(AnnPlayer* player)
{
	assert(player->getBody());

	float height(player->getEyesHeight());

	//TODO define name for the bullet's collision masks
	m_DynamicsWorld->addRigidBody(player->getBody(), BIT(0), BIT(1));
}

void AnnPhysicsEngine::createPlayerPhysicalVirtualBody(AnnPlayer* player, Ogre::SceneNode* node)
{
	//Player need to have a shape (capsule)
	assert(player->getShape());

	//Create a rigid body state through BtOgre
	BtOgre::RigidBodyState *state = new BtOgre::RigidBodyState(node);

	//Get inertia vector
	btVector3 inertia;
	player->getShape()->calculateLocalInertia(player->getMass(), inertia);

	//Set the body to the player
	player->setBody(new btRigidBody(
		player->getMass(), 
		state,
		player->getShape(), 
		inertia));	
}

void AnnPhysicsEngine::createVirtualBodyShape(AnnPlayer* player)
{
	assert(player);
	float radius(0.25f);
	player->setShape(new btCapsuleShape(radius, player->getEyesHeight() - 2 * radius));
}

btDiscreteDynamicsWorld* AnnPhysicsEngine::getWorld()
{
	return m_DynamicsWorld;
}

void AnnPhysicsEngine::step(float delta)
{

	m_DynamicsWorld->stepSimulation(delta,2);
	
}

void AnnPhysicsEngine::stepDebugDrawer()
{	
	if(debugPhysics)
		m_debugDrawer->step();
}

void AnnPhysicsEngine::processCollisionTesting(AnnGameObjectVect& objects)
{
	//TODO make a typedeff for getting off the uglyness here 
	std::vector<struct collisionTest*> pairs;

	//get all collision mask
	for(size_t i = 0; i < objects.size(); i++)
	{
		std::vector<struct collisionTest*> onThisObject = objects[i]->getCollisionMask();

		for(size_t j = 0; j < onThisObject.size(); j++)
			pairs.push_back(onThisObject[j]);
	}

	//process for each maniflod
	int numManifolds = m_Dispatcher->getNumManifolds();
	//m is manifold identifier
	for (int m(0); m <numManifolds; m++)
	{
		btPersistentManifold* contactManifold =
			m_DynamicsWorld->getDispatcher()->getManifoldByIndexInternal(m);

		const btCollisionObject* obA = (btCollisionObject*) contactManifold->getBody0();
		const btCollisionObject* obB = (btCollisionObject*) contactManifold->getBody1();

		//Just get the address of the collision objects
		void* pair1 = (void*) obA;
		void* pair2 = (void*) obB;

		//for each known pair on the collision feedback system
		for(size_t p = 0; p < pairs.size(); p++)
		{
			//Get the bodies from the manifold
			void* body1 = (void*) pairs[p]->Object->getBody();
			void* body2 = (void*) pairs[p]->Receiver->getBody();

			//If there is a collision in either way
			if((pair1 == body1 && pair2 == body2) ||
				(pair2 == body1 && pair1 == body2))
			{
				if(contactManifold->getNumContacts() > 0)
					pairs[p]->collisionState = true;
				else
					pairs[p]->collisionState = false;
				break;
			}
		}
	}
}

void AnnPhysicsEngine::removeRigidBody(btRigidBody* body)
{
	m_DynamicsWorld->removeRigidBody(body);
}

void AnnPhysicsEngine::initPlayerPhysics(AnnPlayer* player, Ogre::SceneNode* node)
{
	createVirtualBodyShape(player);
	createPlayerPhysicalVirtualBody(player, node);
	addPlayerPhysicalBodyToDynamicsWorld(player);
}

void AnnPhysicsEngine::setDebugPhysics(bool state)
{
	debugPhysics = state;
}

void AnnPhysicsEngine::processTriggersContacts(AnnPlayer* player, AnnTriggerObjectVect& triggers)
{
	for(size_t i = 0; i < triggers.size(); i++)
	{
		if(Tools::Geometry::distance(player->getPosition(),
			triggers[i]->getPosition()) <= triggers[i]->getThreshold())
		{
			triggers[i]->setContactInformation(true);
			triggers[i]->atContact();
		}
		else
		{
			triggers[i]->setContactInformation(false);
		}
	}
}