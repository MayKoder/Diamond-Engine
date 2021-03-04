#pragma once

#include "Module.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "PhysX/include/common/PxCoreUtilityTypes.h"
#include "Physx/include/PxSimulationEventCallback.h"
#include "PhysX/include/PxPhysicsAPI.h"
#include "PhysX/include/extensions/PxDefaultAllocator.h"
#include "GameObject.h"
#include "Globals.h"

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}

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

    typedef uint32_t PxU32;
    typedef float PxF32;

    struct PxContactPairHeader;
    struct PxContactPair;
};

enum class JointType {
    FIXED,
    DISTANCE,
    SPHERICAL,
    REVOLUTE,
    PRISMATIC,
    D6,
};

class CollisionDetector : public physx::PxSimulationEventCallback {
public:
    CollisionDetector();
    ~CollisionDetector();

    void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)override;
    void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;

    void onWake(physx::PxActor** actors, physx::PxU32 count)override
    {}
    void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count)override
    {}
    void onSleep(physx::PxActor** actors, physx::PxU32 count) override
    {}
   
    void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override
    {}

    physx::PxFilterFlags CollisionDetector::CollisionDetectorFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
        physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
        physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);
}; // end class

class ModulePhysics : public Module {

public:

    ModulePhysics(Application* app, bool start_enabled = true);
    ~ModulePhysics();

    bool Init();
    update_status PreUpdate(float dt) override;
    update_status Update(float dt) override;
    bool CleanUp();

    void SceneSimulation(double gameTimesetp, bool fetchResults = true);
    void RenderGeometry();


    float4x4 PhysXTransformToF4F(physx::PxTransform transform);

 

    physx::PxRigidStatic* CreateRigidStatic(float3 pos);
    physx::PxRigidDynamic* CreateRigidDynamic(float3 pos, Quat rot);
    physx::PxShape* CreateCollider(float3 size, physx::PxMaterial* material = nullptr);
    physx::PxMaterial* CreateMaterial(float staticFriction = 0.5f, float dynamicFriction = 0.5f, float restitution = 0.1f);

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
    CollisionDetector detector;

    float3 gravity;
};

