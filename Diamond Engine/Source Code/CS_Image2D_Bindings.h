#pragma once

#include "CO_Image2D.h"
#include "RE_Texture.h"

void SwapTwoImages(MonoObject* obj, MonoObject* other_image)
{
	if (EngineExternal == nullptr)
		return;




	C_Image2D* workImag = DECS_CompToComp<C_Image2D*>(obj);

	if (workImag==nullptr)
		return;
	
	C_Image2D* other_image_module = static_cast<C_Image2D*>(EngineExternal->moduleMono->GameObject_From_CSGO(other_image)->GetComponent(Component::TYPE::IMAGE_2D));//DECS_CompToComp<C_Image2D*>(other_image);

	if (other_image == nullptr)
		return;
	
	int texture_uid = workImag->GetTexture()->textureID;
	std::string library_path = workImag->GetTexture()->GetLibraryPath();
	workImag->SetTexture(other_image_module->GetTexture());
	other_image_module->SetTexture(texture_uid, library_path.c_str());

}