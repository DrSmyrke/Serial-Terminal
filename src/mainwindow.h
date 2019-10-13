#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QLabel>
#include "global.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();
private slots:
	void slot_readyRead();
	void slot_sendMess();
private:
	Ui::MainWindow *ui;
	QSerialPort* m_pSPort;
	QLabel* m_pPortLabel;
	QLabel* m_pPortError;

	void rescanPorts();
	bool checkPort(const QString &port);
	void sendData(const QByteArray &data);
};
#endif // MAINWINDOW_H
