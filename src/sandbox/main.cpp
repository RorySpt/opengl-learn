#include "stdafx.h"

#include "display_window.h"
#include "shader.h"

int main(int argc,char** argv)
{

    for(int i = 0;i<argc;++i)
    {
        std::cout << argv[i] << std::endl;
    }
    DisplayWindow w;

    
    w.exec();
    
    return 0;
}
