#include "ParticleEmitter.h"
#include "Globals.h"

#include "PE_Force_Over_Lifetime.h"
#include "PE_Rotate_Over_Lifetime.h"
#include "PE_Size_Over_Lifetime.h"
#include "PE_Color_Over_Lifetime.h"
#include "PE_Velocity_Over_Lifetime.h"
#include "PE_Shape.h"

#include <string>

#include "Application.h"
#include "MO_ResourceManager.h"
#include "MO_Renderer3D.h"

#include "RE_Texture.h"

#include "CO_Camera.h"
#include "CO_Transform.h"

#include "OpenGL.h"
#include "ImGui/imgui.h"
#include "MathGeoLib/include/Math/float4x4.h"

#include "MO_Editor.h"

#include <string>

Emitter::Emitter() :
	toDelete(false),
	VAO(0u),
	instanceVBO(0u),
	vertexVBO(0u),
	texture(nullptr),
	particlesPerSec(0.0f),
	secPerParticle(0.0f),
	lastParticeTime(0.0f),
	lastUsedParticle(0),
	myParticles(),
	myEffects(),
	objTransform(nullptr),
	delaying(false),
	maxDelay(0.0f),
	emitterName("")
{
	memset(particlesLifeTime, 0.1f, sizeof(particlesLifeTime));
	memset(particlesSize, 1.0f, sizeof(particlesSize));
	memset(particlesColor, 1.0f, sizeof(particlesColor));

	particlesSize[0] = 1.0f;
	particlesSize[1] = 1.0f;

	particlesColor[0] = 1;
	particlesColor[1] = 1;
	particlesColor[2] = 1;
	particlesColor[3] = 1;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &vertexVBO);
	glGenBuffers(1, &instanceVBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particleVertices), particleVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glVertexAttribDivisor(0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MAX_PARTICLES * INSTANCE_DATA_LENGHT, NULL, GL_STREAM_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	AddInstancedAttribute(VAO, instanceVBO, 1, 4, INSTANCE_DATA_LENGHT, 0);
	AddInstancedAttribute(VAO, instanceVBO, 2, 4, INSTANCE_DATA_LENGHT, 4);
	AddInstancedAttribute(VAO, instanceVBO, 3, 4, INSTANCE_DATA_LENGHT, 8);
	AddInstancedAttribute(VAO, instanceVBO, 4, 4, INSTANCE_DATA_LENGHT, 12);
	AddInstancedAttribute(VAO, instanceVBO, 5, 4, INSTANCE_DATA_LENGHT, 16);


	CalculatePoolSize();
}


Emitter::~Emitter()
{
	glDeleteBuffers(1, &vertexVBO);
	glDeleteBuffers(1, &instanceVBO);
	glDeleteVertexArrays(1, &VAO);

	vertexVBO = 0;
	instanceVBO = 0;
	VAO = 0;

	if (texture != nullptr)
	{
		EngineExternal->moduleResources->UnloadResource(texture->GetUID());
		texture = nullptr;
	}

	for (int i = myEffects.size() - 1; i >= 0; --i)
	{
		if (myEffects[i] != nullptr)
		{
			delete(myEffects[i]);
			myEffects[i] = nullptr;
		}
		myEffects.erase(myEffects.begin() + i);
	}
	myEffects.clear();
	myParticles.clear();
	emitterName.clear();
	objTransform = nullptr;
	delay.Stop();
}


void Emitter::Update(float dt, bool systemActive)
{
	if (delaying)
	{
		if (delay.Read() >= maxDelay * 1000)
		{
			delay.Stop();
			delaying = false;
		}
	}

	if (delaying == false)
	{
		for (int i = 0; i < myEffects.size(); ++i)
		{
			myEffects[i]->PrepareEffect();
		}


		if (systemActive)
			ThrowParticles(dt);

		for (int i = 0; i < myParticles.size(); ++i)
		{
			if (myParticles[i].currentLifetime >= 0.0f)
			{
				myParticles[i].currentLifetime -= dt;
				for (int j = 0; j < myEffects.size(); ++j)
				{
					myEffects[j]->Update(myParticles[i], dt);
				}

				myParticles[i].speed += myParticles[i].accel * dt;
				myParticles[i].pos += myParticles[i].speed * dt;
			}
		}
	}
	else {
		for (int i = 0; i < myParticles.size(); ++i)
		{
			myParticles[i].currentLifetime = -1.0f;
		}
	}
}


