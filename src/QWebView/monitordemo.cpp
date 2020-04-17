#include "stdafx.h"
#include "monitordemo.h"

MonitorDemo::MonitorDemo(QWidget *parent)
	: QMainWindow(parent)
	, thread_(nullptr)
{
	ui.setupUi(this);

	//connect(ui.vs_ctrl, SIGNAL(DoNotifyJSErrorMsg(QString)), this, SLOT(PTZCtrlCallBack(QString)));
	
	setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
	//setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
	setContextMenuPolicy(Qt::NoContextMenu);

// 	QFile file1("d:/new_vsctrl.html");
// 	file1.open (QIODevice::WriteOnly | QIODevice::Text );
// 	QTextStream out(&file1);
// 	out << ui.vs_ctrl->generateDocumentation();
// 	file1.close();



// 	thread_ = new QThread;
// 	if (thread_)
// 	{
// 		connect(thread_, &QThread::destroyed, thread_, &QThread::deleteLater);
// 		moveToThread(thread_);
// 		thread_->start();
// 	}


//	QObject::connect(ui.vs_ctrl, SIGNAL(DoNotifyJSErrorMsg(QString)), this, SLOT(DoNotifyJSErrorMsg(QString)));

// 	int dev_ = 1;
// 	QString ip_("172.16.210.213");
// 	int port_ = 8000;
// 	QString user_("admin");
// 	QString pwd_("1qaz@WSX");
// 	uint channel_ = 1;
// 	QString channel_name_("");
// 
// 
// 	int monitor_id_ = ui.vs_ctrl->dynamicCall("StartPreviewEx(int, QString, int, QString, QString, uint, QString)", dev_, ip_, port_, user_, pwd_, channel_, channel_name_).toInt();
// 
// 
// 	int win_num = 0;
// 	//{ "type":"3dzoom","left": 0, "top" : 0, "right" : 50, "bottom" : 50, "width" : 100, "height" : 100 }
// 	QJsonObject params;
// 	params["type"] = "getgis";
// 
// 	qDebug() << params;
// 	QString params_str = QString(QJsonDocument(params).toJson());
// 	auto result = ui.vs_ctrl->dynamicCall("PTZTrack(int, QString)", win_num, params_str).toInt();
	
}

void MonitorDemo::InitShow(QRect& rc)
{


	show_rc_ = rc;
	setGeometry(show_rc_);
	show();

	int ishiden = 1;
	int is_enable_ptz_ctrl = 1;
	int result = ui.vs_ctrl->dynamicCall("HideControlBar(int)", ishiden).toInt();
	result = ui.vs_ctrl->dynamicCall("EnablePTZControl(int)", is_enable_ptz_ctrl).toInt();

	//(int backClr, int selBorderClr, int borderClr, int topBarClr)
	ui.vs_ctrl->dynamicCall("SetWindowColor(int, int, int, int)", 0xececec, 0xececec, 0xececec, 0xececec);
	auto dis = QObject::connect(ui.vs_ctrl, SIGNAL(DoNotifyJSErrorMsg(QString)), this, SLOT(DoNotifyJSErrorMsg(QString)));

}

void MonitorDemo::SwitchMonitor(MonitorInfo_T& current_cam_conf)
{
	if (current_cam_conf.ip_.isEmpty() | current_cam_conf.user_name_.isEmpty())
	{
		qDebug() << "MonitorDemo::InitShow monitor conf  is empty ";
	}
	dev_ = current_cam_conf.dev_type_.toInt();
	ip_ = current_cam_conf.ip_;
	port_ = current_cam_conf.port_.toInt();
	user_ = current_cam_conf.user_name_;
	pwd_ = current_cam_conf.pwd_;
	channel_ = current_cam_conf.channel_id_.toUInt();
	channel_name_ = current_cam_conf.name_;

	MonitorCtrlInit();

}

void MonitorDemo::SwitchMonitor(QJsonObject& current_cam_conf)
{
	auto dev = current_cam_conf["dev_type"].toString();
	auto ip = current_cam_conf["ip"].toString();
	auto port = current_cam_conf["port"].toString();
	auto user = current_cam_conf["user"].toString();
	auto pwd = current_cam_conf["pwd"].toString();
	auto channel = current_cam_conf["channel_id"].toString();
	auto channel_name = current_cam_conf["dev_name"].toString();

	if (ip.isEmpty() | user.isEmpty())
	{
		qDebug() << "MonitorDemo::InitShow monitor conf  is empty ";
	}
	dev_ = dev.toInt();
	ip_ = ip;
	port_ = port.toInt();
	user_ = user;
	pwd_ = pwd;
	channel_ = channel.toUInt();
	channel_name_ = channel_name;

	MonitorCtrlInit();
}

void MonitorDemo::UnInit()
{
	int result1 = ui.vs_ctrl->dynamicCall("StopAllProview()").toInt();
	if (thread_)
	{
		if (!thread_->isFinished())
		{
			thread_->quit();
			thread_->wait();
		}
		delete thread_;
		thread_ = nullptr;
	}


}

