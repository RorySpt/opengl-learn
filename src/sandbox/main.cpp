// ReSharper disable CppClangTidyClangDiagnosticInvalidSourceEncoding
#include "stdafx.h"
#include "display_window.h"


void LogPrintEuler(glm::dvec3 euler)
{
    auto quat = glm::dquat{ euler };
    std::cout << std::format("ŷ���ǣ�{: },{: },{: } ��Ӧ����Ԫ����{: 4f},{: 4f},{: 4f},{: 4f}\n"
        , euler[0], euler[1], euler[2]
        , quat[0], quat[1], quat[2], quat[3]
    );
}
void LogPrintVec(glm::dvec3 vec)
{
    std::cout << std::format("���꣺{: 4f},{: 4f},{: 4f}\n"
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

    // ŷ����ת��Ԫ������Ԫ����תŷ����. ����Ƕȣ�����Ƕ�
    auto transform = [](glm::vec3 euler)
    {
        return glm::degrees(glm::eulerAngles(glm::quat{ glm::radians(euler) }));
    };
    
    auto euler = glm::vec3{ 0,104,0 };
    std::cout << "ԭʼ���ݣ�";
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
