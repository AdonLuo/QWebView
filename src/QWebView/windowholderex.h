#pragma once

#include <QWidget>
#include "ui_windowholderex.h"

class WindowContainer;
class WindowContainerEx;

class WindowHolderEx : public QWidget
{
	Q_OBJECT

public:
	WindowHolderEx(QWidget *parent = Q_NULLPTR);
	~WindowHolderEx();
	bool Setup(QJsonObject param);
	void Clear();

	QRect GetItemRect();
	void MoveWidShow(QPoint& pt);
	void ReSetGeometry(QRect& rc);
	
public slots:
	void OnUrlReqParam(const QJsonObject& req_param_obj);

private:
	void PreClose();
	void HandleMessage(QJsonObject message);
	void HandleLayout(QString name);
	void HandleClose(QJsonArray closes);
	void HandleOpen(QJsonArray opens);
	QString GetClass(QString name);
	virtual void resizeEvent(QResizeEvent *event) override;
	virtual void timerEvent(QTimerEvent *event) override;

private:
	Ui::WindowHolderEx ui;
	QThreadPool thread_;
	int topTimer_;
	QRect real_rc_;//������������ʾ�����λ��
	QJsonObject layout_;
	QList<WindowContainer*> containers_;

	QList<WindowContainerEx*> containersex_;
};
