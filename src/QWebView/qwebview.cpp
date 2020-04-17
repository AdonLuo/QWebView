#include "stdafx.h"
#include "qwebview.h"
#include <QProcess>

QWebView::QWebView(QWidget *parent)
	: QMainWindow(parent)
	, is_not_max_(true)
	, is_move_(false)
	, curCefView_(nullptr)
	, win_holder_(nullptr)
	, monitor_view_wid_(nullptr)
	, wid_(nullptr)
{
	ui.setupUi(this);

	setWindowFlags(windowFlags() |/*Qt::Tool |*//* Qt::WindowStaysOnTopHint |*/ Qt::FramelessWindowHint);

	connect(ui.btn_changeColor, SIGNAL(clicked()), this, SLOT(onBtnChangeColorClicked()));

	connect(ui.e_url, &QLineEdit::returnPressed,this,&QWebView::onBtnGoClicked);
	connect(ui.btn_go, &QPushButton::clicked, this, &QWebView::onBtnGoClicked);

	connect(ui.btn_go_back, &QPushButton::clicked, this, &QWebView::onBtnGoClicked);
	connect(ui.btn_reload_or_stop, &QPushButton::clicked, this, &QWebView::onBtnReloadOrStopClicked);
	connect(ui.btn_close, &QPushButton::clicked, this, &QWebView::onBtnCloseClicked);

	size_ = QSize(this->width(), this->height());


	QDir dir = QCoreApplication::applicationDirPath();
	auto uri = QDir::toNativeSeparators(dir.filePath("web/QCefViewTestPage.html"));
	qDebug() << uri;

	curCefView_ = new CustomCefView(uri, ui.Container);
	//auto urlCefView = new CustomCefView("http://www.baidu.com/", this);
	connect(curCefView_, &CustomCefView::UrlReqSignal, this, &QWebView::OnUrlReq, Qt::QueuedConnection);
	connect(curCefView_, &CustomCefView::loadingStateChangedSignal, this, &QWebView::onLoadingStateChangedSignal, Qt::QueuedConnection);
	connect(curCefView_, &CustomCefView::StartExeSignal, this, &QWebView::onStartExe, Qt::QueuedConnection);
	ui.vl_container->addWidget(curCefView_);


// 	win_holder_ = new WindowHolderEx(ui.Container);
// 	connect(this, &QWebView::UrlReqParamSignal, win_holder_, &WindowHolderEx::OnUrlReqParam, Qt::QueuedConnection);
// 	win_holder_->hide();


// 	dlg_ = new TestDlg(curCefView_);
// 	connect(curCefView_, &CustomCefView::UrlReqSignal, dlg_, &TestDlg::OnUrlReq, Qt::QueuedConnection);
// 	dlg_->show();
// 
// 	curCefView_->stackUnder(dlg_);


	ScreenInfo();
}


void QWebView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if ((event->globalPos() - pos()).y() < ui.frame_title->size().height())
		{
			is_move_ = true;
			last_pos_ = event->globalPos() - pos();
		}

		auto selected_wid = qobject_cast<QWidget*>(childAt(event->pos()));
		if (selected_wid)
		{
			qDebug() << selected_wid->objectName() << "   " << selected_wid->parent()->objectName();
		}
	}
	else
	{
		auto tp1 = ui.Container->frameGeometry().topLeft();		auto move_tp1 = mapToParent(tp1);
		qDebug() << "QWebView::mousePressEvent RightClick: " << tp1 << " -- "<< move_tp1;
	}

	event->ignore();
}

void QWebView::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if ((event->globalPos() - pos()).y() < ui.frame_title->size().height())
		{
			is_move_ = false;
		}
	}
	event->ignore();
}

void QWebView::mouseMoveEvent(QMouseEvent *event)
{
	if (is_move_ == true)
	{
		if (is_not_max_ == true)
		{
			auto move_to = event->globalPos() - last_pos_;
			//qDebug() << "QWebView::mouseMoveEvent : "<< move_to << "event->globalPos(): " << event->globalPos() << "last_pos_: " << last_pos_;
			move(move_to);
		}
		else
		{
			QPoint topLeft;

			QPoint mid_temp(this->width() / 2, this->height() / 2);

			topLeft = event->globalPos() - QPoint(size_.width() * float(event->globalPos().x()) /
				float(this->width()), event->globalPos().y());
			rect_ = QRect(topLeft, size_);
			setGeometry(rect_);
// 			auto btn_max_sty = QString(
// 				"QPushButton{border-image: url(:/%1/btn_max.png);}"
// 				"QPushButton:hover{border-image: url(:/%1/btn_max_click.png);}"
// 				"QPushButton:pressed{border-image: url(:/%1/btn_max.png);}").arg(current_skin_name_);
//			ui.btn_max->setStyleSheet(btn_max_sty);
			is_not_max_ = true;
			last_pos_ = event->globalPos() - pos();

		}
	}

	event->ignore();
}