void Emitter::Draw(unsigned int shaderId, Quat newRotation)
{
	if (myParticles.empty() == true)
		return;


	if (texture != nullptr)
		glBindTexture(GL_TEXTURE_2D, texture->textureID);

	GLint modelLoc = glGetUniformLocation(shaderId, "view");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, EngineExternal->moduleRenderer3D->activeRenderCamera->ViewMatrixOpenGL().ptr());

	modelLoc = glGetUniformLocation(shaderId, "projection");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, EngineExternal->moduleRenderer3D->activeRenderCamera->ProjectionMatrixOpenGL().ptr());

	int particlesAlive = 0;
	std::vector<float> vboInfo;
	vboInfo.reserve(sizeof(float) * myParticles.size() * 16);

	int particlesCount = myParticles.size();

	const size_t sizeofFloat4x4 = sizeof(float4x4);
	const size_t sizeofFloat4 = sizeof(float4);

	for (int i = 0; i < particlesCount; ++i)	//Need to order particles
	{
		if (myParticles[i].currentLifetime > 0.0f)
		{
			//new
			Quat finalParticleRot = newRotation * Quat::RotateAxisAngle({ 0.0f,0.0f,1.0f }, myParticles[i].rotation);
			float4x4 matrix = float4x4::FromTRS(myParticles[i].pos, finalParticleRot, float3(myParticles[i].size, myParticles[i].size, 1)).Transposed();

			int lastIndex = vboInfo.size();
			vboInfo.resize(lastIndex + INSTANCE_DATA_LENGHT);

			memcpy(&vboInfo[lastIndex], matrix.v, sizeofFloat4x4);
			memcpy(&vboInfo[lastIndex + 16], &myParticles[i].color, sizeofFloat4);
			//end new

			++particlesAlive;
		}
	}

	if (particlesAlive == 0)
		return;

	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * particlesAlive * INSTANCE_DATA_LENGHT, NULL, GL_STREAM_DRAW);//possible problem here?
	glBufferSubData(GL_ARRAY_BUFFER, 0, vboInfo.size() * sizeof(float), &vboInfo[0]);//possible problem here?
	glBindBuffer(GL_ARRAY_BUFFER, 0);



	//==================================

	glBindVertexArray(VAO);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, particlesAlive);

	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	vboInfo.clear();
}


