#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	setWindowTitle( "Serial Terminal v" + app::conf.version );
	setWindowIcon( QIcon( "://index.ico" ) );
	setMinimumSize( 640, 480 );
}

MainWindow::~MainWindow()
{
	delete ui;
}

