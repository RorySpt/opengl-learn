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
	: DisplayWindow()
	, d_ptr(std::make_shared<IlluminationWidgetPrivate>(this))
{

}

void IlluminationWidget::render_init()
{
	//DisplayWindow::render_init();
}

void IlluminationWidget::render_exit()
{
	//DisplayWindow::render_exit();
}

void IlluminationWidget::render_loop(float delta_time)
{
	//DisplayWindow::render_loop(delta_time);
}
