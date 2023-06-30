#include "stdafx.h"
#include "IlluminationWidget.h"
class IlluminationWidgetPrivate
{
	Q_DECLARE_PUBLIC(IlluminationWidget)
public:
	IlluminationWidgetPrivate(IlluminationWidget* parent):q_ptr(parent) {}

private:
	IlluminationWidget* q_ptr;
};
IlluminationWidget::IlluminationWidget()
	: CGLWindow()
	, d_ptr(std::make_shared<IlluminationWidgetPrivate>(this))
{

}

void IlluminationWidget::render_init()
{
	//CGLWindow::render_init();
}

void IlluminationWidget::render_exit()
{
	//CGLWindow::render_exit();
}

void IlluminationWidget::render_loop(float delta_time)
{
	//CGLWindow::render_loop(delta_time);
}