void QWebView::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if ((event->globalPos() - pos()).y() < ui.frame_title->size().height())
		{
			on_btn_max_clicked();
		}
	}
	event->ignore();
}

void QWebView::moveEvent(QMoveEvent *event)
{

	if (win_holder_ && !win_holder_->isHidden())
	{
		auto tp = ui.Container->geometry().topLeft();		auto move_tp = mapToParent(tp);
		qDebug() << "QWebView::moveEvent: " << tp  <<" -- "<< move_tp;
		win_holder_->MoveWidShow(move_tp);
	}
	else
	{
		auto tp = ui.Container->geometry().topLeft();		auto move_tp = mapToParent(tp);
		qDebug() << "QWebView::moveEvent Test: " << tp << " -- " << move_tp;
	}

	if (monitor_view_wid_ && !monitor_view_wid_->isHidden())
	{
		auto tp = ui.Container->geometry().topLeft();		auto move_tp = mapToParent(tp);
		qDebug() << "QWebView::moveEvent monitor_view_wid_ : " << tp << " -- " << move_tp << mapToGlobal(tp);
		monitor_view_wid_->move(move_tp);
	}
	__super::moveEvent(event);
}

void QWebView::resizeEvent(QResizeEvent *event)
{
	qDebug() << "QWebView::resizeEvent: " << event->size() << "geometry: " << geometry();
	if (win_holder_ && !win_holder_->isHidden() )
	{
		auto rc = Show2NowRect(win_holder_->GetItemRect());
		auto rc_tpl = rc.topLeft();
		auto tp = ui.Container->geometry().topLeft();		auto move_tp = mapToParent(tp);

		auto show_rc_topleft = move_tp + rc_tpl;

		qDebug() << rc ;
		
		auto new_rc = QRect(show_rc_topleft, rc.size());
		qDebug() << new_rc ;
		win_holder_->ReSetGeometry(new_rc);
	}

	if (monitor_view_wid_ && !monitor_view_wid_->isHidden())
	{
// 		auto rc = Show2NowRect(monitor_view_wid_->GetItemRect());
// 		auto rc_tpl = rc.topLeft();
// 		auto tp = ui.Container->geometry().topLeft();//		auto move_tp = mapToParent(tp);
// 
// 		auto show_rc_topleft = move_tp + rc_tpl;
// 
// 		qDebug() << rc;
// 
// 		auto new_rc = QRect(show_rc_topleft, rc.size());
// 		qDebug() << new_rc;

		//monitor_view_wid_->InitShow(new_rc);
	}

	__super::resizeEvent(event);
}

void QWebView::ScreenInfo()
{
	QDesktopWidget * desktop = QApplication::desktop();

	//获取程序所在屏幕是第几个屏幕
	int current_screen = desktop->screenNumber(this);
	//获取程序所在屏幕的尺寸
	QRect rect = desktop->screenGeometry(current_screen);
	//获取所有屏幕总大小
	QRect rectA = desktop->geometry();
	//获取所有屏幕的个数
	int screen_count = desktop->screenCount();
	//获取主屏幕是第几个
	int prim_screen = desktop->primaryScreen();

	QString temp = "total screen size = " + QString::number(screen_count);
	temp = temp + "\ncurrent screen num = " + QString::number(current_screen);
	temp = temp + "\ncurrent screen rect " + QString::number(rect.width()) + "*" + QString::number(rect.height());
	temp = temp + "\nwhole screen rect " + QString::number(rectA.width()) + "*" + QString::number(rectA.height());

	qDebug() << temp;
}

void QWebView::on_btn_min_clicked()
{
	if (this->isMinimized() != true)
	{
		showMinimized();
	}
	else
	{
		showNormal();
	}
}

