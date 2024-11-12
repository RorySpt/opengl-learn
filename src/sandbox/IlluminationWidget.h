#pragma once
#include "display_window.h"
#include <memory>

class IlluminationWidgetPrivate;
class IlluminationWidget :
    public DisplayWindow
{
	Q_DECLARE_PRIVATE(IlluminationWidget)
public:
	IlluminationWidget();
	//virtual ~IlluminationWidget() = default;
	IlluminationWidget(const IlluminationWidget& other) = delete;
	IlluminationWidget(IlluminationWidget&& other) noexcept = delete;
	IlluminationWidget& operator=(const IlluminationWidget& other) = delete;
	IlluminationWidget& operator=(IlluminationWidget&& other) noexcept = delete;

protected:
	void render_init() ;
	void render_exit() ;
	void render_loop(float delta_time) ;
private:
	std::shared_ptr<IlluminationWidgetPrivate> d_ptr;
};