#ifndef STANDALONE
void Emitter::OnEditor(int emitterIndex)
{
	std::string suffixLabel = "##";
	suffixLabel += emitterIndex;
	std::string guiName = "Emitter: '" + emitterName + "'" + suffixLabel;

	if (ImGui::CollapsingHeader(guiName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		strcpy(emitterNameChars, emitterName.c_str());
		guiName = suffixLabel + "Name";
		std::string hint = "Insert Emitter Name Here";
		if (ImGui::InputTextWithHint(guiName.c_str(), hint.c_str(), &emitterNameChars[0], sizeof(emitterNameChars)))
		{
			if (emitterNameChars[0] != '\0')
				emitterName = emitterNameChars;
		}
		ImGui::SameLine();
		guiName = "Delete Emitter" + suffixLabel;
		if (ImGui::Button(guiName.c_str()))
		{
			toDelete = true;
		}

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Indent();


		//IMAGE TEXTURE ===================================================================================
		{

			if (texture != nullptr)
			{
				if (ImGui::ImageButton((ImTextureID)texture->textureID, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0)))
				{
					EngineExternal->moduleResources->UnloadResource(texture->GetUID());
				}
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
					ImGui::Text("Click on the image to erase it");
					ImGui::PopStyleColor();
					ImGui::EndTooltip();
				}
			}
			else
			{
				ImGui::Image((ImTextureID)0, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("_TEXTURE"))
				{
					//Drop asset from Asset window to scene window
					std::string* metaFileDrop = (std::string*)payload->Data;

					if (texture != nullptr)
						EngineExternal->moduleResources->UnloadResource(texture->GetUID());

					std::string libraryName = EngineExternal->moduleResources->LibraryFromMeta(metaFileDrop->c_str());

					texture = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(EngineExternal->moduleResources->GetMetaUID(metaFileDrop->c_str()), libraryName.c_str()));
					LOG(LogType::L_WARNING, "File %s loaded to scene", (*metaFileDrop).c_str());
				}
				ImGui::EndDragDropTarget();
			}

		}
		//END OF IMAGE TEXTURE ============================================================================

		guiName = "Delay" + suffixLabel;
		ImGui::SliderFloat(guiName.c_str(), &maxDelay, 0.0f, 10.0f);

		guiName = "Particle Lifetime" + suffixLabel;
		if (ImGui::DragFloatRange2(guiName.c_str(), &particlesLifeTime[0], &particlesLifeTime[1], 0.25f, 0.01f, 100.0f, "Min: %.01f", "Max: %.01f"))
			CalculatePoolSize();

		guiName = "Particles per Second" + suffixLabel;
		if (ImGui::DragFloat(guiName.c_str(), &particlesPerSec))
			SetParticlesPerSec(particlesPerSec);

		guiName = "Start Size" + suffixLabel;
		ImGui::DragFloatRange2(guiName.c_str(), &particlesSize[0], &particlesSize[1], 0.25f, 0.01f, 5.0f, "Min: %.01f", "Max: %.01f");

		//It doesn't make sense to have Start Speed when we have the velocity effect
		//guiName = "Start Speed" + suffixLabel;
		//ImGui::DragFloatRange2(guiName.c_str(), &particlesSpeed[0], &particlesSpeed[1], 0.25f, 0.1f, 5.0f, "Min: %.1f", "Max: %.1f");			

		guiName = "Start Color (RGBA)" + suffixLabel;
		ImGui::ColorPicker4(guiName.c_str(), particlesColor);

		for (int i = (int)PARTICLE_EFFECT_TYPE::NONE + 1; i < (int)PARTICLE_EFFECT_TYPE::MAX; ++i)
		{
			int index = DoesEffectExist((PARTICLE_EFFECT_TYPE)i);
			if (index != -1)
			{
				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();
				ImGui::Spacing();

				myEffects[index]->OnEditor(emitterIndex);

			}
			/*else
			{
				guiName = (ParticleEffectEnumToString((PARTICLE_EFFECT_TYPE)i)) + suffixLabel;
				if (ImGui::Button(guiName.c_str()))
				{
					CreateEffect((PARTICLE_EFFECT_TYPE)i);
				}
			}*/
		}



		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();


		//COMBO ===============================================
		{

			guiName = "Add Effect##PaShapeEf";
			guiName += emitterIndex;

			std::string textNameDisplay = "NEW EFFECT";

			textNameDisplay += "##PaShapeEf";
			textNameDisplay += emitterIndex;
			if (ImGui::BeginCombo(guiName.c_str(), textNameDisplay.c_str()))
			{
				//================================
				for (int n = (int)PARTICLE_EFFECT_TYPE::NONE + 1; n < (int)PARTICLE_EFFECT_TYPE::MAX; ++n)
				{

					guiName = (ParticleEffectEnumToString((PARTICLE_EFFECT_TYPE)n)) + suffixLabel;

					int index = DoesEffectExist((PARTICLE_EFFECT_TYPE)n);
					if (index == -1)
					{
						//if the effect hasnt been created
						if (ImGui::Selectable(guiName.c_str()))
						{
							CreateEffect((PARTICLE_EFFECT_TYPE)n);
						}
					}
				}

				ImGui::EndCombo();
			}
		}
		//END COMBO ===========================================
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Unindent();
		ImGui::Separator();
		ImGui::Spacing();

	}
}

