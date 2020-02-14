#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include <QWidget>
#include <QPlainTextEdit>

class ConsoleWidget : public QPlainTextEdit
{
	Q_OBJECT
public:
	struct Mode
	{
		enum{
			terminal,
			console
		};
	};
	explicit ConsoleWidget(QWidget *parent = nullptr);
	void output(const QString &text);
	void setMode(const uint8_t mode);
	bool isConsole(){ return m_consoleMode; }
signals:
	void signal_onCommand(const QByteArray &cmd);
	void signal_modeChange(const QString &mode);
private:
	QString m_prompt;
	QStringList m_history;
	int m_historyPos;
	bool m_hexInputMode;
	bool m_consoleMode;

	void insertPrompt(bool insertNewBlock = true);
	void onEnter();
	void historyAdd(const QString &cmd);
	void historyBack();
	void historyForward();
	void scrollDown();
	void checkHexModeRazrjad();
	void backSpace();

	// QWidget interface
protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void contextMenuEvent(QContextMenuEvent *event);
};

#endif // CONSOLEWIDGET_H
