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

// ͳһ����GLfloat������glUniform���غ���
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

// Ƭ����ɫ�����ǰ������
class FragShader;
// ������ɫ�����ǰ������
class VertShader;

// ��ɫ������
class ShaderBase
{
public:
	enum EShaderType
	{
		ST_Auto,// �Զ�ʶ�����ͣ�������չ��.frag/.vert
		ST_Frag,// Ƭ����ɫ������
		ST_Vert // ������ɫ������
	};
	using path_type = std::filesystem::path;// �ļ�·������
	friend class ShaderProgram;				// ��ɫ�����������Ԫ
	CLASS_DEFAULT_COPY_AND_MOVE(ShaderBase) // Ĭ�ϵĸ��ƺ��ƶ����캯��

	[[nodiscard]] unsigned int id() const;	// ������ɫ��ID
	[[nodiscard]] bool valid() const;		// �ж���ɫ���Ƿ���Ч
	[[nodiscard]] EShaderType type()const;	// ������ɫ������
	[[nodiscard]] virtual FragShader* asFragShader() { return nullptr; } // ����Ƭ����ɫ������ָ�룬Ĭ�Ϸ���nullptr
	[[nodiscard]] virtual VertShader* asVertShader() { return nullptr; } // ���ض�����ɫ������ָ�룬Ĭ�Ϸ���nullptr


	// ͨ��·������Ƭ����ɫ��/������ɫ�����ɴ������;����������辭��ShaderProgram����
	[[nodiscard]] static std::shared_ptr<ShaderBase> makeShaderByPath(const std::filesystem::path& shaderPath, EShaderType type = ST_Auto);
	// ͨ�����ֲ�����ɫ������
	[[nodiscard]] static std::shared_ptr<ShaderBase> makeShaderByName(std::string_view shaderName, EShaderType type = ST_Auto);
	// ʹ�ô����ɫ��������ַ���������ɫ����
	[[nodiscard]] static std::shared_ptr<ShaderBase> makeShaderByCode(std::string_view shaderCode, EShaderType type);
	virtual ~ShaderBase();

protected:
	ShaderBase() = default;

	void compile(std::string_view shaderCode); // ������ɫ������
	void checkCompileErrors();					// ���������

	// ͨ��·��ʶ����ɫ������
	[[nodiscard]] static EShaderType identifyShaderByPath(const path_type& shaderPath);


	EShaderType m_type = ST_Auto;	// ��ɫ������
private:
	unsigned int m_hShader = 0;		// ��ɫ�����
	bool m_isValid = false;			// ��ɫ���Ƿ���Ч
};


// Ƭ����ɫ����
class FragShader:public ShaderBase
{
public:
	CLASS_DEFAULT_COPY_AND_MOVE(FragShader)

	friend class ShaderBase; // ��ɫ������Ϊ��Ԫ
	[[nodiscard]] FragShader* asFragShader()override { return this; } // ��������ָ��
	
protected:
	FragShader() { m_type = ST_Frag; }; // ���캯����������ɫ������ΪƬ����ɫ��
};
class VertShader :public ShaderBase
{
public:
	friend class ShaderBase;  // ��ɫ������Ϊ��Ԫ
	[[nodiscard]] VertShader* asVertShader()override { return this; }  // ��������ָ��
	CLASS_DEFAULT_COPY_AND_MOVE(VertShader)
protected:
	VertShader() { m_type = ST_Vert; }; // ���캯����������ɫ������Ϊ������ɫ��
};


// ��ɫ��������
class ShaderProgram
{
public:
	using path_type = std::filesystem::path;// �ļ�·������
	// ����ID;
	unsigned int ID{};

	// ��������ȡ��������ɫ��
	ShaderProgram(const path_type& vertexPath, const path_type& fragmentPath);
	ShaderProgram(const VertShader &vert, const FragShader &frag);
	// ʹ��/����
	void use() const;
	[[nodiscard]] int location(std::string_view) const; // ��ȡ��ɫ��������λ��

	template <typename ...T>
	void glUniform(std::string_view, T ... args)const; // ������ɫ��������ֵ

	// �ṩֱ�Ӵ�����ɫ�����򷽷����Ƽ�ʹ��
	[[nodiscard]] static std::shared_ptr<ShaderProgram> makeShaderByPath(const path_type& vertexPath, const path_type& fragmentPath);
	[[nodiscard]] static std::shared_ptr<ShaderProgram> makeShaderByName(std::string_view vertexName, std::string_view fragmentName);
private:
	void compile(std::string_view vertexCode, std::string_view fragmentCode);	// ����������ɫ������
	void compile(const VertShader& vert, const FragShader& frag);					// ���Ӷ�����ɫ����Ƭ����ɫ��
	void checkCompileErrors(unsigned int shader, const std::string& type) const;	// ���������

	void construct(const path_type& vertexPath, const path_type& fragmentPath);		// ������ɫ������
	void construct(const VertShader& vert, const FragShader& frag);					// ������ɫ������
};

// ��ȡ��ɫ��������λ��
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



// ��ɫ�������࣬���ڹ�������ɫ����������ɫ����Ҫʹ��ShaderManager����ķ�������
class ShaderManager
{
public:
	
	using Path = std::filesystem::path; // �ļ�·������
	static std::list<Path> m_searchPathList; // ��ɫ������·���б�

	static ShaderManager* hInst()
	{
		static ShaderManager* shader = nullptr;
		if(shader == nullptr)
		{
			shader = new ShaderManager;
		}
		return shader;
	}
	[[nodiscard]] std::shared_ptr<FragShader> getOrCreateFragShader(std::string_view path); // ��ȡ�򴴽�Ƭ����ɫ��
	[[nodiscard]] std::shared_ptr<VertShader> getOrCreateVertShader(std::string_view path); // ��ȡ�򴴽�������ɫ��
	[[nodiscard]] std::shared_ptr<ShaderProgram> getOrCreateShaderProgram(std::string_view fragPath, std::string_view vertPath);  // ��ȡ�򴴽���ɫ������
	[[nodiscard]] std::shared_ptr<ShaderProgram> getOrCreateShaderProgram(const VertShader&, const FragShader&); // ��ȡ�򴴽���ɫ������

	[[nodiscard]] constexpr static std::string_view extract_clean_name(std::string_view name);
	
	// shader������·��
	static const std::list<Path>& searchPathList() { return m_searchPathList; }
protected:
	ShaderManager() = default;

private:
	std::map<std::string, std::shared_ptr<ShaderBase>,std::less<>> m_shader_map; // key �ɷ�·����ɵ�name
	std::map<unsigned long long, std::shared_ptr<ShaderProgram>> m_program_map;
};



