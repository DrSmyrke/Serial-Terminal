#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QLabel>
#include "global.h"
#include "consoleWidget.h"

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
private:
	Ui::MainWindow *ui;
	QSerialPort* m_pSPort;
	QLabel* m_pPortLabel;
	QLabel* m_pPortError;
	QLabel* m_pMode;
	ConsoleWidget* m_pConsole;

	void rescanPorts();
	bool checkPort(const QString &port);
	void sendData(const QByteArray &data);
	void updateModeB();
};
#endif // MAINWINDOW_H
