#ifndef MODBUSPACKETCREATOR_H
#define MODBUSPACKETCREATOR_H

#include <QDialog>
#include "global.h"

namespace Ui {
class ModbusPacketCreator;
}

class ModbusPacketCreator : public QDialog
{
	Q_OBJECT

public:
	explicit ModbusPacketCreator(QWidget *parent = nullptr);
	~ModbusPacketCreator();
	QByteArray* getData(){ return &m_data; }
private:
	Ui::ModbusPacketCreator *ui;
	bool m_requestMode;
	bool m_requestAddDataSize;
	QByteArray m_data;

	void calculateCRC();
	void resetTableRows(const int rowCount);
};

#endif // MODBUSPACKETCREATOR_H
