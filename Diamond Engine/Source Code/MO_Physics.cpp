#include "Application.h"

#include"DETime.h"

#include "MO_Physics.h"

#include"CO_Script.h"
#include"GameObject.h"
#include "Component.h"

#include "CO_Collider.h"
#include "CO_RigidBody.h"
#include "CO_MeshRenderer.h"

#include "RE_Mesh.h"


//todelete
#include "MO_Renderer3D.h"


#ifndef _DEBUG
#        pragma comment(lib, "Physx/libx86/_release/PhysX_32.lib")
#        pragma comment(lib, "Physx/libx86/_release/PhysXCommon_32.lib")
#        pragma comment(lib, "Physx/libx86/_release/PhysXExtensions_static_32.lib")
#        pragma comment(lib, "Physx/libx86/_release/PhysXFoundation_32.lib")
#        pragma comment(lib, "Physx/libx86/_release/PhysXPvdSDK_static_32.lib")
#        pragma comment(lib, "Physx/libx86/_release/PhysXCharacterKinematic_static_32.lib")
#        pragma comment(lib, "Physx/libx86/_release/SceneQuery_static_32.lib")
#        pragma comment(lib, "Physx/libx86/_release/PhysXCooking_32.lib")
#		 //pragma comment(lib, "Physx/libx86/_release/PhysXVehicle_static_32.lib")
#    else					  
#        pragma comment(lib, "Physx/libx86/_debug/PhysX_32.lib")
#        pragma comment(lib, "Physx/libx86/_debug/PhysXCommon_32.lib")
#        pragma comment(lib, "Physx/libx86/_debug/PhysXExtensions_static_32.lib")
#        pragma comment(lib, "Physx/libx86/_debug/PhysXFoundation_32.lib")
#        pragma comment(lib, "Physx/libx86/_debug/PhysXPvdSDK_static_32.lib")
#        pragma comment(lib, "Physx/libx86/_debug/PhysXCharacterKinematic_static_32.lib")
#        pragma comment(lib, "Physx/libx86/_debug/SceneQuery_static_32.lib")
#        pragma comment(lib, "Physx/libx86/_debug/PhysXCooking_32.lib")
#		 //pragma comment(lib, "Physx/libx86/_debug/PhysXVehicle_static_32.lib")
#    endif // _DEBUG

using namespace physx;

#define MAX_NUM_MESH_VEC3S  1024
#define MAX_NUM_ACTOR_SHAPES 128

static PxVec3 gVertexBuffer[MAX_NUM_MESH_VEC3S];

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled) {

	mFoundation = nullptr;
	mPhysics = nullptr;
	mPvd = nullptr;
	mCooking = nullptr;
	mMaterial = nullptr;
	mScene = nullptr;
	mDispatcher = nullptr;

	gravity = float3(0.0f, -9.81f, 0.0f);
}

ModulePhysics::~ModulePhysics() {

}

physx::PxFilterFlags contactReportFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

	// all initial and persisting reports for everything, with per-point data
	pairFlags = PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eDETECT_DISCRETE_CONTACT
		| PxPairFlag::eNOTIFY_TOUCH_FOUND
		//| PxPairFlag::eNOTIFY_TOUCH_PERSISTS
		| PxPairFlag::eNOTIFY_CONTACT_POINTS
		| PxPairFlag::eTRIGGER_DEFAULT 
		| PxPairFlag::eDETECT_CCD_CONTACT; 
	return PxFilterFlag::eDEFAULT;
}

bool ModulePhysics::Init() {

	//CollisionDetector* detector = new CollisionDetector();

	//Initialize PhysX mFoundation
#pragma region Foundation_Initialize
	static PxDefaultErrorCallback	mErrorCallback;

	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mAllocator, mErrorCallback);
	if (!mFoundation)
		LOG(LogType::L_ERROR, "PxCreateFoundation failed!")
	else
		LOG(LogType::L_NORMAL, "PxCreateFoundation Created: Succesfully inited PhysX");
#pragma endregion Foundation_Initialize

	//Initialize physics
#pragma region Physics_Initialize
	bool recordMemoryAllocations = true;

	mPvd = PxCreatePvd(*mFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("hello", 5425, 10);
	mPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(), recordMemoryAllocations, mPvd);
	if (!mPhysics)
		LOG(LogType::L_ERROR, "PxCreatePhysics failed!")
	else
		LOG(LogType::L_NORMAL, "PxCreatePhysics Sucessfull");

#pragma endregion Physics_Initialize

	//Initialize Cooking
#pragma region Cooking_Initialize
	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(PxTolerancesScale()));
	if (!mCooking)
		LOG(LogType::L_ERROR, "PxCreateCooking failed!")
	else
		LOG(LogType::L_NORMAL, "PxCooking created Succesfully");
#pragma endregion Cooking_Initialize

	//Initialize Extensions
