#pragma once

#include <QDialog>

#include <QTimeEdit>
#include <QPushButton>

/*! \brief Abstract class for time selection dialogs.
 *
 * Inherited by JumpToTimeDialog and AddBreakpointDialog
 */
class TimeSelectDialog : public QDialog {

	Q_OBJECT

public:
	/*! \brief TimeSelectDialog constructor.
	 *
	 * \param parent the parent widget (the main window).
	 * \param windowTitle the dialog title.
	 * \param timeSelectorLabel the label for the time selection.
	 */
	TimeSelectDialog(QWidget& parent, QString windowTitle, QString timeSelectorLabel);

public slots:

	/*! \brief Function called when the user cancels.
	 */
	virtual void cancel();

	/*! \brief Function called when the user validates.
	 */
	virtual void validate() = 0;

protected:
	/*! \brief Get the time inputted by the user in milliseconds.
	 *
	 * \return the milliseconds.
	 */
	inline qint64 getTime() const;

	/*! \brief Event filter to catch the press of Enter in the QTimeEdit
	 *
	 * \param obj the object from which the event originated.
	 * \param event the event
	 * \return true if Qt must stop processing the event.
	 */
	bool eventFilter(QObject* obj, QEvent* event) override;


	QWidget& parent;
	QTimeEdit timeEditor;
	QPushButton cancelButton, validateButton;
};

/*! \brief Jump to time dialog
 */
class JumpToTimeDialog : public TimeSelectDialog {

	Q_OBJECT

public:
	/*! \brief JumpToTimeDialog constructor.
	 *
	 * \param parent the parent widget (the main window).
	 */
	JumpToTimeDialog(QWidget& parent);

public slots:

	/*! \brief Function called when the user validates.
	 *
	 * This will jump to the specified time.
	 */
	virtual void validate();
};

/*! \brief Add breakpoint dialog
 */
class AddBreakpointDialog : public TimeSelectDialog {

	Q_OBJECT

public:
	/*! \brief AddBreakpointDialog constructor.
	 *
	 * \param parent the parent widget (the main window).
	 */
	AddBreakpointDialog(QWidget& parent);

	/*! \brief Function called when the user validates.
	 *
	 * This will add the specified breakpoint.
	 */
	virtual void validate();
};