void Emitter::TryDeleteUnusedEffects()
{
	//Delete effects
	for (int i = 0; i < myEffects.size(); ++i) {
		if (myEffects[i]->toDelete) {
			delete(myEffects[i]);
			myEffects[i] = nullptr;
			myEffects.erase(myEffects.begin() + i);
			--i;
		}
	}
}
#endif // !STANDALONE


void Emitter::AssignTransform(C_Transform* objTransform)
{
	this->objTransform = objTransform;
}


void Emitter::SaveData(JSON_Object* nObj)
{
	if (texture != nullptr)
	{
		DEJson::WriteString(nObj, "AssetPath", texture->GetAssetPath());
		DEJson::WriteString(nObj, "LibraryPath", texture->GetLibraryPath());
		DEJson::WriteInt(nObj, "texUID", texture->GetUID());
	}

	DEJson::WriteString(nObj, "paEmitterName", emitterName.c_str());

	DEJson::WriteVector2(nObj, "paLifeTime", particlesLifeTime);
	DEJson::WriteVector2(nObj, "paSize", particlesSize);
	DEJson::WriteVector4(nObj, "paColor", &particlesColor[0]);
	DEJson::WriteFloat(nObj, "paPerSec", particlesPerSec);
	JSON_Value* effectsArray = json_value_init_array();
	JSON_Array* jsArray = json_value_get_array(effectsArray);

	int effectsCount = myEffects.size();
	for (int i = 0; i < effectsCount; ++i)
	{
		JSON_Value* nVal = json_value_init_object();
		JSON_Object* obj = json_value_get_object(nVal);

		myEffects[i]->SaveData(obj);
		json_array_append_value(jsArray, nVal);
	}

	json_object_set_value(nObj, "ParticleEffects", effectsArray);
}


void Emitter::LoadData(DEConfig& nObj)
{
	std::string texPath = nObj.ReadString("AssetPath");
	std::string texName = nObj.ReadString("LibraryPath");

	emitterName = nObj.ReadString("paEmitterName");

	if (texName != "")
		texture = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(nObj.ReadInt("texUID"), texName.c_str()));

	float2 paLife = nObj.ReadVector2("paLifeTime");
	particlesLifeTime[0] = paLife.x;
	particlesLifeTime[1] = paLife.y;


	float2 paSize = nObj.ReadVector2("paSize");
	particlesSize[0] = paSize.x;
	particlesSize[1] = paSize.y;

	float4 paCol = nObj.ReadVector4("paColor");
	particlesColor[0] = paCol.x;
	particlesColor[1] = paCol.y;
	particlesColor[2] = paCol.z;
	particlesColor[3] = paCol.w;

	SetParticlesPerSec(nObj.ReadFloat("paPerSec"));

	DEConfig conf(nullptr);
	JSON_Array* effArray = json_object_get_array(nObj.nObj, "ParticleEffects");

	for (size_t i = 0; i < json_array_get_count(effArray); ++i)
	{
		conf.nObj = json_array_get_object(effArray, i);
		int type = conf.ReadInt("paEffectType");

		CreateEffect(static_cast<PARTICLE_EFFECT_TYPE>(type));
		myEffects[i]->LoadData(conf);
	}
}


void Emitter::RestartEmitter()
{
	lastUsedParticle = 0;
	lastParticeTime = secPerParticle - 0.016f;

	int particlesCount = myParticles.size();
	for (int i = 0; i < particlesCount; ++i)
	{
		myParticles[i].currentLifetime = -1.0f;
	}
}


