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
	inline qint64 getMSecs(QTimeEdit const& timeEditor) const;

	QWidget& parent;

	QTimeEdit fromTime, toTime, everyTime;
	QPushButton cancelButton, validateButton;
};
