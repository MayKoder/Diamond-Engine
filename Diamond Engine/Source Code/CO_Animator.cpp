#include "CO_Animator.h"
#include "RE_Animation.h"
#include "OpenGL.h"

#include "MO_Input.h"
#include "Application.h"
#include "MO_Renderer3D.h"
#include "IM_FileSystem.h"
#include "MO_ResourceManager.h"
#include "MO_Scene.h"

#include "GameObject.h"
#include "CO_Material.h"
#include "CO_Transform.h"
#include "CO_Camera.h"
#include "CO_MeshRenderer.h"

#include "ImGui/imgui.h"
#include "DEJsonSupport.h"

#include "MathGeoLib/include/Geometry/Frustum.h"
#include "MathGeoLib/include/Geometry/Plane.h"

#include "DETime.h"

C_Animator::C_Animator(GameObject* _gameobject) : Component(_gameobject),
selectedClip(nullptr), currentAnimation(nullptr), previousAnimation(nullptr),
rootBoneUID(0u), meshRendererUID(0u), currentTimeAnimation(0u), previousTimeAnimation(0u), 
prevAnimTime(0.0f), time(0.0f), blendTime(0.0f), blendTimeDuration(0.0f), defaultBlend(0.2f),
playing(false), started(false), showBones(false)
{
	gameObject = _gameobject;
	name = "Animator";
}

C_Animator::~C_Animator()
{
	rootBone = nullptr;

	selectedClip = nullptr;
	clips.clear();

	currentAnimation = nullptr;
	previousAnimation = nullptr;

	for(std::map<std::string, ResourceAnimation*>::iterator it = animations.begin(); it != animations.end(); it++)
	{
		EngineExternal->moduleResources->UnloadResource(it->second->GetUID());
		it->second = nullptr;
	}
	animations.clear();

	boneMapping.clear();
}

void C_Animator::Start()
{
	if (rootBone == nullptr) 
		return;

	boneMapping.clear();

	std::vector<GameObject*> bones;
	rootBone->CollectChilds(bones);

	for (uint i = 0; i < bones.size(); ++i)
	{
		boneMapping[bones[i]->name] = bones[i];
	}

	if (animations.size() > 0)
	{
		Play(animations.begin()->first, defaultBlend);
	}

	started = true;
}

void C_Animator::Update()
{
	float dt = DETime::deltaTime;
	if (DETime::state == GameState::PLAY)
	{
		if (!started) {
			Start();
		}
	}
	else {
		if (rootBone == nullptr)
		{
			if (rootBoneUID != 0u)
			{
				rootBone = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, rootBoneUID);

				if (meshRendererUID != 0u)
				{
					GameObject* meshRendererObject = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, meshRendererUID);
					if (meshRendererObject != nullptr)
					{
						dynamic_cast<C_MeshRenderer*>(meshRendererObject->GetComponent(Component::TYPE::MESH_RENDERER))->rootBone = rootBone;
					}
				}
				if (rootBone == nullptr)
					rootBoneUID = 0u;
			}
		}
		return;
	}

	if (rootBone != nullptr)
	{
		if (showBones) {
			std::vector<GameObject*> bones;
			rootBone->CollectChilds(bones);
			DrawBones(bones[0]);
		}

		if (currentAnimation != nullptr)
		{
			//Updating animation blend
			float blendRatio = 0.0f;
			if (blendTimeDuration > 0.0f)
			{
				prevAnimTime += dt;
				blendTime += dt;

				if (blendTime >= blendTimeDuration)
				{
					blendTimeDuration = 0.0f;
				}
				else if (previousAnimation && prevAnimTime >= previousAnimation->duration)
				{
					if (previousAnimation->loop == true)
					{
						prevAnimTime = 0.0f;
					}
				}

				if (blendTimeDuration > 0.0f)
					blendRatio = blendTime / blendTimeDuration;
			}
			//Endof Updating animation blend

			time += dt;
			currentTimeAnimation = time * currentAnimation->ticksPerSecond;
			currentTimeAnimation += currentAnimation->initTimeAnim;
			if (currentAnimation && currentTimeAnimation >= currentAnimation->duration) {
				if (currentAnimation->loop == true) {
					time = 0.0f;
				}
				else {
					if (animations.size() > 0) {
						Play(animations.begin()->first, defaultBlend); }
					return;
				}
			}
			UpdateChannelsTransform(currentAnimation, blendRatio > 0.0f ? previousAnimation : nullptr, blendRatio);
		}
	}
}

