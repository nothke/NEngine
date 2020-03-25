#pragma once
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

class Physics
{
public:
	btDefaultCollisionConfiguration*		collisionConfiguration;
	btCollisionDispatcher*					dispatcher;
	btBroadphaseInterface*					overlappingPairCache;
	btSequentialImpulseConstraintSolver*	solver;
	btDiscreteDynamicsWorld*				dynamicsWorld;
	// recommended by bullet instead of vector:
	btAlignedObjectArray<btCollisionShape*>	collisionShapes;

	Physics()
	{
		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher(collisionConfiguration);
		overlappingPairCache = new btDbvtBroadphase();
		solver = new btSequentialImpulseConstraintSolver();
		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,
			overlappingPairCache, solver, collisionConfiguration);

		dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
	}

	void Step(float dt)
	{
		dynamicsWorld->stepSimulation(dt, 10);
	}

	~Physics()
	{
		//remove the rigidbodies from the dynamics world and delete them
		for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body && body->getMotionState())
			{
				delete body->getMotionState();
			}
			dynamicsWorld->removeCollisionObject(obj);
			delete obj;
		}

		//delete collision shapes
		for (int j = 0; j < collisionShapes.size(); j++)
		{
			btCollisionShape* shape = collisionShapes[j];
			collisionShapes[j] = 0;
			delete shape;
		}

		delete dynamicsWorld;
		delete solver;
		delete overlappingPairCache;
		delete dispatcher;
		delete collisionConfiguration;
	}
};