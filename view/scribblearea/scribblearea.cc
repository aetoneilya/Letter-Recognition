#include "scribblearea.h"

#include <QMouseEvent>
#include <QPainter>

#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printdialog)
#include <QPrintDialog>
#include <QPrinter>
#endif
#endif

//! [0]
ScribbleArea::ScribbleArea(QWidget *parent) : QWidget(parent) {
  setAttribute(Qt::WA_StaticContents);
}
//! [0]

//! [1]
bool ScribbleArea::openImage(const QString &fileName)
//! [1] //! [2]
{
  QImage loadedImage;
  if (!loadedImage.load(fileName)) return false;

  image = loadedImage;
  adapt();
  clean = false;
  modified = false;
  update();
  return true;
}
//! [2]

//! [5]
void ScribbleArea::setPenColor(const QColor &newColor)
//! [5] //! [6]
{
  myPenColor = newColor;
}
//! [6]

//! [7]
void ScribbleArea::setPenWidth(int newWidth)
//! [7] //! [8]
{
  myPenWidth = newWidth;
}

void ScribbleArea::set_controller(s21::Controller *new_controler) {
  controller_ = new_controler;
}

void ScribbleArea::SetRecognitionLetterWidget(QLabel *widget) {
  recognition_label = widget;
}
//! [8]

//! [9]
void ScribbleArea::clearImage()
//! [9] //! [10]
{
  image.fill(qRgb(255, 255, 255));
  clean = true;
  modified = true;
  update();
}

void ScribbleArea::update_image_info() {
  double coeff = image.width() / static_cast<double>(532);
  QImage image_ = image.scaled(s21::Image::kWidthInPx * coeff,
                               s21::Image::kHeightInPx * coeff,
                               Qt::KeepAspectRatio, Qt::SmoothTransformation);
  std::vector<double> data(s21::Image::kSizeInPx);
  for (int i = 0; i < s21::Image::kHeightInPx; i++) {
    for (int j = 0; j < s21::Image::kWidthInPx; j++) {
      data[i * s21::Image::kHeightInPx + j] =
          std::abs(image_.pixelColor(i, j).value() - 255);
    }
  }
  if (!clean) {
    SetRecognitionLetter(controller_->AnalyzeRawImage(data));
  } else {
    recognition_label->setText("Начните рисовать");
  }
}
//! [10]

void ScribbleArea::SetRecognitionLetter(char letter) {
  recognition_label->setText("Мне кажется, Вы нарисовали букву <b>" +
                             QString(letter) + "</b>");
}

//! [11]
void ScribbleArea::mousePressEvent(QMouseEvent *event)
//! [11] //! [12]
{
  if (event->button() == Qt::LeftButton) {
    lastPoint = event->position().toPoint();
    scribbling = true;
  }
}

void ScribbleArea::mouseMoveEvent(QMouseEvent *event) {
  if ((event->buttons() & Qt::LeftButton) && scribbling)
    drawLineTo(event->position().toPoint());
}

void ScribbleArea::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton && scribbling) {
    drawLineTo(event->position().toPoint());
    scribbling = false;
  }
  update_image_info();
}

//! [12] //! [13]
void ScribbleArea::paintEvent(QPaintEvent *event)
//! [13] //! [14]
{
  QPainter painter(this);
  QRect dirtyRect = event->rect();
  painter.drawImage(dirtyRect, image, dirtyRect);
}
//! [14]

//! [15]
void ScribbleArea::resizeEvent(QResizeEvent *event)
//! [15] //! [16]
{
  if (width() > image.width() || height() > image.height()) {
    int newWidth = qMax(width() + 128, image.width());
    int newHeight = qMax(height() + 128, image.height());
    resizeImage(&image, QSize(newWidth, newHeight));
    update();
  }
  QWidget::resizeEvent(event);
}
//! [16]

//! [17]
void ScribbleArea::drawLineTo(const QPoint &endPoint)
//! [17] //! [18]
{
  QPainter painter(&image);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setPen(
      QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  painter.drawLine(lastPoint, endPoint);
  clean = false;
  modified = true;

  int rad = (myPenWidth / 2) + 2;
  update(
      QRect(lastPoint, endPoint).normalized().adjusted(-rad, -rad, +rad, +rad));
  lastPoint = endPoint;
}
//! [18]

//! [19]
void ScribbleArea::resizeImage(QImage *image, const QSize &newSize)
//! [19] //! [20]
{
  if (image->size() == newSize) return;

  QImage newImage(newSize, QImage::Format_RGB32);
  newImage.fill(qRgb(255, 255, 255));
  QPainter painter(&newImage);
  painter.drawImage(QPoint(0, 0), *image);
  *image = newImage;
}
//! [20]

QImage ScribbleArea::get_image() {
  QImage result = image;
  return result;
}

void ScribbleArea::adapt() {
  double coeff = image.width() / static_cast<double>(532);
  image = image.scaled(532 * coeff, 532 * coeff, Qt::IgnoreAspectRatio,
                       Qt::SmoothTransformation);
  image = image.convertToFormat(QImage::Format_Grayscale8);
}
//! [22]
