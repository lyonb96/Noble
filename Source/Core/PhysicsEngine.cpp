#include "PhysicsEngine.h"

#include "Time.h"

#include <btBulletDynamicsCommon.h>

namespace Noble
{
	namespace
	{
		// Lets the non-member functions access the arena
		BulletMemory* g_Mem;
	}

	/**
	 * Future plans for this include using a memory arena to properly track allocs
	 */

	/**
	 * Custom aligned allocation function for Bullet to use
	 */
	void* AllocateAligned_Bullet(Size size, I32 align)
	{
		return NE_BUFFER_ALLOC(*g_Mem, size, Size(align));
	}

	/**
	 * Matching free function for Bullet
	 */
	void FreeAligned_Bullet(void* ptr)
	{
		NE_BUFFER_FREE(*g_Mem, ptr);
	}

	PhysicsEngine::PhysicsEngine()
		: m_CollisionConfig(nullptr),
		m_Dispatcher(nullptr),
		m_Interface(nullptr),
		m_Solver(nullptr),
		m_PhysicsWorld(nullptr)
	{}

	bool PhysicsEngine::Initialize()
	{
		// Store the memory arena
		g_Mem = &m_Memory;
		// Set custom alloc/free functions
		btAlignedAllocSetCustomAligned(AllocateAligned_Bullet, FreeAligned_Bullet);

		// Create collision config
		m_CollisionConfig = NE_NEW(m_Memory, btDefaultCollisionConfiguration)();

		// Create dispatcher
		m_Dispatcher = NE_NEW(m_Memory, btCollisionDispatcher)(m_CollisionConfig);

		// Create broadphase interface
		m_Interface = NE_NEW(m_Memory, btDbvtBroadphase)();

		// Create solver
		// Uses overloaded new, which is annoying, but should still forward to the 
		// above custom aligned alloc functions
		m_Solver = new btSequentialImpulseConstraintSolver;

		// Create dynamics world, same situation as above with the overloaded new operator
		m_PhysicsWorld = new btDiscreteDynamicsWorld(m_Dispatcher, m_Interface, m_Solver, m_CollisionConfig);

		return true;
	}

	void PhysicsEngine::Shutdown()
	{
		// These both still use regular delete because it's overloaded
		delete m_PhysicsWorld;
		delete m_Solver;

		// The rest use arena delete
		NE_DELETE(m_Memory, m_Interface);
		NE_DELETE(m_Memory, m_Dispatcher);
		NE_DELETE(m_Memory, m_CollisionConfig);
	}

	void PhysicsEngine::FixedUpdate()
	{
		m_PhysicsWorld->stepSimulation(Time::GetFixedDeltaTime());
	}
}