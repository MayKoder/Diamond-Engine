#include "PE_Force_Over_Lifetime.h"
#include "Particle.h"

#include "ImGui/imgui.h"

PE_ForceOverLifetime::PE_ForceOverLifetime() : ParticleEffect(PARTICLE_EFFECT_TYPE::FORCE_OVER_LIFETIME),
forceType(FORCE_TYPE::DIRECTION), forceMagitude(0.0f)
{
	memset(direction, 0.f, sizeof(direction));

	myGravity = -9.8f;
	gravityModifier = 1.0f;
}


PE_ForceOverLifetime::~PE_ForceOverLifetime()
{
}


void PE_ForceOverLifetime::Update(Particle& particle, float dt)
{
	float3 dirVec = float3(direction[0], direction[1], direction[2]);

	switch (forceType)
	{
	case FORCE_TYPE::DIRECTION:
		particle.accel = dirVec.Normalized() * forceMagitude;
		break;
	case FORCE_TYPE::POINT:
		particle.accel = (dirVec - particle.pos).Normalized() * forceMagitude;
		break;
	case FORCE_TYPE::MAX:
	default:
		LOG(LogType::L_ERROR, "You should be here. Theres no force type assigned");
		break;
	}


	particle.accel.y += myGravity;
}


#ifndef STANDALONE
void PE_ForceOverLifetime::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Force Over Lifetime Effect##";
	suffixLabel += emitterIndex;
	if (ImGui::CollapsingHeader(suffixLabel.c_str(), ImGuiTreeNodeFlags_Bullet))
	{
		suffixLabel = "Delete Force Over Lifetime Effect##";
		suffixLabel += emitterIndex;
		if (ImGui::Button(suffixLabel.c_str()))
			this->toDelete = true;

		ImGui::Spacing();
		ImGui::Indent();


		//=========================================== Combo
		suffixLabel = "Force Type##ForceComboTitle";
		suffixLabel += emitterIndex;

		std::string textNameDisplay = ForceTypeToString(forceType);

		textNameDisplay += "##ForceComboPreview";
		textNameDisplay += emitterIndex;
		if (ImGui::BeginCombo(suffixLabel.c_str(), textNameDisplay.c_str(), ImGuiComboFlags_PopupAlignLeft))
		{
			//================================
			for (int n = 0; n <= (int)FORCE_TYPE::MAX; ++n)
			{
				FORCE_TYPE iterType = (FORCE_TYPE)n;

				bool isModeSelected = (forceType == iterType);


				suffixLabel = ForceTypeToString(iterType);
				suffixLabel += "##ForceComboList";
				suffixLabel += emitterIndex;

				if (ImGui::Selectable(suffixLabel.c_str(), isModeSelected))
				{
					forceType = iterType;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (isModeSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
		//=========================================== end combo


		if (forceType == FORCE_TYPE::DIRECTION)
		{
			suffixLabel = "Force Direction##DirectionalForce";
		}
		else
		{
			suffixLabel = "Force Point##DirectionalForce";

		}
		suffixLabel += emitterIndex;
		ImGui::DragFloat3(suffixLabel.c_str(), direction, 0.1f);

		suffixLabel = "Force Magnitude##MagnitudeForce";
		suffixLabel += emitterIndex;
		ImGui::DragFloat(suffixLabel.c_str(), &forceMagitude);

		suffixLabel = "Gravity Modifier##GravityForce";
		suffixLabel += emitterIndex;
		if (ImGui::DragFloat(suffixLabel.c_str(), &gravityModifier, 0.1f))
		{
			myGravity = -9.8f * gravityModifier;
		}

		ImGui::Unindent();
	}
}
#endif // !STANDALONE


void PE_ForceOverLifetime::SaveData(JSON_Object* nObj)
{
	ParticleEffect::SaveData(nObj);

	DEJson::WriteVector3(nObj, "paFOLT_direction", direction);
	DEJson::WriteFloat(nObj, "paFOLT_magnitude", forceMagitude);
	DEJson::WriteFloat(nObj, "paFOLT_gravityMod", gravityModifier);
	DEJson::WriteFloat(nObj, "paFOLT_gravity", myGravity);
	DEJson::WriteInt(nObj, "paFOLT_forceType", (int)forceType);
}


void PE_ForceOverLifetime::LoadData(DEConfig& nObj)
{
	ParticleEffect::LoadData(nObj);
	float3 acc = nObj.ReadVector3("paFOLT_direction");
	direction[0] = acc.x;
	direction[1] = acc.y;
	direction[2] = acc.z;

	forceMagitude = nObj.ReadFloat("paFOLT_magnitude");
	gravityModifier = nObj.ReadFloat("paFOLT_gravityMod");
	myGravity = nObj.ReadFloat("paFOLT_gravity");
	forceType = (FORCE_TYPE)nObj.ReadInt("paFOLT_forceType");
}

std::string PE_ForceOverLifetime::ForceTypeToString(FORCE_TYPE type)
{
	std::string ret = "NONE";

	switch (type)
	{
	case FORCE_TYPE::DIRECTION:
		ret = "DIRECTION";
		break;
	case FORCE_TYPE::POINT:
		ret = "POINT";
		break;
	case FORCE_TYPE::MAX:
		break;
	default:
		break;
	}
	return ret;
}
