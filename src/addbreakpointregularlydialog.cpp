#include "addbreakpointregularlydialog.hpp"

#include "mainwindow.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

#include <QEvent>
#include <QKeyEvent>

#include <QApplication>

AddBreakpointRegularlyDialog::AddBreakpointRegularlyDialog(QWidget& parent)
      : QDialog(&parent)
      , parent(parent)
      , fromTime()
      , toTime()
      , everyTime()
      , cancelButton("Cancel")
      , validateButton("OK") {
	MainWindow& mwParent = dynamic_cast<MainWindow&>(parent);

	QVBoxLayout* mainLayout = new QVBoxLayout;

	QTime currentPosition = QTime(0, 0, 0, 0).addMSecs(mwParent.getVideoPlayer().getPosition());
	QTime endPosition = QTime(0, 0, 0, 0).addMSecs(mwParent.getVideoPlayer().getDuration());

	QFormLayout* formLayout = new QFormLayout;
	fromTime.setTime(currentPosition);
	fromTime.setDisplayFormat("HH:mm:ss.zzz");
	formLayout->addRow("From: ", &fromTime);

	toTime.setTime(endPosition);
	toTime.setDisplayFormat("HH:mm:ss.zzz");
	formLayout->addRow("To: ", &toTime);

	everyTime.setTime(QTime(0, 0, 1, 0));
	everyTime.setDisplayFormat("HH:mm:ss.zzz");
	formLayout->addRow("Every: ", &everyTime);

	QWidget* formWidget = new QWidget;
	formWidget->setLayout(formLayout);

	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(&cancelButton);
	buttonsLayout->addWidget(&validateButton);

	QWidget* buttonsWidget = new QWidget;
	buttonsWidget->setLayout(buttonsLayout);

	mainLayout->addWidget(formWidget);
	mainLayout->addWidget(buttonsWidget);

	setLayout(mainLayout);
	setWindowTitle("Add breakpoints regularly");

	connect(&cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
	connect(&validateButton, SIGNAL(clicked()), this, SLOT(validate()));

	qApp->installEventFilter(this);
}

void AddBreakpointRegularlyDialog::cancel() {
	done(1);
}

void AddBreakpointRegularlyDialog::validate() {
	MainWindow& mwParent = dynamic_cast<MainWindow&>(parent);

	qint64 fromTimeMSecs = getMSecs(fromTime), toTimeMSecs = getMSecs(toTime),
	       everyTimeMSecs = getMSecs(everyTime);

	std::vector<qint64> breakpoints{};

	for(qint64 breakpointPos = fromTimeMSecs; breakpointPos <= toTimeMSecs;
	    breakpointPos += everyTimeMSecs) {
		breakpoints.push_back(breakpointPos);
	}

	mwParent.addProjectBreakpoints(breakpoints);

	done(0);
}

inline qint64 AddBreakpointRegularlyDialog::getMSecs(QTimeEdit const& timeEditor) const {
	return QTime(0, 0, 0, 0).msecsTo(timeEditor.dateTime().time());
}

bool AddBreakpointRegularlyDialog::eventFilter(QObject* obj, QEvent* event) {
	if((obj == &fromTime || obj == &toTime || obj == &everyTime) &&
	   event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
		if(keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
			validate();
			return true;
		}
	}
	return false;
}
