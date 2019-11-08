#include "hexviewer.h"

#include <QPainter>
#include <QDebug>
#include <QPaintEvent>
#include <QFontDatabase>

HexViewer::HexViewer(QWidget *parent)
	: QWidget(parent)
	, m_bufferSize(65535)
	, m_drawArea( 0, 0 )
{
	//setAttribute( Qt::WA_StaticContents );
	//setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
	//qDebug()<<this->sizePolicy();
}

void HexViewer::appendData(const QByteArray &data)
{
	m_buffer.append( data );

	if( m_buffer.size() > m_bufferSize ){
		uint16_t r = static_cast<uint16_t>( m_buffer.size() ) - m_bufferSize;
		m_buffer.remove( 0, r );
	}

	this->update();
}

QSize HexViewer::sizeHint() const
{
	return m_drawArea;
}

void HexViewer::paintEvent(QPaintEvent *event)
{
	int id = QFontDatabase::addApplicationFont("://Terminus.ttf");
	QString family = QFontDatabase::applicationFontFamilies(id).at(0);

	QRect area = event->rect();
	//qDebug()<<"HexViewer::paintEvent"<<area<<event->region();
	uint16_t asciiX = static_cast<uint16_t>( area.width() ) - 200;
	QPainter p(this);
	QPen pen;
	pen.setColor(Qt::lightGray);
	p.setPen(pen);
	p.drawLine( asciiX, 0, asciiX, area.height() );
	//p.drawRect(0,0,100,100);
	p.setFont(QFont(family, 12, QFont::Normal));

	QPen penHText(QColor("#000000"));
	p.setPen(penHText);

	uint16_t x = 5;
	uint16_t ax = 5;
	uint16_t y = 15;
	uint8_t ln = 1;

	for( uint16_t i = 0; i < m_buffer.size(); i++ ){
		auto sym = m_buffer.mid( i, 1 );
		p.drawText( x, y, QString(sym.toHex()) );
		p.drawText( asciiX + ax, y, QString(sym) );

		x += 20;
		ax += 10;

		if( ln == 8 ){
			x += 10;
			ax += 5;
		}

		if( ln >= 16 ){
			y += 15;
			x = 5;
			ax = 5;
			ln = 0;
		}

		ln++;
	}

	if( y < area.height() ) y = static_cast<uint16_t>( area.height() );
	m_drawArea.setWidth( area.width() );
	m_drawArea.setHeight( y );
}
