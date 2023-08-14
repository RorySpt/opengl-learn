// ReSharper disable CppClangTidyClangDiagnosticInvalidSourceEncoding
#include "stdafx.h"
#include "display_window.h"


void LogPrintEuler(glm::dvec3 euler)
{
    auto quat = glm::dquat{ euler };
    std::cout << std::format("欧拉角：{: },{: },{: } 对应的四元数：{: 4f},{: 4f},{: 4f},{: 4f}\n"
        , euler[0], euler[1], euler[2]
        , quat[0], quat[1], quat[2], quat[3]
    );
}
void LogPrintVec(glm::dvec3 vec)
{
    std::cout << std::format("坐标：{: 4f},{: 4f},{: 4f}\n"
        , vec[0], vec[1], vec[2]
    );
}
int main(int argc,char** argv)
{

    for(int i = 0;i<argc;++i)
    {
        std::cout << argv[i] << std::endl;
    }
    DisplayWindow w;
    
    
    w.exec();

    // 欧拉角转四元数，四元数再转欧拉角. 输入角度，输出角度
    auto transform = [](glm::vec3 euler)
    {
        return glm::degrees(glm::eulerAngles(glm::quat{ glm::radians(euler) }));
    };
    
    auto euler = glm::vec3{ 0,104,0 };
    std::cout << "原始数据：";
    LogPrintEuler(euler);
    
    euler = transform(euler);
    LogPrintEuler(euler);
    
    euler = transform(euler);
    LogPrintEuler(euler);
    
    euler = transform(euler);
    LogPrintEuler(euler);
    
    euler = transform(euler);
    LogPrintEuler(euler);

    glm::quat r_y{ glm::radians(glm::vec3(0, 104, 0))};
    glm::quat r_z{ glm::radians(glm::vec3(0, 0, 45))};
    //glm::quat{ glm::radians(glm::vec3(0, 45, 0))};
    LogPrintVec(r_y * glm::vec3{1, 0, 0});
    LogPrintVec(r_z * glm::vec3{1, 0, 0});

    return 0;
}
