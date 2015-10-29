#include "doubleclickablelabel.hpp"

DoubleClickableLabel::DoubleClickableLabel(QString const& text)
      : QLabel(text) {}

void DoubleClickableLabel::mouseDoubleClickEvent(QMouseEvent*) {
	emit mouseDoubleClicked();
}
