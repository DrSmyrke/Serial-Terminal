#ifndef HEXVIEWER_H
#define HEXVIEWER_H

#include <QWidget>
#include <QLabel>

class HexViewer : public QWidget
{
	Q_OBJECT
public:
	explicit HexViewer(QWidget *parent = nullptr);
	void setBufferSize(uint16_t size){ m_bufferSize = size; }
	void appendData(const QByteArray &data);
private:
	uint16_t m_bufferSize;
	QByteArray m_buffer;
	QSize m_drawArea;
	QLabel* m_pHexL;
	QLabel* m_pAsciiL;

protected:
	void paintEvent(QPaintEvent *event);
};

#endif // HEXVIEWER_H
