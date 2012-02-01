#include <QtGui>
#include <QtCore>
#include <iostream>

#include "Timeline.h"
#include "TestWidget.h"

Timeline *tw = 0;
TestWidget *testWidget = 0;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    
    QWidget *window = new QWidget();
    window->resize(400,300);
    window->show();
      
    QVBoxLayout vbox(window);
    vbox.setAlignment(Qt::AlignTop);
    
    tw = new Timeline(400, 0, 10/*0000*/, 1, window);
    tw->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tw->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tw->setSliderIndex(3);
    
    QVector<int> bookmarks;
    bookmarks << 0 << 1 << 4 << 5 << 6 << 8 << 9 << 10;
    tw->addBookmarks(bookmarks);
    tw->show();
    
    vbox.addWidget(tw);

    // Start test widget
    testWidget = new TestWidget(tw);
    testWidget->setWindowTitle("Testwidget");
    
    return app.exec();
}
