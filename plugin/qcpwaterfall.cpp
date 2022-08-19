#include "qcpwaterfall.h"

#include <QAction>

double estimatePeriod(QVector<uint64_t> tsVec)
{
    double sum = 0;
    for (int i = 0; i < tsVec.size() - 1; i++) {
        sum += tsVec[i + 1] - tsVec[i];
    }

    return sum / (tsVec.size() - 1);
}

QCPWaterfall::QCPWaterfall(QCPAxis *keyAxis, QCPAxis *valueAxis)
    : QCPColorMap(keyAxis, valueAxis)
{

}

/**
 * @brief QCPWaterfall::setSize
 * @param size size of Color Map
 * @param defVal value to show
 */
void QCPWaterfall::setSize(const QSize &size, double defVal)
{
    data()->setSize(size.width(), size.height());
    m_defValue = defVal;

    for (int i = 0; i < size.height(); ++i) {
        for (int j = 0; j < size.width(); ++j) {
            data()->setCell(j, i, defVal);
        }
    }
}

/**
 * @brief QCPWaterfall::addData
 * @param vector row r column
 * @param orient horizontal or vertical
 */
void QCPWaterfall::addData(uint64_t timestamp, const QList<double> &yVec, const QList<double> &zVec, Qt::Orientation orient)
{
    auto W = data()->keySize();
    auto H = data()->valueSize();

    // t
    m_timeVector.enqueue(timestamp);

    // Y
    if (yVec.size() != data()->valueSize())
        data()->setValueSize(qMax(yVec.size(), data()->valueSize()));

    if (m_timeVector.size() < 2) {
        double maxY = qMax(yVec.first(), yVec.last());
        double minY = qMin(yVec.first(), yVec.last());
        data()->setValueRange({minY, maxY});
    } else {
        bool isOk;
        auto rangeY = getValueRange(isOk);
        double maxY = qMax(qMax(rangeY.upper, yVec.first()), yVec.last());
        double minY = qMin(qMin(rangeY.lower, yVec.first()), yVec.last());
        data()->setValueRange({minY, maxY});
    }

    // t, Z
    bool doUpdate = false;

    if (orient == Qt::Horizontal) {
        int pos = H > m_lastRowIndex ? m_lastRowIndex++ : H - 1;
        for (int i = 0; i < zVec.size(); ++i) {
            data()->setCell(i, pos, zVec[i]);
        }
        if (H == m_lastRowIndex) {
            data()->removeRow(0);
            doUpdate = true;
        }
    } else {
        int pos = W > m_lastColumnIndex ? m_lastColumnIndex++ : W - 1;
        for (int i = 0; i < zVec.size(); ++i) {
            data()->setCell(pos, i, zVec[i]);
        }
        if (W == m_lastColumnIndex) {
            data()->removeColumn(0);
            doUpdate = true;
        }
    }

    if (doUpdate) {
        m_timeVector.dequeue();
        data()->setKeyRange(QCPRange(m_timeVector.first(), m_timeVector.last()));
    } else {
        double period = estimatePeriod(m_timeVector.toVector());
        data()->setKeyRange(QCPRange(m_timeVector.first(), m_timeVector.first() + period * data()->keySize()));
    }

    colorScale()->setDataRange(data()->dataBounds());

    rescaleAxes();
}

/*
     gpGrayscale  ///< Continuous lightness from black to white (suited for non-biased data representation)
    ,gpHot       ///< Continuous lightness from black over firey colors to white (suited for non-biased data representation)
    ,gpCold      ///< Continuous lightness from black over icey colors to white (suited for non-biased data representation)
    ,gpNight     ///< Continuous lightness from black over weak blueish colors to white (suited for non-biased data representation)
    ,gpCandy     ///< Blue over pink to white
    ,gpGeography ///< Colors suitable to represent different elevations on geographical maps
    ,gpIon       ///< Half hue spectrum from black over purple to blue and finally green (creates banding illusion but allows more precise magnitude estimates)
    ,gpThermal   ///< Colors suitable for thermal imaging, ranging from dark blue over purple to orange, yellow and white
    ,gpPolar     ///< Colors suitable to emphasize polarity around the center, with blue for negative, black in the middle and red for positive values
    ,gpSpectrum  ///< An approximation of the visible light spectrum (creates banding illusion but allows more precise magnitude estimates)
    ,gpJet       ///< Hue variation similar to a spectrum, often used in numerical visualization (creates banding illusion but allows more precise magnitude estimates)
    ,gpHues
 */

const QStringList GradientNames = {"Grayscale", "Hot", "Cold", "Night", "Candy", "Geo", "Ion", "Thermal", "Polar", "Spectrum", "Jet", "Hues"};

QCPWaterfallScale::QCPWaterfallScale(QCustomPlot *parentPlot)
    : QCPColorScale(parentPlot)
{
    parentPlot->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(parentPlot, &QWidget::customContextMenuRequested,
        [this] (const QPoint &pos) {
        QMenu contextMenu(tr("Context menu"), static_cast<QWidget*>(this->parentPlot()));

        QList<QAction*> actVector;
        for (int i = 0; i < QCPColorGradient::gpHues; ++i) {
            actVector.push_back(new QAction(GradientNames[i], this));
            actVector.back()->setData(i);
            auto action = actVector.back();
            connect(action, &QAction::triggered, [&, action]() {
                // QAction *act = static_cast<QAction*>(sender());
                /*QCPColorGradient gradient(static_cast<QCPColorGradient::GradientPreset>(action->data().toInt()));
                gradient.setNanHandling(QCPColorGradient::nhNanColor);
                gradient.setNanColor(Qt::white);
                setGradient(gradient);*/
                setGradient(static_cast<QCPColorGradient::GradientPreset>(action->data().toInt()));
                this->parentPlot()->update();
            });
            contextMenu.addAction(action);
        }
        contextMenu.exec(this->parentPlot()->mapToGlobal(pos));
    });

}

void QCPWaterfallScale::setGradient(const QCPColorGradient::GradientPreset &preset)
{
    QCPColorGradient gradient(preset);
    gradient.setNanHandling(QCPColorGradient::nhNanColor);
    QColor color;
    switch (preset) {
    case QCPColorGradient::gpGrayscale:
    color = Qt::red;
        break;
    case QCPColorGradient::gpHot:
    case QCPColorGradient::gpCold:
    case QCPColorGradient::gpNight:
    case QCPColorGradient::gpCandy:
    case QCPColorGradient::gpThermal:   ///< Colors suitable for thermal imaging, ranging from dark blue over purple to orange, yellow and white
    color = Qt::black;
    break;

    case QCPColorGradient::gpGeography: ///< Colors suitable to represent different elevations on geographical maps
    case QCPColorGradient::gpIon:       ///< Half hue spectrum from black over purple to blue and finally green (creates banding illusion but allows more precise magnitude estimates)
    case QCPColorGradient::gpPolar:     ///< Colors suitable to emphasize polarity around the center, with blue for negative, black in the middle and red for positive values
    case QCPColorGradient::gpSpectrum:  ///< An approximation of the visible light spectrum (creates banding illusion but allows more precise magnitude estimates)
    case QCPColorGradient::gpJet:       ///< Hue variation similar to a spectrum, often used in numerical visualization (creates banding illusion but allows more precise magnitude estimates)
    case QCPColorGradient::gpHues:
    color = Qt::white;
    break;
    }

    gradient.setNanColor(color);
    QCPColorScale::setGradient(gradient);
}
