#pragma once
#include <memory>
#include <vector>

/**
 * \brief 继承 IRender 
 */
class IRender
{
	virtual void init() = 0;
	virtual void exit() = 0;
	virtual void tick(float deltaTime) = 0;
	virtual void draw(float deltaTime) = 0; // specific invoke by processDraw
};



// 使用局部静态变量保证初始化顺序
inline std::vector<std::shared_ptr<IRender>>& GlobalRenderList()
{
	static std::vector<std::shared_ptr<IRender>> global_render_list;
	return global_render_list;
}


// 通过全局变量构造函数的方式将RenderType自动添加到GlobalRenderList
template<typename RenderType> requires std::is_base_of_v<IRender, RenderType>&& requires {new RenderType; }
class RenderCreateAndRecord
{
public:
	RenderCreateAndRecord()
	{
		GlobalRenderList().emplace_back(std::make_shared<RenderType>());
	}
	RenderCreateAndRecord(std::shared_ptr<RenderType> render)
	{
		GlobalRenderList().emplace_back(render);
	}
};


