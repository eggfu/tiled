/*
 * hexagonalrenderer.cpp
 * Copyright 2011, Marcus Stjärnås <>
 *
 * This file is part of libtiled.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "hexagonalrenderer.h"

#include "map.h"
#include "mapobject.h"
#include "tile.h"
#include "tilelayer.h"
#include "tileset.h"
#include "imagelayer.h"

using namespace Tiled;

QRect HexagonalRenderer::boundingRect(const QRect &rect) const
{
   return tileRectToBoundingPixelRect(rect).toRect();
}

QRectF HexagonalRenderer::boundingRect(const MapObject *object) const
{
    // TODO: Object support
    Q_ASSERT(0);
    return tileRectToBoundingPixelRect(object->bounds());
}

QPainterPath HexagonalRenderer::shape(const MapObject *object) const
{
    // TODO: Object support
    Q_ASSERT(0);
    return QPainterPath();
}

void HexagonalRenderer::drawGrid(QPainter *painter, const QRectF &rect,
                                 QColor gridColor) const
{
    const int tileWidth = map()->tileWidth();
    const int tileHeight = map()->tileHeight();

    if (tileWidth <= 0 || tileHeight <= 0 || rect.isNull())
        return;

    int startX = 0;
    int startY = 0;
    int endX = map()->width();
    int endY = map()->height();

    gridColor.setAlpha(128);

    QPen gridPen(gridColor);
    //gridPen.setDashPattern(QVector<qreal>() << 2 << 2);
    painter->setPen(gridPen);

    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {
            painter->drawConvexPolygon(createHexagonForTile(x, y));
        }
    }
}

void HexagonalRenderer::drawTileLayer(QPainter *painter,
                                       const TileLayer *layer,
                                       const QRectF &exposed) const
{
    int startX = layer->x();
    int startY = layer->y();
    int endX = qMin(startX + layer->width(), map()->width());
    int endY = qMin(startY + layer->height(), map()->height());

    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {

            // Compensate for the layer position
            const Cell &cell = layer->cellAt(x - layer->x(), y - layer->y());
            if (cell.isEmpty())
                continue;

            const QPixmap &img = cell.tile->image();
            const int flipX = cell.flippedHorizontally ? -1 : 1;
            const int flipY = cell.flippedVertically ? -1 : 1;
            const int offsetX = cell.flippedHorizontally ? img.width() : 0;
            const int offsetY = cell.flippedVertically ? 0 : img.height();

            //qDebug("TODO: flipped %d-%d & exposed %lfx%lf", flipX, flipY, exposed.x(), exposed.y());

            QPolygonF pos = createHexagonForTile(x, y);
            if (pos.boundingRect().intersects(exposed))
                painter->drawPixmap(pos.boundingRect().toRect(), img);
        }
    }
}

void HexagonalRenderer::drawTileSelection(QPainter *painter,
                                           const QRegion &region,
                                           const QColor &color,
                                           const QRectF &exposed) const
{
    foreach (const QRect &r, region.rects()) {

        painter->setBrush(QBrush(color, Qt::SolidPattern));
        painter->setPen(Qt::NoPen);
        for (int i=0; i<r.height(); ++i) {
            for (int j=0; j<r.width(); ++j) {
                QPolygonF hexagon = createHexagonForTile(r.x()+j, r.y()+i);
                if (hexagon.boundingRect().intersects(exposed))
                    painter->drawConvexPolygon(hexagon);
            }
        }
        painter->setPen(Qt::SolidLine);
        painter->setBrush(QBrush(color, Qt::NoBrush));
    }
}

void HexagonalRenderer::drawMapObject(QPainter *painter,
                                       const MapObject *object,
                                       const QColor &color) const
{
    // TODO: Object support
    Q_ASSERT(0);
}

void HexagonalRenderer::drawImageLayer(QPainter *painter,
                                       const ImageLayer *imageLayer,
                                       const QRectF &exposed) const
{
    Q_UNUSED(exposed)

    const QPointF layerPos = tileToPixelCoords(imageLayer->x(), imageLayer->y());
    const QPixmap &img = imageLayer->image();

    painter->drawPixmap(layerPos, img);
}

QRectF HexagonalRenderer::tileRectToBoundingPixelRect(QRectF rect) const
{
    QRectF united;
    for (int i=0; i<rect.height(); ++i) {
        for (int j=0; j<rect.width(); ++j) {
            QPolygonF hexagon = createHexagonForTile(rect.x()+j, rect.y()+i);
            united = united.unite(hexagon.boundingRect());
        }
    }
    return united;
}


QSize FlatHexRenderer::mapSize() const
{
    return QSize(map()->width() * map()->tileWidth() * .75 + (map()->tileWidth() * .25),
                 map()->height() * map()->tileHeight() + (map()->tileHeight() * .5));
}

QPointF FlatHexRenderer::pixelToTileCoords(qreal x, qreal y) const
{
    const int xoffset = qRound(x / (map()->tileWidth() * .75));
    const int yoffset = y / map()->tileHeight() - ((xoffset % 2 == 0) ? 0 : .5);

    for (int i=-1; i<2; ++i) {
        for (int j=-1; j<2; ++j) {
            QPolygonF poly = createHexagonForTile(xoffset+j, yoffset+i);
            if (poly.containsPoint(QPointF(x, y), Qt::OddEvenFill)) {
                    return QPointF(xoffset+j, yoffset+i);
            }
        }
    }

    //Q_ASSERT(0); // Should never arrive here...
    return QPointF(0.,0.);
}

QPointF FlatHexRenderer::tileToPixelCoords(qreal x, qreal y) const
{
    const int tileWidth = map()->tileWidth();
    const int tileHeight = map()->tileHeight();

    int px = x * 0.75 * tileWidth;
    int py = y * tileHeight;
    if (int(x) & 1)
        py += 0.5 * tileHeight;

    return QPointF(px, py);
}

QPolygonF FlatHexRenderer::createHexagonForTile(const int x, const int y) const
{
    const int width = map()->tileWidth();
    const int height = map()->tileHeight();

    const qreal xoffset = x * width * .75;
    const qreal yoffset = y * height + ((x % 2 == 0) ? 0 : height * .5);

    QPolygonF hexagon;
    hexagon << QPointF(xoffset + (width * .25),    yoffset);
    hexagon << QPointF(xoffset + (width * .75),    yoffset);
    hexagon << QPointF(xoffset + width,            yoffset + (height * .5));
    hexagon << QPointF(xoffset + (width * .75),    yoffset + height);
    hexagon << QPointF(xoffset + (width * .25),    yoffset + height);
    hexagon << QPointF(xoffset,                    yoffset + (height * .5));

    return hexagon;
}



QSize IsoHexRenderer::mapSize() const
{
    return QSize(map()->width() * map()->tileWidth() + (map()->tileWidth() * .5) + 2,
                 map()->height() * map()->tileHeight() * .75 + (map()->tileHeight() * .25) + 2);
}

QPointF IsoHexRenderer::pixelToTileCoords(qreal x, qreal y) const
{
    const int yoffset = qRound(y / (map()->tileHeight() * .75));
    const int xoffset = x / map()->tileWidth() - ((yoffset % 2 == 0) ? 0 : .5);

    for (int i=-1; i<2; ++i) {
        for (int j=-1; j<2; ++j) {
            QPolygonF poly = createHexagonForTile(xoffset+j, yoffset+i);
            if (poly.containsPoint(QPointF(x, y), Qt::OddEvenFill)) {
                    return QPointF(xoffset+j, yoffset+i);
            }
        }
    }

    //Q_ASSERT(0); // Should never arrive here...
    return QPointF(0.,0.);
}

QPointF IsoHexRenderer::tileToPixelCoords(qreal x, qreal y) const
{
    const int tileWidth = map()->tileWidth();
    const int tileHeight = map()->tileHeight();

    int px = x * tileWidth;
    int py = y * .75 * tileHeight;
    if (int(y) & 1)
        px += 0.5 * tileWidth;

    return QPointF(px, py);
}

QPolygonF IsoHexRenderer::createHexagonForTile(const int x, const int y) const
{
    const int width = map()->tileWidth();
    const int height = map()->tileHeight();

    const qreal xoffset = x * width + ((y % 2 == 0) ? 0 : width * .5);
    const qreal yoffset = y * height * .75;

    QPolygonF hexagon;
    hexagon << QPointF(xoffset + (width * .5),     yoffset);
    hexagon << QPointF(xoffset + width,            yoffset + (height * .25));
    hexagon << QPointF(xoffset + width,            yoffset + (height * .75));
    hexagon << QPointF(xoffset + (width * .5),     yoffset + height);
    hexagon << QPointF(xoffset,                    yoffset + (height * .75));
    hexagon << QPointF(xoffset,                    yoffset + (height * .25));

    return hexagon;
}
