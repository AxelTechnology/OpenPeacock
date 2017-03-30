
#include <QColor>
#include <QtGlobal>
#include <QtGui>
#include <QPolygon>
#include <math.h>
#include "qknob.h"
#include <QPainter>





void QKnob::drawCircleOutside(QPainter *painter)
{
    painter->save();
    //draw cricle big
    delta = abs((frameNo % 128) - diameterCircleBig / 2);
    alpha = 255 - (delta * delta) / 4 - diameterCircleBig;

//    QRadialGradient grad(0.5, 0.5, 0.5);
//    grad.setColorAt(0.0, QColor(120,120,120));
//    grad.setColorAt(0.7, QColor(80,80,80));
//    grad.setColorAt(1.0, QColor(55,55,55));
//    grad.setCoordinateMode( QGradient::ObjectBoundingMode );
//    QBrush gBrush(grad );


    //Removed gradient for 2D effect
//    double gradPerc1 = 0.7;
//    double gradPerc2 = 0.5;
//    QRadialGradient grad(0.5, 0.5, 0.5);
//    grad.setColorAt(0.0, color);
//    grad.setColorAt(0.8, QColor(color.red() * gradPerc1, color.green() * gradPerc1, color.blue() * gradPerc1));
//    grad.setColorAt(1.0, QColor(color.red() * gradPerc2, color.green() * gradPerc2, color.blue() * gradPerc2));
//    grad.setCoordinateMode( QGradient::ObjectBoundingMode );
    QBrush gBrush(m_background); //QBrush gBrush(grad);
    QPen gPen(m_foreground);
    if(editingValue)
        gPen.setColor(m_editingColor);

    if (alpha > 0) {
         painter->setPen(gPen);
         painter->setBrush(gBrush);
         painter->drawEllipse(QRectF(-diameterCircleBig / 2.0, -diameterCircleBig / 2.0 + knobOffset, diameterCircleBig, diameterCircleBig));
    }
    painter->restore();

}


void QKnob::drawArea(QPainter *painter)
{
    painter->save();
    QPainterPath path2;
    path2.arcMoveTo(-diameterCircleBig/2.0, -diameterCircleBig/2.0 + knobOffset, diameterCircleBig, diameterCircleBig,90);
    path2.arcTo(-diameterCircleBig/2.0, -diameterCircleBig/2.0 + knobOffset, diameterCircleBig, diameterCircleBig, 90, degree );
    path2.lineTo(0, knobOffset); // line up to the first arc
    path2.closeSubpath();

    QColor areaColor = m_foreground;
    if(editingValue)
        areaColor = m_editingColor;

    //Removed gradient for 2D effect
//    double gradPerc1 = 0.7;
//    double gradPerc2 = 0.5;
//    QRadialGradient grad;
//    if(degree < 0)
//        grad = QRadialGradient(0,1,1);
//    else
//        grad = QRadialGradient(1,1,1);

//    grad.setColorAt(0.0, areaColor);
//    grad.setColorAt(0.8, QColor(areaColor.red() * gradPerc1, areaColor.green() * gradPerc1, areaColor.blue() * gradPerc1));
//    grad.setColorAt(1.0, QColor(areaColor.red() * gradPerc2, areaColor.green() * gradPerc2, areaColor.blue() * gradPerc2));
//    grad.setCoordinateMode( QGradient::ObjectBoundingMode );
    QBrush gBrush(areaColor);    //QBrush gBrush(grad );

    painter->fillPath(path2, gBrush); // QBrush(m_foreground));

    painter->restore();
}


void QKnob::drawBackground(QPainter *painter)
{
    painter->save();
    QRect rect1(-50, -50, 100, 100);
    QColor bgColor = m_background;

    painter->fillRect(rect1, QBrush(bgColor));

    painter->restore();
}

void QKnob::setClipping(QPainter *painter)
{

    QRectF fullArea = QRectF(QRect(-60, -60, 120, 120));

    QPainterPath fullPath;
    fullPath.moveTo(fullArea.topLeft());
    fullPath.lineTo(fullArea.bottomLeft());
    fullPath.lineTo(fullArea.bottomRight());
    fullPath.lineTo(fullArea.topRight());
    fullPath.lineTo(fullArea.topLeft());

    QPainterPath innerCirclePath;
    innerCirclePath.addEllipse(QRect(-diameterCircleLittle / 2.0, -diameterCircleLittle / 2.0 + knobOffset, diameterCircleLittle, diameterCircleLittle));

    QRectF triangleArea = QRectF(-diameterCircleBig/2.0,knobOffset, 100,diameterCircleLittle);
    QPainterPath trianglePath;
    trianglePath.moveTo(triangleArea.left() + (triangleArea.width() / 2), triangleArea.top());
    trianglePath.lineTo(triangleArea.bottomLeft());
    trianglePath.lineTo(triangleArea.bottomRight());
    trianglePath.lineTo(triangleArea.left() + (triangleArea.width() / 2), triangleArea.top());

    QPainterPath clipPath = fullPath - trianglePath - innerCirclePath;

    painter->setClipPath(clipPath);

}

