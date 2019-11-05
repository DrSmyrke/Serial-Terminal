#include "hexviewer.h"

#include <QPainter>
#include <QDebug>

HexViewer::HexViewer(QWidget *parent)
	: QWidget(parent)
	, m_bufferSize(65535)
{

}

void HexViewer::appendData(const QByteArray &data)
{
	m_buffer.append( data );

	if( m_buffer.size() > m_bufferSize ){
		uint16_t r = static_cast<uint16_t>( m_buffer.size() ) - m_bufferSize;
		m_buffer.remove( 0, r );
	}
}

void HexViewer::paintEvent(QPaintEvent *event)
{
	QPainter p(this);
	QPen pen;
	pen.setColor(Qt::lightGray);
	p.setPen(pen);
	p.drawLine(0,0,100,500);
	p.drawRect(0,0,100,100);
	p.setFont(QFont("://play-regular.ttf", 10, QFont::Bold));

	QPen penHText(QColor("#000000"));
	p.setPen(penHText);

	uint16_t x = 5;
	uint16_t y = 15;
	uint8_t ln = 0;

	for( uint16_t i = 0; i < m_buffer.size(); i++ ){
		auto sym = m_buffer.mid( i, 1 ).toHex();
		p.drawText( x, y, QString(sym) );

		x += 17;

		if( ln == 7 ) x += 10;

		if( ln >= 16 ){
			y += 15;
			x = 5;
			ln = 0;
		}

		ln++;
	}

	Q_UNUSED( event )
}
