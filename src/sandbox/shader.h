#pragma once
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <random>
#include <type_traits>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common.h"

inline void glUniform(GLint location, GLfloat v0) { glUniform1f(location, v0); }
inline void glUniform(GLint location, GLfloat v0, GLfloat v1) { glUniform2f(location, v0, v1); }
inline void glUniform(GLint location, GLfloat v0, GLfloat v1, GLfloat v2) { glUniform3f(location, v0, v1, v2); }
inline void glUniform(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) { glUniform4f(location, v0, v1, v2, v3); }
inline void glUniform(GLint location, glm::vec2 vec2) { glUniform2f(location, vec2.x, vec2.y); }
inline void glUniform(GLint location, glm::vec3 vec3) { glUniform3f(location, vec3.x, vec3.y, vec3.z); }
inline void glUniform(GLint location, glm::vec4 vec4) { glUniform4f(location, vec4.x, vec4.y, vec4.z, vec4.w); }
inline void glUniform(GLint location, GLint v0) { glUniform1i(location, v0); }
inline void glUniform(GLint location, GLint v0, GLint v1) { glUniform2i(location, v0, v1); }
inline void glUniform(GLint location, GLint v0, GLint v1, GLint v2) { glUniform3i(location, v0, v1, v2); }
inline void glUniform(GLint location, GLint v0, GLint v1, GLint v2, GLint v3) { glUniform4i(location, v0, v1, v2, v3); }
inline void glUniform(GLint location, const glm::mat4& mat) { glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0].x); }
inline void glUniform(GLint location, const glm::mat3& mat) { glUniformMatrix3fv(location, 1, GL_FALSE, &mat[0].x); }
inline void glUniform(GLint location, const glm::mat2& mat) { glUniformMatrix2fv(location, 1, GL_FALSE, &mat[0].x); }

class FragShader;
class VertShader;
class ShaderBase
{
public:
	enum shader_type
	{
		ST_Unknown,
		ST_Frag,
		ST_Vert
	};
	using path_type = std::filesystem::path;
	friend class ShaderProgram;
	CLASS_DEFAULT_COPY_AND_MOVE(ShaderBase)

	[[nodiscard]] unsigned int id() const;
	[[nodiscard]] bool valid() const;
	[[nodiscard]] shader_type type()const;
	[[nodiscard]] virtual FragShader* asFragShader() { return nullptr; }
	[[nodiscard]] virtual VertShader* asVertShader() { return nullptr; }
	[[nodiscard]] static std::shared_ptr<ShaderBase> makeShaderByPath(const path_type& shaderPath, shader_type type = ST_Unknown);
	[[nodiscard]] static std::shared_ptr<ShaderBase> makeShaderByName(const std::string& shaderName, shader_type type = ST_Unknown);
	[[nodiscard]] static std::shared_ptr<ShaderBase> makeShaderByCode(const std::string& shaderCode, shader_type type);
	virtual ~ShaderBase();

protected:
	ShaderBase(){}

	void compile(const std::string& shaderCode);
	void checkCompileErrors();
	[[nodiscard]] static shader_type identifyShaderByPath(const path_type& shaderPath);
	
private:
	unsigned int m_hShader = 0;
	bool m_isValid = false;
	shader_type m_type = ST_Unknown;
};
class FragShader:public ShaderBase
{
public:
	CLASS_DEFAULT_COPY_AND_MOVE(FragShader)

	friend class ShaderBase;
	[[nodiscard]] FragShader* asFragShader()override { return this; }
	
protected:
	FragShader() = default;
};
class VertShader :public ShaderBase
{
public:
	friend class ShaderBase;
	[[nodiscard]] VertShader* asVertShader()override { return this; }
	CLASS_DEFAULT_COPY_AND_MOVE(VertShader)
protected:
	VertShader() = default;
};



class ShaderProgram
{
public:
	using path_type = std::filesystem::path;
	// 程序ID;
	unsigned int ID{};

