#pragma once

#include "Module.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "PhysX/include/common/PxCoreUtilityTypes.h"
#include "PhysX/include/PxPhysicsAPI.h"
#include "PhysX/include/extensions/PxDefaultAllocator.h"
#include "GameObject.h"
#include "Globals.h"
class ComponentCollider;
class C_Collider;

namespace physx
{
    class PxPvd;
    class PxPvdSceneClient;
    class PxFoundation;
    class PxPhysics;
    class PxScene;
    class PxMaterial;
    class PxActor;
    class PxActorShape;
    class PxRigidBody;
    class PxRigidActor;
    class PxRigidStatic;
    class PxRigidDynamic;
    class PxControllerManager;
    class PxVolumeCache;
    class PxSimulationEventCallback;
    class PxQueryFilterCallback;
    class RaycastCCDManager;
    class PxCooking;
    class PxConvexMesh;
    class PxBase;
    class PxDefaultCpuDispatcher;
    class PxGeometry;
    class PxGeometryHolder;
    class PxShape;
    class PxVec3;
    class PxJoint;
    class PxFilterData;
    class PxQueryHitType;
    class PxDefaultAllocator;

    //class PxFixedSizeLookupTable;

    //Vehicles
    class PxVehicleDrive4W;
    class PxVehicleChassisData;
    class PxVehicleWheelsSimData;
    class PxVehicleDriveSimData4W;
    class PxVehicleDrivableSurfaceToTireFrictionPairs;
    class PxVehicleWheels;
    class PxVehicleKeySmoothingData;
    class PxVehiclePadSmoothingData;
    class PxVehicleDrive4WRawInputData;

    typedef uint32_t PxU32;
    typedef float PxF32;

};

enum class JointType {
    FIXED,
    DISTANCE,
    SPHERICAL,
    REVOLUTE,
    PRISMATIC,
    D6,
};

class ModulePhysics : public Module {

public:

    ModulePhysics(Application* app, bool start_enabled = true);
    ~ModulePhysics();

    bool Init();
    update_status Update(float dt);
    bool CleanUp();

    void SceneSimulation(float gameTimesetp, bool fetchResults = true);
    void RenderGeometry();

   // void renderActors(physx::PxRigidActor** actors, const physx::PxU32 numActors, bool shadows);
   // void renderGeometry(const physx::PxGeometry& geom);
    void renderGeometryHolder(const physx::PxGeometryHolder& h);

   // void DrawGeometry(GeometryType type, float3 pos = { 0.0f, 0.0f, 0.0f }, float radius = 3.0f, float3 size = { 1.0f, 1.0f, 1.0f });
    void DrawCollider(C_Collider* collider);
    float4x4 PhysXTransformToF4F(physx::PxTransform transform);

    void WakeUpGeometry(GameObject* gameObject);
    // SPACE - M - N testing geometries
  //  physx::PxRigidDynamic* CreateGeometry(GeometryType type = GeometryType::NONE, float3 pos = { 0.0f, 0.0f, 0.0f }, float mass = 10.0f, float radius = 3.0f, float3 size = { 1.0f, 1.0f, 1.0f });

    physx::PxRigidStatic* CreateRigidStatic(float3 pos);
    physx::PxRigidDynamic* CreateRigidDynamic(float3 pos);
    physx::PxShape* CreateCollider(float3 size, physx::PxMaterial* material = nullptr);
    physx::PxMaterial* CreateMaterial(float staticFriction = 0.5f, float dynamicFriction = 0.5f, float restitution = 0.1f);

    //Release rigid Static/Dynamic actors and detach collider shapes if needed.
    void ReleaseActor(physx::PxRigidActor* actor);

    physx::PxTransform TRStoPxTransform(float3 pos, float3 rot);

public:
    physx::PxFoundation* mFoundation;
    physx::PxPhysics* mPhysics;
    physx::PxPvd* mPvd;
    physx::PxCooking* mCooking;
    physx::PxMaterial* mMaterial;
    physx::PxScene* mScene;
    physx::PxDefaultCpuDispatcher* mDispatcher;
    physx::PxDefaultAllocator		mAllocator;

    float3 gravity;
};