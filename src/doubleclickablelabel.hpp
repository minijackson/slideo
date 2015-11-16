#pragma once

#include <QLabel>

/*! \brief A QLabel that you can double-click.
 *
 * Detailed description
 */
class DoubleClickableLabel : public QLabel {

	Q_OBJECT

public:
	/*! \brief DoubleClickableLabel constructor.
	 *
	 * \param text the text of the label.
	 */
	explicit DoubleClickableLabel(QString const& text);

signals:
	/*! \brief Signal emitted when the user double-clicked on the current label.
	 */
	void mouseDoubleClicked() const;
protected:
	/*! \brief Reimplementation to be aware of mouse double-click events.
	 */
	virtual void mouseDoubleClickEvent(QMouseEvent*) override;
private:
};
