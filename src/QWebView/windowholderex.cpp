#include "stdafx.h"
#include "windowholderex.h"
#include "windowcontainer.h"
#include "windowcontainerex.h"

#include <psapi.h>
#pragma comment(lib, "psapi.lib")

#define Test_Ex

WindowHolderEx::WindowHolderEx(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() | Qt::Tool | Qt::FramelessWindowHint );

	setAttribute(Qt::WA_DeleteOnClose); //关闭时自动删除
	resize(400, 300); //默认大小

#ifndef Test_Ex
	containers_.push_back(new WindowContainer(&thread_, this));
#else 
	containersex_.push_back(new WindowContainerEx(&thread_, this));
#endif // Test_Ex
	//默认样式
	//
	
	layout_ = QJsonDocument::fromJson("{\"name\":\"1x1\",\"margin\":0,\"spacing\":0,\"items\":[{\"p_x\":0,\"p_y\":0,,\"x\":0,\"y\":0,\"width\":1,\"height\":1}]}").object();
}

WindowHolderEx::~WindowHolderEx()
{
}

bool WindowHolderEx::Setup(QJsonObject param)
{
	thread_.setMaxThreadCount(1); //业务处理线程
	topTimer_ = startTimer(500);

	PreClose(); //关闭残留的附加进程
	HandleMessage(param); //初始化参数
	return true;
}

void WindowHolderEx::Clear()
{
	thread_.waitForDone();
	killTimer(topTimer_);
}

QRect WindowHolderEx::GetItemRect()
{
	return real_rc_;
}


void WindowHolderEx::MoveWidShow(QPoint& pt)
{
	if (!pt.isNull())
	{
		auto moveto = pt + real_rc_.topLeft();
		move(moveto);
		show();
	}
	else
	{
		qDebug() << u8"WindowHolderEx 需要显示的位置无效" << pt;
	}

}

void WindowHolderEx::ReSetGeometry(QRect& rc)
{
	setGeometry(rc);
	show();
}

void WindowHolderEx::OnUrlReqParam(const QJsonObject& req_param_obj)
{
	if (!req_param_obj.isEmpty())
	{
		HandleMessage(req_param_obj);
		show();
		return;
	}
}

void WindowHolderEx::PreClose()
{
#ifndef Test_Ex
	//LOG_TRACE("预关闭已开启的附加应用程序");
	auto classarr = qApp->property("config").toJsonObject()["class"].toArray();
	if (!EnumWindows([](HWND hwnd, LPARAM lParam)
	{
		auto classarr = (QJsonArray*)lParam;
		DWORD processId = 0;
		GetWindowThreadProcessId(hwnd, &processId);
		HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
		char filename[MAX_PATH] = { 0 };
		GetModuleFileNameEx(process, nullptr, filename, MAX_PATH);
		QFileInfo fileInfo(filename);
		for (auto classref : *classarr)
		{
			auto classobj = classref.toObject();
			if (classobj["name"].toString() == fileInfo.baseName())
			{
				if (!TerminateProcess(process, 0))
				{
					//LOG_ERROR("关闭进程失败, process: {}, err: {}", processId, GetLastError());
				}
			}
		}
		CloseHandle(process);
		return TRUE;
	}, (LPARAM)&classarr))
	{
		//LOG_ERROR("预关闭失败, err: {}", GetLastError());
	}
#endif // Test_Ex


}

void WindowHolderEx::HandleMessage(QJsonObject message)
{
	if (message.contains("quit") && message["quit"].toBool())
	{
		//LOG_TRACE("关闭应用程序");
		close();
		return;
	}
	if (message.contains("visible") && message["visible"].isBool())
	{
		auto visible = message["visible"].toBool();
		//LOG_TRACE("设置显示隐藏应用程序, visible: {}", visible);
		setVisible(visible);
	}

	if (message.contains("x") && message.contains("y") && message["x"].isDouble() && message["y"].isDouble() && 
		message.contains("p_x") && message.contains("p_y") && message["p_x"].isDouble() && message["p_y"].isDouble())
	{
		auto x = message["x"].toInt() , y = message["y"].toInt();
		auto p_x = message["p_x"].toInt(), p_y = message["p_y"].toInt();
		//LOG_TRACE("设置窗口位置, {}x{}", x, y);
		real_rc_.setTopLeft(QPoint(x, y)); 
		auto pt = QPoint(p_x, p_y) + QPoint(x, y);
		move(pt);
	}
	if (message.contains("width") && message.contains("height") && message["width"].isDouble() && message["height"].isDouble())
	{
		auto width = message["width"].toInt(), height = message["height"].toInt();
		//LOG_TRACE("设置窗口大小, {}x{}", width, height);
		real_rc_.setSize(QSize(width, height));
		resize(width, height);
	}
	if (message.contains("layout") && message["layout"].isString())
	{
		auto layout = message["layout"].toString();
		//LOG_TRACE("设置窗口布局: {}", layout.toStdString());
		HandleLayout(layout);
	}
	if (message.contains("close") && message["close"].isArray())
	{
		auto closearr = message["close"].toArray();
		//LOG_TRACE("关闭窗口: {}", QJsonDocument(closearr).toJson(QJsonDocument::Compact).toStdString());
		HandleClose(closearr);
	}
	if (message.contains("open") && message["open"].isArray())
	{
		auto openarr = message["open"].toArray();
		//LOG_TRACE("打开窗口: {}", QJsonDocument(openarr).toJson(QJsonDocument::Compact).toStdString());

		//实际打开一个进程，使其展示在指定位置
		HandleOpen(openarr);
	}
}

