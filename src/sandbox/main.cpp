// ReSharper disable CppClangTidyClangDiagnosticInvalidSourceEncoding
#include "stdafx.h"
#include "display_window.h"
#include "utils/synced_stream.h"

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

extern void common_unit_testing();
extern void delegate_unit_test();

int main(int argc,char** argv)
{
    //sync::println("hello, world!");
    //delegate_unit_test();
    //thread_pool_unit_test();
    for(int i = 0;i<argc;++i)
    {
        sync::println(comm::GetCurrentTimeString() + ' ' +argv[i]);
    }
    std::chrono::hh_mm_ss hh_mm_ss(std::chrono::zoned_time{ std::chrono::current_zone(),
        std::chrono::system_clock::now() }.get_local_time().time_since_epoch());

    sync::println("{}:{}:{}", (hh_mm_ss.hours() - std::chrono::duration_cast<std::chrono::days>(hh_mm_ss.hours())).count(), hh_mm_ss.minutes().count(), hh_mm_ss.seconds().count());
    DisplayWindow w;
    
    
    w.exec();

    return 0;
}
