#include <QApplication>

#include "controller/controller.h"
#include "model/model.h"
#include "view/main_window.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  s21::Model model;
  s21::Controller controller(&model);
  s21::MainWindow view(&controller);
  view.show();

  return app.exec();
}
