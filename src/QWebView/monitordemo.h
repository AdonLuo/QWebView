#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_monitordemo.h"
#include "common.h"


class MonitorDemo : public QMainWindow
{
	Q_OBJECT

public:
	MonitorDemo(QWidget *parent = Q_NULLPTR);
	void InitShow(QRect& rc);
	void SwitchMonitor(MonitorInfo_T& current_cam_conf);
	void SwitchMonitor(QJsonObject& current_cam_conf);
	void UnInit();
	void MonitorCtrl(QString& ctrl,QString& cmd);
	void OnPtzPreSet(QString& cmd, int index, QString& name);
signals:
	void NotifyCamPragam(QJsonObject& new_obj);
public slots:

	void OnMonitorCtrl(QString& ctrl, QString& cmd);
	void OnPtzZoomInRect(QRect& rc, QSize& sz);

	void PTZCtrlCallBack(QString pragma);
	void DoNotifyJSErrorMsg(QString bstrMsg);
protected:
	void MonitorCtrlInit();

	//切换时 是否uninit?回调是否要解绑

	virtual void closeEvent(QCloseEvent *event) override;


	virtual void mouseDoubleClickEvent(QMouseEvent *event) override;


	virtual void mousePressEvent(QMouseEvent *event) override;

private:
	Ui::MonitorDemoClass ui;

	QRect show_rc_;
	int monitor_id_;
	QThread* thread_;

	int dev_;
	QString ip_;
	int port_;
	QString user_;
	QString pwd_;
	uint channel_;
	QString channel_name_;

};
