#include "console.h"
#include <QScrollBar>
#include <QTextBlock>

Console::Console(QWidget *parent)
	: QPlainTextEdit(parent)
	, m_prompt(">: ")
	, m_locked(false)
	, m_history()
	, m_historyPos( -1 )
	, m_hexInputMode(false)
	, m_consoleMode(true)
{
	QPalette p = this->palette();
	p.setColor(QPalette::Base, Qt::black);
	p.setColor(QPalette::Text, Qt::green);
	setPalette(p);

	int id = QFontDatabase::addApplicationFont("://Terminus.ttf");
	QString family = QFontDatabase::applicationFontFamilies(id).at(0);

	QFont font;
	font.setFamily( family );
	font.setPointSize( 12 );
	font.setWeight( QFont::Normal );

	this->setFont( font );

	insertPrompt( false );
}

void Console::output(const QString &text)
{
	this->textCursor().insertBlock();
	QTextCharFormat format;
	format.setForeground(Qt::gray);
	this->textCursor().setBlockCharFormat(format);
	this->textCursor().insertText( text );

	format.setForeground(Qt::green);
	this->textCursor().setBlockCharFormat(format);

	if( m_consoleMode ){
		m_locked = false;
		scrollDown();
	}else{
		insertPrompt();
	}
}

void Console::insertPrompt(bool insertNewBlock)
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

void Console::onEnter()
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

	if( m_consoleMode ){
		m_locked = true;
	}

	if( cmdBuf.size() > 0 ){
		emit signal_onCommand( cmdBuf );
	}
}

void Console::historyAdd(const QString &cmd)
{
	m_history.append(cmd);
	m_historyPos = m_history.length();
}

void Console::historyBack()
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

void Console::historyForward()
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

void Console::scrollDown()
{
	QScrollBar *vbar = this->verticalScrollBar();
	vbar->setValue( vbar->maximum() );
}

void Console::checkHexModeRazrjad()
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

void Console::mousePressEvent(QMouseEvent *event)
{
	this->setFocus();
}

void Console::keyPressEvent(QKeyEvent *event)
{
	if( m_locked ){
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
