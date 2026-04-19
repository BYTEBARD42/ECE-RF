/*
 *  Constellation diagram plot for IQ signal visualization.
 *  Plots In-phase (I) vs Quadrature (Q) samples as a scatter diagram.
 *
 *  This file is part of spectr.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 */

#include "constellationplot.h"
#include <cmath>
#include <algorithm>
#include <vector>

ConstellationPlot::ConstellationPlot(std::shared_ptr<AbstractSampleSource> source)
    : Plot(source)
{
    setHeight(300); // Taller than default to give a nice square-ish plot area
}

void ConstellationPlot::paintBack(QPainter &painter, QRect &rect, range_t<size_t> sampleRange)
{
    // Dark background for the constellation area
    painter.save();
    painter.fillRect(rect, QColor(10, 10, 18));
    painter.restore();
}

void ConstellationPlot::paintMid(QPainter &painter, QRect &rect, range_t<size_t> sampleRange)
{
    if (sampleRange.length() == 0) return;

    // Cast to complex sample source
    auto src = dynamic_cast<SampleSource<std::complex<float>>*>(sampleSource.get());
    if (!src) return;

    // Determine how many samples to grab (cap for performance)
    size_t count = std::min(static_cast<size_t>(sampleRange.length()), maxSamples);
    size_t start = sampleRange.minimum;

    // If we have more samples than maxSamples, take evenly spaced ones
    // by adjusting the start to center the selection
    if (sampleRange.length() > maxSamples) {
        start = sampleRange.minimum + (sampleRange.length() - maxSamples) / 2;
    }

    auto samples = src->getSamples(start, count);
    if (samples == nullptr) return;

    // Calculate a square plot area centered in the rect
    int plotSize = std::min(rect.width(), rect.height()) - 20;
    if (plotSize < 50) return;

    QRect plotArea(
        rect.x() + (rect.width() - plotSize) / 2,
        rect.y() + (rect.height() - plotSize) / 2,
        plotSize,
        plotSize
    );

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    drawGrid(painter, plotArea);

    // drawConstellation now returns the reference amplitude it used for scaling
    // so we can display a meaningful value in the info line
    float refAmp = drawConstellation(painter, plotArea, samples.get(), count);

    // Draw title
    painter.setPen(QColor(86, 212, 200));
    QFont titleFont = painter.font();
    titleFont.setPointSize(9);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.drawText(plotArea.left(), plotArea.top() - 6, "I/Q Constellation");

    // Draw axis labels
    QFont labelFont = painter.font();
    labelFont.setPointSize(7);
    labelFont.setBold(false);
    painter.setFont(labelFont);
    painter.setPen(QColor(166, 173, 200));
    painter.drawText(plotArea.right() - 30, plotArea.center().y() + plotSize / 2 + 14, "I →");
    painter.drawText(plotArea.center().x() - plotSize / 2 - 4, plotArea.top() - 2, "Q ↑");

    // Info line: show the 95th-percentile reference amplitude rather than the raw
    // peak so the user knows the scale is noise-robust
    painter.setPen(QColor(166, 173, 200, 180));
    QFont infoFont = painter.font();
    infoFont.setPointSize(7);
    painter.setFont(infoFont);
    QString info = QString("Samples: %1 | Ref amp (p95): %2")
                       .arg(count)
                       .arg(QString::number(refAmp, 'f', 3));
    painter.drawText(plotArea.left(), plotArea.bottom() + 14, info);

    painter.restore();
}