void QWebView::on_btn_max_clicked()
{
	QPoint top_left;

	if (is_not_max_ == true)
	{
		QRect rect = this->rect();
		top_left = mapToGlobal(rect.topLeft());
		size_ = QSize(this->width(), this->height());

	}
	else
	{
		QPoint mid_temp(this->width() / 2, this->height() / 2);
		top_left = QPoint((this->width() / 2) - (size_.width() / 2), (this->height() / 2) - (size_.height() / 2));
	}
	rect_ = QRect(top_left, size_);


	if (is_not_max_ == false)
	{
		setGeometry(rect_);

// 		auto btn_max_sty = QString(
// 			"QPushButton{border-image: url(:/%1/btn_max.png);}"
// 			"QPushButton:hover{border-image: url(:/%1/btn_max_click.png);}"
// 			"QPushButton:pressed{border-image: url(:/%1/btn_max.png);}").arg(current_skin_name_);
// 		ui.btn_max->setStyleSheet(btn_max_sty);

		is_not_max_ = true;
	}
	else
	{

		QRect rc = QApplication::desktop()->availableGeometry();
		this->setGeometry(rc);

// 		auto btn_max_restore = QString(
// 			"QPushButton{border-image: url(:/%1/btn_restore.png);}"
// 			"QPushButton:hover{border-image: url(:/%1/btn_restore_click.png);}"
// 			"QPushButton:pressed{border-image: url(:/%1/btn_restore_click.png);}").arg(current_skin_name_);
// 
// 		ui.btn_max->setStyleSheet(btn_max_restore);
		is_not_max_ = false;

	}
}

void QWebView::onBtnCloseClicked()
{
	if (win_holder_)
	{
		win_holder_->close();
	}

	if (curCefView_)
	{
		//curCefView_->closeBrowser();
		curCefView_->close();
		curCefView_ = nullptr;
	}


	qDebug() << "Close Wid";
	//qApp->exit();
	qApp->quit();
}

void QWebView::onBtnGoClicked()
{
// 	RunMonitorViewClient();
// 	return;
	QString strUrl = ui.e_url->text();
	QUrl urlCheck(strUrl);
	if (urlCheck.isValid())
	{
		if (curCefView_)
		{
			curCefView_->navigateToUrl(strUrl);
		}

	}
	else
	{
		qDebug() << strUrl << "is unvalid!";
	}
}

void QWebView::onBtnGoBackClicked()
{

	if (curCefView_)
	{
		curCefView_->browserGoBack();
	}
}

void QWebView::onBtnGoForwardClicked()
{
	if (curCefView_)
	{
		curCefView_->browserGoForward();
	}
}

void QWebView::onBtnReloadOrStopClicked()
{

	if (curCefView_)
	{
		auto text = ui.btn_reload_or_stop->text();
		if (text == "Stop")
		{
			curCefView_->browserStopLoad();
		}
		else if (text == "ReLoad")
		{
			curCefView_->browserReload();
		}

	}
}

void QWebView::onBtnChangeColorClicked()
{
// 	if (curCefView_)
// 	{
// 		curCefView_->changeColor();
// 	}

	QJsonObject object;
	auto param = QString("qcef://open/{\"x\":750,\"y\":80,\"width\":400,\"height\":300,\"layout\":\"1x1\",\"open\":[{\"window\":0,\"path\":\"E:/Program Files (x86)/WXWork/WXWork.exe\",\"param\":\"\"}]}");
	qDebug() << u8"onStartExe: " << param;
	if (param.startsWith("qcef://open/"))
	{
		param.remove("qcef://open/");
		if (param.endsWith('/'))
		{
			param.remove(param.length() - 1, 1);
		}
		param = QUrl::fromPercentEncoding(param.toUtf8());

		object = QJsonDocument::fromJson(param.toUtf8()).object(); //json格式化输入参数
		if (object.isEmpty())
		{
			return;
		}
		auto x = object["x"].toInt(), y = object["y"].toInt();
		auto width = object["width"].toInt(), height = object["height"].toInt();
		qDebug() << "object before :" << object;
		auto tp = ui.Container->geometry().topLeft();		auto parent_tp = mapToParent(tp);		object["p_x"] = parent_tp.x();		object["p_y"] = parent_tp.y();		qDebug() << "Translation : " << object;		emit UrlReqParamSignal(object);



// 		if (!monitor_view_wid_)
// 		{
// 			monitor_view_wid_ = new MonitorDemo;
// 		}
// 		if (monitor_view_wid_)
// 		{
// 			auto pt = parent_tp + QPoint(x, y);
// 			auto real_rc = QRect(pt, QSize(width, height));
// 			monitor_view_wid_->InitShow(real_rc);
// 			//connect(monitor_view_wid_, &MonitorDemo::NotifyCamPragam, this, &QWebView::onNotifyCamPragam);
// 
// 			QJsonObject tmp;
// 			tmp["dev_type"] = "1";
// 			tmp["ip"] = "172.16.26.1";
// 			tmp["port"] = "8000";
// 			tmp["user"] = "admin";
// 
// 			tmp["pwd"] = "evecom@123";
// 			tmp["channel_id"] = "15";
// 			tmp["dev_name"] = "10F北侧消防通道05";
// 			tmp["dev_id"] = "ECD5E9C0DB2C41BD86D8F739302B73F7";
// 
// 
// 			monitor_view_wid_->SwitchMonitor(tmp);
// 
// 		}
	}

}

