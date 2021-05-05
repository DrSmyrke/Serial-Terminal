#include "modbuspacketcreator.h"
#include "ui_modbuspacketcreator.h"

ModbusPacketCreator::ModbusPacketCreator(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ModbusPacketCreator)
{
	ui->setupUi(this);

	m_requestMode			= true;
	m_requestAddDataSize	= false;


	ui->tableWidget->setColumnCount( 2 );
	ui->tableWidget->setHorizontalHeaderLabels( QStringList() << tr( "Value" ) << "HEX" );


	connect( ui->idBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value){
		resetTableRows( ui->countBox->value() );
		calculateCRC();
	} );
	connect( ui->codeBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value){
		resetTableRows( ui->countBox->value() );
		calculateCRC();
	} );
	connect( ui->countBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value){
		resetTableRows( value );
		calculateCRC();
	} );
	connect( ui->addrBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value){
		resetTableRows( ui->countBox->value() );
		calculateCRC();
	} );
	connect( ui->addButton, &QPushButton::clicked, this, &ModbusPacketCreator::accept );
	connect( ui->requestRB, &QRadioButton::toggled, this, [this](bool value){
		m_requestMode = value;
		if( m_requestMode ){
			ui->addrBox->setEnabled( true );
		}else{
			ui->addrBox->setEnabled( false );
		}
		resetTableRows( value );
		calculateCRC();
	} );

	connect( ui->tableWidget, &QTableWidget::cellChanged, this, [this](int row, int column){
		auto text = ui->tableWidget->item( row, column )->text();
		if( column == 0 && ui->tableWidget->item( row, 1 ) != nullptr ){
			uint16_t value = text.toUShort();
			auto hex = QString::number( value, 16 ).toUpper();
			if( hex.length() == 1 ) hex = "0" + hex;
			ui->tableWidget->item( row, 1 )->setText( hex );
		}
		if( column == 1 ){
			uint16_t value = text.toUShort( nullptr, 16 );
			ui->tableWidget->item( row, 0 )->setText( QString::number( value ) );
		}
		calculateCRC();
	} );

	connect( ui->requestAddDataSizeCB, &QCheckBox::clicked, this, [this](bool checked){
		m_requestAddDataSize = checked;
		calculateCRC();
	} );

	calculateCRC();
}

ModbusPacketCreator::~ModbusPacketCreator()
{
	delete ui;
}

void ModbusPacketCreator::calculateCRC()
{
	m_data.clear();

	uint8_t code		= (uint8_t)ui->codeBox->value();

	m_data.append( (uint8_t)ui->idBox->value() );
	m_data.append( code );

	if( m_requestMode ){
		uint16_t addr	= (uint16_t)ui->addrBox->value();
		uint8_t addrH, addrL;
		addrH			= addr >> 8;
		addrL			= addr;
		m_data.append( addrH );
		m_data.append( addrL );

		uint16_t count	= (uint16_t)ui->countBox->value();
		uint8_t countH, countL;
		countH			= count >> 8;
		countL			= count;
		m_data.append( countH );
		m_data.append( countL );

		if( m_requestAddDataSize ){
			if( code == 6 || code == 16 ) count *= 2;
			m_data.append( count );
		}

		for( uint16_t i = 0; i < ui->tableWidget->rowCount(); i++ ){
			if( code == 6 || code == 16){
				uint16_t value	= (uint16_t)ui->tableWidget->item( i, 0 )->text().toShort();
				uint8_t valueH, valueL;
				valueH			= value >> 8;
				valueL			= value;
				m_data.append( valueH );
				m_data.append( valueL );
			}else{
				uint8_t value	= (uint8_t)ui->tableWidget->item( i, 0 )->text().toShort();
				m_data.append( value );
			}
		}
	}else{
		uint8_t count	= (uint8_t)ui->countBox->value();

		if( code == 3 || code == 4 ) count *= 2;

		m_data.append( count );

		for( uint16_t i = 0; i < ui->tableWidget->rowCount(); i++ ){
			if( code == 3 || code == 4 ){
				uint16_t value	= (uint16_t)ui->tableWidget->item( i, 0 )->text().toShort();
				uint8_t valueH, valueL;
				valueH			= value >> 8;
				valueL			= value;
				m_data.append( valueH );
				m_data.append( valueL );
			}else{
				uint8_t value	= (uint8_t)ui->tableWidget->item( i, 0 )->text().toShort();
				m_data.append( value );
			}
		}
	}

	uint16_t crc = app::calculateCRC( m_data.data(), m_data.size() );
	uint8_t crcH, crcL;
	crcH			= crc >> 8;
	crcL			= crc;
	m_data.append( crcH );
	m_data.append( crcL );

	ui->crcLE->setText( QString::number( crc, 16 ) );
}

void ModbusPacketCreator::resetTableRows(const int rowCount)
{
	auto prewLen		= ui->tableWidget->rowCount();

	uint16_t code		= (uint16_t)ui->codeBox->value();

	if( m_requestMode ){
		if( code >= 1 && code <= 4 ){
			ui->tableWidget->setRowCount( 0 );
			return;
		}
	}else{
		if( code > 4 ){
			ui->tableWidget->setRowCount( 0 );
			return;
		}
	}

	ui->tableWidget->setRowCount( rowCount );

	for( uint16_t i = prewLen; i < rowCount; i++ ){
		QTableWidgetItem *valItem = new QTableWidgetItem();
		valItem->setText( "0" );

		QTableWidgetItem *hexItem = new QTableWidgetItem();
		hexItem->setText( "0x00" );

		ui->tableWidget->setItem( i, 0, valItem );
		ui->tableWidget->setItem( i, 1, hexItem );
	}
}