void Emitter::CalculatePoolSize()
{
	int poolSize = ceilf(particlesPerSec * max(particlesLifeTime[0], particlesLifeTime[1]));

	if (poolSize < 0)
		poolSize = 0;

	SetPoolSize(poolSize);
}

void Emitter::SetPoolSize(unsigned int poolSize)
{
	myParticles.resize(poolSize);
	lastUsedParticle = 0;
}

void Emitter::CreateParticles(unsigned int particlesToAdd)
{
	//TODO this method is too similar to throw particles, consider merging these into a single method

	unsigned int lastIndex = myParticles.size();
	myParticles.resize(lastIndex + particlesToAdd);
	float3 startingPos = objTransform->globalTransform.TranslatePart();

	for (int i = lastIndex; i < myParticles.size(); ++i)
	{
		PrepareParticleToSpawn(myParticles[i], startingPos);

		for (int j = 0; j < myEffects.size(); ++j)
		{
			myEffects[j]->Spawn(myParticles[i]);
		}
	}
}

void Emitter::ThrowParticles(float dt)
{
	//find particles to spawn this frame
	float myDT = min(dt, 0.016f);

	float timeSinceLastThrow = myDT + lastParticeTime;

	float numberOfParticlesToSpawnF = timeSinceLastThrow * particlesPerSec;
	int numberOfParticlesToSpawn = numberOfParticlesToSpawnF;
	float extraParticle = numberOfParticlesToSpawnF - numberOfParticlesToSpawn;

	if (particlesPerSec > 0.0f)
	{
		lastParticeTime = extraParticle * secPerParticle;
	}
	else
	{
		lastParticeTime = 0.0f;
	}

	//LOG(LogType::L_NORMAL, "PARTICLES SPAWNED THIS FRAME %i", numberOfParticlesToSpawn);

	float3 startingPos = objTransform->globalTransform.TranslatePart();
	//spawn particles

	if (myParticles.empty() == false) {
		for (int i = 0; i < numberOfParticlesToSpawn; ++i)
		{
			int unusedIndex = FindUnusedParticle();

			PrepareParticleToSpawn(myParticles[unusedIndex], startingPos);

			for (int j = 0; j < myEffects.size(); ++j)
			{
				myEffects[j]->Spawn(myParticles[unusedIndex]);
			}
		}
	}
}

int Emitter::DoesEffectExist(PARTICLE_EFFECT_TYPE type)
{
	for (int i = 0; i < myEffects.size(); ++i)
	{
		if (myEffects[i] != nullptr && myEffects[i]->type == type)
		{
			return i;
		}
	}

	return -1;
}

std::string Emitter::ParticleEffectEnumToString(PARTICLE_EFFECT_TYPE type)
{
	std::string ret = "";
	switch (type)
	{
	case PARTICLE_EFFECT_TYPE::NONE:
		break;
	case PARTICLE_EFFECT_TYPE::FORCE_OVER_LIFETIME:
		ret = "Force Over Lifetime Effect";
		break;
	case PARTICLE_EFFECT_TYPE::ROTATE_OVER_LIFETIME:
		ret = "Rotate Over Lifetime Effect";
		break;
	case PARTICLE_EFFECT_TYPE::SIZE_OVER_LIFETIME:
		ret = "Size Over Lifetime Effect";
		break;
	case PARTICLE_EFFECT_TYPE::COLOR_OVER_LIFETIME:
		ret = "Color Over Lifetime Effect";
		break;
	case PARTICLE_EFFECT_TYPE::VELOCITY_OVER_LIFETIME:
		ret = "Velocity Over Lifetime Effect";
		break;
	case PARTICLE_EFFECT_TYPE::SHAPE:
		ret = "Shape Effect";
		break;
	case PARTICLE_EFFECT_TYPE::MAX:
		break;
	default:
		break;
	}
	return ret;
}

