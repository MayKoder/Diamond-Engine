#include "PE_Color_Over_Lifetime.h"
#include "Particle.h"

#include "ImGui/imgui.h"


PE_ColorOverLifetime::PE_ColorOverLifetime() :ParticleEffect(PARTICLE_EFFECT_TYPE::COLOR_OVER_LIFETIME), editGradient(false)
{
}

PE_ColorOverLifetime::~PE_ColorOverLifetime()
{
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
			if (ImGui::Button(suffixLabel.c_str()))
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

	JSON_Value* gradientArray = json_value_init_array();
	JSON_Array* jsArray = json_value_get_array(gradientArray);

	
	for (std::list<ImGradientMark*>::iterator i = marks.begin(); i != marks.end(); ++i)
	{
		JSON_Value* nVal = json_value_init_object();
		JSON_Object* obj = json_value_get_object(nVal);

		DEJson::WriteVector4(obj, "paColorOLTcolor", i._Ptr->_Myval->color);
		DEJson::WriteFloat(obj, "paColorOLTposition", i._Ptr->_Myval->position);

		json_array_append_value(jsArray, nVal);
	}	

	json_object_set_value(nObj, "Gradients", gradientArray);
}


void PE_ColorOverLifetime::LoadData(DEConfig& nObj)
{
	gradient.clearMarks();
	ParticleEffect::LoadData(nObj);

	DEConfig conf(nullptr);
	JSON_Array* effArray = json_object_get_array(nObj.nObj, "Gradients");
	for (size_t i = 0; i < json_array_get_count(effArray); ++i)
	{
		conf.nObj = json_array_get_object(effArray, i);
		
		float4 col = conf.ReadVector4("paColorOLTcolor");
		float pos = conf.ReadFloat("paColorOLTposition");

		gradient.addMark(pos, ImColor(col.x, col.y, col.z, col.w));
	}
}