#include "Timeline.h"
#include <iostream>
#include <algorithm>
#include <QPixmap>


Timeline::Timeline(QWidget *parent) {
    setParent(parent);
    
    // Default property values
    bookmarkHeight = 6;
    marginTopBot = 5;
    marginLR = 5;
    bgColor = QColor(Qt::white);
    ordered_width = 400;
    minIndex = 0;
    steps = 10;
    stepSize = 1;
    
    reconfigure_slidebar = false;
    
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    // Setup scene
    //ordered_width = getWidth();
    scene = new QGraphicsScene;
    scene->setBackgroundBrush(getBackgroundColor());
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setScene(scene);
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // Add items
    slidebar = new SlideBarItem(getMinIndex(), getSteps(), getStepSize());
    slidebar->setBookmarkHeight(getBookmarkHeight());
    slidebar->setOrderedWidth(ordered_width - getMarginLR()*2);
    
    //slidebar->setPos(-(slidebar->getWidth() / 2), 0);
    //slidebar->setPos(0,0);
    scene->addItem(slidebar);
    
    startmarker = new BoundarySlider(slidebar, getMinIndex());
    slidebar->addTimeMarker(startmarker);
    //setStartMarkerIndex(getMinIndex());
    
    endmarker = new BoundarySlider(slidebar, getMaxIndex());
    slidebar->addTimeMarker(endmarker);
    //setEndMarkerIndex(getMaxIndex());
    
    connect(slidebar->getIndexSlider(), SIGNAL(sliderReleased(Slider*, int)), this, SLOT(sliderReleased(Slider*, int)));
    connect(startmarker, SIGNAL(sliderReleased(Slider*, int)), this, SLOT(sliderReleased(Slider*, int)));
    connect(endmarker, SIGNAL(sliderReleased(Slider*, int)), this, SLOT(sliderReleased(Slider*, int)));
    connect(slidebar, SIGNAL(bookmarkClicked(int)), this, SLOT(bookmarkClickedSlot(int)));
    
    /* Timeout, at the moment only used for TestWidget */
    timer = new QTimer(this);
    timer->setInterval(25);
    connect(timer, SIGNAL(timeout()), this, SLOT(fireTimeout()));
    timer->start();
    
    
    updateScene();
    
}

int Timeline::getBookmarkHeight() const {
    return bookmarkHeight;
}

void Timeline::setBookmarkHeight(int height) {
    bookmarkHeight = height;
}

int Timeline::getMarginTopBot() {
    return marginTopBot;
}

void Timeline::setMarginTopBot(int margin) {
    marginTopBot = margin;
}

int Timeline::getMarginLR() {
    return marginLR;
}

void Timeline::setMarginLR(int margin) {
    marginLR = margin;
}

const QColor& Timeline::getBackgroundColor() const {
    return bgColor;
}

void Timeline::setBackgroundColor(const QColor & color) {
    bgColor = color;
}

int Timeline::getWidth() {
    return ordered_width;
}

void Timeline::setWidth(int width) {
    ordered_width = width;
}

unsigned Timeline::getMinIndex() {
    return minIndex;
}

void Timeline::setMinIndex(unsigned minIndex) {
    this->minIndex = minIndex;
    reconfigure_slidebar = true;
}

unsigned Timeline::getSteps() {
    return steps;
}

void Timeline::setSteps(unsigned steps) {
    this->steps = steps;
    reconfigure_slidebar = true;
}

unsigned Timeline::getStepSize() {
    return stepSize;
}

void Timeline::setStepSize(unsigned stepSize) {
    this->stepSize = stepSize;
    reconfigure_slidebar = true;
}

int Timeline::getSliderIndex() {
    return slidebar->getIndexSlider()->getLastIndex();
}

int Timeline::getStartMarkerIndex() {
    return startmarker->getLastIndex();
}

void Timeline::setStartMarkerIndex(int idx) {
    setSliderIndex(idx, startmarker);
    
}

int Timeline::getEndMarkerIndex() {
    return endmarker->getLastIndex();
}

void Timeline::setEndMarkerIndex(int idx) {
    setSliderIndex(idx, endmarker);
}

void Timeline::setSliderIndex(int idx) {
    std::cout << "Setting slider index to " << idx << std::endl;
    setSliderIndex(idx, slidebar->getIndexSlider());
}

// This method is private!
void Timeline::setSliderIndex(int idx, Slider* slider) {
    // TODO check boundaries!!!
    slider->setLastIndex(idx);
    updateScene();
}

void Timeline::addBookmark(int idx) {
    if(hasBookmarkAtIndex(idx)) {
        return;
    }
    
    QVector<int> vec;
    vec << idx;
    addBookmarks(vec);
}

void Timeline::addBookmarks(QVector<int> bookmarks) {
    Q_FOREACH(int bookmark, bookmarks) {
        this->bookmarks << bookmark;
    }
    slidebar->updateBookmarks(this->bookmarks);
}

void Timeline::removeBookmark(int idx) {
    for(int i = 0; i < bookmarks.size(); i++) {
        if (bookmarks.at(i) == idx) {
            bookmarks.remove(i);    
            emit bookmarkRemoved(idx);
            slidebar->updateBookmarks(bookmarks);
            break;
        }
    }
}

