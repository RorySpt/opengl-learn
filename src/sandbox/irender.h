#pragma once
#include <memory>
#include <vector>

class IRender
{
	virtual void init() = 0;
	virtual void render_exit() = 0;
	virtual void render_tick(float deltaTime) = 0;
	virtual void render_draw(float deltaTime) = 0; // specific invoke by processDraw
};


inline static std::vector<std::shared_ptr<IRender>> global_render_list;


template<typename RenderType> requires std::is_base_of_v<IRender, RenderType>&& requires {new RenderType; }
class RenderCreateAndRecord
{
	RenderCreateAndRecord()
	{
		global_render_list.emplace_back(std::make_shared<RenderType>());
	}
	RenderCreateAndRecord(std::shared_ptr<RenderType> render)
	{
		global_render_list.emplace_back(render);
	}
};