	// 构造器读取并构建着色器
	ShaderProgram(const path_type& vertexPath, const path_type& fragmentPath);
	ShaderProgram(const VertShader &vert, const FragShader &frag);
	// 使用/激活
	void use() const;
	[[nodiscard]] int location(std::string_view) const;

	template <typename ...T>
	void glUniform(std::string_view, T ... args)const;

	[[nodiscard]] static std::shared_ptr<ShaderProgram> makeShaderByPath(const path_type& vertexPath, const path_type& fragmentPath);
	[[nodiscard]] static std::shared_ptr<ShaderProgram> makeShaderByName(const std::string& vertexName, const std::string& fragmentName);
private:
	void compile(const std::string& vertexCode, const std::string& fragmentCode);
	void compile(const VertShader& vert, const FragShader& frag);
	void checkCompileErrors(unsigned int shader, const std::string& type) const;

	void construct(const path_type& vertexPath, const path_type& fragmentPath);
	void construct(const VertShader& vert, const FragShader& frag);
};

inline int ShaderProgram::location(const std::string_view string_view) const
{
	return glGetUniformLocation(ID, string_view.data());
}

template <typename ... T>
void ShaderProgram::glUniform(const std::string_view string, T ... args)const
{
	::glUniform(location(string),
		 static_cast<std::conditional_t<std::is_floating_point_v<T>, GLfloat,
			std::conditional_t<std::is_integral_v<T>, GLint, T&&>>
		>(args)...
		);
}

inline std::shared_ptr<ShaderProgram> ShaderProgram::makeShaderByPath(const path_type& vertexPath,
                                                                      const path_type& fragmentPath)
{
	const std::shared_ptr vert = ShaderBase::makeShaderByPath(vertexPath, ShaderBase::ST_Vert);
	const std::shared_ptr frag = ShaderBase::makeShaderByPath(fragmentPath, ShaderBase::ST_Frag);
	std::shared_ptr<ShaderProgram> program;
	if (vert && frag)
	{
		program = std::make_shared<ShaderProgram>(*vert->asVertShader(), *frag->asFragShader());
	}
	return program;
}

inline std::shared_ptr<ShaderProgram> ShaderProgram::makeShaderByName(const std::string& vertexName,
	const std::string& fragmentName)
{
	const std::shared_ptr vert = ShaderBase::makeShaderByName(vertexName, ShaderBase::ST_Vert);
	const std::shared_ptr frag = ShaderBase::makeShaderByName(fragmentName, ShaderBase::ST_Frag);

	std::shared_ptr<ShaderProgram> program;
	if (vert && frag)
	{
		program = std::make_shared<ShaderProgram>(*vert->asVertShader(), *frag->asFragShader());
	}
	return program;
}

class ShaderManager
{
public:
	using path_type = std::filesystem::path;
	static std::list<path_type> m_searchPathList;

	static ShaderManager* hInst()
	{
		static ShaderManager* shader = nullptr;
		if(shader == nullptr)
		{
			shader = new ShaderManager;
		}
		return shader;
	}
	[[nodiscard]] std::shared_ptr<FragShader> getOrCreateFragShader(const std::string& path);
	[[nodiscard]] std::shared_ptr<VertShader> getOrCreateVertShader(const std::string& path);
	[[nodiscard]] std::shared_ptr<ShaderProgram> getOrCreateShaderProgram(const std::string& fragPath, const std::string& vertPath);
	[[nodiscard]] std::shared_ptr<ShaderProgram> getOrCreateShaderProgram(const VertShader&, const FragShader&);

	static const std::list<path_type>& searchPathList() { return m_searchPathList; }
protected:
	ShaderManager() = default;

private:
	std::map<std::string, std::shared_ptr<ShaderBase>> m_shader_map;
	std::map<unsigned long long, std::shared_ptr<ShaderProgram>> m_program_map;
};



