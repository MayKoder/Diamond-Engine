#include "Application.h"

#include"DETime.h"

#include "MO_Physics.h"

#include "CO_Collider.h"

#ifndef _DEBUG
#        pragma comment(lib, "Physx/libx86/_release/PhysX_32.lib")
#        pragma comment(lib, "Physx/libx86/_release/PhysXCommon_32.lib")
#        pragma comment(lib, "Physx/libx86/_release/PhysXExtensions_static_32.lib")
#        pragma comment(lib, "Physx/libx86/_release/PhysXFoundation_32.lib")
#        pragma comment(lib, "Physx/libx86/_release/PhysXPvdSDK_static_32.lib")
#        pragma comment(lib, "Physx/libx86/_release/PhysXCharacterKinematic_static_32.lib")
#        pragma comment(lib, "Physx/libx86/_release/SceneQuery_static_32.lib")
#        pragma comment(lib, "Physx/libx86/_release/PhysXCooking_32.lib")
#		 pragma comment(lib, "Physx/libx86/_release/PhysXVehicle_static_32.lib")
#    else					  
#        pragma comment(lib, "Physx/libx86/_debug/PhysX_32.lib")
#        pragma comment(lib, "Physx/libx86/_debug/PhysXCommon_32.lib")
#        pragma comment(lib, "Physx/libx86/_debug/PhysXExtensions_static_32.lib")
#        pragma comment(lib, "Physx/libx86/_debug/PhysXFoundation_32.lib")
#        pragma comment(lib, "Physx/libx86/_debug/PhysXPvdSDK_static_32.lib")
#        pragma comment(lib, "Physx/libx86/_debug/PhysXCharacterKinematic_static_32.lib")
#        pragma comment(lib, "Physx/libx86/_debug/SceneQuery_static_32.lib")
#        pragma comment(lib, "Physx/libx86/_debug/PhysXCooking_32.lib")
#		 pragma comment(lib, "Physx/libx86/_debug/PhysXVehicle_static_32.lib")
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

bool ModulePhysics::Init() {

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
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
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

//	PxRigidStatic* groundPlane = PxCreatePlane(*mPhysics, PxPlane(0, 1, 0, 0), *mMaterial);
//	mScene->addActor(*groundPlane);

	//Init vehicle after foundation and physics
	PxInitVehicleSDK(*mPhysics);

	return true;
}

update_status ModulePhysics::Update(float gameTimestep) {


	if(DETime::state == GameState::PLAY)
		SceneSimulation(gameTimestep);

	//TODO: REMOVE OR REPLACE
	mScene->setGravity(PxVec3(gravity.x, gravity.y, gravity.z));

	RenderGeometry();

	return update_status::UPDATE_CONTINUE;
}

void ModulePhysics::SceneSimulation(float gameTimestep, bool fetchResults) {

	mScene->simulate(gameTimestep);
	mScene->fetchResults(fetchResults);

}

