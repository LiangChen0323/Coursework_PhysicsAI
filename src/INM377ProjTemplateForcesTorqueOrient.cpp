/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2006 Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/



#define CUBE_HALF_EXTENTS 1

#define EXTRA_HEIGHT 1.f

#include "INM377ProjTemplateForcesTorqueOrient.h"
#include "GlutStuff.h"
#include "GLDebugFont.h"

///btBulletDynamicsCommon.h is the main Bullet include file, contains most common include files.
#include "btBulletDynamicsCommon.h"

#include <stdio.h> //printf debugging
#include "GLDebugDrawer.h"

#if 0
extern btAlignedObjectArray<btVector3> debugContacts;
extern btAlignedObjectArray<btVector3> debugNormals;
#endif 

static GLDebugDrawer	sDebugDrawer;


INM377ProjTemplateTorqueOrient::INM377ProjTemplateTorqueOrient()
:m_ccdMode(USE_CCD)
{
	setDebugMode(btIDebugDraw::DBG_DrawText+btIDebugDraw::DBG_NoHelpText);
	setCameraDistance(btScalar(40.));
}


void INM377ProjTemplateTorqueOrient::clientMoveAndDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	//simple dynamics world doesn't handle fixed-time-stepping
	//float ms = getDeltaTimeMicroseconds();
	
	///step the simulation
	if (m_dynamicsWorld)
	{
		m_dynamicsWorld->stepSimulation(1./60.,0);//ms / 1000000.f);
		//optional but useful: debug drawing
		m_dynamicsWorld->debugDrawWorld();
	}
		
	renderme(); 

//	displayText();
#if 0
	for (int i=0;i<debugContacts.size();i++)
	{
		getDynamicsWorld()->getDebugDrawer()->drawContactPoint(debugContacts[i],debugNormals[i],0,0,btVector3(1,0,0));
	}
#endif

	glFlush();

	swapBuffers();

}


void INM377ProjTemplateTorqueOrient::displayText()
{
	int lineWidth=440;
	int xStart = m_glutScreenWidth - lineWidth;
	int yStart = 20;

	if((getDebugMode() & btIDebugDraw::DBG_DrawText)!=0)
	{
		setOrthographicProjection();
		glDisable(GL_LIGHTING);
		glColor3f(0, 0, 0);
		char buf[124];
		
		glRasterPos3f(xStart, yStart, 0);
		switch (m_ccdMode)
		{
		case USE_CCD:
			{
				sprintf_s(buf,"Predictive contacts and motion clamping");
				break;
			}
		case USE_NO_CCD:
			{
				sprintf_s(buf,"CCD handling disabled");
				break;
			}
		default:
			{
				sprintf_s(buf,"unknown CCD setting");
			};
		};

		GLDebugDrawString(xStart,20,buf);
		glRasterPos3f(xStart, yStart, 0);
		sprintf_s(buf,"Press 'p' to change CCD mode");
		yStart+=20;
		GLDebugDrawString(xStart,yStart,buf);
		glRasterPos3f(xStart, yStart, 0);
		sprintf_s(buf,"Press '.' or right mouse to shoot bullets");
		yStart+=20;
		GLDebugDrawString(xStart,yStart,buf);
		glRasterPos3f(xStart, yStart, 0);
		sprintf_s(buf,"space to restart, h(elp), t(ext), w(ire)");
		yStart+=20;
		GLDebugDrawString(xStart,yStart,buf);
		
		resetPerspectiveProjection();
		glEnable(GL_LIGHTING);
	}	

}



void INM377ProjTemplateTorqueOrient::displayCallback(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	
	renderme();

	displayText();

	//optional but useful: debug drawing to detect problems
	if (m_dynamicsWorld)
	{
		m_dynamicsWorld->debugDrawWorld();
	}
#if 0
	for (int i=0;i<debugContacts.size();i++)
	{
		getDynamicsWorld()->getDebugDrawer()->drawContactPoint(debugContacts[i],debugNormals[i],0,0,btVector3(1,0,0));
	}
#endif

	glFlush();
	swapBuffers();
}


