// ReSharper disable CppClangTidyClangDiagnosticInvalidSourceEncoding
#include "stdafx.h"
#include "display_window.h"


void LogPrint(glm::dvec3 euler)
{
    auto quat = glm::dquat{ euler };
    std::cout << std::format("欧拉角：{: },{: },{: } 对应的四元数：{: 4f},{: 4f},{: 4f},{: 4f}\n"
        , euler[0], euler[1], euler[2]
        , quat[0], quat[1], quat[2], quat[3]
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

    //// 欧拉角转四元数，四元数再转欧拉角. 输入角度，输出角度
    //auto transform = [](glm::dvec3 euler)
    //{
    //    return glm::degrees(glm::eulerAngles(glm::dquat{ glm::radians(euler) }));
    //};
    //
    //auto euler = glm::dvec3{ 0,98,0 };
    //std::cout << "原始数据：";
    //LogPrint(euler);
    //
    //euler = transform({ 0,98,0 });
    //LogPrint(euler);
    //
    //euler = transform(euler);
    //LogPrint(euler);
    //
    //euler = transform(euler);
    //LogPrint(euler);
    //
    //euler = transform(euler);
    //LogPrint(euler);
    //return 0;
}
