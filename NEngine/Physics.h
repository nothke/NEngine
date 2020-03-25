#pragma once
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "Model.h"
#include "Conversion.h"

class Physics
{
	struct BodyModelPair
	{
		Model& model;
		btRigidBody* body;

		BodyModelPair(btRigidBody* body, Model& model) : body(body), model(model) {}

		void UpdateModelPosition()
		{
			auto motion = body->getMotionState();

			if (motion)
			{
				btTransform trans;
				motion->getWorldTransform(trans);
				glm::mat4 mat;
				trans.getOpenGLMatrix(&mat[0][0]);

				model.SetLocalToWorld(mat);
				// TODO: Directly set transform?
				//model.SetPosition(from(trans.getOrigin()));
				//model.SetRotation(from(trans.getRotation()));
			}
		}
	};

public:
	btDefaultCollisionConfiguration*		collisionConfiguration;
	btCollisionDispatcher*					dispatcher;
	btBroadphaseInterface*					overlappingPairCache;
	btSequentialImpulseConstraintSolver*	solver;
	btDiscreteDynamicsWorld*				dynamicsWorld;
	// recommended by bullet instead of vector:
	btAlignedObjectArray<btCollisionShape*>	collisionShapes;

	std::vector<BodyModelPair> bodyModelPairs;

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

		for (auto& pair : bodyModelPairs)
		{
			pair.UpdateModelPosition();
		}

		/*
		for (size_t i = 0; i < dynamicsWorld->getNumCollisionObjects(); i++)
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);

			btTransform trs;
			if (body && body->getMotionState())
			{
				body->getMotionState()->getWorldTransform(trs);

				if (body->getMass() != 0)
				{
					const btVector3 pos = trs.getOrigin();
					rbMonkey.SetPosition(vec3(pos.getX(), pos.getY(), pos.getZ()));
					const btQuaternion rot = trs.getRotation();
					rbMonkey.SetRotation(quat(rot.getX(), rot.getY(), rot.getZ(), rot.getW()));

					std::cout << pos.getY() << std::endl;
				}
			}
			else
				trs = obj->getWorldTransform();
		}
		*/
	}

	btCollisionShape* AddShape(btCollisionShape* shape)
	{
		collisionShapes.push_back(shape);
		return shape;
	}

	btRigidBody* CreateBody(btCollisionShape* shape, btScalar mass, btVector3 position, btQuaternion rotation)
	{
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(position);
		transform.setRotation(rotation);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			shape->calculateLocalInertia(mass, localInertia);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);
		return body;
	}

	void BindBodyToModel(btRigidBody* body, Model& model)
	{
		BodyModelPair pair(body, model);
		pair.UpdateModelPosition();
		bodyModelPairs.push_back(pair);
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