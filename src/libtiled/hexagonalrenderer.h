/*
 * hexagonalrenderer.h
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

#ifndef HEXAGONALRENDERER_H
#define HEXAGONALRENDERER_H

#include "maprenderer.h"

namespace Tiled {

/**
 * The hexagonal map renderer.
 */
class TILEDSHARED_EXPORT HexagonalRenderer : public MapRenderer
{
public:
    HexagonalRenderer(const Map *map) : MapRenderer(map) {}

    virtual QSize mapSize() const = 0;

    QRect boundingRect(const QRect &rect) const;

    QRectF boundingRect(const MapObject *object) const;
    QPainterPath shape(const MapObject *object) const;

    void drawGrid(QPainter *painter, const QRectF &rect, QColor grid) const;

    void drawTileLayer(QPainter *painter, const TileLayer *layer,
                       const QRectF &exposed = QRectF()) const;

    void drawTileSelection(QPainter *painter,
                           const QRegion &region,
                           const QColor &color,
                           const QRectF &exposed) const;

    void drawMapObject(QPainter *painter,
                       const MapObject *object,
                       const QColor &color) const;

    void drawImageLayer(QPainter *painter,
                        const ImageLayer *layer,
                        const QRectF &exposed = QRectF()) const;


    virtual QPolygonF createHexagonForTile(const int x, const int y) const = 0;
    QRectF tileRectToBoundingPixelRect(QRectF rect) const;

    using MapRenderer::pixelToTileCoords;
    virtual QPointF pixelToTileCoords(qreal x, qreal y) const = 0;

    using MapRenderer::tileToPixelCoords;
    virtual QPointF tileToPixelCoords(qreal x, qreal y) const = 0;
};

class TILEDSHARED_EXPORT FlatHexRenderer : public HexagonalRenderer
{
public:
    FlatHexRenderer(const Map *map) : HexagonalRenderer(map) {}

    QSize mapSize() const;

    QPolygonF createHexagonForTile(const int x, const int y) const;

    using MapRenderer::pixelToTileCoords;
    QPointF pixelToTileCoords(qreal x, qreal y) const;

    using MapRenderer::tileToPixelCoords;
    QPointF tileToPixelCoords(qreal x, qreal y) const;
};

class TILEDSHARED_EXPORT IsoHexRenderer : public HexagonalRenderer
{
public:
    IsoHexRenderer(const Map *map) : HexagonalRenderer(map) {}

    QSize mapSize() const;

    QPolygonF createHexagonForTile(const int x, const int y) const;

    using MapRenderer::pixelToTileCoords;
    QPointF pixelToTileCoords(qreal x, qreal y) const;

    using MapRenderer::tileToPixelCoords;
    QPointF tileToPixelCoords(qreal x, qreal y) const;
};

} // namespace Tiled

#endif // HEXAGONALRENDERER_H
