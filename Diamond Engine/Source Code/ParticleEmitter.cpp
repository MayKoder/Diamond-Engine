#include "ParticleEmitter.h"
#include "Globals.h"

#include "PE_Spawn.h"
#include "PE_Spawn_Area.h"
#include "PE_Move.h"
#include "PE_Rotate.h"
#include <string>

#include "Application.h"
#include "MO_ResourceManager.h"
#include "MO_Renderer3D.h"

#include "RE_Texture.h"

#include "CO_Camera.h"

#include "OpenGL.h"
#include "ImGui/imgui.h"
#include "MathGeoLib/include/Math/float4x4.h"

#include <string>

Emitter::Emitter() :
	toDelete(false),
	texture(nullptr),
	particlesPerSec(0.0f),
	lastParticeTime(0),
	myParticles(),
	myEffects()
{
	memset(particlesLifeTime, 0.0f, sizeof(particlesLifeTime));

	glGenBuffers(1, &VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VAO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(particleVAO), particleVAO, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	CalculatePoolSize();
}

Emitter::~Emitter()
{
	if (texture != nullptr)
		EngineExternal->moduleResources->UnloadResource(texture->GetUID());

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
}


void Emitter::Update(float dt, bool systemActive)
{
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
		}
	}
}

void Emitter::Draw(unsigned int shaderId)
{
	if (texture != nullptr)
		glBindTexture(GL_TEXTURE_2D, texture->textureID);

	glBindBuffer(GL_ARRAY_BUFFER, VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	GLint modelLoc = glGetUniformLocation(shaderId, "view");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, EngineExternal->moduleRenderer3D->activeRenderCamera->ViewMatrixOpenGL().ptr());

	modelLoc = glGetUniformLocation(shaderId, "projection");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, EngineExternal->moduleRenderer3D->activeRenderCamera->ProjectionMatrixOpenGL().ptr());

	int particlesCount = myParticles.size();
	for (int i = 0; i < particlesCount; ++i)	//Need to order particles
	{
		if (myParticles[i].currentLifetime > 0)
		{
			modelLoc = glGetUniformLocation(shaderId, "model_matrix");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, float4x4::FromTRS(myParticles[i].pos, Quat::FromEulerXYZ(0, 0, myParticles[i].rotation), float3(1, 1, 1)).Transposed().ptr());

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


#ifndef STANDALONE
void Emitter::OnEditor(int emitterIndex)
{
	std::string guiName = "Emitter";
	std::string suffixLabel = "##";
	suffixLabel += emitterIndex;

	if (ImGui::CollapsingHeader(guiName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool poolToEdit = false;

		guiName = "Particle Lifetime ##" + suffixLabel;
		if (ImGui::DragFloatRange2("Lifetime", &particlesLifeTime[0], &particlesLifeTime[1], 0.25f, 0.1f, 100.0f, "Min: %.1f", "Max: %.1f"))
		{
			poolToEdit = true;
		}

		guiName = "Particles per Second ##" + suffixLabel;
		if (ImGui::DragFloat(guiName.c_str(), &particlesPerSec))
		{
			poolToEdit = true;
		}

		if (poolToEdit)
		{
			CalculatePoolSize();
		}


		for (int i = (int)PARTICLE_EFFECT_TYPE::NONE + 1; i < (int)PARTICLE_EFFECT_TYPE::MAX; ++i)
		{
			int index = DoesEffectExist((PARTICLE_EFFECT_TYPE)i);
			if (index != -1)
			{
				myEffects[index]->OnEditor(emitterIndex);
			}
			else
			{
				guiName = (ParticleEffectEnumToString((PARTICLE_EFFECT_TYPE)i)) + suffixLabel;
				if (ImGui::Button(guiName.c_str()))
				{
					CreateEffect((PARTICLE_EFFECT_TYPE)i);
				}
			}
		}

		if (texture != nullptr)
			ImGui::Image((ImTextureID)texture->textureID, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));

		else
			ImGui::Image((ImTextureID)0, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));

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
		ImGui::Separator();
		ImGui::Spacing();
	}
}
#endif // !STANDALONE

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
}

void Emitter::CreateParticles(unsigned int particlesToAdd)
{
	unsigned int lastIndex = myParticles.size();
	myParticles.resize(lastIndex + particlesToAdd);
	for (int i = 0; i < myEffects.size(); ++i)
	{
		for (unsigned int j = lastIndex; j < myParticles.size(); ++j)
		{
			myEffects[i]->Spawn(myParticles[j]);
		}
	}
}

void Emitter::ThrowParticles(float dt)
{
	//find particles to spawn this frame
	float timeSinceLastThrow = dt + lastParticeTime;

	float numberOfParticlesToSpawnF = timeSinceLastThrow * particlesPerSec;
	int numberOfParticlesToSpawn = numberOfParticlesToSpawnF;
	float extraParticle = numberOfParticlesToSpawnF - numberOfParticlesToSpawn;

	if (particlesPerSec > 0.0f)
	{
		lastParticeTime = extraParticle / particlesPerSec; //TODO should we optimize this division?
	}
	else
	{
		lastParticeTime = 0.0f;
	}


	//LOG(LogType::L_NORMAL, "PARTICLES SPAWNED THIS FRAME %i", numberOfParticlesToSpawn);


	//spawn particles
	for (int i = 0; i < myParticles.size(); ++i)
	{
		if (myParticles[i].currentLifetime < 0.0f)
		{
			for (int j = 0; j < myEffects.size(); ++j)
			{
				myEffects[j]->Spawn(myParticles[i]);
				myParticles[i].maxLifetime = myParticles[i].currentLifetime = EngineExternal->GetRandomIntFast(particlesLifeTime[0], particlesLifeTime[1]);
			}
			--numberOfParticlesToSpawn;
		}

		if (numberOfParticlesToSpawn == 0)
			break;
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
	case PARTICLE_EFFECT_TYPE::SPAWN:
		ret = "Spawn Effect";
		break;
	case PARTICLE_EFFECT_TYPE::AREA_SPAWN:
		ret = "Area Spawn Effect";
		break;
	case PARTICLE_EFFECT_TYPE::MOVE:
		ret = "Move Effect";
		break;
	case PARTICLE_EFFECT_TYPE::RANDOM_MOVE:
		ret = "Random Move Effect";
		break;
	case PARTICLE_EFFECT_TYPE::ROTATE:
		ret = "Rotate Effect";
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
	case PARTICLE_EFFECT_TYPE::SPAWN:
		newEffect = new PE_Spawn();
		break;
	case PARTICLE_EFFECT_TYPE::AREA_SPAWN:
		newEffect = new PE_Spawn_Area();
		break;
	case PARTICLE_EFFECT_TYPE::MOVE:
		newEffect = new PE_Move();
		break;
	case PARTICLE_EFFECT_TYPE::RANDOM_MOVE:
		//TODO
		break;
	case PARTICLE_EFFECT_TYPE::ROTATE:
		newEffect = new PE_Rotate();
		break;
	case PARTICLE_EFFECT_TYPE::MAX:
		//TODO
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