void MyTickCallback(btDynamicsWorld *world, btScalar timeStep) {
		world->clearForces();
		btRigidBody* body0 = static_cast<INM377ProjTemplateTorqueOrient *>(world->getWorldUserInfo())->body000;
		btScalar mass = body0->getInvMass();
		btVector3 vel = body0->getLinearVelocity();
		btVector3 gravity = body0->getGravity();
		btVector3 dir = btVector3(0, 0, 1);
		btVector3 thrust = 7.0 * dir;
		btVector3 drag = -3 * vel;
		btVector3 lift = - 0.5 * gravity * vel.length();
		body0->applyCentralForce(thrust + lift + gravity + drag);

		btRigidBody* bbody0 = static_cast<INM377ProjTemplateTorqueOrient *>(world->getWorldUserInfo())->boid;
		btScalar bmass = bbody0->getInvMass();
		btVector3 bvel = bbody0->getLinearVelocity();
		btVector3 bgravity = bbody0->getGravity();
		btVector3 bdir = btVector3(0, 0.2, 1);
		btTransform btrans(bbody0->getOrientation());
		btVector3 up(0, 1, 0);
		btVector3 btop = btrans * up;
		btVector3 front = btrans * btVector3(1, 0, 0);
		btVector3 bdir1 = bvel.safeNormalize();
		btVector3  avel = bbody0->getAngularVelocity();
		btVector3 bthrust = 3.5 * front;
		btVector3 bdrag = - 3 * bvel;
		btVector3 blift = - 1.00 * bgravity * bvel.length();
		bbody0->applyCentralForce(bthrust + blift + bgravity + bdrag);
		bbody0->applyTorque(2 * front.cross(bdir) - 5.0*avel);
		bbody0->applyTorque(- 0.5 * up);
		bbody0->applyTorque(0.5 * btop.cross(up) - 5*avel);
}

void	INM377ProjTemplateTorqueOrient::initPhysics()
{
	setTexturing(true);
	setShadows(false);

	setCameraDistance(50.f);

	// init world
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	btVector3 worldMin(-1000, -1000, -1000);
	btVector3 worldMax(1000, 1000, 1000);
	m_overlappingPairCache = new btAxisSweep3(worldMin, worldMax);

	m_constraintSolver = new btSequentialImpulseConstraintSolver();

	btDiscreteDynamicsWorld* wp = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_constraintSolver, m_collisionConfiguration);
	//	wp->getSolverInfo().m_numIterations = 20; // default is 10
	m_dynamicsWorld = wp;
	m_dynamicsWorld->setInternalTickCallback(MyTickCallback, static_cast<void *>(this), true);
	

	///create a few basic rigid bodies
	btBoxShape* box = new btBoxShape(btVector3(btScalar(110.),btScalar(1.),btScalar(110.)));
//	box->initializePolyhedralFeatures();
	btCollisionShape* groundShape = box;