void QKnob::drawCircleInternal(QPainter *painter)
{
    painter->save();
    //draw circle internal
    delta = abs((frameNo % 128) - diameterCircleLittle / 2);
    alpha = 255 - (delta * delta) / 4 - diameterCircleLittle;

    QColor bgColor = m_background;
    if (alpha > 0) {
        painter->setPen(QPen(bgColor));
        painter->setBrush(QBrush(bgColor));
        painter->drawEllipse(QRectF(-diameterCircleLittle / 2.0, -diameterCircleLittle / 2.0 + knobOffset , diameterCircleLittle, diameterCircleLittle));
     }

    painter->restore();

}




void QKnob::drawTriangle(QPainter *painter)
{
    painter->save();
    //draw triangle
   QRectF rect = QRectF(-diameterCircleBig/2.0,knobOffset, 100,diameterCircleLittle);

   QPainterPath path;
   path.moveTo(rect.left() + (rect.width() / 2), rect.top());
   path.lineTo(rect.bottomLeft());
   path.lineTo(rect.bottomRight());
   path.lineTo(rect.left() + (rect.width() / 2), rect.top());

   QColor bgColor = m_background;

   painter->fillPath(path, QBrush(bgColor));
   //ainter->setClipPath(path, Qt::IntersectClip);

   painter->restore();


}

void QKnob::drawLabelBg(QPainter *painter)
{
    if(m_knobMode == QKnobMode_ONLYCONFIRMEDVALUE)
        return;

    //painter->save();
    //painter->resetTransform();

    painter->fillRect(-diameterCircleBig/2.0, 38, diameterCircleBig, 20, m_background);

    //painter->restore();
}


void QKnob::drawLabel(QPainter *painter, QFont *textFont, QFont *extraTextFont)
{
    if (textFont == 0)
        return;

    painter->save();
    //draw text

    //QFont timesFont("Times", 16);
    textFont->setStyleStrategy(QFont::ForceOutline);
    textFont->setBold(true);

//    int textX = 0;

//    if (m_labelSize != 0)
//        textX = /*-60 + (120 / 2) */ - m_labelSize->width() / 2;

//    //textPath.addText(-20,60, *textFont, text);
//    textPath.addText(textX, 60, *textFont, text);
//    painter->fillPath(textPath, QBrush(m_foreground));

    QColor fgColor = m_foreground;

    painter->setFont(*textFont);
    //painter->setBrush(QBrush(m_foreground));
    painter->setPen(QPen(fgColor));
    if(m_knobMode == QKnobMode_INSTANTVALUE)
        drawText(*painter, 0, 60, Qt::AlignBottom | Qt::AlignHCenter, text);
    else if(m_knobMode == QKnobMode_ONLYCONFIRMEDVALUE)
        drawText(*painter, 0, 2, Qt::AlignBottom | Qt::AlignHCenter, text);

    if(editingValue && (m_knobMode == QKnobMode_INSTANTVALUE))
    {
        painter->setPen(QPen(m_editingColor));
        drawText(*painter, 0, 2, Qt::AlignBottom | Qt::AlignHCenter, instantText);
    }

    painter->setPen(QPen(fgColor));
    painter->setFont(*extraTextFont);
    if(!m_rightLabelDraw.isEmpty())
        drawText(*painter, 40, 35, Qt::AlignBottom | Qt::AlignLeft, m_rightLabelDraw);
    if(!m_leftLabelDraw.isEmpty())
        drawText(*painter, -40, 35, Qt::AlignBottom | Qt::AlignRight, m_leftLabelDraw);

    painter->restore();
}


/*
 * When you pass a starting point for drawing text, you're effectively drawing text on an a large rectangle
 * that has the bottom-left corner at the given point. So all you need is to offer a suitable "infinite"
 * rectangle based on your starting point and the chosen alignment
 * http://stackoverflow.com/questions/24831484/how-to-align-qpainter-drawtext-around-a-point-not-a-rectangle
 */
void QKnob::drawText(QPainter & painter, qreal x, qreal y, Qt::Alignment flags,
              const QString & text, QRectF * boundingRect)
{
   const qreal size = 32767.0;
   QPointF corner(x, y - size);
   if (flags & Qt::AlignHCenter) corner.rx() -= size/2.0;
   else if (flags & Qt::AlignRight) corner.rx() -= size;
   if (flags & Qt::AlignVCenter) corner.ry() += size/2.0;
   else if (flags & Qt::AlignTop) corner.ry() += size;
   else flags |= Qt::AlignBottom;
   QRectF rect(corner, QSizeF(size, size));
   painter.drawText(rect, flags, text, boundingRect);
}




