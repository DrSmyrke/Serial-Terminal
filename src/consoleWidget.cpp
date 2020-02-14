#include "consoleWidget.h"
#include <QScrollBar>
#include <QTextBlock>
//TODO: remove qdebug
#include <QDebug>

ConsoleWidget::ConsoleWidget(QWidget *parent) : QPlainTextEdit(parent)
{
	m_history		= QStringList();
	m_historyPos	= -1;
	m_consoleMode	= false;
	m_hexInputMode	= false;
	m_prompt		= ">: ";

	QPalette p = this->palette();
	p.setColor(QPalette::Base, Qt::black);
	p.setColor(QPalette::Text, Qt::green);
	setPalette(p);

	int id = QFontDatabase::addApplicationFont("://Terminus.ttf");
	QString family = QFontDatabase::applicationFontFamilies(id).at(0);

	QFont font;
	font.setFamily( family );
	//font.setPointSize( 12 );
	font.setWeight( QFont::Normal );

	this->setFont( font );

	insertPrompt( false );
}

void ConsoleWidget::output(const QString &text)
{
	QTextCharFormat format;
	format.setForeground(Qt::gray);

	if( !m_consoleMode ){
		this->textCursor().insertBlock();
		this->textCursor().setBlockCharFormat(format);
		this->textCursor().insertText( text );
	}else{
		this->textCursor().setBlockCharFormat(format);
		for( uint32_t i = 0; i < text.length(); i++ ){
			if( text[i] == "\r" ){
				continue;
			}
			if( text[i] == "\b" ){
				backSpace();
				continue;
			}

			this->textCursor().insertText( text[i] );
		}
	}


	format.setForeground(Qt::green);
	this->textCursor().setBlockCharFormat(format);

	qDebug()<<"<:"<<text;

	if( m_consoleMode ){
		scrollDown();
	}else{
		insertPrompt();
	}
}

void ConsoleWidget::insertPrompt(bool insertNewBlock)
{
	if(insertNewBlock){
		this->textCursor().insertBlock();
	}

	QTextCharFormat format;
	format.setForeground(Qt::green);
	this->textCursor().setBlockCharFormat(format);
	this->textCursor().insertText( m_prompt );

	scrollDown();
}

void ConsoleWidget::onEnter()
{
	//TODO: Сделать отправку Enter
	if(this->textCursor().positionInBlock() == m_prompt.length()){
		insertPrompt();
		return;
	}

	QString cmd = this->textCursor().block().text().mid( m_prompt.length() );

	historyAdd(cmd);

	QByteArray cmdBuf;

	if( cmd.left( 2 ) == "0x" ){
		cmd.remove( 0, 2 );
		cmd = cmd.replace(" ","");

		for( uint16_t i = 0; i < cmd.length(); i += 2 ){
			bool res = false;
			auto byte = cmd.left( 2 ).toUShort( &res, 16 );
			if( !res ){
				continue;
			}
			cmdBuf.append( byte );
		}
	}

	if( cmdBuf.size() == 0 ){
		cmdBuf.append( cmd );
	}

	if( cmdBuf.size() > 0 ){
		emit signal_onCommand( cmdBuf );
	}
}

void ConsoleWidget::historyAdd(const QString &cmd)
{
	m_history.append(cmd);
	m_historyPos = m_history.length();
}

void ConsoleWidget::historyBack()
{
	if( m_historyPos <= 0 ){
		return;
	}
	QTextCursor cursor = textCursor();
	cursor.movePosition(QTextCursor::StartOfBlock);
	cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	cursor.insertText( m_prompt + m_history.at(m_historyPos-1));
	setTextCursor(cursor);
	m_historyPos--;
}

void ConsoleWidget::historyForward()
{
	if(m_historyPos == m_history.length()){
		return;
	}
	QTextCursor cursor = textCursor();
	cursor.movePosition(QTextCursor::StartOfBlock);
	cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
	cursor.removeSelectedText();
	if(m_historyPos == m_history.length() - 1)
		cursor.insertText(m_prompt);
	else
		cursor.insertText(m_prompt + m_history.at(m_historyPos + 1));
	setTextCursor(cursor);
	m_historyPos++;
}

