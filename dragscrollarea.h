#ifndef DRAGSCROLLAREA_H
#define DRAGSCROLLAREA_H

#include <QScrollArea>
#include <QScrollBar>
#include <QMouseEvent>
#include <QPoint>

class DragScrollArea : public QScrollArea {
    Q_OBJECT

public:
    explicit DragScrollArea(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool dragging = false;
    QPoint lastPos;
};

#endif // DRAGSCROLLAREA_H
