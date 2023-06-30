#pragma once
#include "camera.h"
#include "common.h"
#include "vertices.h"
#include "IModel.h"
#include "shader.h"



class BoxModel:public IModel
{
public:
	
	BoxModel();
    ~BoxModel();

	BoxModel(const BoxModel& other) = delete;
	BoxModel(BoxModel&& other) noexcept = default;
	BoxModel& operator=(const BoxModel& other) = delete;
	BoxModel& operator=(BoxModel&& other) noexcept = default;

	void draw(const Camera& camera, const glm::mat4& wMat) override;
	void draw(const Camera& camera, const std::vector<glm::mat4>& wMats) override;


protected:
	void drawBegin();
	void drawEnd();
	//void setView(const glm::mat4& view) override;
    //void setProj(const glm::mat4& proj) override;
    //void setModel(const glm::mat4& model) override;
private:
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
    std::shared_ptr<ShaderProgram> shader;
    std::array< unsigned int, 2> textures{ 0, 0};
};