void ConstellationPlot::drawGrid(QPainter &painter, const QRect &plotArea)
{
    // Semi-transparent grid background
    painter.fillRect(plotArea, QColor(18, 18, 30));

    // Border
    QPen borderPen(QColor(49, 50, 68), 1);
    painter.setPen(borderPen);
    painter.drawRect(plotArea);

    // Grid lines
    QPen gridPen(QColor(49, 50, 68, 120), 1, Qt::DotLine);
    painter.setPen(gridPen);

    int cx = plotArea.center().x();
    int cy = plotArea.center().y();
    int halfW = plotArea.width() / 2;
    int halfH = plotArea.height() / 2;

    // Draw quarter grid lines
    for (int i = 1; i <= 3; i++) {
        int offsetX = (halfW * i) / 4;
        int offsetY = (halfH * i) / 4;

        // Vertical
        painter.drawLine(cx + offsetX, plotArea.top(), cx + offsetX, plotArea.bottom());
        painter.drawLine(cx - offsetX, plotArea.top(), cx - offsetX, plotArea.bottom());
        // Horizontal
        painter.drawLine(plotArea.left(), cy + offsetY, plotArea.right(), cy + offsetY);
        painter.drawLine(plotArea.left(), cy - offsetY, plotArea.right(), cy - offsetY);
    }

    // Draw center cross (axes) more prominently
    QPen axisPen(QColor(69, 71, 90), 1, Qt::SolidLine);
    painter.setPen(axisPen);
    painter.drawLine(cx, plotArea.top(), cx, plotArea.bottom());    // Q axis (vertical)
    painter.drawLine(plotArea.left(), cy, plotArea.right(), cy);     // I axis (horizontal)

    // Draw unit circle
    QPen circlePen(QColor(86, 212, 200, 60), 1, Qt::DashLine);
    painter.setPen(circlePen);
    int radius = std::min(halfW, halfH);
    painter.drawEllipse(QPoint(cx, cy), radius, radius);

    // Draw half-unit circle
    QPen halfCirclePen(QColor(86, 212, 200, 30), 1, Qt::DotLine);
    painter.setPen(halfCirclePen);
    painter.drawEllipse(QPoint(cx, cy), radius / 2, radius / 2);
}

