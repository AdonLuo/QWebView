#include "stdafx.h"
#include "windowcontainerex.h"

WindowContainerEx::WindowContainerEx(QThreadPool* thread, QWidget *parent)
	: QFrame(parent)
	, thread_(thread)
	, monitor_view_wid_(nullptr)
{
	SetupUI();

	connect(this, &WindowContainerEx::Opened, this, [this](quint64 process, quint64 hwnd)
	{
		tips_->hide();
	});
	connect(this, &WindowContainerEx::TipsTextChanged, this, [this](QString text)
	{
		tips_->setText(text);
	});
}

WindowContainerEx::~WindowContainerEx()
{
	Close();
}

void WindowContainerEx::Open(QJsonObject open)
{
	Close();
	tips_->setText(u8"正在打开,请稍后...");
	tips_->show();
	auto command = open["path"].toString() + " " + open["param"].toString();
	if (!monitor_view_wid_)
	{
		monitor_view_wid_ = new MonitorDemo;
	}

	auto rc = contentsRect();
	auto real_rc = QRect(mapToGlobal(rc.topLeft()), size());;
	if (monitor_view_wid_)
	{
		monitor_view_wid_->InitShow(real_rc);
		//connect(monitor_view_wid_, &MonitorDemo::NotifyCamPragam, this, &QWebView::onNotifyCamPragam);

		QJsonObject tmp;
		tmp["dev_type"] = "1";
		tmp["ip"] = "172.16.26.1";
		tmp["port"] = "8000";
		tmp["user"] = "admin";

		tmp["pwd"] = "evecom@123";
		tmp["channel_id"] = "15";
		tmp["dev_name"] = "10F北侧消防通道05";
		tmp["dev_id"] = "ECD5E9C0DB2C41BD86D8F739302B73F7";


		monitor_view_wid_->SwitchMonitor(tmp);
		tips_->hide();
	}
// 	QtConcurrent::run(thread_, [=]()
// 	{
// 
// 
// 
// 		if (!monitor_view_wid_)
// 		{
// 			emit TipsTextChanged(u8"打开失败!");
// 			//TerminateProcess(processInfo.hProcess, 0);
// 			return;
// 		}
// 		//emit Opened((quint64)processInfo.hProcess, (quint64)hwnd);
// 	});


}

void WindowContainerEx::Close()
{
	if (monitor_view_wid_)
	{
		monitor_view_wid_->close();
	}
	close();
}

void WindowContainerEx::SetupUI()
{
	setObjectName("windowcontainerex");

	auto layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	tips_ = new QLabel(this);
	tips_->setObjectName("tips");
	tips_->setAlignment(Qt::AlignCenter);
	tips_->setText(u8"正在打开,请稍后...");
	tips_->hide();
	layout->addWidget(tips_);
}

void WindowContainerEx::resizeEvent(QResizeEvent *event)
{

		auto rc = contentsRect();
		qDebug() << "contentsRect: " << rc << mapToGlobal(rc.topLeft()) << size();
		auto real_rc = QRect(mapToGlobal(rc.topLeft()), size());;


		qDebug() << "geometry: " << geometry() << " Trans: " << real_rc;

		//SetWindowPos(hwnd_, HWND_TOP, rc.x(), rc.y(), rc.width(), rc.height(), SWP_SHOWWINDOW);
		if (monitor_view_wid_)
		{
			monitor_view_wid_->InitShow(real_rc);
		}
}

