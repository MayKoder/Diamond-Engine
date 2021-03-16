#include "PE_Color_Over_Lifetime.h"
#include "Particle.h"

#include "ImGui/imgui.h"


PE_ColorOverLifetime::PE_ColorOverLifetime() :ParticleEffect(PARTICLE_EFFECT_TYPE::COLOR_OVER_LIFETIME), editGradient(false)
{
}

PE_ColorOverLifetime::~PE_ColorOverLifetime()
{
	gradient.getMarks().clear();
}


void PE_ColorOverLifetime::Update(Particle& particle, float dt)
{
	//Linear downgrade - The particle's visibility goes down linearly with time
	if (particle.maxLifetime != 0.0f)
	{
		float percentatge = particle.currentLifetime / particle.maxLifetime;
		float myColor[4];
		gradient.getColorAt(percentatge, myColor);
		particle.color = { myColor[0] ,myColor[1] ,myColor[2] ,myColor[3] };
	}
}


#ifndef STANDALONE
void PE_ColorOverLifetime::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "Color Over Lifetime Effect##";
	suffixLabel += emitterIndex;
	if (ImGui::CollapsingHeader(suffixLabel.c_str(), ImGuiTreeNodeFlags_Leaf))
	{
		if (editGradient)
		{
			suffixLabel = "Save Edited Gradient##ColorGradient";
			suffixLabel += emitterIndex;
			if(ImGui::Button(suffixLabel.c_str()))
			{
				editGradient = false;
			}

			static ImGradientMark* draggingMark = nullptr;
			static ImGradientMark* selectedMark = nullptr;

			ImGui::GradientEditor(&gradient, draggingMark, selectedMark);

		}
		else
		{
			if (ImGui::GradientButton(&gradient))
			{
				editGradient = true;
			}
		}
	}

}
#endif // !STANDALONE


void PE_ColorOverLifetime::SaveData(JSON_Object* nObj)
{
	ParticleEffect::SaveData(nObj);
	
	std::list<ImGradientMark*> marks = gradient.getMarks();

	std::list<ImGradientMark*>::iterator i = marks.begin();

	DEJson::WriteVector4(nObj, "paColorOLTcolor1", i._Ptr->_Myval->color);
	DEJson::WriteFloat(nObj, "paColorOLTposition1", i._Ptr->_Myval->position);
	i++;

	DEJson::WriteVector4(nObj, "paColorOLTcolor2", i._Ptr->_Myval->color);
	DEJson::WriteFloat(nObj, "paColorOLTposition2", i._Ptr->_Myval->position);
	
}


void PE_ColorOverLifetime::LoadData(DEConfig& nObj)
{
	ParticleEffect::LoadData(nObj);

	float4 col = nObj.ReadVector4("paColorOLTcolor1");
	float pos = nObj.ReadFloat("paColorOLTposition1");

	gradient.addMark(pos, ImColor(col.x, col.y, col.z, col.w));

	col = nObj.ReadVector4("paColorOLTcolor2");
	pos = nObj.ReadFloat("paColorOLTposition2");
	gradient.addMark(pos, ImColor(col.x, col.y, col.z, col.w));
}