void MonitorDemo::MonitorCtrl(QString& ctrl, QString& cmd)
{
	int result = -1;
	int monitor_id = monitor_id_;
	int monitor_speed = 2;
	int monitor_cmd;
	
// TILT_UP 21 云台上仰
// TILT_DOWN 22 云台下俯
// PAN_LEFT 23 云台左转
// PAN_RIGHT 24 云台右转
// UP_LEFT 25 云台上仰和左转
// UP_RIGHT 26 云台上仰和右转
// DOWN_LEFT 27 云台下俯和左转
// DOWN_RIGHT 28 云台下俯和右转
// 	ZOOM_IN = 11			//焦距变大(倍率变大) 
// 	ZOOM_OUT = 12		//焦距变小(倍率变小) 


	if (cmd == u8"UP")
	{
		//qDebug() << "ctrl - up";
		monitor_cmd = 21;
	}
	else if (cmd == u8"DOWN")
	{
		//qDebug() << "ctrl - down";
		monitor_cmd = 22;
	}
	else if (cmd == u8"LEFT")
	{
		//qDebug() << "ctrl - left";
		monitor_cmd = 23;
	}
	else if (cmd == u8"RIGHT")
	{
		//qDebug() << "ctrl - right";
		monitor_cmd = 24;
	}
	else if (cmd == u8"UP_LEFT")
	{
		monitor_cmd = UP_LEFT;
	}
	else if (cmd == u8"UP_RIGHT")
	{
		monitor_cmd = UP_RIGHT;
	}
	else if (cmd == u8"DOWN_LEFT")
	{
		monitor_cmd = DOWN_LEFT;
	}
	else if (cmd == u8"DOWN_RIGHT")
	{
		monitor_cmd = DOWN_RIGHT;
	}
	else if (cmd == u8"NEAR")
	{
		//qDebug() << "ctrl - near";
		monitor_cmd = 11;
	}
	else if (cmd == u8"FAR")
	{
		//qDebug() << "ctrl - far";
		monitor_cmd = 12;
	}

	if (ctrl == u8"start")
	{
		result = ui.vs_ctrl->dynamicCall("StartPTZControl(int, int, int)", monitor_id, monitor_cmd, monitor_speed).toInt();

	}
	else if (ctrl == u8"stop")
	{
		result = ui.vs_ctrl->dynamicCall("StopPTZControl(int, int, int)", monitor_id, monitor_cmd, monitor_speed).toInt();
	}

	//qDebug() << "MonitorDemo::MonitorCtrl: " << result <<"ctrl - cmd"<< ctrl <<" - " << cmd;
}



void MonitorDemo::OnPtzPreSet(QString& cmd, int index, QString& name)
{
	int win_num = 0;
	//{"type":"preset","order":"set/clear/goto","index":1}
	QJsonObject params;
	params["type"] = "preset";
	params["order"] = cmd;
	params["index"] = index;
	params["name"] = name;
	
	//qDebug() << params;

	QString params_str = QString(QJsonDocument(params).toJson());

	auto result = ui.vs_ctrl->dynamicCall("PTZTrack(int, QString)", win_num, params_str).toInt();
}

void MonitorDemo::OnMonitorCtrl(QString& ctrl, QString& cmd)
{
	//qDebug() << "MonitorDemo::OnMonitorCtrl: " << ctrl <<" -- > "<< cmd ;
	MonitorCtrl(ctrl, cmd);
}

void MonitorDemo::OnPtzZoomInRect(QRect& rc, QSize& sz)
{
	//qDebug() << "MonitorDemo::OnPtzZoomInRect hide" << rc << sz;

	int win_num = 0;
	//{ "type":"3dzoom","left": 0, "top" : 0, "right" : 50, "bottom" : 50, "width" : 100, "height" : 100 }
	QJsonObject params;
	params["type"] = "3dzoom";
	params["left"] = rc.topLeft().x();
	params["top"] = rc.topLeft().y();
	params["right"] = rc.bottomRight().x();
	params["bottom"] = rc.bottomRight().y();
	params["width"] = sz.width();
	params["height"] = sz.height();

	//qDebug() << params;

	QString params_str = QString(QJsonDocument(params).toJson());

	auto result = ui.vs_ctrl->dynamicCall("PTZTrack(int, QString)", win_num, params_str).toInt();

}

void MonitorDemo::PTZCtrlCallBack(QString pragma)
{
	qDebug() << "MonitorDemo::PTZCtrlCallBack :" << pragma;
}

void MonitorDemo::DoNotifyJSErrorMsg(QString bstrMsg)
{

	QJsonDocument doc = QJsonDocument::fromJson(bstrMsg.toLocal8Bit().data());
	if (!doc.isNull())
	{
		QJsonObject obj = doc.object();

		auto type = obj["type"].toString();
		if (type == "uploadgis" || type == "getgis")
		{
			emit NotifyCamPragam(obj);
		}
		else if (type == "preset")
		{
			qDebug() << obj;
		}


	}
	else
	{
		qDebug() << "MonitorDemo::PTZCtrlCallBack to json err >>>>>> " << bstrMsg;
	}
	

}

void MonitorDemo::MonitorCtrlInit()
{

	monitor_id_ = ui.vs_ctrl->dynamicCall("StartPreviewEx(int, QString, int, QString, QString, uint, QString)", dev_, ip_, port_, user_, pwd_, channel_, channel_name_).toInt();
	int win_num = 0;
	QJsonObject params;
	params["type"] = "getgis";
	//qDebug() << params;
	QString params_str = QString(QJsonDocument(params).toJson());
	auto result = ui.vs_ctrl->dynamicCall("PTZTrack(int, QString)", win_num, params_str).toInt();


}




void MonitorDemo::closeEvent(QCloseEvent *event)
{
	int result1 = ui.vs_ctrl->dynamicCall("StopAllProview()").toInt();
}

void MonitorDemo::mouseDoubleClickEvent(QMouseEvent *event)
{
	//qDebug() << "------------MonitorDemo::mouseDoubleClickEvent------------" << event->pos();
	if (Qt::RightButton == event->button())
	{
		auto pos = event->pos();
	}
}

void MonitorDemo::mousePressEvent(QMouseEvent *event)
{
	//this->lower();

}
