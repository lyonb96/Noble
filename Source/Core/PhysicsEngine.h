#pragma once

#include "BulletForward.h"
#include "Memory.h"
#include "Types.h"

namespace Noble
{
	typedef MemoryArena<BasicAllocator, DefaultTracking> BulletMemory;

	/**
	 * Handles Bullet physics integration with the engine
	 */
	class PhysicsEngine
	{
	public:

		// no copy or move
		NO_COPY_NO_MOVE(PhysicsEngine)

		PhysicsEngine();

		/**
		 * Initializes the physics engine
		 */
		bool Initialize();

		/**
		 * Shuts down the physics engine and frees memory
		 */
		void Shutdown();

		/**
		 * Called once per fixed tick
		 */
		void FixedUpdate();

	private:

		// Memory arena for Bullet to use
		BulletMemory m_Memory;
		// Collision config
		btDefaultCollisionConfiguration* m_CollisionConfig;
		// Dispatcher
		btCollisionDispatcher* m_Dispatcher;
		// Broadphase interface
		btBroadphaseInterface* m_Interface;
		// Solver
		btSequentialImpulseConstraintSolver* m_Solver;
		// Simulation world
		btDynamicsWorld* m_PhysicsWorld;
	};
}