#include <fstream>
#include <Windows.h>
#include <string>
#include <nlohmann/json.hpp>

constexpr std::string_view path_container_jpg("C:/Users/zhang/Pictures/Applications/OpenGLExample/container.jpg");
constexpr std::string_view path_awe_png("C:/Users/zhang/Pictures/Applications/OpenGLExample/awesomeface.png");
constexpr std::string_view dir_shader(R"(C:/WorkSpace/MyFile/MyCode/OpenGLExample/Shader)");
constexpr std::string_view dir_picture(R"(C:/Users/zhang/Pictures/Material)");
constexpr std::string_view path_materials(R"(C:/WorkSpace/MyFile/MyCode/OpenGLExample/Materials/materials.txt)");


constexpr std::string_view data_path = R"(C:\WorkSpace\MyCode\opengl-learn\data\data.json)";
int main()
{
	nlohmann::json jd;

	auto& path = jd["paths"];
	path["shader-dir"] = R"(C:/WorkSpace/MyFile/MyCode/OpenGLExample/Shader)";
	path["picture-dir"] = R"(C:/Users/zhang/Pictures/Material)";
	path["materials-path"] = R"(C:/WorkSpace/MyFile/MyCode/OpenGLExample/Materials/materials.txt)";

	auto& pic = path["picture-files"];
	pic["awe_png"] = "C:/Users/zhang/Pictures/Applications/OpenGLExample/awesomeface.png";
	pic["container_jpg"] = "C:/Users/zhang/Pictures/Applications/OpenGLExample/awesomeface.png";


	auto s = jd.dump(4);

	std::ofstream o(data_path.data());


	o << s;

	o.close();
}	

