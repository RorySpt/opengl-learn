#include "stdafx.h"

#include "shader.h"


unsigned ShaderBase::id() const
{ return m_hShader; }

bool ShaderBase::valid() const
{ return m_isValid; }

ShaderBase::shader_type ShaderBase::type() const
{ return m_type; }

std::shared_ptr<ShaderBase> ShaderBase::makeShaderByPath(const ShaderBase::path_type& shaderPath, shader_type type)
{
    std::string shaderCode;
    std::ifstream shaderFile;
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    if (type == ST_Unknown)
    {
        type = identifyShaderByPath(shaderPath);
        if (type == ST_Unknown)
        {
            std::cout << "ERROR::SHADER::UNKNOWN_SHADER_TYPE " << shaderPath << std::endl;
            return nullptr;
        }
    }


    try
    {
        std::stringstream shaderString;
        // 打开文件
        shaderFile.open(shaderPath);
        shaderString << shaderFile.rdbuf();
        // 关闭文件处理器
        shaderFile.close();
        // 转换数据流到string
        shaderCode = shaderString.str();
    }
    catch (std::ifstream::failure& )
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ "<< shaderPath << std::endl;
        return nullptr;
    }
    
    return makeShaderByCode(shaderCode, type);
}

std::shared_ptr<ShaderBase> ShaderBase::makeShaderByName(const std::string& shaderName, shader_type type)
{
    if(type == shader_type::ST_Unknown)
    {
        std::cout << "ERROR::SHADER::UNKNOWN_SHADER_TYPE " << shaderName << std::endl;
        return nullptr;
    }
    std::string_view extension = ST_Frag ? "frag" : "vert";

    for(auto& path:ShaderManager::searchPathList())
    {
        std::filesystem::path shaderPath;
        // 文件名以目录分隔符开头时忽略分隔符，原始处理方式为替换原根路径
        if(shaderName.at(0)=='/'|| shaderName.at(0) == '\\')
            shaderPath = path / std::string_view(shaderName.data() + 1);
        else
			shaderPath = path / shaderName;
        shaderPath.replace_extension(type == ST_Frag ? "frag" : "vert");
        if (exists(shaderPath))
            return makeShaderByPath(shaderPath);
    }
    
    std::cout << "ERROR::SHADER::SHADER_NOT_EXISTS" << std::endl;
    return nullptr;

}

std::shared_ptr<ShaderBase> ShaderBase::makeShaderByCode(const std::string& shaderCode, shader_type type)
{
    std::shared_ptr<ShaderBase> shader;
    switch (type)
    {
    case ST_Frag:shader = std::shared_ptr<ShaderBase>(new FragShader);break;
    case ST_Vert:shader = std::shared_ptr<ShaderBase>(new VertShader);break;
    case ST_Unknown:
    default:return nullptr;
    }
    shader->m_type = type;
    shader->compile(shaderCode);
    return shader;
}

ShaderBase::~ShaderBase()
{
	if (valid())
	{
		glDeleteShader(m_hShader);
	}
}

ShaderBase::shader_type ShaderBase::identifyShaderByPath(const ShaderBase::path_type& shaderPath)
{
	const auto extension = shaderPath.extension().string();
    if (extension == (".frag"))
        return shader_type::ST_Frag;
    else if (extension == (".vert"))
        return shader_type::ST_Vert;
    else
        return shader_type::ST_Unknown;
}