#pragma region Extensions_Initialize
	if (!PxInitExtensions(*mPhysics, mPvd))
		LOG(LogType::L_ERROR, "PxInitExtensions failed!")
	else
		LOG(LogType::L_NORMAL, "PxInitextension Succesfull");
#pragma endregion Extensions_Initialize

	//Initialize Scene
#pragma region Scene_Initialize
	PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	mDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = mDispatcher;
	sceneDesc.filterShader = contactReportFilterShader;

	mScene = mPhysics->createScene(sceneDesc);
#pragma endregion Scene_Initialize

	//Initialize SceneClient
#pragma region SceneClient_Initialize
	PxPvdSceneClient* pvdClient = mScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
#pragma endregion SceneClient_Initialize

	//Initialize Material with default values staticFric 0.5  DynamicFric 0.5  restitution 0.1
	mMaterial = CreateMaterial();



	mScene->setSimulationEventCallback(&detector);
	mScene->setGravity(PxVec3(gravity.x, gravity.y, gravity.z));
	mScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	mScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

	return true;
}

update_status ModulePhysics::PreUpdate(float dt)
{
	if (DETime::state == GameState::PLAY)
		SceneSimulation(DETime::deltaTime);


	
	return update_status::UPDATE_CONTINUE;
}

update_status ModulePhysics::Update(float gameTimestep) {

	//RenderGeometry();
	const PxRenderBuffer& rb = mScene->getRenderBuffer();
	for (PxU32 i = 0; i < rb.getNbLines(); i++)
	{
		const PxDebugLine& line = rb.getLines()[i];


	/*	glLineWidth(2.0f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glBegin(GL_LINES);
		glVertex3f(line.pos0.x, line.pos0.y, line.pos0.z);
		glVertex3f(line.pos1.x, line.pos1.y, line.pos1.z);
		
		glEnd();
		glColor3f(1.0f, 1.0f, 1.0f);*/

	}
	return update_status::UPDATE_CONTINUE;
}

void ModulePhysics::SceneSimulation(double gameTimestep, bool fetchResults) {
	PxReal step = gameTimestep;
	//if(step < 0.002)
	//step = 0.002;
	mScene->simulate(step);
		mScene->fetchResults(fetchResults);

	PxU32 nbActors = mScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	if (nbActors)
	{
		std::vector<PxRigidActor*> actors(nbActors);
		mScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
		for (int i = 0; i < actors.size(); i++)
		{
			GameObject* contact = static_cast<GameObject*>(actors[i]->userData);
			C_RigidBody* body = dynamic_cast<C_RigidBody*>(contact->GetComponent(Component::TYPE::RIGIDBODY));
			body->Step();
		}
	}
	

}

bool ModulePhysics::CleanUp() {
	PX_RELEASE(mScene);
	PX_RELEASE(mMaterial);
	PX_RELEASE(mPhysics);

	if (mPvd)
	{
		PxPvdTransport* transport = mPvd->getTransport();
		mPvd->release(); mPvd = NULL;
		PX_RELEASE(transport);
	}

	PX_RELEASE(mCooking);
	PxCloseExtensions(); // Needed to close extensions we inited before
	PX_RELEASE(mDispatcher);

	//Remember to release the last
	PX_RELEASE(mFoundation);

	return true;
}

void ModulePhysics::RenderGeometry() {

	PxGetPhysics().getScenes(&mScene, 1);
	PxU32 nbActors = mScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	if (nbActors)
	{
		std::vector<PxRigidActor*> actors(nbActors);
		mScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
		//renderActors(&actors[0], static_cast<PxU32>(actors.size()), false);
	}

}



physx::PxRigidDynamic* ModulePhysics::CreateRigidDynamic(float3 pos, Quat rot) {
	
	physx::PxQuat rotation = { rot.x,  rot.y, rot.z, rot.w };

	PxTransform transform(pos.x, pos.y, pos.z, rotation);

	PxRigidDynamic* dynamicBody = nullptr;
	dynamicBody = mPhysics->createRigidDynamic(transform);
	mScene->addActor(*dynamicBody);
	return dynamicBody;
}
//
physx::PxShape* ModulePhysics::CreateBoxCollider(float3 size, PxMaterial* material) {

	PxShape* colliderShape = nullptr;

	if (material == nullptr)
		material = mMaterial;

	
	colliderShape = mPhysics->createShape(PxBoxGeometry(size.x, size.y, size.z), *material, true);

	return colliderShape;
}

physx::PxShape* ModulePhysics::CreateSphereCollider(float radius, PxMaterial* material) {

	PxShape* colliderShape = nullptr;

	if (material == nullptr)
		material = mMaterial;


	colliderShape = mPhysics->createShape(PxSphereGeometry(radius), *material, true);

	return colliderShape;
}