void C_Animator::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);
	DEJson::WriteInt(nObj, "RootBone UID", rootBone == nullptr ? 0 : rootBone->UID);
	DEJson::WriteInt(nObj, "MeshRendererUID", meshRendererUID);

	JSON_Value* animationsValue = json_value_init_array();
	JSON_Array* animationsArray = json_value_get_array(animationsValue);

	for (std::map<std::string, ResourceAnimation*>::iterator it = animations.begin(); it != animations.end(); ++it)
	{
		json_array_append_number(animationsArray, it->second->GetUID());
	}

	json_object_set_value(nObj, "Animations", animationsValue);
	
}

void C_Animator::LoadData(DEConfig& nObj)
{
	Component::LoadData(nObj);
	rootBoneUID = nObj.ReadInt("RootBone UID");
	meshRendererUID = nObj.ReadInt("MeshRendererUID");

	JSON_Array* animationsArray = nObj.ReadArray("Animations");

	for (size_t i = 0; i < json_array_get_count(animationsArray); i++)
	{
		uint animationUID = json_array_get_number(animationsArray, i);
		ResourceAnimation* animation = dynamic_cast<ResourceAnimation*>(EngineExternal->moduleResources->RequestResource(animationUID, Resource::Type::ANIMATION));

		if (animation != nullptr) {
			AddAnimation(animation);
		}
	}
}

#ifndef STANDALONE