void ShaderBase::compile(const std::string& shaderCode)
{
    if (type() == ST_Unknown)
        return;
    m_hShader = glCreateShader(type() == ST_Vert ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
    const char* vShaderCode = shaderCode.c_str();
    glShaderSource(m_hShader, 1, &vShaderCode, nullptr);
    glCompileShader(m_hShader);
    // 获取着色器编译状态信息，打印编译错误（如果有的话）
    checkCompileErrors();
}

void ShaderBase::checkCompileErrors()
{
    int success;
    char infoLog[1024];

    glGetShaderiv(m_hShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(m_hShader, 1024, nullptr, infoLog);
        std::cout << std::format("ERROR::SHADER::COMPILATION_FAILED of type: {}\n", infoLog) << std::endl;
    }
    else
    {
        m_isValid = true;
    }
}

ShaderProgram::ShaderProgram(const path_type& vertexPath, const path_type& fragmentPath)
{
    construct(vertexPath, fragmentPath);
}


ShaderProgram::ShaderProgram(const VertShader& vert, const FragShader& frag)
{
    construct(vert, frag);
}

void ShaderProgram::construct(const path_type& vertexPath, const path_type& fragmentPath)
{
	const std::shared_ptr vert = ShaderBase::makeShaderByPath(vertexPath, ShaderBase::ST_Vert);
	const std::shared_ptr frag = ShaderBase::makeShaderByPath(fragmentPath, ShaderBase::ST_Frag);

    if (vert->valid() && frag->valid())
        compile(*vert->asVertShader(), *frag->asFragShader());
}

void ShaderProgram::construct(const VertShader& vert, const FragShader& frag)
{
    compile(vert, frag);
}

void ShaderProgram::use() const
{
    glUseProgram(ID);
}


void ShaderProgram::compile(const std::string& vertexCode, const std::string& fragmentCode)
{
    unsigned int vertexShader, fragmentShader;

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 编译顶点着色器
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
    glCompileShader(vertexShader);
    // 获取着色器编译状态信息，打印编译错误（如果有的话）
    checkCompileErrors(vertexShader, "VERTEX");
    

    // 编译片段着色器
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
    glCompileShader(fragmentShader);
    checkCompileErrors(fragmentShader, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");


    //删除着色器对象，它们已经链接到我们的程序中了，已经不再需要了
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void ShaderProgram::compile(const VertShader& vert, const FragShader& frag)
{
    if (!vert.valid() || !frag.valid())
    {
        std::cout << std::format("ERROR::SHADER::INPUT_SHADER_INVALID!\n") << std::endl;
        return;
    }
    ID = glCreateProgram();
    glAttachShader(ID, vert.m_hShader);
    glAttachShader(ID, frag.m_hShader);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
}

void ShaderProgram::checkCompileErrors(unsigned shader, const std::string& type) const
{
    int success;
    char infoLog[1024];

    if(type == "PROGRAM")
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << std::format("ERROR::SHADER::PROGRAM_LINKING_ERROR of type: {}\n", infoLog) << std::endl;
        }
    }else
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << std::format("ERROR::SHADER::COMPILATION_FAILED of type: {}\n", infoLog) << std::endl;
        }
    }
   
}

std::shared_ptr<FragShader> ShaderManager::getOrCreateFragShader(const std::string& path)
{
    auto& ref = m_shader_map[path];
    if (!ref)
    {
        ref = ShaderBase::makeShaderByPath(path, ShaderBase::ST_Frag);
    }
    return std::dynamic_pointer_cast<FragShader>(ref);
}

std::shared_ptr<VertShader> ShaderManager::getOrCreateVertShader(const std::string& path)
{
    auto& ref = m_shader_map[path];
    if (!ref)
    {
        ref = ShaderBase::makeShaderByPath(path, ShaderBase::ST_Vert);
    }
    return std::dynamic_pointer_cast<VertShader>(ref);
}

std::shared_ptr<ShaderProgram> ShaderManager::getOrCreateShaderProgram(const std::string& fragPath, const std::string& vertPath)
{
    auto vert = getOrCreateVertShader(vertPath);
    auto frag = getOrCreateFragShader(fragPath);
    if (!frag || !vert)return nullptr;
    unsigned long long key = (static_cast<unsigned long long>(vert->id()) << sizeof(unsigned int) * 8) + frag->id();
    auto& ref = m_program_map[key];
    if (!ref)
    {
        ref = ShaderProgram::makeShaderByPath(vertPath, fragPath);
    }
    return std::dynamic_pointer_cast<ShaderProgram>(ref);
}

std::shared_ptr<ShaderProgram> ShaderManager::getOrCreateShaderProgram(const VertShader& vert, const FragShader& frag)
{
    unsigned long long key = (static_cast<unsigned long long>(vert.id()) << sizeof(unsigned int) * 8) + frag.id();
    auto& ref = m_program_map[key];
    if (!ref)
    {
        ref = std::make_shared<ShaderProgram>(vert, frag);
    }
    return std::dynamic_pointer_cast<ShaderProgram>(ref);
}


std::list<ShaderManager::path_type> ShaderManager::m_searchPathList;

class _SearchPathListAdder
{
public:
    _SearchPathListAdder(ShaderManager::path_type path)
	{
        ShaderManager::m_searchPathList.emplace_back(path);
	}
};

_SearchPathListAdder _adder(comm::dir_shader);