void Timeline::removeAllBookmarks() {
    bookmarks.clear();
    slidebar->updateBookmarks(bookmarks);
}

bool Timeline::hasBookmarkAtIndex(int idx) {
    for(int i = 0; i < bookmarks.size(); i++) {
        if (bookmarks.at(i) == idx) {
            return true;
        }
    }
    return false;
}

void Timeline::resizeEvent(QResizeEvent * event) {
    // TODO Update all items in the scene
    
    QSize size = event->size();
    
//    std::cout << "ResizeEvent!" << std::endl;
//    std::cout << "TimelineWidget newsize: H=" << size.height() << ", W=" << size.width() << std::endl;
    
    QSizeF sceneSize = sceneRect().size();
//    std::cout << "Current scenerect size: H=" << sceneSize.height() << ", W=" << sceneSize.width() << std::endl;
    
    updateScene(size);
}

void Timeline::updateScene() {
    updateScene(size());
}

void Timeline::updateScene(QSizeF newSize) {
    std::cout << "updateScene: " << std::endl;
    if(reconfigure_slidebar) {
        reconfigureSlidebar();
    }
    scene->setBackgroundBrush(getBackgroundColor());
    qreal sceneHeight = std::max(getBookmarkHeight(), slidebar->getIndexSlider()->height()) + getMarginTopBot()*2;
//    qreal sceneWidth = this->width();
    qreal sceneWidth = newSize.width() + getMarginLR()*2;//ordered_width;//std::max((double)SlideBarItem::DESIGNED_WIDTH, (double)width) + PADDING;
    //scene = new QGraphicsScene(-sceneWidth/2.0, -sceneHeight/2.0, sceneWidth, sceneHeight); // TODO fit correct size
    setMaximumHeight(sceneHeight);
    setMinimumHeight(sceneHeight);
//    setMinimumWidth(sceneWidth);
    scene->setSceneRect(-getMarginLR(), -sceneHeight/2.0, sceneWidth, sceneHeight);
    
    slidebar->setOrderedWidth(newSize.width() - getMarginLR()*2);
//    std::cout << "updateScene(): updating slider positions" << std::endl;
    Q_FOREACH(Slider *slider, slidebar->getAllSliders()) {
        //bs.setPos(bs.boundarySnapPos(slidebar->markerPositionForIndex(bs.g)))
        unsigned index = slidebar->markerIndex(slider);
//        std::cout << "Logical marker position: " << index << ", old pos: " << slider->getLastIndex() << std::endl;
        
//// **** TODO Tried to remove pointing offset and do boundary safe positioning but there seems to be a coordinate system mismatch. ****
//        slider->setPos(slider->boundarySnapPos(QPointF(slidebar->markerPositionForIndex(slider->getLastIndex()), slider->pos().y()),
//                                                    0,
//                                                    slidebar->boundingRect().left(),
//                                                    slidebar->boundingRect().right())
//                                                );

//        std::cout << "old slider xPos: " << slider->pos().x() << ", new pos: " << slidebar->markerPositionForIndex(slider->getLastIndex()) << std::endl;
        slider->setPos(slidebar->markerPositionForIndex(slider->getLastIndex()), slider->pos().y());
    }

//    std::cout << "this->height(): " << this->height() << std::endl;
//    std::cout << "this->width(): " << this->width() << std::endl;
//    std::cout << "this->viewport()->size().width()" << this->viewport()->size().width() << std::endl;
    
    update();

}

void Timeline::bookmarkClickedSlot(int idx) {
    std::cout << "********************** BOOKMARK CLICKED at index " << idx << "!!!!! ****************" << std::endl;
    emit bookmarkClicked(idx);
}

void Timeline::fireTimeout() {
    emit timeout();
}

void Timeline::sliderReleased(Slider* slider, int idx) {
    Slider* indexSlider = slidebar->getIndexSlider();
    
    if(slider == indexSlider) {
        emit indexSliderReleased(idx);
        std::cout << "********************** INDEX SLIDER RELEASED at index " << idx << "!!!!! ****************" << std::endl;
    } else if (slider == startmarker) {
        emit startMarkerReleased(idx);
        std::cout << "********************** START MARKER RELEASED at index " << idx << "!!!!! ****************" << std::endl;
    } else if (slider == endmarker) {
        emit endMarkerReleased(idx);
        std::cout << "********************** END MARKER RELEASED at index " << idx << "!!!!! ****************" << std::endl;
    } else {
        std::cout << "********************** UNKNOWN SLIDER RELEASED at index " << idx << "!!!!! ****************" << std::endl;
    }
}

void Timeline::reconfigureSlidebar() {
    std::cout << "reconfigureSlidebar()" << std::endl;
    reconfigure_slidebar = false;
    slidebar->reconfigure(getMinIndex(), getSteps(), getStepSize());

    setStartMarkerIndex(getMinIndex());
    setEndMarkerIndex(getMaxIndex());
}

unsigned Timeline::getMaxIndex() {
    return getMinIndex() + getSteps() * getStepSize();
}