physx::PxShape* ModulePhysics::CreateMeshCollider(PxRigidActor* aConvexActor, GameObject* parent)
{
	C_MeshRenderer* mesh = dynamic_cast<C_MeshRenderer*>(parent->GetComponent(Component::TYPE::MESH_RENDERER));
	ResourceMesh* resMesh = mesh->GetRenderMesh();

	PxVec3* convexVerts = new PxVec3[resMesh->vertices_count];
	for (int i = 0; i < resMesh->vertices_count; i++)
	{
		PxVec3 vertex;
		vertex.x = resMesh->vertices[VERTEX_ATTRIBUTES * i];
		vertex.y = resMesh->vertices[VERTEX_ATTRIBUTES * i + 1];
		vertex.z = resMesh->vertices[VERTEX_ATTRIBUTES * i + 2];
		
		convexVerts[i] = vertex;
	}

	
	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = resMesh->vertices_count;
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = convexVerts;
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

#ifdef _DEBUG
	// mesh should be validated before cooking without the mesh cleaning
	//bool res = mCooking->validateConvexMesh(convexDesc);
	//PX_ASSERT(res);
#endif

	PxConvexMesh* aConvexMesh = mCooking->createConvexMesh(convexDesc,
		mPhysics->getPhysicsInsertionCallback());

	PxShape* aConvexShape = mPhysics->createShape(PxConvexMeshGeometry(aConvexMesh), *mMaterial);
	delete[] convexVerts;

	return aConvexShape;
	//aConvexShape->getGeometry().convexMesh().convexMesh->getVertices(;
}

physx::PxMaterial* ModulePhysics::CreateMaterial(float staticFriction, float dynamicFriction, float restitution) {

	PxMaterial* material = nullptr;
	material = mPhysics->createMaterial(staticFriction, dynamicFriction, restitution);

	return material;
}

void ModulePhysics::ReleaseActor(PxRigidActor* actor) {

	physx::PxShape* shapes[128];
	actor->getShapes(shapes, actor->getNbShapes());

	for (size_t i = 0; i < actor->getNbShapes(); i++)
		actor->detachShape(*shapes[i]);

	//App->physX->mScene->removeActor(*actor);

	actor->release();
	actor = nullptr;
}


float4x4 ModulePhysics::PhysXTransformToF4F(PxTransform transform) {

	float3 pos = { transform.p.x, transform.p.y, transform.p.z };
	Quat rot = { transform.q.x, transform.q.y, transform.q.z, transform.q.w };

	float4x4 matrix = float4x4(rot, pos);

	return matrix;
}


PxTransform ModulePhysics::TRStoPxTransform(float3 pos, float3 rot) {

	Quat rotation = Quat::FromEulerXYZ(rot.x * DEGTORAD, rot.y * DEGTORAD, rot.z * DEGTORAD);

	PxVec3 Pxposition(pos.x, pos.y, pos.z);
	PxQuat PxRotation(rotation.x, rotation.y, rotation.z, rotation.w);

	return PxTransform(Pxposition, PxRotation);
}

CollisionDetector::CollisionDetector()
{

}
CollisionDetector::~CollisionDetector()
{

}

void CollisionDetector::onContact(const PxContactPairHeader& pairHeader,
	const PxContactPair* pairs, PxU32 nbPairs)
{
	
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];

		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			for (size_t k= 0; k < 2; ++k)
			{
				GameObject* contact = static_cast<GameObject*>(pairHeader.actors[k]->userData);

				std::vector< Component*> scripts = contact->GetComponentsOfType(Component::TYPE::SCRIPT);
				for (size_t l = 0; l < scripts.size(); l++)
				{
					C_Script* script = dynamic_cast<C_Script*>(scripts[l]);
					if (script)
					{
						GameObject* contact2 = static_cast<GameObject*>(pairHeader.actors[ k == 0 ? 1 : 0]->userData);
						script->CollisionCallback(false, contact2);
					}
				}
			}

			/*if ((pairHeader.actors[0] == mSubmarineActor) ||
				(pairHeader.actors[1] == mSubmarineActor))*/
		}
	}
}

void CollisionDetector::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	LOG(LogType::L_NORMAL, "trigger detected");
	for (PxU32 i = 0; i < count; i++)
	{
		const PxTriggerPair& cp = pairs[i];

		GameObject* contact = static_cast<GameObject*>(pairs->triggerActor->userData);
		GameObject* contact2 = static_cast<GameObject*>(pairs->otherActor->userData);

		if (contact != nullptr) 
		{
			std::vector< Component*> scripts = contact->GetComponentsOfType(Component::TYPE::SCRIPT);
			for (size_t i = 0; i < scripts.size(); i++)
			{
				C_Script* script = dynamic_cast<C_Script*>(scripts[i]);
				if (script)
					script->CollisionCallback(true, contact2);
			}
		
			
		}

		if (contact2 != nullptr) 
		{
			std::vector< Component*> scripts = contact2->GetComponentsOfType(Component::TYPE::SCRIPT);
			for (size_t i = 0; i < scripts.size(); i++)
			{
				C_Script* script = dynamic_cast<C_Script*>(scripts[i]);
				if (script)
					script->CollisionCallback(true, contact);
			}
		}
		
	}
}
