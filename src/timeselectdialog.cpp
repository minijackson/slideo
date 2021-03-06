#include "timeselectdialog.hpp"

#include "mainwindow.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

#include <QTime>

#include <QEvent>
#include <QKeyEvent>

#include <QApplication>

TimeSelectDialog::TimeSelectDialog(QWidget& parent, QString windowTitle, QString timeSelectorLabel)
      : QDialog(&parent)
      , parent(parent)
      , timeEditor()
      , cancelButton("Cancel")
      , validateButton("OK") {
	MainWindow& mwParent = dynamic_cast<MainWindow&>(parent);

	QTime currentPosition = QTime(0, 0, 0, 0).addMSecs(mwParent.getVideoPlayer().getPosition());

	QVBoxLayout* mainLayout = new QVBoxLayout;

	QFormLayout* formLayout = new QFormLayout;
	timeEditor.setTime(currentPosition);
	timeEditor.setDisplayFormat("HH:mm:ss.zzz");
	formLayout->addRow(timeSelectorLabel, &timeEditor);

	QWidget* formWidget = new QWidget;
	formWidget->setLayout(formLayout);

	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(&cancelButton);
	buttonsLayout->addWidget(&validateButton);

	validateButton.setDefault(true);

	QWidget* buttonsWidget = new QWidget;
	buttonsWidget->setLayout(buttonsLayout);

	mainLayout->addWidget(formWidget);
	mainLayout->addWidget(buttonsWidget);

	setLayout(mainLayout);
	setWindowTitle(windowTitle);

	connect(&cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
	connect(&validateButton, SIGNAL(clicked()), this, SLOT(validate()));

	qApp->installEventFilter(this);
}

void TimeSelectDialog::cancel() {
	done(1);
}

qint64 TimeSelectDialog::getTime() const {
	return QTime(0, 0, 0, 0).msecsTo(timeEditor.dateTime().time());
}

bool TimeSelectDialog::eventFilter(QObject* obj, QEvent* event) {
	if(obj == &timeEditor && event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
		if(keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
			validate();
			return true;
		}
	}
	return false;
}

JumpToTimeDialog::JumpToTimeDialog(QWidget& parent)
      : TimeSelectDialog(parent, "Jump to time", "Time to jump to:") {}

void JumpToTimeDialog::validate() {
	MainWindow& mwParent = dynamic_cast<MainWindow&>(parent);
	mwParent.setVideoPlayerPosition(QTime(0, 0, 0, 0).msecsTo(timeEditor.dateTime().time()));
	done(0);
}

AddBreakpointDialog::AddBreakpointDialog(QWidget& parent)
      : TimeSelectDialog(parent, "Add breakpoint", "Breakpoint position:") {}

void AddBreakpointDialog::validate() {
	MainWindow& mwParent = dynamic_cast<MainWindow&>(parent);
	mwParent.addProjectBreakpoint(getTime());
	done(0);
}