void ConsoleWidget::scrollDown()
{
	QScrollBar *vbar = this->verticalScrollBar();
	vbar->setValue( vbar->maximum() );
}

void ConsoleWidget::checkHexModeRazrjad()
{
	if( !m_hexInputMode ) return;

	QString cmd = this->textCursor().block().text().mid( m_prompt.length() );

	if( cmd.left( 2 ) == "0x" ){
		cmd.remove( 0, 2 );
	}

	if( cmd.replace(" ","").length() % 2 == 0 ){
		this->textCursor().insertText( " " );
	}
}

void ConsoleWidget::backSpace()
{
	this->textCursor().deletePreviousChar();
}

void ConsoleWidget::setMode(const uint8_t mode)
{
	switch (mode) {
		case ConsoleWidget::Mode::terminal:
			m_consoleMode = false;
			emit signal_modeChange( tr("terminal") );
		break;
		case ConsoleWidget::Mode::console:
			m_consoleMode = true;
			emit signal_modeChange( tr("console") );
		break;
	}
}

void ConsoleWidget::mousePressEvent(QMouseEvent *event)
{
	Q_UNUSED(event)
	this->setFocus();
}

void ConsoleWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	Q_UNUSED(event)
}

void ConsoleWidget::keyPressEvent(QKeyEvent *event)
{
	if( m_consoleMode ){
		//QPlainTextEdit::keyPressEvent(event);
		QByteArray data;
		data.append( event->text().toUtf8() );
		qDebug()<<event<<data.toHex()<<data;
		emit signal_onCommand( data );
		return;
	}

	//m_hexInputModeinsertPrompt();

	if( m_hexInputMode ){
		if(event->key() == Qt::Key_Space) return;
		if(event->key() == Qt::Key_Left) return;
		if(event->key() == Qt::Key_Right) return;
	}

	if(event->key() >= 0x20 && event->key() <= 0x7e
			&& (event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::ShiftModifier)){
		QString cmd = this->textCursor().block().text().mid( m_prompt.length() );
		if( cmd.left( 2 ) == "0x" ){
			m_hexInputMode = true;
		}else{
			m_hexInputMode = false;
		}
		QPlainTextEdit::keyPressEvent(event);

		checkHexModeRazrjad();
	}

	if(event->key() == Qt::Key_Backspace
		   && event->modifiers() == Qt::NoModifier
		   && textCursor().positionInBlock() > m_prompt.length()){
		QString cmd = this->textCursor().block().text().mid( m_prompt.length() );
		if( cmd.right( 1 ) == " " && cmd.left( 2 ) == "0x" ){
			QPlainTextEdit::keyPressEvent(event);
		}
		QPlainTextEdit::keyPressEvent(event);
	}

	if(event->key() == Qt::Key_Left
		   && event->modifiers() == Qt::NoModifier
		   && textCursor().positionInBlock() > m_prompt.length()){
		QPlainTextEdit::keyPressEvent(event);
	}

	if(event->key() == Qt::Key_Right && event->modifiers() == Qt::NoModifier){
		QPlainTextEdit::keyPressEvent(event);
	}

	if(event->key() == Qt::Key_Delete && event->modifiers() == Qt::NoModifier){
		QPlainTextEdit::keyPressEvent(event);
	}

	if(event->key() == Qt::Key_Return && event->modifiers() == Qt::NoModifier){
		onEnter();
	}
	if(event->key() == Qt::Key_Enter && event->modifiers() == Qt::KeypadModifier){
		onEnter();
	}

	if(event->key() == Qt::Key_Up && event->modifiers() == Qt::NoModifier){
		historyBack();
	}
	if(event->key() == Qt::Key_Down && event->modifiers() == Qt::NoModifier){
		historyForward();
	}
}

void ConsoleWidget::contextMenuEvent(QContextMenuEvent *event)
{
	Q_UNUSED(event)
}
