# opengl-learn

#### 介绍
个人opengl学习项目

#### 软件架构
软件架构说明

我把一些更详细的说明放到了[这里](https://roryspt.github.io/2023/07/04/learn-opengl/project-details/)

##### 第三方库

- opengl
- glad
- glfw 
- glm
- imgui

资源文件暂时还没有整理，应该还运行不了

#### 编译说明

> ​	使用cmake 3.26+vs2022+vcpkg
>
> ​	暂时仅支持windows平台

关于vcpkg的使用参考：[vcpkg-getting-started](https://vcpkg.io/en/getting-started.html)

1. 打开终端执行以下命令

   ```powershell
   vcpkg install glfw3:x64-windows glm:x64-windows imgui[docking-experimental]:x64-windows
   ```

2. 拉取项目 git clone git@gitee.com:roryspt/opengl-learn.git

3. 进入项目文件夹，执行以下命令（将\<your-vcpkg-directory\>改为你的vcpkg项目所在路径）

   ```bash
   mkdir .build
   cd .build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=<your-vcpkg-directory>\scripts\buildsystems\vcpkg.cmake
   cmake . --build
   ```



#### 使用说明

1.  xxxx
2.  xxxx
3.  xxxx

#### 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request


#### 特技

1.  使用 Readme\_XXX.md 来支持不同的语言，例如 Readme\_en.md, Readme\_zh.md
2.  Gitee 官方博客 [blog.gitee.com](https://blog.gitee.com)
3.  你可以 [https://gitee.com/explore](https://gitee.com/explore) 这个地址来了解 Gitee 上的优秀开源项目
4.  [GVP](https://gitee.com/gvp) 全称是 Gitee 最有价值开源项目，是综合评定出的优秀开源项目
5.  Gitee 官方提供的使用手册 [https://gitee.com/help](https://gitee.com/help)
6.  Gitee 封面人物是一档用来展示 Gitee 会员风采的栏目 [https://gitee.com/gitee-stars/](https://gitee.com/gitee-stars/)
