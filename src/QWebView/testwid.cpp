#include "stdafx.h"
#include "testwid.h"

TestWid::TestWid(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
}

TestWid::~TestWid()
{
}
