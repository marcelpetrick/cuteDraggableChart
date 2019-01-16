/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "chartview.h"
#include <QtCore/QtMath>
#include <QtCore/QDebug>

//#include <QXYSeries> // for the example
#include <QLineSeries> // for the example

QT_CHARTS_USE_NAMESPACE

ChartView::ChartView(QWidget *parent)
    : QChartView(new QChart(), parent),
      m_scatter(nullptr),
      m_scatter2(nullptr)
{
    // new idea: replace the current chart with a new one, which has replaced mouseEvent-handler
    //QChart* newChart = new QChart();
    //newChart->installEventFilter();
    //setChart(newChart);

    // end of new

    setRenderHint(QPainter::Antialiasing);

    chart()->setTitle("Click to interact with scatter points");

    m_scatter = new QScatterSeries();
    m_scatter->setName("scatter1");
    for (qreal x(0.5); x <= 4.0; x += 0.5) {
        for (qreal y(0.5); y <= 4.0; y += 0.5)
            *m_scatter << QPointF(x, y);
    }
    m_scatter2 = new QScatterSeries();
    m_scatter2->setName("scatter2");

    chart()->addSeries(m_scatter2);
    chart()->addSeries(m_scatter);

    connect(m_scatter, &QScatterSeries::clicked, this, &ChartView::handleClickedPoint);

    // -----------------------------

    qDebug() << "add QLineSeries here .."; // todom remove
    QLineSeries* line = new QLineSeries(this); // just a simple line from bottom to 4
    line->setName("draggable line");
    line->setPen(QPen(QColor(Qt::red)));
    line->append(1.2, 0.0);
    line->append(1.2, 4.0);

    chart()->addSeries(line);

    connect(line, &QLineSeries::pressed, this, &ChartView::slotHandlePressed);
    connect(line, &QLineSeries::released, this, &ChartView::slotHandleReleased);
    // todom add something for released

    //--------------
    chart()->createDefaultAxes();
    chart()->axisX()->setRange(0, 4.5);
    chart()->axisY()->setRange(0, 4.5);
}

ChartView::~ChartView()
{
}

void ChartView::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "-------------------------------------";
    qDebug() << "### ChartView::mouseMoveEvent! ###";
    //qDebug() << "localPos:" << event->localPos().x() << event->localPos().y();
    //qDebug() << "widgetPos:" << event->x() << event->y();

    // actually both yield the same result!

    //! @attention for further action, read:
    //! https://stackoverflow.com/questions/44067831/get-mouse-coordinates-in-qchartviews-axis-system/44078533#44078533

    auto const widgetPos = event->localPos();
    auto const scenePos = mapToScene(QPoint(static_cast<int>(widgetPos.x()), static_cast<int>(widgetPos.y())));
    auto const chartItemPos = chart()->mapFromScene(scenePos);
    auto const valueGivenSeries = chart()->mapToValue(chartItemPos);

    qDebug() << "widgetPos:" << widgetPos;
    qDebug() << "scenePos:" << scenePos;
    qDebug() << "chartItemPos:" << chartItemPos;
    qDebug() << "valueGivenSeries:" << valueGivenSeries;

    //## gives something like ##
    //widgetPos: QPointF(942,114)
    //scenePos: QPointF(942,114)
    //chartItemPos: QPointF(942,114)
    //valueGivenSeries: QPointF(1.34584,0.522989)

    // todo connect to this signal and save it for further usage: like when pressed/released-signals are emitted, the lineSeries is moved
    emit signalCursorChartPositionChanged(valueGivenSeries);

    QChartView::mouseMoveEvent(event);
}

void ChartView::handleClickedPoint(const QPointF &point)
{
    QPointF clickedPoint = point;
    // Find the closest point from series 1
    QPointF closest(INT_MAX, INT_MAX);
    qreal distance(INT_MAX);
    const auto points = m_scatter->points();
    for (const QPointF &currentPoint : points) {
        qreal currentDistance = qSqrt((currentPoint.x() - clickedPoint.x())
                                      * (currentPoint.x() - clickedPoint.x())
                                      + (currentPoint.y() - clickedPoint.y())
                                      * (currentPoint.y() - clickedPoint.y()));
        if (currentDistance < distance) {
            distance = currentDistance;
            closest = currentPoint;
        }
    }

    // Remove the closes point from series 1 and append it to series 2
    m_scatter->remove(closest);
    m_scatter2->append(closest);
}

void ChartView::slotHandlePressed(const QPointF &point)
{
    qDebug() << "slotHandlePressed:" << point;

}

void ChartView::slotHandleReleased(const QPointF &point)
{
    qDebug() << "slotHandleReleased:" << point;

}
