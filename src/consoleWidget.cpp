#include "consoleWidget.h"
#include <QScrollBar>
#include <QTextBlock>

ConsoleWidget::ConsoleWidget(QWidget *parent) : QPlainTextEdit(parent)
{
	m_history		= QStringList();
	m_historyPos	= -1;
	m_consoleMode	= false;
	m_hexInputMode	= false;
	m_viewOnlyMode	= false;
	m_prompt		= ">: ";
	m_hexSymCounter	= 0;

	QPalette p = this->palette();
	p.setColor(QPalette::Base, Qt::black);
	p.setColor(QPalette::Text, Qt::green);
	setPalette(p);


	QFont font;

#ifdef __linux__

#elif _WIN32

#endif

	int id = QFontDatabase::addApplicationFont("://Play-Regular.ttf");
	QString family = QFontDatabase::applicationFontFamilies(id).at(0);
	font.setFamily( family );
	font.setPointSize( 11 );
	font.setWeight( QFont::Normal );
	font.setStyleHint( QFont::Monospace, QFont::PreferAntialias );

	this->setFont( font );

	insertPrompt( false );
}

void ConsoleWidget::output(const QString &text, bool newLine)
{
	QTextCharFormat format;
	format.setForeground(Qt::gray);

	if( !m_consoleMode ){
		this->textCursor().insertBlock();
		this->textCursor().setBlockCharFormat(format);
		this->textCursor().insertText( text );
	}else{
		this->textCursor().setBlockCharFormat(format);
		for( int32_t i = 0; i < text.length(); i++ ){
			if( text[i] == "\r" ){
				continue;
			}
			if( text[i] == "\b" ){
				backSpace();
				continue;
			}

			this->textCursor().insertText( text[i] );
			if( m_hexInputMode && m_hexSymCounter++ == 1 ){
				m_hexSymCounter = 0;
				this->textCursor().insertText( " " );
			}
		}
		if( newLine ) this->textCursor().insertBlock();
	}


	format.setForeground(Qt::green);
	this->textCursor().setBlockCharFormat(format);

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

void ConsoleWidget::setViewHexOnly(bool viewHexOnly)
{
	m_hexInputMode = viewHexOnly;
	this->setReadOnly( m_hexInputMode );
}

void ConsoleWidget::addCmdSym(const QString sym)
{
	checkInputHexMode();
	this->textCursor().insertText( sym );
	checkHexModeRazrjad();
	scrollDown();
}

void ConsoleWidget::onEnter()
{
	if( m_viewOnlyMode ){
		return;
	}
	if(this->textCursor().positionInBlock() == m_prompt.length()){
		insertPrompt();
		return;
	}

	QString cmd = this->textCursor().block().text().mid( m_prompt.length() );

	historyAdd(cmd);

	//QByteArray cmdBuf;

	if( m_hexInputMode ){
		if( m_hexData.size() > 0 ){
			emit signal_onCommand( m_hexData );
		}
	}else{
		if( cmd.length() > 0 ){
			emit signal_onCommand( cmd.toUtf8() );
		}
	}
//	if( cmd.left( 2 ) == "0x" ){
//		cmd.remove( 0, 2 );
//		cmd = cmd.replace(" ","");

//		for( uint16_t i = 0; i < cmd.length(); i += 2 ){
//			bool res = false;
//			auto byte = cmd.mid( i, 2 ).toUShort( &res, 16 );
//			if( !res ){
//				continue;
//			}
//			cmdBuf.append( byte );
//		}
//	}

//	if( cmdBuf.size() == 0 ){
//		cmdBuf.append( cmd );
//	}

//	if( cmdBuf.size() > 0 ){
//		emit signal_onCommand( cmdBuf );
//	}
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
	if(m_historyPos == m_history.length() - 1){
		cursor.insertText(m_prompt);
	}else{
		cursor.insertText(m_prompt + m_history.at(m_historyPos + 1));
	}
	setTextCursor(cursor);
	m_historyPos++;
}

void ConsoleWidget::scrollDown()
{
	QScrollBar *vbar = this->verticalScrollBar();
	vbar->setValue( vbar->maximum() );
}

void ConsoleWidget::checkInputHexMode()
{
	QString cmd = this->textCursor().block().text().mid( m_prompt.length() );
	if( cmd.left( 2 ) == "0x" ){
		m_hexInputMode = true;
	}else{
		m_hexInputMode = false;
	}
}

void ConsoleWidget::checkHexModeRazrjad()
{
	m_hexData.clear();

	if( !m_hexInputMode ) return;

	QString cmd = this->textCursor().block().text().mid( m_prompt.length() );

	if( cmd.left( 2 ) == "0x" ){
		cmd.remove( 0, 2 );
	}

	if( cmd.length() == 0 ){
		return;
	}

	cmd = cmd.replace(" ","");

	if( cmd.length() % 2 == 0 ){
		this->textCursor().insertText( " " );
	}

	for( uint16_t i = 0; i < cmd.length(); i += 2 ){
		bool res = false;
		auto byte = cmd.mid( i, 2 ).toUShort( &res, 16 );
		if( !res ){
			continue;
		}
		m_hexData.append( byte );
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
	if( m_viewOnlyMode && !m_hexInputMode ){
		return;
	}

	QPlainTextEdit::mousePressEvent(event);
}

void ConsoleWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	Q_UNUSED(event)
	if( m_viewOnlyMode ){
		return;
	}
}

void ConsoleWidget::keyPressEvent(QKeyEvent *event)
{
	if( m_hexInputMode && event->key() == Qt::Key_C	&& event->modifiers() == Qt::ControlModifier ){
		QPlainTextEdit::keyPressEvent(event);
		return;
	}
	if( m_viewOnlyMode ){
		return;
	}

	if( m_consoleMode ){
		QByteArray data;
		data.append( event->text().toUtf8() );
		emit signal_onCommand( data );
		return;
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

	checkInputHexMode();

	if( m_hexInputMode ){
		if(event->key() == Qt::Key_Space) return;
		if(event->key() == Qt::Key_Left) return;
		if(event->key() == Qt::Key_Right) return;

		if( ( event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9 ) || ( event->key() >= Qt::Key_A && event->key() <= Qt::Key_F ) ){
			QPlainTextEdit::keyPressEvent(event);
			checkHexModeRazrjad();
		}
	}else{
		if( event->key() >= Qt::Key_Space && event->key() <= Qt::Key_AsciiTilde ){
			QPlainTextEdit::keyPressEvent(event);
		}
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
