#include "PE_Shape.h"
#include "PE__Spawn_Shape_Base.h"

#include "PE_Spawn_Area.h"
#include "PE_Spawn_Cone.h"
#include "PE_Spawn_Sphere.h"

#include "CO_Transform.h"
#include "MathGeoLib/include/Math/float4x4.h"

#include "Application.h"
#include "ImGui/imgui.h"

PE_Shape::PE_Shape(C_Transform* objTransform) : ParticleEffect(PARTICLE_EFFECT_TYPE::SHAPE),
mySpawnShape(nullptr), hasInitialSpeed(false), particlesSpeed(0.0f), randomSpeedMultiplier(0.0f), objectTransform(nullptr)
{
	memset(shapeOffset, 0.0f, sizeof(shapeOffset));
	objectTransform = objTransform;
}

PE_Shape::~PE_Shape()
{
	if (mySpawnShape != nullptr)
	{
		delete(mySpawnShape);
		mySpawnShape = nullptr;
	}
	objectTransform = nullptr;
}

void PE_Shape::Spawn(Particle& particle)
{
	if (mySpawnShape != nullptr)
	{
		float initSpeed = 0.0f;
		if (hasInitialSpeed)
		{
			initSpeed = particlesSpeed;

			if (randomSpeedMultiplier != 0.0f)
			{
				initSpeed += initSpeed * randomSpeedMultiplier * EngineExternal->GetRandomFloat(-1.0f, 1.0f);
			}
		}

		mySpawnShape->Spawn(particle, hasInitialSpeed, initSpeed, objectTransform->globalTransform, shapeOffset);
	}
}

void PE_Shape::PrepareEffect()
{

}


