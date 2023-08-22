#pragma once
#include "common.h"
#include "material.h"
#include "IModel.h"

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


class BoxModel_SingleColor :
    public IModel
{
public:
	CLASS_NO_COPY_DEFAULT_MOVE(BoxModel_SingleColor)

	BoxModel_SingleColor();
	virtual ~BoxModel_SingleColor();

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

inline void BoxModel_SingleColor::setLight(const Light& light)
{
	_light = light;
	
}
inline void BoxModel_SingleColor::setMaterial(const Material& material)
{
	_material = material;
}



class BoxModel_SimpleTexture :
	public IModel
{
public:
	CLASS_NO_COPY_AND_MOVE(BoxModel_SimpleTexture)

	BoxModel_SimpleTexture();
	virtual ~BoxModel_SimpleTexture();

	void draw(const Camera& camera, const glm::mat4& wMat) override;
	void draw(const Camera& camera, const std::vector<glm::mat4>& wMats) override;

	void setLight(const LightSource& light);
	void setLight(const std::vector<LightSource>& light);
	void setMaterial(const Material3&);

protected:
	void drawBegin();
	void drawEnd();
private:
	void initBuffers();
	void initTextures();
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;
	std::shared_ptr<ShaderProgram> shader;
	Material3 _material;
	std::vector<LightSource> _lights;
public:
	float _emission_ratio = 1.0;
};

inline void BoxModel_SimpleTexture::setLight(const LightSource& light)
{
	if (!_lights.empty())_lights[0] = light;
	else _lights.emplace_back(light);
}
inline void BoxModel_SimpleTexture::setLight(const std::vector<LightSource>& lights)
{
	_lights = lights;
}
inline void BoxModel_SimpleTexture::setMaterial(const Material3& material)
{
	_material = material;
}
