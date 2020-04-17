#pragma once

#include <QWidget>
#include "ui_testwid.h"

class TestWid : public QWidget
{
	Q_OBJECT

public:
	TestWid(QWidget *parent = Q_NULLPTR);
	~TestWid();

private:
	Ui::TestWid ui;
};