void Emitter::CreateEffect(PARTICLE_EFFECT_TYPE type)
{
	ParticleEffect* newEffect = nullptr;

	switch (type)
	{
	case PARTICLE_EFFECT_TYPE::NONE:
		break;
	case PARTICLE_EFFECT_TYPE::FORCE_OVER_LIFETIME:
		newEffect = new PE_ForceOverLifetime();
		break;
	case PARTICLE_EFFECT_TYPE::ROTATE_OVER_LIFETIME:
		newEffect = new PE_RotateOverLifetime();
		break;
	case PARTICLE_EFFECT_TYPE::SIZE_OVER_LIFETIME:
		newEffect = new PE_SizeOverLifetime();
		break;
	case PARTICLE_EFFECT_TYPE::COLOR_OVER_LIFETIME:
		newEffect = new PE_ColorOverLifetime();
		break;
	case PARTICLE_EFFECT_TYPE::VELOCITY_OVER_LIFETIME:
		newEffect = new PE_VelocityOverLifetime();
		break;
	case PARTICLE_EFFECT_TYPE::SHAPE:
		newEffect = new PE_Shape(objTransform);
		break;
	case PARTICLE_EFFECT_TYPE::MAX:
		break;
	default:
		break;
	}

	if (newEffect == nullptr)
	{
		LOG(LogType::L_WARNING, "Couldn't create a particle effect of type %i", (int)type);
		return;
	}


	if (myEffects.empty() || myEffects[myEffects.size() - 1]->type < type) //if vector is empty or type greater than the last element
	{
		myEffects.push_back(newEffect);
	}
	else if (myEffects[0]->type > type) //if type goes before first element in the vector
	{
		myEffects.insert(myEffects.begin(), newEffect);
	}
	else//general case
	{
		for (int i = 1; i < myEffects.size(); ++i)
		{
			if (type > myEffects[i - 1]->type && type < myEffects[i]->type)
			{
				myEffects.insert(myEffects.begin() + i, newEffect);
			}
		}
	}
}

void Emitter::PrepareParticleToSpawn(Particle& p, float3& startingPos)
{
	//we reset all particle variables
	p.accel = { 0.0f,0.0f,0.0f };
	p.rotation = 0.0f;
	p.rotationSpeed = 0.0f;
	p.speed = { 0.0f,0.0f,0.0f };
	p.maxLifetime = p.currentLifetime = EngineExternal->GetRandomFloat(particlesLifeTime[0], particlesLifeTime[1]);
	p.size = EngineExternal->GetRandomFloat(particlesSize[0], particlesSize[1]);
	p.color[0] = particlesColor[0];
	p.color[1] = particlesColor[1];
	p.color[2] = particlesColor[2];
	p.color[3] = particlesColor[3];
	p.pos = startingPos; //particles start at the center of the obj (but spawn methods modify this pos for the time being)
}

int Emitter::FindUnusedParticle()
{
	// Finds a Particle in ParticlesContainer which isn't used yet.
// (i.e. life < 0);
	int maxParticles = myParticles.size();

	for (int i = lastUsedParticle; i < maxParticles; ++i)
	{
		if (myParticles[i].currentLifetime < 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < lastUsedParticle; ++i)
	{
		if (myParticles[i].currentLifetime < 0.0f)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one
}

void Emitter::SetParticlesPerSec(int newParticlesPerSec)
{
	particlesPerSec = max(newParticlesPerSec, 0.0f);

	if (particlesPerSec != 0.0f)
		secPerParticle = 1 / particlesPerSec;
	else
		secPerParticle = 0.0f;

	CalculatePoolSize();
}

void Emitter::AddInstancedAttribute(unsigned int vao, unsigned int vbo, int attributeIndex, int dataSize, int instancedDataLength, int offset)
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glEnableVertexAttribArray(attributeIndex);
	glVertexAttribPointer(attributeIndex, dataSize, GL_FLOAT, GL_FALSE, instancedDataLength * sizeof(float), (void*)(offset * sizeof(float)));
	glVertexAttribDivisor(attributeIndex, 1);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