// ---------------------------------------------------------------------------
//  drawConstellation — noise-robust implementation
//
//  Two key changes vs. the original:
//
//  1. PERCENTILE-BASED SCALING
//     The original used the absolute peak amplitude as the scale reference.
//     A single noise spike (or an occasional transient) would push maxAmp up,
//     compressing the real constellation into a tiny region in the centre.
//     We now sort the per-sample amplitudes and use the 95th percentile as the
//     reference.  The top 5 % of outliers are simply clipped to the plot edge
//     rather than forcing the entire scale to accommodate them.
//
//  2. 2-D DENSITY HEATMAP
//     Instead of drawing every sample as a dot with equal visual weight, we
//     accumulate hits into a fixed-resolution grid and render each cell with
//     brightness proportional to log(density).  Because noise is spread
//     uniformly it produces a faint, even haze across the grid.  Signal
//     energy concentrates at the constellation points and produces clearly
//     distinct bright clusters — the log scale compresses the wide dynamic
//     range and prevents noisy backgrounds from washing out the signal.
//
//  Return value: the p95 reference amplitude used for scaling (displayed by
//  paintMid in the info line — no other caller needs to change).
// ---------------------------------------------------------------------------
float ConstellationPlot::drawConstellation(QPainter &painter, const QRect &plotArea,
                                            std::complex<float> *samples, size_t count)
{
    if (count == 0) return 0.0f;

    // -----------------------------------------------------------------------
    // 1. Compute per-sample amplitudes and derive a robust scale reference
    //    using the 95th percentile.  This makes the plot immune to the ~5 %
    //    worst noise / transient spikes that would otherwise dominate the
    //    scale when using a raw maximum.
    // -----------------------------------------------------------------------
    std::vector<float> amps(count);
    for (size_t i = 0; i < count; i++)
        amps[i] = std::abs(samples[i]);

    // Work on a copy so the original order is preserved for the render pass
    std::vector<float> sorted = amps;
    std::sort(sorted.begin(), sorted.end());

    // 95th percentile index (clamp to valid range)
    size_t p95idx = static_cast<size_t>(0.95f * static_cast<float>(count - 1));
    p95idx = std::min(p95idx, count - 1);
    float refAmp = sorted[p95idx];

    // Safety guard: if p95 is essentially zero the signal is absent
    if (refAmp < 1e-10f) {
        refAmp = sorted.back();          // try absolute max as last resort
        if (refAmp < 1e-10f) return 0.0f;
    }

    // Map [-refAmp, +refAmp] → plot area, leaving a 5 % margin.
    // Samples beyond refAmp are clamped to the edge (not discarded) so that
    // outliers remain visible as edge artefacts rather than silently vanishing.
    int halfW  = plotArea.width()  / 2;
    int halfH  = plotArea.height() / 2;
    float scale = 0.95f * static_cast<float>(std::min(halfW, halfH)) / refAmp;

    // -----------------------------------------------------------------------
    // 2. Accumulate hits into a 2-D density grid.
    //    Resolution of GRID×GRID cells; 128 gives a good balance between
    //    spatial detail and memory / fill cost.
    // -----------------------------------------------------------------------
    static constexpr int GRID = 128;
    std::vector<int> density(GRID * GRID, 0);
    int maxDensity = 0;

    int cx = plotArea.center().x();
    int cy = plotArea.center().y();

    for (size_t i = 0; i < count; i++) {
        float I =  samples[i].real();
        float Q = -samples[i].imag();   // flip Y: screen y increases downwards

        // Scale to [-1, +1] normalised coordinates, then to grid indices.
        // std::clamp keeps outliers inside the grid border cell.
        float nx = (I * scale) / static_cast<float>(halfW);  // [-1..1] approx
        float ny = (Q * scale) / static_cast<float>(halfH);

        int gx = static_cast<int>((nx + 1.0f) * 0.5f * (GRID - 1) + 0.5f);
        int gy = static_cast<int>((ny + 1.0f) * 0.5f * (GRID - 1) + 0.5f);

        gx = std::max(0, std::min(GRID - 1, gx));
        gy = std::max(0, std::min(GRID - 1, gy));

        int &cell = density[gy * GRID + gx];
        ++cell;
        if (cell > maxDensity) maxDensity = cell;
    }

    if (maxDensity == 0) return refAmp;

    // -----------------------------------------------------------------------
    // 3. Render the density grid as a heatmap.
    //    Brightness is mapped through a logarithmic curve so that:
    //      • a cell hit by even 1 sample is faintly visible (noise floor)
    //      • a cell hit by many samples saturates to bright teal/white
    //    This gives a perceptually clear separation between the noise haze
    //    (dim, uniform) and the constellation clusters (bright, localised).
    //
    //    Colour ramp:  dim blue-grey (sparse) → teal → near-white (dense)
    // -----------------------------------------------------------------------
    const float logMax = std::log1p(static_cast<float>(maxDensity));
    const float cellW  = static_cast<float>(plotArea.width())  / GRID;
    const float cellH  = static_cast<float>(plotArea.height()) / GRID;

    painter.setPen(Qt::NoPen);

    for (int gy = 0; gy < GRID; gy++) {
        for (int gx = 0; gx < GRID; gx++) {
            int d = density[gy * GRID + gx];
            if (d == 0) continue;

            // Logarithmic normalisation: t ∈ (0, 1]
            float t = std::log1p(static_cast<float>(d)) / logMax;

            // Colour: lerp from dim blue (0,40,80) → bright teal-white
            //   r: 0   → 180   g: 160 → 230   b: 200 → 255
            int r = static_cast<int>(180.0f * t);
            int g = static_cast<int>(160.0f + 70.0f * t);
            int b = static_cast<int>(200.0f + 55.0f * t);
            int a = static_cast<int>(30.0f  + 225.0f * t);   // faint for 1 hit, opaque for many

            int px = plotArea.left() + static_cast<int>(gx * cellW);
            int py = plotArea.top()  + static_cast<int>(gy * cellH);
            int pw = std::max(1, static_cast<int>(cellW) + 1);
            int ph = std::max(1, static_cast<int>(cellH) + 1);

            painter.fillRect(px, py, pw, ph, QColor(r, g, b, a));
        }
    }

    return refAmp;
}
