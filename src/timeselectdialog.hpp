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
	inline qint64 getTime() const;

	QWidget& parent;
	QTimeEdit timeEditor;
	QPushButton cancelButton, validateButton;
private:
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

protected:
private:
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

protected:
private:
};
