#pragma once

class WindowContainer : public QFrame
{
    Q_OBJECT

public:
    WindowContainer(QThreadPool* thread, QWidget *parent = Q_NULLPTR);
    ~WindowContainer();
    void Open(QJsonObject open);
    void Close();

signals:
    void Opened(quint64 process, quint64 hwnd);
    void TipsTextChanged(QString text);

private:
    void SetupUI();
    HWND GetProcessHwnd(HANDLE process, DWORD processId, std::string className, int timeout);
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    QThreadPool* thread_;

    HANDLE process_;
    HWND hwnd_;

    QLabel* tips_;
};
