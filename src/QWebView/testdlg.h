#pragma once

#include <QDialog>
#include "ui_testdlg.h"

class TestDlg : public QDialog
{
	Q_OBJECT

public:
	TestDlg(QWidget *parent = Q_NULLPTR);
	~TestDlg();
	bool Setup(QJsonObject param);
	void Clear();

public slots:
	void OnUrlReq(const QString& req);

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
	Ui::TestDlg ui;
};
