#ifndef SRC_VIEW_MAIN_WINDOW_H_
#define SRC_VIEW_MAIN_WINDOW_H_

#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <chrono>  // NOLINT [build/c++11]
#include <memory>
#include <vector>

#include "../controller/controller.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

namespace s21 {

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(Controller *controller, QWidget *parent = nullptr);
  ~MainWindow();

 private slots:
  /* Settings */
  void on_network_type_currentIndexChanged(int index);
  void on_layers_slider_valueChanged(int value);
  void on_epoch_box_valueChanged(int value);
  void on_weight_saving_checkbox_stateChanged(int state);
  void on_learning_speed_textChanged(QString number);
  void on_save_weights_clicked();
  void on_load_weights_clicked();
  void on_open_train_button_clicked();
  void on_open_test_button_clicked();

  /* Testing */
  void on_weight_mode_clicked();
  void on_cross_validation_mode_clicked();
  void on_sampe_part_box_valueChanged(double value);
  void on_group_ammount_box_valueChanged(int value);
  void on_begin_test_button_clicked();
  void on_end_test_button_clicked();

  /* Learning */
  void on_begin_studying_box_clicked();
  void on_stop_studying_button_clicked();

  /* Painting */
  void on_load_image_button_clicked();

  void on_clear_button_clicked();

 private:
  QString ElidedText(const std::string &text, const QLabel *label) const;
  void ShowMessage(QMessageBox::Icon type, const QString &title,
                   const QString &message);
  void SetTrainEpochInformation(std::size_t current_epoch,
                                std::size_t total_epochs);
  void SetTestProgress(std::size_t progress);
  void SetTrainProgress(std::size_t progress);
  void SetNetworkInfo(NetworkSettings settings,
                      std::size_t epoch = std::string::npos,
                      std::size_t accuracy = std::string::npos);

  /* Custom slots */
  void on_train_dataset_loaded(std::string filename, std::size_t dataset_size);
  void on_train_dataset_error(const std::string &message);
  void on_test_dataset_loaded(std::string filename, std::size_t dataset_size);
  void on_test_dataset_error(const std::string &message);

  void TestUiLock();
  void TestUiUnlock();
  void TrainUiLock();
  void TrainUiUnlock();

  void OnTestStart();
  void OnTestProgress(std::size_t progress);
  void OnTestEnd(NetworkTestMetrics metrics);
  void OnTestError(const std::string &message);

  void OnTrainStart();
  void OnTrainEpochProgress(std::size_t progress);
  void OnTrainEpochEnd(std::size_t epoch);
  void OnTrainEpochTestStart();
  void OnTrainEpochTestProgress(std::size_t progress);
  void OnTrainEpochTestEnd(std::size_t accuracy, std::size_t epoch);
  void OnTrainEnd();
  void OnTrainError(const std::string &message);

  Ui::MainWindow *ui;
  Controller *controller_;
};

}  // namespace s21

#endif  // SRC_VIEW_MAIN_WINDOW_H_
