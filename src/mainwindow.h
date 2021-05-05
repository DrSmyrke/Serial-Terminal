#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QLabel>
#include <QTimer>
#include "global.h"
#include "consoleWidget.h"
#include "windows/modbuspacketcreator.h"

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
	QTimer* m_pTimer;
	bool m_searchModbusF;
	bool m_hexDataAtNewLineF;
	uint32_t m_timeout;
	ModbusPacketCreator* m_pModbuspacketCreator;

	void rescanPorts();
	bool checkPort(const QString &port);
	void sendData(const QByteArray &data);
	void updateModeB();
	void setConfigString();
};
#endif // MAINWINDOW_H
