#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QPaintEvent>
#include <QOpenGLWidget>

class QDrawPixmaps : public QOpenGLWidget
{
public:
    QDrawPixmaps(){}

    bool b_UseBeginEnd = false;
    bool b_UseSaveRestore = false;

    void keyPressEvent(QKeyEvent* event) override
    {
        switch( event->key() )
        {
            case 'b':
            case 'B':
                b_UseBeginEnd = b_UseBeginEnd ? false : true;
                break;
            case 's':
            case 'S':
                b_UseSaveRestore = b_UseSaveRestore ? false : true;
                break;
            default:
                qDebug() << "You Pressed: " << static_cast<char>(event->key());
                break;
        }

        this->repaint();
    }

    void paintEvent(QPaintEvent *event) override
    {
        QPainter Painter(this);
        QBrush brushBackground = QBrush(Qt::black);

        Painter.setRenderHint(QPainter::Antialiasing);
        Painter.setRenderHint(QPainter::SmoothPixmapTransform);
        Painter.fillRect(event->rect(),brushBackground);

        QRect drawRect = QRect(0,0,1,1);
        QPen redPen = QPen(Qt::red);
        int iCount = 0;

        Painter.setPen(redPen);

        // avoid nesting begin/end calls or a warning will come up
        // ... does Qt ignore subsequent calls to begin?
        bool bFirstPaint = true;

        for (QPixmap pixmap: m_RegPixmaps)
        {
            drawRect = QRect(iCount*150,iCount*10,150,150);
            if ( b_UseBeginEnd && !bFirstPaint )
                Painter.begin(this);

            bFirstPaint = false;

            if ( b_UseSaveRestore )
                Painter.save();

            Painter.drawPixmap(drawRect,pixmap.copy().scaled(150,150));
            Painter.setPen(redPen);
            Painter.drawRect(drawRect);

            if ( b_UseSaveRestore )
                Painter.restore();

            if ( b_UseBeginEnd )
                Painter.end();

            iCount++;
        }

        if ( b_UseBeginEnd )
            Painter.begin(this);

        QString strUseBeginEnd = QString("Use Begin End: " ) + QString(b_UseBeginEnd ? "True" : "False");
        QString strUseSaveRestore = QString("Use Save Restore: ") + QString(b_UseSaveRestore ? "True" : "False");

        QString strToggleSave = QString("Press S To Toggle QPainter::Save/Restore");
        QString strToggleBegin = QString("Press B To Toggle QPainter::Begin/End");
        QFont font = QFont("Serif",32,2);
        Painter.setFont(font);
        Painter.setPen(QPen(Qt::green));
        Painter.drawText(QPoint(20,40),strUseBeginEnd);
        Painter.drawText(QPoint(20,80),strUseSaveRestore);
        Painter.drawText(QPoint(20,300),strToggleSave);
        Painter.drawText(QPoint(20,340),strToggleBegin);

        Painter.end();
    }


    void createRectPixmaps()
    {
        for ( int i = 0; i < 6; i++ )
        {
            QImage rectImage(300,300, QImage::Format_ARGB32);
            QPainter imgPainter(&rectImage);

            Qt::GlobalColor penColor = static_cast<Qt::GlobalColor>(std::rand() % 16 + 2);
            Qt::GlobalColor brushColor = static_cast<Qt::GlobalColor>(std::rand() % 16 + 2);

            QPen pen = QPen(penColor, 1, Qt::SolidLine);
            QBrush brush = QBrush(brushColor, Qt::SolidPattern);


            imgPainter.setRenderHint(QPainter::Antialiasing);
            imgPainter.setRenderHint(QPainter::LosslessImageRendering);

            rectImage.fill(QColor::fromRgba(0xff0f0f0f));

            //imgPainter.begin(&rectImage);
            imgPainter.setPen(pen);
            imgPainter.setBrush(brush);
            imgPainter.drawRect(50,50,200,200);
            imgPainter.setPen(QPen(Qt::yellow));
            imgPainter.setBrush(QBrush(Qt::darkMagenta));
            imgPainter.drawRoundedRect(75,75,150,150,20,20);
            //imgPainter.end();

            QPixmap rectPixmap(200,200);
            rectPixmap.convertFromImage(rectImage);

            m_RegPixmaps.append(rectPixmap);
        }
    }

    QList<QPixmap> m_RegPixmaps;

};


QDrawPixmaps* g_DrawPixmaps;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    g_DrawPixmaps = new QDrawPixmaps();
    g_DrawPixmaps->setParent(ui->centralwidget);
    g_DrawPixmaps->setGeometry(QRect(0,0,100,100));
    g_DrawPixmaps->createRectPixmaps();
    g_DrawPixmaps->show();
    g_DrawPixmaps->raise();
}

void MainWindow::resizeEvent(QResizeEvent*)
{
#if defined(Q_OS_ANDROID)
    int iWidth = this->screen()->availableGeometry().width();
    int iHeight = this->screen()->availableGeometry().height();
    this->setGeometry(QRect(0,0,iWidth,iHeight));
#else
    int iWidth = this->window()->childrenRect().width();
    int iHeight = this->window()->childrenRect().height();
#endif

    g_DrawPixmaps->setGeometry(QRect(0,0,iWidth,iHeight));
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
   g_DrawPixmaps->keyPressEvent(event);
}


MainWindow::~MainWindow()
{
    delete g_DrawPixmaps;
    delete ui;
}