bool C_Animator::OnEditor()
{	
	if (Component::OnEditor() == true)
	{
		ImGui::Checkbox("Show Bones", &showBones);
		// RootBone and MeshRenderer =====================================================================================================

		if (rootBone == nullptr)
		{
			ImGui::Text("Drop a GameObject here to set the root bone:");
			ImGui::Button("Root Bone not set");
		}
		else {
			ImGui::Text("Root Bone: ");
			ImGui::SameLine();
			ImGui::Button(rootBone->name.c_str());
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_GAMEOBJECT"))
			{
				int uid = *(int*)payload->Data;

				GameObject* dropGO = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, uid);
				rootBone = dropGO;

			}
			ImGui::EndDragDropTarget();
		}
		ImGui::Spacing();


		if (meshRendererUID == 0u) {
			ImGui::Button("Drop here to set mesh renderer object");
		}
		else {
			ImGui::Text("MeshRenderer Game Object UID: ");
			char uidText[16];
			sprintf_s(uidText, "%d", meshRendererUID);
			ImGui::SameLine();
			ImGui::Button(uidText);
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_GAMEOBJECT"))
			{
				int uid = *(int*)payload->Data;

				GameObject* dropGO = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, uid);
				if (rootBone != nullptr) {
					C_MeshRenderer* meshRenderer = dynamic_cast<C_MeshRenderer*>(dropGO->GetComponent(Component::TYPE::MESH_RENDERER));
					if (meshRenderer != nullptr) {
						meshRenderer->rootBone = rootBone;
						meshRendererUID = dropGO->UID;
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		//=======================================================================================================================================

		if (currentAnimation == nullptr) {
			ImGui::Text("Current Animation: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "None");
		}
		else {
			ImGui::Text("Current Animation: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", currentAnimation->animationName);
			ImGui::Text("Duration: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%.2f", currentAnimation->duration);
			ImGui::Text("Ticks per second: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%.2f", currentAnimation->ticksPerSecond);
		}

		//List of existing animations
		static char newName[32];

		std::string animation_to_remove = "";
		ImGui::Text("Select a new animation");
		for (std::map<std::string, ResourceAnimation*>::iterator it = animations.begin(); it != animations.end(); ++it)
		{
			std::string animName = it->first;

			if (currentAnimation == it->second) {
				animName += " (Current)";
			}

			if (ImGui::Button(animName.c_str())) {
				Play(animName, defaultBlend);
				time = 0.f;
				sprintf_s(newName, animName.c_str());
			}

			ImGui::SameLine();
			ImGui::PushID(it->second->GetUID());
			if (ImGui::Button("Remove Animation")) {
				animation_to_remove = it->first;
			}
			ImGui::PopID();
		}
		if (animation_to_remove.size() > 0) 
		{
			EngineExternal->moduleResources->UnloadResource(animations[animation_to_remove]->GetUID());
			animations.erase(animation_to_remove);
		}

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Button("Drop new animation here");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_ANIMATION"))
			{
				std::string* path = (std::string*)payload->Data;

				ResourceAnimation* droppedAnimation = nullptr;

				if (path->find("Assets") != std::string::npos)
				{
					droppedAnimation = dynamic_cast<ResourceAnimation*>(EngineExternal->moduleResources->RequestFromAssets(path->c_str()));
				}
				else //Take from Library
				{
					std::string uid;
					FileSystem::GetFileName(path->c_str(), uid, false);

					droppedAnimation = dynamic_cast<ResourceAnimation*>(EngineExternal->moduleResources->RequestResource(std::atoi(uid.c_str()), Resource::Type::ANIMATION));
				}


				if (droppedAnimation != nullptr) {
					AddAnimation(droppedAnimation);
				}

			}
			ImGui::EndDragDropTarget();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (currentAnimation != nullptr)
		{
			ImGui::InputText("Name", newName, IM_ARRAYSIZE(newName));
			ImGui::Checkbox("Loop", &currentAnimation->loop);

			if (ImGui::Button("Save Animation")) {
				SaveAnimation(currentAnimation, newName);
			}
			ImGui::Separator();
		}

		ImGui::Text("Previous Animation Time: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%.2f", prevAnimTime);
		ImGui::Text("Current Animation Time: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", currentTimeAnimation);
		ImGui::Text("blendTime: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%.2f", blendTime);

		ImGui::Spacing();
		if (playing)
		{
			ImGui::Text("Playing: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "true");
		}
		else
		{
			ImGui::Text("Playing: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "false");
		}
		ImGui::Spacing();

		// Clips ===============================================================================================

		//Table header
		ImGui::Columns(3, "Clips");
		ImGui::Separator();
		ImGui::Text("Clips");
		ImGui::NextColumn();
		ImGui::Text("Start");
		ImGui::NextColumn();
		ImGui::Text("End");
		ImGui::NextColumn();
		ImGui::Separator();

		for (size_t i = 0; i < clips.size(); i++)
		{
			if (ImGui::Button(clips[i].name)) {
				selectedClip = &clips[i];
			}
			ImGui::NextColumn();
			ImGui::InputFloat("##start", &clips[i].startFrame, 0.0f, 0.0f, 0);
			ImGui::NextColumn();
			ImGui::InputFloat("##end", &clips[i].endFrame, 0.0f, 0.0f, 0);
			ImGui::NextColumn();
		}

		ImGui::Separator();
		ImGui::Columns(1);
		ImGui::Spacing();

		if (ImGui::Button("+")) 
		{
			if (currentAnimation != nullptr) {
				AddClip(currentAnimation);
			}
		}
		
		ImGui::SameLine();
		if (ImGui::Button("-")) 
		{
			if (selectedClip != nullptr)
			{
				std::vector<AnimationClip> remainingClips;
				for (size_t i = 0; i < clips.size(); i++)
				{
					if (selectedClip != &clips[i])
						remainingClips.push_back(clips[i]);
				}

				selectedClip = nullptr;
				clips = remainingClips;
				remainingClips.clear();
			}
		}

		ImGui::Spacing();

		if (selectedClip != nullptr) 
		{
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Text("Selected Clip");
			ImGui::InputText("Name", selectedClip->name, IM_ARRAYSIZE(selectedClip->name));
			ImGui::InputFloat("Start Frame", &selectedClip->startFrame, 1.0f, 0.0f, 0.0f);
			ImGui::InputFloat("End Frame", &selectedClip->endFrame, 1.0f, 0.0f, 0.0f);
			ImGui::Checkbox("Loop", &selectedClip->loop);
		}

		ImGui::Spacing();

		if (ImGui::Button("Apply")) {
			for (size_t i = 0; i < clips.size(); i++) {
				ResourceAnimation* animation = ClipToAnimation(clips[i]);
				AddAnimation(animation);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			selectedClip = nullptr;
			clips.clear();
		}

		ImGui::Spacing();
	}

	return true;	
}

void C_Animator::SaveAnimation(ResourceAnimation* animation, const char* name)
{
	std::string old_name = animation->animationName;
	animations.erase(currentAnimation->animationName);
	sprintf_s(currentAnimation->animationName, name);
	animations[currentAnimation->animationName] = currentAnimation;

	char* buffer;
	uint size = currentAnimation->SaveCustomFormat(currentAnimation, &buffer);

	//Save in Library
	FileSystem::Save(currentAnimation->GetLibraryPath(), buffer, size, false);

	//Save a copy in Assets 
	std::string old_assets_path = "Assets/Animations/" + old_name + ".anim";
	std::string new_assets_path = "Assets/Animations/" + std::string(name) + ".anim";

	EngineExternal->moduleResources->RenameAsset(old_assets_path.c_str(), new_assets_path.c_str(), buffer, size, animation);

	old_assets_path.clear();
	new_assets_path.clear();
	RELEASE_ARRAY(buffer);
}

#endif // !STANDALONE

void C_Animator::StoreBoneMapping(GameObject* gameObject)
{
	boneMapping[gameObject->name] = gameObject;
	for (int i = 0; i < gameObject->children.size(); i++)
	{
		StoreBoneMapping(gameObject->children[i]);
	}
}

void C_Animator::Play(std::string animName, float blendDuration)
{
	std::map<std::string, ResourceAnimation*>::iterator it = animations.find(animName);
	if (it == animations.end())
		return;

	previousAnimation = currentAnimation;
	prevAnimTime = time;
	currentAnimation = animations[animName];
	blendTimeDuration = blendDuration;
	blendTime = 0.0f;
	time = 0;
}

void C_Animator::Pause()
{
	active = false;
}

void C_Animator::Resume()
{
	active = true;
}

void C_Animator::AddAnimation(ResourceAnimation* anim)
{
	if (anim != nullptr) 
		animations[anim->animationName] = anim;
}

void C_Animator::AddClip(ResourceAnimation* anim)
{
	if (anim == nullptr)
		return;

	AnimationClip clip;

	strcpy(clip.name, anim->animationName);
	clip.startFrame = anim->initTimeAnim;
	clip.endFrame = anim->initTimeAnim + anim->duration;
	clip.originalAnimation = anim;

	clips.push_back(clip);
}

void C_Animator::UpdateChannelsTransform(const ResourceAnimation* settings, const ResourceAnimation* blend, float blendRatio)
{
	uint currentFrame = currentTimeAnimation;

	//LOG(LogType::L_NORMAL,"%i", currentFrame);
	//LOG(LogType::L_NORMAL, "%i", currentFrame);

	uint prevBlendFrame = 0;
	if (blend != nullptr)
	{
		prevBlendFrame = (blend->ticksPerSecond * prevAnimTime) + blend->initTimeAnim;
	}
	//LOG(LogType::L_NORMAL, "%i", currentFrame);
	std::map<std::string, GameObject*>::iterator boneIt;
	for (boneIt = boneMapping.begin(); boneIt != boneMapping.end(); ++boneIt)
	{
		C_Transform* transform = dynamic_cast<C_Transform*>(boneIt->second->GetComponent(Component::TYPE::TRANSFORM));

		if (settings->channels.find(boneIt->first.c_str()) == settings->channels.end()) continue;

		const Channel& channel = settings->channels.find(boneIt->first.c_str())->second;
	
		float3 position = GetChannelPosition(channel, currentFrame, transform->position);
		Quat rotation = GetChannelRotation(channel, currentFrame, transform->rotation);
		float3 scale = GetChannelScale(channel, currentFrame, transform->localScale);


		//BLEND
		if (blend != nullptr)
		{
			std::map<std::string, Channel>::const_iterator foundChannel = blend->channels.find(boneIt->first.c_str());
			if (foundChannel != blend->channels.end()) {
				const Channel& blendChannel = foundChannel->second;

				position = float3::Lerp(GetChannelPosition(blendChannel, prevBlendFrame, transform->position), position, blendRatio);
				rotation = Quat::Slerp(GetChannelRotation(blendChannel, prevBlendFrame, transform->rotation), rotation, blendRatio);
				scale = float3::Lerp(GetChannelScale(blendChannel, prevBlendFrame, transform->localScale), scale, blendRatio);
			}

		}

		transform->position = position;
		transform->eulerRotation = rotation.ToEulerXYZ() * RADTODEG;
		transform->localScale = scale;
		transform->updateTransform = true;
	}
}

Quat C_Animator::GetChannelRotation(const Channel& channel, float currentKey, Quat default) const
{
	Quat rotation = default;

	if (channel.rotationKeys.size() > 0)
	{
		std::map<double, Quat>::const_iterator previous = channel.GetPrevRotKey(currentKey);
		std::map<double, Quat>::const_iterator next = channel.GetNextRotKey(currentKey);

		if (channel.rotationKeys.begin()->first == -1) return rotation;


		//If both keys are the same, no need to blend
		if (previous == next)
			rotation = previous->second;
		else //blend between both keys
		{
			//0 to 1
			float ratio = (currentKey - previous->first) / (next->first - previous->first);
			rotation = previous->second.Slerp(next->second, ratio);
		}
	}
	return rotation;
}

float3 C_Animator::GetChannelPosition(const Channel& channel, float currentKey, float3 default) const
{
	float3 position = default;

	if (channel.positionKeys.size() > 0)
	{
		std::map<double, float3>::const_iterator previous = channel.GetPrevPosKey(currentKey);
		std::map<double, float3>::const_iterator next = channel.GetNextPosKey(currentKey);

		if (channel.positionKeys.begin()->first == -1) return position;

		//If both keys are the same, no need to blend
		if (previous == next)
			position = previous->second;
		else //blend between both keys
		{
			//0 to 1
			float ratio = (currentKey - previous->first) / (next->first - previous->first);
			position = previous->second.Lerp(next->second, ratio);
		}
	}

	return position;
}

float3 C_Animator::GetChannelScale(const Channel & channel, float currentKey, float3 default) const
{
	float3 scale = default;

	if (channel.scaleKeys.size() > 0)
	{
		std::map<double, float3>::const_iterator previous = channel.GetPrevScaleKey(currentKey);
		std::map<double, float3>::const_iterator next = channel.GetPrevScaleKey(currentKey);

		if (channel.scaleKeys.begin()->first == -1) return scale;


		//If both keys are the same, no need to blend
		if (previous == next)
			scale = previous->second;
		else //blend between both keys
		{
			//0 to 1
			float ratio = (currentKey - previous->first) / (next->first - previous->first);
			scale = previous->second.Lerp(next->second, ratio);
		}
	}
	return scale;
}

void C_Animator::DrawBones(GameObject* gameObject)
{
	glColor3f(1.f, 0.f, 0.f);
	glLineWidth(4.f);
	glBegin(GL_LINES);

	//Draw lines
	float3 position;
	Quat rotation;
	float3 scale;

	if (gameObject->parent != nullptr) 
	{
		gameObject->parent->transform->globalTransform.Decompose(position, rotation, scale);
		glVertex3f(position.x, position.y, position.z);
	}

	gameObject->transform->globalTransform.Decompose(position, rotation, scale);
	glVertex3f(position.x, position.y, position.z);
	//LOG(LogType::L_NORMAL, "Name: %s  %f,%f,%f",bones->first.c_str(), position.x, position.y, position.z);

	if (gameObject->children.size() > 0) {
		for (uint i = 0; i < gameObject->children.size(); i++)
		{
			DrawBones(gameObject->children[i]);
		}
	}
	glEnd();
	glLineWidth(1.f);
	glColor3f(1.f, 1.f, 1.f);
}

ResourceAnimation* C_Animator::ClipToAnimation(AnimationClip clip)
{
	ResourceAnimation* animation = new ResourceAnimation(EngineExternal->moduleResources->GenerateNewUID());

	strcpy(animation->animationName, clip.name);
	animation->duration = clip.endFrame - clip.startFrame;
	animation->initTimeAnim = clip.startFrame;
	animation->loop = clip.loop;
	animation->ticksPerSecond = clip.originalAnimation->ticksPerSecond;

	//Add all animation properties such as channels
	//Create .anim 

	return animation;
}

AnimationClip::AnimationClip() : name("No name"), startFrame(0), endFrame(0), originalAnimation(nullptr), loop(false) {}
