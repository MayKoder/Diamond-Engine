#ifndef __CO_IMAGE2D_H__
#define __CO_IMAGE2D_H__

#include "Component.h"

class ResourceTexture;
class ResourceMaterial;

class C_Image2D : public Component
{
public:
	C_Image2D(GameObject* gameObject);
	~C_Image2D() override;

#ifndef STANDALONE
	bool OnEditor() override;
#endif // !STANDALONE

	void RenderImage(float* transform, ResourceMaterial* material, unsigned int VAO);

	ResourceTexture* GetTexture() const;
	void SetTexture(ResourceTexture* tex);
	void SetTexture(int UID, const char* library_path);


	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

private:
	ResourceTexture* texture = nullptr;
};

#endif // !__CO_IMAGE2D_H__
