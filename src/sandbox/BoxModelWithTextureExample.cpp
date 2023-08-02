#include "stdafx.h"
#include "shader.h"
#include "BoxModelWithTextureExample.h"
namespace ModelExample
{
    BoxModel::BoxModel()
    {
        int pic_width, pic_height, pic_nrChannels;
        unsigned char* pic_data = nullptr;

        shader = ShaderProgram::makeShaderByName("shader.vert", "shader.frag");
        //shader->use();
        // 0. 复制顶点数组到到缓冲中供OpenGL使用
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // 1. 绑定VAO
        glBindVertexArray(VAO);
        // 2. 把顶点数组复制到缓冲中供OpenGL使用
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // 1. 设置顶点属性指针
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
        //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
        //glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), VOID_POINTER(3 * sizeof(float)));

        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        //创建纹理
        glGenTextures(1, &textures[0]);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        // 为当前绑定的纹理对象设置环绕、过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


        stbi_set_flip_vertically_on_load(true);
        // 加载并生成纹理
        pic_data = stbi_load(comm::path_container_jpg.data(), &pic_width, &pic_height, &pic_nrChannels, 0);
        if (pic_data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pic_width, pic_height, 0, GL_RGB, GL_UNSIGNED_BYTE, pic_data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "LOAD_TEXTURE_FAILED " << comm::path_container_jpg.data() << std::endl;
        }
        glGenTextures(1, &textures[1]);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        // 为当前绑定的纹理对象设置环绕、过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        stbi_image_free(pic_data);
        pic_data = stbi_load(comm::path_awe_png.data(), &pic_width, &pic_height, &pic_nrChannels, 0);
        if (pic_data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pic_width, pic_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pic_data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "LOAD_TEXTURE_FAILED " << comm::path_awe_png.data() << std::endl;
        }
        stbi_image_free(pic_data);


        glUniform1i(shader->location("ourTexture1"), 0);
        glUniform1i(shader->location("ourTexture2"), 1);

    }

    BoxModel::~BoxModel()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void BoxModel::draw(const Camera& camera, const glm::mat4& wMat)
    {
        drawBegin();
        shader->glUniform("view", camera.getViewMatrix());
        shader->glUniform("proj", camera.getProjMatrix());
        shader->glUniform("model", wMat);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        drawEnd();
    }

    void BoxModel::draw(const Camera& camera, const std::vector<glm::mat4>& wMats)
    {
        drawBegin();
        shader->glUniform("view", camera.getViewMatrix());
        shader->glUniform("proj", camera.getProjMatrix());
        for (auto& mat : wMats)
        {
            shader->glUniform("model", mat);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        drawEnd();
    }

    void BoxModel::drawBegin()
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glUseProgram(shader->ID);
        glBindVertexArray(VAO);
    }

    void BoxModel::drawEnd()
    {
    }


}