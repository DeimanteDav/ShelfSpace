#include "dragscrollarea.h"

DragScrollArea::DragScrollArea(QWidget *parent) : QScrollArea(parent) {
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setWidgetResizable(true);
    setMouseTracking(true);
}

void DragScrollArea::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        dragging = true;
        lastPos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
    QScrollArea::mousePressEvent(event);
}

void DragScrollArea::mouseMoveEvent(QMouseEvent *event) {
    if (dragging) {
        QPoint delta = event->pos() - lastPos;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        lastPos = event->pos();
    }
    QScrollArea::mouseMoveEvent(event);
}

void DragScrollArea::mouseReleaseEvent(QMouseEvent *event) {
    dragging = false;
    setCursor(Qt::ArrowCursor);
    QScrollArea::mouseReleaseEvent(event);
}
