#include <QApplication>
#include "mainwindow.hpp"

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);

	app.setApplicationName("Slideo");
	app.setApplicationDisplayName("Slideo");

	MainWindow mainWindow;
	mainWindow.show();

	return app.exec();
}
