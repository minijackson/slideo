#pragma once

#include <QDialog>
#include <QTimeEdit>
#include <QPushButton>

/*! \brief AddBreakpointRegularly description
 *
 * Detailed description
 */
class AddBreakpointRegularlyDialog : public QDialog {

	Q_OBJECT

public:
	/*! \brief AddBreakpointRegularly description
	 *
	 * Detailed description
	 */
	AddBreakpointRegularlyDialog(QWidget& parent);

public slots:

	/*! \brief Function called when the user cancels.
	 */
	virtual void cancel();

	/*! \brief Function called when the user validates.
	 */
	virtual void validate();

protected:
	/*! \brief Get a QTime as milliseconds.
	 *
	 * \param timeEditor the QTime to convert.
	 * \return the milliseconds.
	 */
	inline qint64 getMSecs(QTimeEdit const& timeEditor) const;

	/*! \brief Event filter to catch the press of Enter in the QTimeEdits
	 *
	 * \param obj the object from which the event originated.
	 * \param event the event
	 * \return true if Qt must stop processing the event.
	 */
	bool eventFilter(QObject* obj, QEvent* event) override;

	QWidget& parent;

	QTimeEdit fromTime, toTime, everyTime;
	QPushButton cancelButton, validateButton;
};
