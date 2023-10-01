// ReSharper disable CppClangTidyClangDiagnosticInvalidSourceEncoding
#include "stdafx.h"
#include "display_window.h"
#include "utils/synced_stream.h"
#include "utils/thread_pool.h"


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
void common_unit_testing();
void delegate_unit_test();
int main(int argc,char** argv)
{
    //sync::println("hello, world!");
    //delegate_unit_test();
    //thread_pool_unit_test();
    for(int i = 0;i<argc;++i)
    {
        std::cout << argv[i] << std::endl;
    }
    DisplayWindow w;
    
    
    w.exec();

    return 0;
}