void WindowHolderEx::HandleLayout(QString name)
{
	auto layouts = qApp->property("config").toJsonObject()["layout"].toArray();
	for (auto layoutRef : layouts)
	{
		auto layout = layoutRef.toObject();
		if (layout["name"].toString() == name)
		{
			layout_ = layout;
			break;
		}
	}
	//添加容器窗口
	auto items = layout_["items"].toArray();

#ifndef Test_Ex

	if (containers_.size() < items.size())
	{
		auto count = items.size() - containers_.size();
		for (auto i = 0; count > i; ++i)
		{
			containers_.push_back(new WindowContainer(&thread_, this));
		}
	}

#else 

	if (containersex_.size() < items.size())
	{
		auto count = items.size() - containersex_.size();
		for (auto i = 0; count > i; ++i)
		{
			containersex_.push_back(new WindowContainerEx(&thread_, this));
		}
	}


#endif // Test_Ex




	QResizeEvent event(size(), QSize(0, 0));
	qApp->sendEvent(this, &event);
}

void WindowHolderEx::HandleClose(QJsonArray closes)
{
	for (auto closeRef : closes)
	{
		auto close = closeRef.toObject();
		if (close.contains("window") && close["window"].isDouble())
		{
			auto window = close["window"].toInt();

#ifndef Test_Ex
			if (containers_.size() > window)
			{
				containers_[window]->Close();
			}
#else 

			if (containersex_.size() > window)
			{
				containersex_[window]->Close();
			}
#endif // Test_Ex


		}
	}
}

void WindowHolderEx::HandleOpen(QJsonArray opens)
{
	for (auto openRef : opens)
	{
		auto open = openRef.toObject();
		if (!open.contains("window") || !open["window"].isDouble())
		{
			continue;
		}
		auto window = open["window"].toInt();

#ifndef Test_Ex
		if (containers_.size() <= window)
		{
			continue;
		}
#else 
		if (containersex_.size() <= window)
		{
			continue;
		}
#endif // Test_Ex



		qDebug() << u8"open: " << open;

#ifndef Test_Ex
		QFileInfo info(open["path"].toString());
		auto cls = GetClass(info.completeBaseName());
		if (!cls.isEmpty())
		{
			open["class"] = cls;
			containers_[window]->Open(open);
	
		}
#else 
		containersex_[window]->Open(open);
#endif // Test_Ex



	}
}

QString WindowHolderEx::GetClass(QString name)
{
	auto classes = qApp->property("config").toJsonObject()["class"].toArray();
	for (auto classRef : classes)
	{
		auto cls = classRef.toObject();
		if (cls["name"].toString() == name)
		{
			return cls["class"].toString();
		}
	}
	return QString();
}

void WindowHolderEx::resizeEvent(QResizeEvent *event)
{
	auto gridLayout = ui.gl_win_holder_ex;
	gridLayout->setMargin(layout_["margin"].toInt());
	gridLayout->setSpacing(layout_["spacing"].toInt());
	auto items = layout_["items"].toArray();

#ifndef Test_Ex
	for (auto i = 0; items.size() > i; ++i)
	{
		auto item = items[i].toObject();
		gridLayout->addWidget(containers_[i], item["y"].toInt(), item["x"].toInt(),
			item["height"].toInt(), item["width"].toInt());
		containers_[i]->show();
	}
	for (auto i = items.size(); containers_.size() > i; ++i)
	{
		containers_[i]->hide();
	}
#else 
	for (auto i = 0; items.size() > i; ++i)
	{
		auto item = items[i].toObject();
		gridLayout->addWidget(containersex_[i], item["y"].toInt(), item["x"].toInt(),
			item["height"].toInt(), item["width"].toInt());
		containersex_[i]->show();
	}
	for (auto i = items.size(); containersex_.size() > i; ++i)
	{
		containersex_[i]->hide();
	}
#endif // Test_Ex






}

void WindowHolderEx::timerEvent(QTimerEvent *event)
{
	qDebug() << "WindowHolderEx::timerEvent";
	SetWindowPos((HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}
