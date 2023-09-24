#pragma once
#include "stdafx.h"
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <type_traits>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common.h"

// 统一设置GLfloat参数的glUniform重载函数
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

// 片段着色器类的前向声明
class FragShader;
// 顶点着色器类的前向声明
class VertShader;

// 着色器基类
class ShaderBase
{
public:
	enum EShaderType
	{
		ST_Auto,// 自动识别类型，根据拓展名.frag/.vert
		ST_Frag,// 片段着色器类型
		ST_Vert // 顶点着色器类型
	};
	using path_type = std::filesystem::path;// 文件路径类型
	friend class ShaderProgram;				// 着色器程序类的友元
	CLASS_DEFAULT_COPY_AND_MOVE(ShaderBase) // 默认的复制和移动构造函数

	[[nodiscard]] unsigned int id() const;	// 返回着色器ID
	[[nodiscard]] bool valid() const;		// 判断着色器是否有效
	[[nodiscard]] EShaderType type()const;	// 返回着色器类型
	[[nodiscard]] virtual FragShader* asFragShader() { return nullptr; } // 返回片段着色器对象指针，默认返回nullptr
	[[nodiscard]] virtual VertShader* asVertShader() { return nullptr; } // 返回顶点着色器对象指针，默认返回nullptr


	// 通过路径创建片段着色器/顶点着色器（由传入类型决定），后需经过ShaderProgram链接
	[[nodiscard]] static std::shared_ptr<ShaderBase> makeShaderByPath(const std::filesystem::path& shaderPath, EShaderType type = ST_Auto);
	// 通过名字查找着色器代码
	[[nodiscard]] static std::shared_ptr<ShaderBase> makeShaderByName(std::string_view shaderName, EShaderType type = ST_Auto);
	// 使用存放着色器代码的字符串创建着色器类
	[[nodiscard]] static std::shared_ptr<ShaderBase> makeShaderByCode(std::string_view shaderCode, EShaderType type);
	virtual ~ShaderBase();

protected:
	ShaderBase() = default;

	void compile(std::string_view shaderCode); // 编译着色器代码
	void checkCompileErrors();					// 检查编译错误

	// 通过路径识别着色器类型
	[[nodiscard]] static EShaderType identifyShaderByPath(const path_type& shaderPath);


	EShaderType m_type = ST_Auto;	// 着色器类型
private:
	unsigned int m_hShader = 0;		// 着色器句柄
	bool m_isValid = false;			// 着色器是否有效
};


// 片段着色器类
class FragShader:public ShaderBase
{
public:
	CLASS_DEFAULT_COPY_AND_MOVE(FragShader)

	friend class ShaderBase; // 着色器基类为友元
	[[nodiscard]] FragShader* asFragShader()override { return this; } // 返回自身指针
	
protected:
	FragShader() { m_type = ST_Frag; }; // 构造函数，设置着色器类型为片段着色器
};
class VertShader :public ShaderBase
{
public:
	friend class ShaderBase;  // 着色器基类为友元
	[[nodiscard]] VertShader* asVertShader()override { return this; }  // 返回自身指针
	CLASS_DEFAULT_COPY_AND_MOVE(VertShader)
protected:
	VertShader() { m_type = ST_Vert; }; // 构造函数，设置着色器类型为顶点着色器
};


// 着色器程序类
class ShaderProgram
{
public:
	using path_type = std::filesystem::path;// 文件路径类型
	// 程序ID;
	unsigned int ID{};

	// 构造器读取并构建着色器
	ShaderProgram(const path_type& vertexPath, const path_type& fragmentPath);
	ShaderProgram(const VertShader &vert, const FragShader &frag);
	// 使用/激活
	void use() const;
	[[nodiscard]] int location(std::string_view) const; // 获取着色器变量的位置

	template <typename ...T>
	void glUniform(std::string_view, T ... args)const; // 设置着色器变量的值

	// 提供直接创建着色器程序方法，推荐使用
	[[nodiscard]] static std::shared_ptr<ShaderProgram> makeShaderByPath(const path_type& vertexPath, const path_type& fragmentPath);
	[[nodiscard]] static std::shared_ptr<ShaderProgram> makeShaderByName(std::string_view vertexName, std::string_view fragmentName);
private:
	void compile(std::string_view vertexCode, std::string_view fragmentCode);	// 编译链接着色器代码
	void compile(const VertShader& vert, const FragShader& frag);					// 链接顶点着色器和片段着色器
	void checkCompileErrors(unsigned int shader, const std::string& type) const;	// 检查编译错误

	void construct(const path_type& vertexPath, const path_type& fragmentPath);		// 构建着色器程序
	void construct(const VertShader& vert, const FragShader& frag);					// 构建着色器程序
};

// 获取着色器变量的位置
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



// 着色器管理类，用于管理共享着色器，共享着色器需要使用ShaderManager里面的方法创建
class ShaderManager
{
public:
	
	using Path = std::filesystem::path; // 文件路径类型
	static std::list<Path> m_searchPathList; // 着色器搜索路径列表

	static ShaderManager* hInst()
	{
		static ShaderManager* shader = nullptr;
		if(shader == nullptr)
		{
			shader = new ShaderManager;
		}
		return shader;
	}
	[[nodiscard]] std::shared_ptr<FragShader> getOrCreateFragShader(std::string_view path); // 获取或创建片段着色器
	[[nodiscard]] std::shared_ptr<VertShader> getOrCreateVertShader(std::string_view path); // 获取或创建顶点着色器
	[[nodiscard]] std::shared_ptr<ShaderProgram> getOrCreateShaderProgram(std::string_view fragPath, std::string_view vertPath);  // 获取或创建着色器程序
	[[nodiscard]] std::shared_ptr<ShaderProgram> getOrCreateShaderProgram(const VertShader&, const FragShader&); // 获取或创建着色器程序

	[[nodiscard]] constexpr static std::string_view extract_clean_name(std::string_view name);
	
	// shader的搜索路径
	static const std::list<Path>& searchPathList() { return m_searchPathList; }
protected:
	ShaderManager() = default;

private:
	std::map<std::string, std::shared_ptr<ShaderBase>,std::less<>> m_shader_map; // key 由非路径组成的name
	std::map<unsigned long long, std::shared_ptr<ShaderProgram>> m_program_map;
};



