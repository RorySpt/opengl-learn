#pragma once
#include "common.h"
#include "material.h"
#include "IModel.h"
#include "shader.h"
#include "light.h"

struct Light {
	glm::vec3 ambient;		// 环境光
	glm::vec3 diffuse;		// 漫反射光
	glm::vec3 specular;		// 镜面光
	glm::vec3 position;		// 光源位置

	operator LightSourcePoint() const
	{
		return {
			{
				ambient,
			diffuse,
			specular
			},
			position,
			1.f,
			0.f,
			0.f
		};
	}
};


class LightBoxModel :
    public IModel
{
public:
	CLASS_NO_COPY_DEFAULT_MOVE(LightBoxModel)

	LightBoxModel();
	virtual ~LightBoxModel();

	void init() override;
	void finished() override;

	void draw(const Camera& camera, const glm::mat4& wMat) override;
	void draw(const Camera& camera, const std::vector<glm::mat4>& wMats) override;

	
	void setLight(const Light& light);
	void setMaterial(const Material&);
protected:
	void drawBegin();
	void drawEnd();
private:
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;
	std::shared_ptr<ShaderProgram> shader;

	Light _light;
	Material _material;
};

inline void LightBoxModel::setLight(const Light& light)
{
	_light = light;
	
}
inline void LightBoxModel::setMaterial(const Material& material)
{
	_material = material;
}



class LightBoxModel2 :
	public IModel
{
public:
	CLASS_NO_COPY_AND_MOVE(LightBoxModel2)

	LightBoxModel2();
	virtual ~LightBoxModel2();

	void draw(const Camera& camera, const glm::mat4& wMat) override;
	void draw(const Camera& camera, const std::vector<glm::mat4>& wMats) override;

	void setLight(const LightSourcePoint& light);
	void setMaterial(const Material3&);
protected:
	void drawBegin();
	void drawEnd();
private:
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;
	std::shared_ptr<ShaderProgram> shader;
	Material3 _material;
	LightSourcePoint _light;
};

inline void LightBoxModel2::setLight(const LightSourcePoint& light)
{
	_light = light;

}
inline void LightBoxModel2::setMaterial(const Material3& material)
{
	_material = material;
}
