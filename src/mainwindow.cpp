#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	m_pPortLabel = new QLabel(this);
	m_pPortError = new QLabel(this);
	m_pMode = new QLabel(this);
	m_pConsole = new ConsoleWidget( this );
	m_pHexConsole = new ConsoleWidget( this );

	m_pSPort = new QSerialPort(this);
		m_pSPort->setBaudRate( 115200 );
		m_pSPort->setDataBits( QSerialPort::DataBits::Data8 );
		m_pSPort->setParity( QSerialPort::Parity::NoParity );
		m_pSPort->setStopBits( QSerialPort::StopBits::OneStop );
		m_pSPort->setFlowControl( QSerialPort::FlowControl::NoFlowControl );

	setWindowTitle( "Serial Terminal v" + app::conf.version );
	setWindowIcon( QIcon( "://index.ico" ) );
	setMinimumSize( 640, 480 );

	rescanPorts();

	m_pSPort->setPortName( ui->portBox->currentText() );
	m_pPortLabel->setText( ui->portBox->currentText() );
	ui->connectB->setText( tr( "OPEN" ) );
	ui->statusbar->addWidget( m_pPortLabel );
	ui->statusbar->addWidget( m_pPortError );
	ui->statusbar->addWidget( m_pMode );
	ui->vBox->addWidget( m_pConsole );
	ui->vBox->addWidget( m_pHexConsole );
	ui->clearB->setIcon( this->style()->standardIcon(QStyle::SP_LineEditClearButton) );

	connect( ui->connectB, &QPushButton::clicked, this, [this](){
		if( !m_pSPort->isOpen() ){
			if( m_pSPort->open( QIODevice::ReadWrite ) ){
				ui->connectB->setText( tr( "CLOSE" ) );
				ui->speedBox->setEnabled( false );
				ui->portBox->setEnabled( false );
				setConfigString();
				m_pConsole->setFocus();
			}else{
				m_pPortError->setText( m_pSPort->errorString() );
			}
		}else{
			m_pSPort->close();
			if( !m_pSPort->isOpen() ){
				ui->connectB->setText( tr( "OPEN" ) );
				ui->speedBox->setEnabled( true );
				ui->portBox->setEnabled( true );
				m_pPortError->clear();
			}
		}
	} );
	connect( ui->clearB, &QPushButton::clicked, this, [this](){
		m_pConsole->clear();
		m_pConsole->insertPrompt( false );
		m_pConsole->setFocus();
	} );
	connect( ui->hexB, &QPushButton::toggled, this, [this](bool checked){
		m_pConsole->setFocus();
		if( checked ){
			m_pHexConsole->show();
		}else{
			m_pHexConsole->hide();
		}
	} );

	connect( m_pSPort, &QSerialPort::readyRead, this, &MainWindow::slot_readyRead );
	connect( ui->portBox, &QComboBox::currentTextChanged, this, [this](const QString &portName){
		m_pSPort->setPortName( portName );
		m_pPortLabel->setText( portName );
	} );

	connect( ui->speedBox, &QComboBox::currentTextChanged, this, [this](const QString &speed){
		bool res = false;
		auto value = speed.toUInt( &res, 10 );
		if( res ){
			m_pSPort->setBaudRate( value );
		}
	} );

	connect( m_pConsole, &ConsoleWidget::signal_onCommand, this, [this](const QByteArray &cmd){
		if( cmd == "help" ){
			m_pConsole->output( "====== HELP MENU ========\nmode input hex\nmode input ascii" );
			return;
		}
		sendData( cmd );
	} );

	connect( m_pConsole, &ConsoleWidget::signal_modeChange, this, [this](const QString &text){
		m_pMode->setText( QString("Mode: %1").arg( text ) );
	} );

	connect( ui->modeB, &QPushButton::clicked, this, [this](){
		if( m_pConsole->isConsole() ){
			m_pConsole->setMode( ConsoleWidget::Mode::terminal );
		}else{
			m_pConsole->setMode( ConsoleWidget::Mode::console );
		}
		m_pConsole->output( "\r\n====== MODE CHANGE ========\r\n" );
		updateModeB();
		m_pConsole->setFocus();
	} );

	//configure
	m_pConsole->setFocus();
	m_pConsole->setMode( ConsoleWidget::Mode::terminal );
	m_pHexConsole->hide();
	m_pHexConsole->setMode( ConsoleWidget::Mode::console );
	updateModeB();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::slot_readyRead()
{
	QByteArray buff;
	while( m_pSPort->bytesAvailable() ) buff.append( m_pSPort->readAll() );

	m_pConsole->output( buff );
}

void MainWindow::rescanPorts()
{
	ui->portBox->clear();
	for( auto portInfo:QSerialPortInfo::availablePorts() ){
		ui->portBox->addItem( portInfo.portName() );
	}

	/*
#ifdef __linux__

#elif _WIN32
	for( uint8_t i = 1; i < 250; i++ ){
		auto str = QString("COM%1").arg( i );sendData
		if( checkPort( str ) ) ui->portBox->addItem( str );
	}
#endif
	*/
}

bool MainWindow::checkPort(const QString &port)
{
	bool res = false;

	if( m_pSPort->isOpen() ) m_pSPort->close();

	m_pSPort->setPortName( port );

	if( m_pSPort->open( QIODevice::ReadOnly ) ){
		m_pSPort->close();
		res = true;
	}

	return res;
}

void MainWindow::sendData(const QByteArray &data)
{
	if( !m_pSPort->isOpen() ){
		m_pConsole->output( "Port is NOT open" );
		return;
	}
	if( data.size() == 0 ) return;
	m_pSPort->write( data );
}

void MainWindow::updateModeB()
{
	if( m_pConsole->isConsole() ){
		ui->modeB->setText( tr("Console") );
	}else{
		ui->modeB->setText( tr("Terminal") );
	}
}

void MainWindow::setConfigString()
{
	QString str = QString("%1-%2-").arg( m_pSPort->baudRate() ).arg( m_pSPort->dataBits() );

	switch ( m_pSPort->parity() ) {
		case QSerialPort::Parity::NoParity:		str += "N";	break;
		case QSerialPort::Parity::EvenParity:	str += "E";	break;
		case QSerialPort::Parity::OddParity:	str += "O";	break;
		case QSerialPort::Parity::SpaceParity:	str += "S";	break;
		case QSerialPort::Parity::MarkParity:	str += "M";	break;
		default:	str += "U";	break;
	}

	str += "-";
	str += QString::number( m_pSPort->stopBits() );

	m_pPortError->setText( str );
}