void QWebView::onLoadingStateChangedSignal(bool isLoading, bool canGoBack, bool canGoForward)
{
	if (isLoading) 
	{
		ui.btn_reload_or_stop->setText("Stop");
	}
	else 
	{
		ui.btn_reload_or_stop->setText("ReLoad");
	}

	ui.btn_go_back->setEnabled(canGoBack);
	ui.btn_go_forward->setEnabled(canGoForward);

	qDebug() << "[ onLoadingStateChangedSignal ]:" << isLoading <<u8" --- GOBACK---- " << canGoBack << u8" --- canGoForward---- " << canGoForward;
}

void QWebView::onCurrentChanged(int index)
{
	if (curCefView_)
	{
		auto url = curCefView_->GetUrl();
		ui.e_url->setText(url);
	}

}

void QWebView::onStartExe(const QVariantList& arguments)
{
	QJsonObject object;
	auto param = arguments.at(0).toString();
	qDebug() << u8"onStartExe: " << param;
	if (param.startsWith("qcef://open/"))
	{
		param.remove("qcef://open/");
		if (param.endsWith('/'))
		{
			param.remove(param.length() - 1, 1);
		}
		param = QUrl::fromPercentEncoding(param.toUtf8());

		object = QJsonDocument::fromJson(param.toUtf8()).object(); //json格式化输入参数
		if (object.isEmpty())
		{
			return;
		}
		qDebug() << "object before :" << object;
		auto tp = ui.Container->geometry().topLeft();		auto parent_tp = mapToParent(tp);		object["p_x"] = parent_tp.x();		object["p_y"] = parent_tp.y();		qDebug() << "Translation : " << object;		emit UrlReqParamSignal(object);

	}

}

void QWebView::onNotifyCamPragam(QJsonObject& new_obj)
{
	qDebug() <<"QWebView::onNotifyCamPragam>>>>>>>>>>>>>>>:"<< new_obj;
}

void QWebView::OnUrlReq(const QString& req)
{
	auto param = req;
	QJsonObject object;
	if (param.startsWith("qcef://move/"))
	{
		param.remove("qcef://move/");
		if (param.endsWith('/'))
		{
			param.remove(param.length() - 1, 1);
		}
		param = QUrl::fromPercentEncoding(param.toUtf8());

		object = QJsonDocument::fromJson(param.toUtf8()).object(); //json格式化输入参数
		if (object.isEmpty())
		{
			qDebug() << "fromJson err:" << param;
			return;
		}
		qDebug() << "object before :" << object;
		auto tp = ui.Container->geometry().topLeft();		auto parent_tp = mapToParent(tp);		object["p_x"] = parent_tp.x();		object["p_y"] = parent_tp.y();		qDebug() << "Translation : " << object;		emit UrlReqParamSignal(object);		

	}

	if (param.startsWith("qcef://open/"))
	{
		param.remove("qcef://open/");
		if (param.endsWith('/'))
		{
			param.remove(param.length() - 1, 1);
		}
		param = QUrl::fromPercentEncoding(param.toUtf8());

		object = QJsonDocument::fromJson(param.toUtf8()).object(); //json格式化输入参数
		if (object.isEmpty())
		{
			return;
		}
		qDebug() << "object before :" << object;
		auto tp = ui.Container->geometry().topLeft();		auto parent_tp = mapToParent(tp);		object["p_x"] = parent_tp.x();		object["p_y"] = parent_tp.y();		qDebug() << "Translation : " << object;		emit UrlReqParamSignal(object);

	}

}

void QWebView::ReqParamParse(const QJsonObject& req_param_obj)
{

}

QRect QWebView::Show2NowRect(QRect& real_rc)
{
	auto w = size_.width();
	auto h = size_.height();

	qDebug() <<"Real2ShowRect: "<< w << " - " << h;
	auto witdh_ratio = width() * 1.0 / w;//1920*1080
	auto height_ratio = height() * 1.0 / h;

	qDebug() << "witdh_height_ratio: " << witdh_ratio << " - " << height_ratio;

	QRect show_rc;
	show_rc.setLeft(qRound(witdh_ratio * real_rc.left()));
	show_rc.setTop(qRound(height_ratio * real_rc.top()));
	show_rc.setRight(qRound(witdh_ratio * (real_rc.right() + 1)) - 1);
	show_rc.setBottom(qRound(height_ratio * (real_rc.bottom() + 1)) - 1);


	qDebug() << real_rc << show_rc;

	return show_rc;
}