#ifndef STANDALONE
void PE_Shape::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Shape Effect##";
	suffixLabel += emitterIndex;
	if (ImGui::CollapsingHeader(suffixLabel.c_str(), ImGuiTreeNodeFlags_Bullet))
	{
		suffixLabel = "Delete Shape Effect##";
		suffixLabel += emitterIndex;
		if (ImGui::Button(suffixLabel.c_str()))//TODO all particle effects delete buttons should look like this fix them
			this->toDelete = true;


		ImGui::Spacing();
		ImGui::Indent();

		suffixLabel = "Has Initial Speed##PaShapeEf";
		suffixLabel += emitterIndex;
		ImGui::Checkbox(suffixLabel.c_str(), &hasInitialSpeed);

		if (hasInitialSpeed)
		{
			suffixLabel = "Initial Out Speed##PaShapeEf";
			suffixLabel += emitterIndex;
			ImGui::DragFloat(suffixLabel.c_str(), &particlesSpeed);

			suffixLabel = "Random Speed Multiplier##PaShapeEf";
			suffixLabel += emitterIndex;
			ImGui::DragFloat(suffixLabel.c_str(), &randomSpeedMultiplier);

		}

		suffixLabel = "Shape Offset##PaShapeEf";
		suffixLabel += emitterIndex;
		ImGui::DragFloat3(suffixLabel.c_str(), shapeOffset);


		//=========================================== Combo
		suffixLabel = "Shape Type##PaShapeEf";
		suffixLabel += emitterIndex;

		std::string textNameDisplay = "NONE";
		if (mySpawnShape != nullptr)
		{
			GetShapeTypeString(mySpawnShape->GetType(), textNameDisplay);
		}

		textNameDisplay += "##PaShapeEf";
		textNameDisplay += emitterIndex;
		if (ImGui::BeginCombo(suffixLabel.c_str(), textNameDisplay.c_str(), ImGuiComboFlags_PopupAlignLeft))
		{
			suffixLabel = "NONE##PaShapeEf";
			suffixLabel += emitterIndex;
			const bool noneSelected = (mySpawnShape == nullptr);
			if (ImGui::Selectable(suffixLabel.c_str(), noneSelected))
			{
				if (mySpawnShape != nullptr)
				{
					ChangeSpawnShape(PE_SPAWN_SHAPE_TYPE::MAX);
				}
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (noneSelected)
				ImGui::SetItemDefaultFocus();

			//================================
			for (int n = 0; n < (int)PE_SPAWN_SHAPE_TYPE::MAX; ++n)
			{
				PE_SPAWN_SHAPE_TYPE iterType = (PE_SPAWN_SHAPE_TYPE)n;

				bool isShapeSelected = false;
				if (mySpawnShape != nullptr)
				{
					isShapeSelected = (mySpawnShape->GetType() == iterType);
				}

				GetShapeTypeString(iterType, suffixLabel);
				suffixLabel += "##ListPaShapeEf";
				suffixLabel += emitterIndex;

				if (ImGui::Selectable(suffixLabel.c_str(), isShapeSelected))
				{
					ChangeSpawnShape((PE_SPAWN_SHAPE_TYPE)n);
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (isShapeSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
		//=========================================== end combo

		if (mySpawnShape != nullptr)
		{
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Indent();

			mySpawnShape->OnEditor(emitterIndex);

			ImGui::Unindent();
			ImGui::Spacing();
			ImGui::Separator();
		}
		ImGui::Unindent();
	}
}
#endif // !STANDALONE

void PE_Shape::SaveData(JSON_Object* nObj)
{
	ParticleEffect::SaveData(nObj);
	DEJson::WriteBool(nObj, "paShapeHasInitSpeed", hasInitialSpeed);
	DEJson::WriteFloat(nObj, "paShapePartSpeed", particlesSpeed);
	DEJson::WriteFloat(nObj, "paShapeRandSpeed", randomSpeedMultiplier);

	PE_SPAWN_SHAPE_TYPE saveType = PE_SPAWN_SHAPE_TYPE::MAX;
	if (mySpawnShape != nullptr)
	{
		saveType = mySpawnShape->GetType();
		mySpawnShape->SaveData(nObj);
	}
	DEJson::WriteInt(nObj, "paShapeType", (int)saveType);
	DEJson::WriteVector3(nObj, "PaShapeOffset", shapeOffset);

}

void PE_Shape::LoadData(DEConfig& nObj)
{
	ParticleEffect::LoadData(nObj);
	hasInitialSpeed = nObj.ReadBool("paShapeHasInitSpeed");
	particlesSpeed = nObj.ReadFloat("paShapePartSpeed");
	randomSpeedMultiplier = nObj.ReadFloat("paShapeRandSpeed");

	mySpawnShape = ChangeSpawnShape((PE_SPAWN_SHAPE_TYPE)nObj.ReadInt("paShapeType"));

	if (mySpawnShape != nullptr)
	{
		mySpawnShape->LoadData(nObj);
	}

	float3 offset = nObj.ReadVector3("PaShapeOffset");
	shapeOffset[0] = offset.x;
	shapeOffset[1] = offset.y;
	shapeOffset[2] = offset.z;
}

PE_SpawnShapeBase* PE_Shape::ChangeSpawnShape(PE_SPAWN_SHAPE_TYPE newType)
{

	if (mySpawnShape == nullptr)
	{
		mySpawnShape = CreateSpawnShape(newType);
	}
	else if (newType != mySpawnShape->GetType())
	{
		delete(mySpawnShape);
		mySpawnShape = nullptr;
		mySpawnShape = CreateSpawnShape(newType);

	}

	return mySpawnShape;
}

PE_SpawnShapeBase* PE_Shape::CreateSpawnShape(PE_SPAWN_SHAPE_TYPE newType)
{
	PE_SpawnShapeBase* newShape = nullptr;
	//TODO construct different shapes here factory method
	switch (newType)
	{
	case PE_SPAWN_SHAPE_TYPE::AREA:
		newShape = new PE_SpawnShapeArea();
		break;
	case PE_SPAWN_SHAPE_TYPE::SPHERE:
		newShape = new PE_SpawnShapeSphere();
		break;
	case PE_SPAWN_SHAPE_TYPE::CONE:
		newShape = new PE_SpawnShapeCone();
		break;
	case PE_SPAWN_SHAPE_TYPE::MAX:
		break;
	default:
		break;
	}
	return newShape;
}

void PE_Shape::GetShapeTypeString(PE_SPAWN_SHAPE_TYPE newType, std::string& out)
{
	out = "NONE";

	switch (newType)
	{
	case PE_SPAWN_SHAPE_TYPE::AREA:
		out = "AREA";
		break;
	case PE_SPAWN_SHAPE_TYPE::SPHERE:
		out = "SPHERE";
		break;
	case PE_SPAWN_SHAPE_TYPE::CONE:
		out = "CONE";
		break;
	case PE_SPAWN_SHAPE_TYPE::MAX:
		break;
	default:
		break;
	}
}
