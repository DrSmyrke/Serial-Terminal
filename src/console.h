#ifndef CONSOLE_H
#define CONSOLE_H

#include <QWidget>
#include <QPlainTextEdit>

class Console : public QPlainTextEdit
{
	Q_OBJECT
public:
	explicit Console(QWidget *parent = nullptr);
	void output(const QString &text);
signals:
	void signal_onCommand(const QByteArray &cmd);
private:
	QString m_prompt;
	QStringList m_history;
	bool m_locked;
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

	// QWidget interface
protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event) {};
	void keyPressEvent(QKeyEvent *event);
	void contextMenuEvent(QContextMenuEvent *event) {};
};

#endif // CONSOLE_H
