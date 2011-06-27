#include "../include/doubleslider.h"

DoubleSlider::DoubleSlider(QWidget* parent)
{
    connect(this, SIGNAL(rangeChanged(int, int)), this, SLOT(updateRange(int, int)));
    connect(this, SIGNAL(sliderReleased()), this, SLOT(movePressedHandle()));
    setUpperValue(minimum());
    setLowerValue(maximum());
    qDebug() << minimum();
    qDebug() << maximum();
    update();
}


int DoubleSlider::pixelPosToRangeValue(int pos)
{
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    int sliderMin = 0;
    int sliderMax = 0;
    int sliderLength = 0;
    const QSlider* p = (QSlider*)this;
    const QRect gr = p->style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, p);
    const QRect sr = p->style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, p);
    if (p->orientation() == Qt::Horizontal)
    {
        sliderLength = sr.width();
        sliderMin = gr.x();
        sliderMax = gr.right() - sliderLength + 1;
    }
    else
    {
        sliderLength = sr.height();
        sliderMin = gr.y();
        sliderMax = gr.bottom() - sliderLength + 1;
    }
    return QStyle::sliderValueFromPosition(p->minimum(), p->maximum(), pos - sliderMin,
                                           sliderMax - sliderMin, opt.upsideDown);
}

void DoubleSlider::setSpan(int lower, int upper) {
    const int low = qBound(minimum(), qMin(lower, upper), maximum());
    const int upp = qBound(minimum(), qMax(lower, upper), maximum());
    if (low != lower || upp != upper)
    {
        if (low != lower)
        {
            lower = low;
            emit lowerValueChanged(low);
        }
        if (upp != upper)
        {
            upper = upp;
            emit upperValueChanged(upp);
        }
        emit spanChanged(lower, upper);
        update();
    }
}

QVector<int> DoubleSlider::value() {
    QVector<int> v;
    v << lower;
    v << upper;
    return(v);
}

/*
#ifdef Q_OS_MAC
    QRect clip = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
    painter->setClipRect(clip);
#endif
    painter->drawComplexControl(QStyle::CC_Slider, opt);
}
*/


/*!
    \property DoubleSlider::lowerValue
    \brief the lower value of the span
 */
int DoubleSlider::lowerValue() const
{
    return qMin(lower, upper);
}

void DoubleSlider::setLowerValue(int lower)
{
    setSpan(lower, upper);
}

/*!
    \property DoubleSlider::upperValue
    \brief the upper value of the span
 */
int DoubleSlider::upperValue() const
{
    return qMax(lower, upper);
}

void DoubleSlider::setUpperValue(int upper)
{
    setSpan(lower, upper);
}


/*!
    \reimp
 */
void DoubleSlider::mousePressEvent(QMouseEvent* event)
{
    QStyleOptionSlider opt;
    QStyle::SubControl control = style()->hitTestComplexControl(QStyle::CC_Slider, &opt, event->pos(), this);
    if (control != QStyle::SC_SliderHandle) {
        event->ignore();
        return;
    }
    selectedHandle = clickedHandle(event);
    qDebug() << control;

    //handleMousePress(event->pos(), upperPressed, upper, DoubleSlider::UpperHandle);
    //if (upperPressed != QStyle::SC_SliderHandle)
    //    handleMousePress(event->pos(), lowerPressed, lower, DoubleSlider::LowerHandle);
    //firstMovement = true;
    event->accept();
}

DoubleSlider::Handle DoubleSlider::clickedHandle(QMouseEvent* event)
{
    int x = event->pos().x();
    //if(x < )
    return DoubleSlider::HandleNone;
}

/*!
    \reimp
 */
void DoubleSlider::mouseMoveEvent(QMouseEvent* event)
{
    if (selectedHandle == DoubleSlider::HandleNone) {
        return;
    }
    int x = event->pos().x();
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    if (selectedHandle == DoubleSlider::HandleUpper) {
        if (x <= lower) {
            selectedHandle == DoubleSlider::HandleLower;
            lower = x;
        } else {
            upper = x;
        }
    }
    event->accept();
}

/*!
    \reimp
 */
void DoubleSlider::mouseReleaseEvent(QMouseEvent* event)
{
    QSlider::mouseReleaseEvent(event);
    setSliderDown(false);
    selectedHandle == DoubleSlider::HandleNone;
    update();
}

void DoubleSlider::setRange(int min, int max) {
    setMinimum(min);
    setMaximum(max);
}

QVector<int> DoubleSlider::getValues() const {
    QVector<int> r;
    r << lower;
    r << upper;
    return r;
}
/*!
    \reimp
 */
void DoubleSlider::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QStylePainter painter(this);

    // ticks
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    opt.subControls = QStyle::SC_SliderTickmarks;
    painter.drawComplexControl(QStyle::CC_Slider, opt);
}