bool ModulePhysics::CleanUp() {

	//if (App->vehicle->gVehicle4W != nullptr) {
	//	App->vehicle->gVehicle4W->getRigidDynamicActor()->release();
	//	App->vehicle->gVehicle4W->free();
	//}

	//PX_RELEASE(App->vehicle->gBatchQuery);
	//App->vehicle->gVehicleSceneQueryData->free(mAllocator);
	//PX_RELEASE(App->vehicle->gFrictionPairs);
	//PxCloseVehicleSDK(); //->Close vehicle sdk before close physics and foundation

	//PX_RELEASE(mScene);
	//PX_RELEASE(mMaterial);
	//PX_RELEASE(mPhysics);

	//if (mPvd)
	//{
	//	PxPvdTransport* transport = mPvd->getTransport();
	//	mPvd->release(); mPvd = NULL;
	//	PX_RELEASE(transport);
	//}

	//PX_RELEASE(mCooking);
	//PxCloseExtensions(); // Needed to close extensions we inited before
	//PX_RELEASE(mDispatcher);

	////Remember to release the last
	//PX_RELEASE(mFoundation);

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

//void ModulePhysics::renderActors(PxRigidActor** actors, const PxU32 numActors, bool shadows)
//{
//	const PxVec3 color = PxVec3(0.0f, 0.75f, 0.0f);
//
//	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
//	for (PxU32 i = 0; i < numActors; i++)
//	{
//		const PxU32 nbShapes = actors[i]->getNbShapes();
//		PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
//		actors[i]->getShapes(shapes, nbShapes);
//		const bool sleeping = actors[i]->is<PxRigidDynamic>() ? actors[i]->is<PxRigidDynamic>()->isSleeping() : false;
//
//		for (PxU32 j = 0; j < nbShapes; j++)
//		{
//			const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
//			const PxGeometryHolder h = shapes[j]->getGeometry();
//
//			if (shapes[j]->getFlags() & PxShapeFlag::eTRIGGER_SHAPE)
//				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//
//			// render object
//			glPushMatrix();
//			glMultMatrixf(&shapePose.column0.x);
//			if (sleeping)
//			{
//				const PxVec3 darkColor = color * 0.25f;
//				glColor4f(darkColor.x, darkColor.y, darkColor.z, 1.0f);
//			}
//			else
//				glColor4f(color.x, color.y, color.z, 1.0f);
//
//			renderGeometryHolder(h);
//			glPopMatrix();
//			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//
//		}
//	}
//}

void ModulePhysics::renderGeometryHolder(const PxGeometryHolder& h) {
	//renderGeometry(h.any());
};
//
//void ModulePhysics::renderGeometry(const PxGeometry& geom)
//{
//	switch (geom.getType())
//	{
//	case PxGeometryType::eCONVEXMESH:
//	{
//		const PxConvexMeshGeometry& convexGeom = static_cast<const PxConvexMeshGeometry&>(geom);
//
//		//Compute triangles for each polygon.
//		const PxVec3& scale = convexGeom.scale.scale;
//		PxConvexMesh* mesh = convexGeom.convexMesh;
//		const PxU32 nbPolys = mesh->getNbPolygons();
//		const PxU8* polygons = mesh->getIndexBuffer();
//		const PxVec3* verts = mesh->getVertices();
//		PxU32 nbVerts = mesh->getNbVertices();
//		PX_UNUSED(nbVerts);
//
//		PxU32 numTotalTriangles = 0;
//		for (PxU32 i = 0; i < nbPolys; i++)
//		{
//			PxHullPolygon data;
//			mesh->getPolygonData(i, data);
//
//			const PxU32 nbTris = PxU32(data.mNbVerts - 2);
//			const PxU8 vref0 = polygons[data.mIndexBase + 0];
//			PX_ASSERT(vref0 < nbVerts);
//			for (PxU32 j = 0; j < nbTris; j++)
//			{
//				const PxU32 vref1 = polygons[data.mIndexBase + 0 + j + 1];
//				const PxU32 vref2 = polygons[data.mIndexBase + 0 + j + 2];
//
//				//generate face normal:
//				PxVec3 e0 = verts[vref1] - verts[vref0];
//				PxVec3 e1 = verts[vref2] - verts[vref0];
//
//				PX_ASSERT(vref1 < nbVerts);
//				PX_ASSERT(vref2 < nbVerts);
//
//				PxVec3 fnormal = e0.cross(e1);
//				fnormal.normalize();
//
//				if (numTotalTriangles * 6 < MAX_NUM_MESH_VEC3S)
//				{
//					gVertexBuffer[numTotalTriangles * 6 + 0] = fnormal;
//					gVertexBuffer[numTotalTriangles * 6 + 1] = verts[vref0];
//					gVertexBuffer[numTotalTriangles * 6 + 2] = fnormal;
//					gVertexBuffer[numTotalTriangles * 6 + 3] = verts[vref1];
//					gVertexBuffer[numTotalTriangles * 6 + 4] = fnormal;
//					gVertexBuffer[numTotalTriangles * 6 + 5] = verts[vref2];
//					numTotalTriangles++;
//				}
//			}
//		}
//		glPushMatrix();
//		glScalef(scale.x, scale.y, scale.z);
//		glEnableClientState(GL_NORMAL_ARRAY);
//		glEnableClientState(GL_VERTEX_ARRAY);
//		glNormalPointer(GL_FLOAT, 2 * 3 * sizeof(float), gVertexBuffer);
//		glVertexPointer(3, GL_FLOAT, 2 * 3 * sizeof(float), gVertexBuffer + 1);
//		glDrawArrays(GL_TRIANGLES, 0, int(numTotalTriangles * 3));
//		glPopMatrix();
//	}
//	break;
//
//	case PxGeometryType::eTRIANGLEMESH:
//	{
//		const PxTriangleMeshGeometry& triGeom = static_cast<const PxTriangleMeshGeometry&>(geom);
//
//		const PxTriangleMesh& mesh = *triGeom.triangleMesh;
//		const PxVec3 scale = triGeom.scale.scale;
//
//		const PxU32 triangleCount = mesh.getNbTriangles();
//		const PxU32 has16BitIndices = mesh.getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES;
//		const void* indexBuffer = mesh.getTriangles();
//
//		const PxVec3* vertexBuffer = mesh.getVertices();
//
//		const PxU32* intIndices = reinterpret_cast<const PxU32*>(indexBuffer);
//		const PxU16* shortIndices = reinterpret_cast<const PxU16*>(indexBuffer);
//		PxU32 numTotalTriangles = 0;
//		for (PxU32 i = 0; i < triangleCount; ++i)
//		{
//			PxVec3 triVert[3];
//
//			if (has16BitIndices)
//			{
//				triVert[0] = vertexBuffer[*shortIndices++];
//				triVert[1] = vertexBuffer[*shortIndices++];
//				triVert[2] = vertexBuffer[*shortIndices++];
//			}
//			else
//			{
//				triVert[0] = vertexBuffer[*intIndices++];
//				triVert[1] = vertexBuffer[*intIndices++];
//				triVert[2] = vertexBuffer[*intIndices++];
//			}
//
//			PxVec3 fnormal = (triVert[1] - triVert[0]).cross(triVert[2] - triVert[0]);
//			fnormal.normalize();
//
//			if (numTotalTriangles * 6 < MAX_NUM_MESH_VEC3S)
//			{
//				gVertexBuffer[numTotalTriangles * 6 + 0] = fnormal;
//				gVertexBuffer[numTotalTriangles * 6 + 1] = triVert[0];
//				gVertexBuffer[numTotalTriangles * 6 + 2] = fnormal;
//				gVertexBuffer[numTotalTriangles * 6 + 3] = triVert[1];
//				gVertexBuffer[numTotalTriangles * 6 + 4] = fnormal;
//				gVertexBuffer[numTotalTriangles * 6 + 5] = triVert[2];
//				numTotalTriangles++;
//			}
//		}
//		glPushMatrix();
//		glScalef(scale.x, scale.y, scale.z);
//		glEnableClientState(GL_NORMAL_ARRAY);
//		glEnableClientState(GL_VERTEX_ARRAY);
//		glNormalPointer(GL_FLOAT, 2 * 3 * sizeof(float), gVertexBuffer);
//		glVertexPointer(3, GL_FLOAT, 2 * 3 * sizeof(float), gVertexBuffer + 1);
//		glDrawArrays(GL_TRIANGLES, 0, int(numTotalTriangles * 3));
//		glDisableClientState(GL_VERTEX_ARRAY);
//		glDisableClientState(GL_NORMAL_ARRAY);
//		glPopMatrix();
//	}
//	break;
//
//	case PxGeometryType::ePLANE:
//		break;
//	}
//
//}

//--------------CREATE A NEW GEOMETRY--------------//
//
//physx::PxRigidDynamic* ModulePhysics::CreateGeometry(GeometryType type, float3 pos, float mass, float radius, float3 size) {
//
//	physx::PxRigidDynamic* geometry = nullptr;
//	PxTransform position = PxTransform(pos.x, pos.y, pos.z);
//
//	switch (type)
//	{
//	case GeometryType::BOX:
//	{
//		PxShape* shape = mPhysics->createShape(PxBoxGeometry(size.x, size.y, size.z), *mMaterial);
//
//		geometry = mPhysics->createRigidDynamic(position);
//		geometry->attachShape(*shape);
//
//		geometry->setAngularDamping(0.5f);
//		geometry->setLinearVelocity(PxVec3(0));
//
//		//LOG("CREATED BOX");
//	}
//	break;
//	case GeometryType::SPHERE:
//	{
//		PxShape* shape = mPhysics->createShape(PxSphereGeometry(radius), *mMaterial);
//
//		geometry = PxCreateDynamic(*mPhysics, position, PxSphereGeometry(radius), *mMaterial, mass);
//		geometry->attachShape(*shape);
//
//		geometry->setAngularDamping(0.05f);
//
//		//LOG("CREATED SPHERE");
//
//	}
//	break;
//	case GeometryType::CAPSULE:
//	{
//		PxReal halfHeight = size.y / 2;
//
//		geometry = mPhysics->createRigidDynamic(PxTransform(position));
//
//		PxShape* shape = PxRigidActorExt::createExclusiveShape(*geometry, PxCapsuleGeometry(radius, halfHeight), *mMaterial);
//		geometry->attachShape(*shape);
//
//		geometry->setAngularDamping(0.05f);
//
//		//LOG("CREATED CAPSULE");
//	}
//	break;
//	case GeometryType::NONE:
//		break;
//	}
//
//	geometry->setMass(mass);
//	mScene->addActor(*geometry);
//
//	return geometry;
//
//}

////--------------DRAW THE GEOMETRY--------------//
//void ModulePhysics::DrawGeometry(GeometryType type, float3 pos, float radius, float3 size) {
//
//	switch (type)
//	{
//	case GeometryType::BOX:
//	{
//		glBegin(GL_QUADS);
//		glPushMatrix();
//
//		glNormal3f(-1.0f, 0.0f, 0.0f); //FRONT
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(0.0f, 0.0f, 1.0f);
//		glVertex3f(1.0f, 0.0f, 1.0f);
//		glVertex3f(1.0f, 0.0f, 0.0f);
//
//		glNormal3f(0.0f, 0.0f, -1.0f); //BOTTOM
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(1.0f, 0.0f, 0.0f);
//		glVertex3f(1.0f, 1.0f, 0.0f);
//		glVertex3f(0.0f, 1.0f, 0.0f);
//
//		glNormal3f(1.0f, 0.0f, 0.0f); //BACK
//		glVertex3f(1.0f, 1.0f, 0.0f);
//		glVertex3f(1.0f, 1.0f, 1.0f);
//		glVertex3f(0.0f, 1.0f, 1.0f);
//		glVertex3f(0.0f, 1.0f, 0.0f);
//
//		glNormal3f(0.0f, 0.0f, 1.0f); //TOP
//		glVertex3f(1.0f, 1.0f, 1.0f);
//		glVertex3f(1.0f, 0.0f, 1.0f);
//		glVertex3f(0.0f, 0.0f, 1.0f);
//		glVertex3f(0.0f, 1.0f, 1.0f);
//
//		glNormal3f(0.0f, 1.0f, 0.0f); //LEFT
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(0.0f, 1.0f, 0.0f);
//		glVertex3f(0.0f, 1.0f, 1.0f);
//		glVertex3f(0.0f, 0.0f, 1.0f);
//
//		glNormal3f(0.0f, -1.0f, 0.0f); //RIGHT
//		glVertex3f(1.0f, 0.0f, 0.0f);
//		glVertex3f(1.0f, 0.0f, 1.0f);
//		glVertex3f(1.0f, 1.0f, 1.0f);
//		glVertex3f(1.0f, 1.0f, 0.0f);
//
//		glScalef(size.x, size.y, size.z);
//		glTranslatef(pos.x, pos.y, pos.z);
//
//		glPopMatrix();
//		glEnd();
//
//	}
//	break;
//	case GeometryType::SPHERE:
//	{
//		GLfloat x, y, z, alpha, beta; // Storage for coordinates and angles        
//		int gradation = 10;
//
//		for (alpha = 0.0; alpha < PI; alpha += PI / gradation)
//		{
//			glBegin(GL_TRIANGLE_STRIP);
//			glPushMatrix();
//
//			for (beta = 0.0; beta < 2.01 * PI; beta += PI / gradation)
//			{
//				x = cos(beta) * sin(alpha);
//				y = sin(beta) * sin(alpha);
//				z = cos(alpha);
//				glVertex3f(x, y, z);
//				x = cos(beta) * sin(alpha + PI / gradation);
//				y = sin(beta) * sin(alpha + PI / gradation);
//				z = cos(alpha + PI / gradation);
//				glVertex3f(x, y, z);
//			}
//
//			glTranslatef(pos.x, 0.0f, 0.0f);
//			glScalef(radius, radius, radius);
//
//			glPopMatrix();
//			glEnd();
//		}
//	}
//	break;
//	case GeometryType::CAPSULE:
//	{
//		glBegin(GL_QUAD_STRIP);
//		glPushMatrix();
//		glTranslatef(-pos.x, 0.0f, 0.0f);
//		//glScalef(2.0f * pos.x, radius, radius);
//
//		for (int i = 0; i < 480; i += (360 / 16)) {
//			float a = i * M_PI / 180; // degrees to radians
//			glVertex3f(2 * cos(a), 2 * sin(a), 0.0);
//			glVertex3f(2 * cos(a), 2 * sin(a), 4);
//		}
//		glEnd();
//
//		glPopMatrix();
//		glEnd();
//	}
//	break;
//	case GeometryType::NONE:
//		break;
//	}
//
//}

physx::PxRigidStatic* ModulePhysics::CreateRigidStatic(float3 pos) {

	PxTransform position(pos.x, pos.y, pos.z);

	PxRigidStatic* staticBody = nullptr;
	staticBody = mPhysics->createRigidStatic(position);

	mScene->addActor(*staticBody);
	return staticBody;
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
physx::PxShape* ModulePhysics::CreateCollider(float3 size, PxMaterial* material) {

	PxShape* colliderShape = nullptr;

	if (material == nullptr)
		material = mMaterial;

	/*switch (colliderType) {
	case GeometryType::BOX:
		colliderShape = mPhysics->createShape(PxBoxGeometry(size.x, size.y, size.z), *material, true);
		break;
	case GeometryType::SPHERE:
		colliderShape = mPhysics->createShape(PxSphereGeometry(size.MaxElement()), *material, true);
		break;
	case GeometryType::CAPSULE:
		colliderShape = mPhysics->createShape(PxCapsuleGeometry(size.x, size.y), *material, true);
		break;
	}*/
	colliderShape = mPhysics->createShape(PxBoxGeometry(size.x, size.y, size.z), *material, true);

	return colliderShape;
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

//void ModulePhysics::DrawCollider(C_Collider* collider)
//{
//
//
//	float4x4 transform = collider->transform->GetGlobalTransposed() * PhysXTransformToF4F(collider->colliderShape->getLocalPose());
//
//	switch (collider->type)
//	{
//	case GeometryType::BOX:
//	{
//		PxBoxGeometry boxCollider;
//		collider->colliderShape->getBoxGeometry(boxCollider);
//		float3 size = { boxCollider.halfExtents.x, boxCollider.halfExtents.y, boxCollider.halfExtents.z };
//		App->renderer3D->DrawBoxCollider(transform, size);
//	}
//	break;
//	case GeometryType::SPHERE:
//	{
//		PxSphereGeometry sphereCollider;
//		collider->colliderShape->getSphereGeometry(sphereCollider);
//		App->renderer3D->DrawSphereCollider(transform, sphereCollider.radius);
//	}
//	break;
//	case GeometryType::CAPSULE:
//	{
//		PxCapsuleGeometry capsuleCollider;
//		collider->colliderShape->getCapsuleGeometry(capsuleCollider);
//		App->renderer3D->DrawCapsuleCollider(transform, capsuleCollider.halfHeight, capsuleCollider.radius);
//	}
//	break;
//
//	}
//}
//
//void ModulePhysics::WakeUpGeometry(GameObject* gameObject) {
//	gameObject->GetRigidbody()->rigid_dynamic->wakeUp();
//}


PxTransform ModulePhysics::TRStoPxTransform(float3 pos, float3 rot) {

	Quat rotation = Quat::FromEulerXYZ(rot.x * DEGTORAD, rot.y * DEGTORAD, rot.z * DEGTORAD);

	PxVec3 Pxposition(pos.x, pos.y, pos.z);
	PxQuat PxRotation(rotation.x, rotation.y, rotation.z, rotation.w);

	return PxTransform(Pxposition, PxRotation);
}