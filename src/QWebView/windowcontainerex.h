#pragma once
#include "monitordemo.h"


class WindowContainerEx : public QFrame
{
	Q_OBJECT

public:
	WindowContainerEx(QThreadPool* thread, QWidget *parent = Q_NULLPTR);
	~WindowContainerEx();

	void Open(QJsonObject open);
	void Close();

signals:
	void Opened(quint64 process, quint64 hwnd);
	void TipsTextChanged(QString text);

private:
	void SetupUI();
	virtual void resizeEvent(QResizeEvent *event) override;

private:
	QThreadPool* thread_;
	QLabel* tips_;

	MonitorDemo* monitor_view_wid_;
};