//	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),50);
	
	m_collisionShapes.push_back(groundShape);
	//m_collisionShapes.push_back(new btCylinderShape (btVector3(CUBE_HALF_EXTENTS,CUBE_HALF_EXTENTS,CUBE_HALF_EXTENTS)));
	m_collisionShapes.push_back(new btBoxShape (btVector3(CUBE_HALF_EXTENTS,CUBE_HALF_EXTENTS,CUBE_HALF_EXTENTS)));

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0,0,0));

	//We can also use DemoApplication::localCreateRigidBody, but for clarity it is provided here:
	{
		btScalar mass(0.);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0,0,0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass,localInertia);

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,groundShape,localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);
		body->setFriction(0.5);

		//body->setRollingFriction(0.3);
		//add the body to the dynamics world
		m_dynamicsWorld->addRigidBody(body);
	}


	{
		//create a few dynamic rigidbodies
		// Re-using the same collision is better for memory usage and performance

//		btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		
//		btCollisionShape* colShape = new btSphereShape(btScalar(1.));
//		m_collisionShapes.push_back(colShape);

		/// Create Dynamic Objects
//		btTransform startTransform;
//		startTransform.setIdentity();

//		btScalar	mass(1.f);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
//		bool isDynamic = (mass != 0.f);

//		btVector3 localInertia(0,0,0);
//		if (isDynamic)
//			colShape->calculateLocalInertia(mass,localInertia);

		btCollisionShape* colShape = new btBoxShape(btVector3(5, 3, 5));
		m_collisionShapes.push_back(colShape);
		btTransform trans;
		trans.setIdentity();
		btCollisionShape* shape = m_collisionShapes[2];
		btVector3 pos(0, 0, 0);
		trans.setOrigin(pos);
		btScalar mass(1.0f);
		// shape->calculateLocalInertia(mass, LocalInertia);
		body000 = localCreateRigidBody(mass, trans, shape);
		body000->setAnisotropicFriction(shape->getAnisotropicRollingFrictionDirection(), btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION);
		body000->setFriction(0.5);
//		body000->setLinearVelocity(btVector3(1, 0, 0));
		body000->activate(true);

//		btCollisionShape* bShape = new btBoxShape(btVector3(5, 3, 5));
		btConvexHullShape * bShape = new btConvexHullShape();
		bShape->addPoint(btVector3(10, 0, 0));
		bShape->addPoint(btVector3(0, 5, 0));
		bShape->addPoint(btVector3(0, 0, 5));
		bShape->addPoint(btVector3(0, 0, -5));

		m_collisionShapes.push_back(bShape);
		btTransform btrans;
		btrans.setIdentity();
//		btCollisionShape* bshape = m_collisionShapes[3];
		btVector3 bpos(20, 0, 0);
		btrans.setOrigin(bpos);
		btScalar bmass(1.0f);
		btVector3 bLocalInertia;
		bShape->calculateLocalInertia(bmass, bLocalInertia);
		boid = localCreateRigidBody(bmass, btrans, bShape);
		boid->setAnisotropicFriction(bShape->getAnisotropicRollingFrictionDirection(), btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION);
		boid->setFriction(0.5);
		//		boid->setLinearVelocity(btVector3(1, 0, 0));
		boid->activate(true);

	}

}

void	INM377ProjTemplateTorqueOrient::clientResetScene()
{
	exitPhysics();
	initPhysics();
}

void INM377ProjTemplateTorqueOrient::keyboardCallback(unsigned char key, int x, int y)
{
	if (key=='p')
	{
		switch (m_ccdMode)
		{
			case USE_CCD:
			{
				m_ccdMode = USE_NO_CCD;
				break;
			}
			case USE_NO_CCD:
			default:
			{
				m_ccdMode = USE_CCD;
			}
		};
		clientResetScene();
	} else
	{
		DemoApplication::keyboardCallback(key,x,y);
	}
}


void	INM377ProjTemplateTorqueOrient::shootBox(const btVector3& destination)
{

	if (m_dynamicsWorld)
	{
		float mass = 1.f;
		btTransform startTransform;
		startTransform.setIdentity();
		btVector3 camPos = getCameraPosition();
		startTransform.setOrigin(camPos);

		setShootBoxShape ();


		btRigidBody* body = this->localCreateRigidBody(mass, startTransform,m_shootBoxShape);
		body->setLinearFactor(btVector3(1,1,1));
		//body->setRestitution(1);

		btVector3 linVel(destination[0]-camPos[0],destination[1]-camPos[1],destination[2]-camPos[2]);
		linVel.normalize();
		linVel*=m_ShootBoxInitialSpeed;

		body->getWorldTransform().setOrigin(camPos);
		body->getWorldTransform().setRotation(btQuaternion(0,0,0,1));
		body->setLinearVelocity(linVel);
		body->setAngularVelocity(btVector3(0,0,0));
		body->setContactProcessingThreshold(1e30);

		///when using m_ccdMode, disable regular CCD
		if (m_ccdMode==USE_CCD)
		{
			body->setCcdMotionThreshold(CUBE_HALF_EXTENTS);
			body->setCcdSweptSphereRadius(0.4f);
		}
		
	}
}




void	INM377ProjTemplateTorqueOrient::exitPhysics()
{

	//cleanup in the reverse order of creation/initialization

	//remove the rigidbodies from the dynamics world and delete them
	int i;
	for (i=m_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		m_dynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}

	//delete collision shapes
	for (int j=0;j<m_collisionShapes.size();j++)
	{
		btCollisionShape* shape = m_collisionShapes[j];
		delete shape;
	}
	m_collisionShapes.clear();

	delete m_dynamicsWorld;
	
//	delete m_solver;
	
//	delete m_broadphase;
	
//	delete m_dispatcher;

//	delete m_collisionConfiguration;

	
}




