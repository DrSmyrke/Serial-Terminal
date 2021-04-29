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

struct ModbusRTUpkt{
	uint8_t id;
	uint8_t cmd;
	uint8_t addr[2];
	uint8_t count[2];
	uint8_t data[64];
};

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
	ConsoleWidget* m_pHexConsole;
	bool m_searchModbusF;

	void rescanPorts();
	bool checkPort(const QString &port);
	void sendData(const QByteArray &data);
	void updateModeB();
	void setConfigString();
	unsigned short calculateCRC(const char *data, const unsigned char length);
};
#endif // MAINWINDOW_H
