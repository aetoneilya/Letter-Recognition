#include "main_window.h"

#include "ui_main_window.h"

namespace s21 {

MainWindow::MainWindow(Controller *controller, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), controller_(controller) {
  ui->setupUi(this);

  // TODO(ppoutine) Check it on macOS
  std::setlocale(LC_ALL, "C");

  auto configuration = controller_->GetConfiguration();

  switch (configuration.GetTestType()) {
    case Configuration::TestType::kCrossValidation:
      ui->cross_validation_mode->setChecked(true);
      break;
    case Configuration::TestType::kWeight:
      ui->weight_mode->setChecked(true);
      break;
  }
  ui->sampe_part_box->setValue(configuration.GetSelectionPart());
  ui->group_ammount_box->setValue(configuration.GetNumberOfGroups());
  ui->network_type->setCurrentIndex(
      static_cast<int>(configuration.GetNetworkType()));
  ui->layers_slider->setValue(configuration.GetNumberOfHiddenLayers());
  ui->epoch_box->setValue(configuration.GetEpochs());
  ui->weight_saving_checkbox->setChecked(
      configuration.GetSaveWeightsEachEpoch());
  ui->learning_speed->setText(
      QString::fromStdString(std::to_string(configuration.GetLearningRate())));

  ui->image_label->set_controller(controller_);
  ui->image_label->SetRecognitionLetterWidget(ui->recognition_result);

  ui->widget->legend->setVisible(true);

  ui->widget->addGraph();
  ui->widget->graph(0)->setPen(QPen(Qt::blue));
  ui->widget->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));
  ui->widget->graph(0)->setName("Зависимость точности от поколения");
  ui->widget->graph(0)->setLineStyle(QCPGraph::lsLine);
  ui->widget->graph(0)->setScatterStyle(
      QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
  ui->widget->xAxis->setRange(0, 10);
  ui->widget->xAxis->setLabel("Поколение");
  ui->widget->yAxis->setRange(0, 100);
  ui->widget->yAxis->setLabel("Точность");
  ui->widget->xAxis->setTicks(false);
  ui->widget->yAxis->setTicks(true);
  ui->widget->xAxis->setTickLabels(false);
  ui->widget->yAxis->setTickLabels(true);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_weight_mode_clicked() {
  auto conf = controller_->GetConfiguration();
  conf.SetTestType(Configuration::TestType::kWeight);
  controller_->SetConfiguration(conf);
}

void MainWindow::on_cross_validation_mode_clicked() {
  auto conf = controller_->GetConfiguration();
  conf.SetTestType(Configuration::TestType::kCrossValidation);
  controller_->SetConfiguration(conf);
}

void MainWindow::on_sampe_part_box_valueChanged(double value) {
  auto conf = controller_->GetConfiguration();
  conf.SetSelectionPart(value);
  controller_->SetConfiguration(conf);
}

void MainWindow::on_group_ammount_box_valueChanged(int value) {
  auto conf = controller_->GetConfiguration();
  conf.SetNumberOfGroups(value);
  controller_->SetConfiguration(conf);
}

void MainWindow::on_begin_test_button_clicked() {
  controller_->Test(
      [this]() -> void {
        QMetaObject::invokeMethod(
            this, [this]() { std::invoke(&MainWindow::OnTestStart, this); },
            Qt::QueuedConnection);
      },
      [this](std::size_t progress) -> void {
        QMetaObject::invokeMethod(
            this,
            [this, progress]() {
              std::invoke(&MainWindow::OnTestProgress, this, progress);
            },
            Qt::QueuedConnection);
      },
      [this](NetworkTestMetrics metrics) -> void {
        QMetaObject::invokeMethod(
            this,
            [this, metrics]() {
              std::invoke(&MainWindow::OnTestEnd, this, metrics);
            },
            Qt::QueuedConnection);
      },
      [this](const std::string &message) -> void {
        QMetaObject::invokeMethod(
            this,
            [this, message]() {
              std::invoke(&MainWindow::OnTestError, this, message);
            },
            Qt::QueuedConnection);
      });
}

void MainWindow::on_end_test_button_clicked() {
  controller_->StopTest();

  TestUiUnlock();
}

void MainWindow::on_network_type_currentIndexChanged(int index) {
  auto conf = controller_->GetConfiguration();
  conf.SetNetworkType(static_cast<NetworkType>(index));
  controller_->SetConfiguration(conf);
}

void MainWindow::on_layers_slider_valueChanged(int value) {
  auto conf = controller_->GetConfiguration();
  conf.SetNumberOfHiddenLayers(value);
  controller_->SetConfiguration(conf);
}

void MainWindow::on_epoch_box_valueChanged(int value) {
  auto conf = controller_->GetConfiguration();
  conf.SetEpochs(value);
  controller_->SetConfiguration(conf);
}

void MainWindow::on_weight_saving_checkbox_stateChanged(int state) {
  auto conf = controller_->GetConfiguration();
  conf.SetSaveWeightsEachEpoch(static_cast<bool>(state));
  controller_->SetConfiguration(conf);
}

void MainWindow::on_learning_speed_textChanged(QString number) {
  auto conf = controller_->GetConfiguration();
  conf.SetLearningRate(number.toDouble());
  controller_->SetConfiguration(conf);
}

void MainWindow::on_save_weights_clicked() {
  if (controller_->IsNetworkCreated()) {
    QString filename = QFileDialog::getSaveFileName(
        this, tr("Сохранить файл с данными о весах"), QDir::currentPath());
    if (!filename.isNull()) {
      WeightWriter::Write(filename.toStdString(), controller_->GetWeights(),
                          controller_->GetSettings());
    }
  }
}

void MainWindow::on_load_weights_clicked() {
  QString filename = QFileDialog::getOpenFileName(
      this, tr("Открыть файл с данными о весах"), QDir::currentPath());

  if (!filename.isNull()) {
    controller_->SetWeights(
        filename.toStdString(),
        [this](const NetworkSettings &settings, std::size_t epoch,
               std::size_t accuracy) {
          if (epoch == std::string::npos) epoch = 0;
          if (accuracy == std::string::npos) accuracy = 0;
          SetNetworkInfo(settings, epoch, accuracy);
        },
        [this](const std::string &message) {
          ShowMessage(QMessageBox::Icon::Critical,
                      QGuiApplication::applicationDisplayName(),
                      tr("Ошибка при загрузке весов: %1").arg(message.c_str()));
        });
  }
}

void MainWindow::on_open_train_button_clicked() {
  QString filename = QFileDialog::getOpenFileName(
      this, tr("Открыть тренировочный набор"), QDir::currentPath());

  if (!filename.isNull()) {
    ui->open_train_button->setEnabled(false);

    controller_->SetTrainDataset(
        filename.toStdString(),
        [this](std::string filename, std::size_t dataset_size) {
          QMetaObject::invokeMethod(
              this,
              [this, filename, dataset_size]() {
                std::invoke(&MainWindow::on_train_dataset_loaded, this,
                            filename, dataset_size);
              },
              Qt::QueuedConnection);
        },
        [this](const std::string &message) {
          QMetaObject::invokeMethod(
              this,
              [this, message]() {
                std::invoke(&MainWindow::on_train_dataset_error, this, message);
              },
              Qt::QueuedConnection);
        });
  }
}

void MainWindow::on_open_test_button_clicked() {
  QString filename = QFileDialog::getOpenFileName(
      this, tr("Открыть тестовый набор"), QDir::currentPath());

  if (!filename.isNull()) {
    ui->open_test_button->setEnabled(false);

    controller_->SetTestDataset(
        filename.toStdString(),
        [this](std::string filename, std::size_t dataset_size) {
          QMetaObject::invokeMethod(
              this,
              [this, filename, dataset_size]() {
                std::invoke(&MainWindow::on_test_dataset_loaded, this, filename,
                            dataset_size);
              },
              Qt::QueuedConnection);
        },
        [this](const std::string &message) {
          QMetaObject::invokeMethod(
              this,
              [this, message]() {
                std::invoke(&MainWindow::on_test_dataset_error, this, message);
              },
              Qt::QueuedConnection);
        });
  }
}

QString MainWindow::ElidedText(const std::string &text,
                               const QLabel *label) const {
  QFontMetrics metrics(label->font());
  return metrics.elidedText(QString::fromStdString(text), Qt::ElideRight,
                            label->width());
}

void MainWindow::ShowMessage(QMessageBox::Icon type, const QString &title,
                             const QString &message) {
  QMessageBox message_box(
      type, title, message, QMessageBox::StandardButton::Ok, this,
      Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowStaysOnTopHint);
  message_box.setModal(true);
  message_box.exec();
}

void MainWindow::on_load_image_button_clicked() {
  QString fileName =
      QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
  ui->image_label->openImage(fileName);
  ui->image_label->update_image_info();
}

void MainWindow::on_begin_studying_box_clicked() {
  controller_->Train(
      [this]() -> void {
        QMetaObject::invokeMethod(
            this, [this]() { std::invoke(&MainWindow::OnTrainStart, this); },
            Qt::QueuedConnection);
      },
      [this](std::size_t progress) -> void {
        QMetaObject::invokeMethod(
            this,
            [this, progress]() {
              std::invoke(&MainWindow::OnTrainEpochProgress, this, progress);
            },
            Qt::QueuedConnection);
      },
      [this](std::size_t epoch) -> void {
        QMetaObject::invokeMethod(
            this,
            [this, epoch]() {
              std::invoke(&MainWindow::OnTrainEpochEnd, this, epoch + 1);
            },
            Qt::QueuedConnection);
      },
      [this]() {
        QMetaObject::invokeMethod(
            this,
            [this]() { std::invoke(&MainWindow::OnTrainEpochTestStart, this); },
            Qt::QueuedConnection);
      },
      [this](std::size_t progress) {
        QMetaObject::invokeMethod(
            this,
            [this, progress]() {
              std::invoke(&MainWindow::OnTrainEpochTestProgress, this,
                          progress);
            },
            Qt::QueuedConnection);
      },
      [this](NetworkTestMetrics metrics, std::size_t epoch) {
        QMetaObject::invokeMethod(
            this,
            [this, metrics, epoch]() {
              std::invoke(&MainWindow::OnTrainEpochTestEnd, this,
                          metrics.accuracy_percent, epoch);
            },
            Qt::QueuedConnection);
      },
      [this]() {
        QMetaObject::invokeMethod(
            this, [this]() { std::invoke(&MainWindow::OnTrainEnd, this); },
            Qt::QueuedConnection);
      },
      [this](const std::string &message) -> void {
        QMetaObject::invokeMethod(
            this,
            [this, message]() {
              std::invoke(&MainWindow::OnTrainError, this, message);
            },
            Qt::QueuedConnection);
      });
}

void MainWindow::on_stop_studying_button_clicked() {
  controller_->StopTrain();

  TrainUiUnlock();
}

void MainWindow::on_train_dataset_loaded(std::string filename,
                                         std::size_t dataset_size) {
  ui->train_dataset_path->setText(ElidedText(
      filename.substr(filename.find_last_of('/') + 1), ui->train_dataset_path));

  ui->train_dataset_size->setText(
      ElidedText(std::to_string(dataset_size), ui->train_dataset_size));

  ui->open_train_button->setEnabled(true);
}

void MainWindow::on_train_dataset_error(const std::string &message) {
  ShowMessage(QMessageBox::Critical, QGuiApplication::applicationDisplayName(),
              tr("Ошибка при загрузке набора данных: %1").arg(message.c_str()));

  ui->open_train_button->setEnabled(true);
}

void MainWindow::on_test_dataset_loaded(std::string filename,
                                        std::size_t dataset_size) {
  ui->test_dataset_path->setText(ElidedText(
      filename.substr(filename.find_last_of('/') + 1), ui->test_dataset_path));

  ui->test_dataset_size->setText(
      ElidedText(std::to_string(dataset_size), ui->test_dataset_size));

  ui->open_test_button->setEnabled(true);
}

void MainWindow::on_test_dataset_error(const std::string &message) {
  ShowMessage(QMessageBox::Critical, QGuiApplication::applicationDisplayName(),
              tr("Ошибка при загрузке набора данных: %1").arg(message.c_str()));

  ui->open_test_button->setEnabled(true);
}

void MainWindow::on_clear_button_clicked() {
  ui->image_label->clearImage();
  ui->image_label->update_image_info();
}

void MainWindow::OnTestStart() { TestUiLock(); }

void MainWindow::OnTestProgress(std::size_t progress) {
  SetTestProgress(progress);
}

void MainWindow::OnTestEnd(NetworkTestMetrics metrics) {
  ui->acc_label->setText(QString::number(metrics.accuracy, 'f', 2) + " (" +
                         QString::number(metrics.accuracy_percent) + "%)");
  ui->precision_label->setText(QString::number(metrics.precision, 'f', 2));
  ui->recall_label->setText(QString::number(metrics.recall, 'f', 2));
  ui->f_measure_label->setText(QString::number(metrics.fscore, 'f', 2));
  ui->time_label->setText(
      QString::number(std::chrono::duration_cast<std::chrono::seconds>(
                          std::chrono::nanoseconds(metrics.time))
                          .count()) +
      " second(-s)");

  TestUiUnlock();
}

void MainWindow::OnTestError(const std::string &message) {
  ShowMessage(QMessageBox::Icon::Critical,
              QGuiApplication::applicationDisplayName(),
              tr("Ошибка при тестировании: %1").arg(message.c_str()));
  TestUiUnlock();
}

void MainWindow::OnTrainStart() {
  TrainUiLock();

  SetTrainEpochInformation(1, controller_->GetConfiguration().GetEpochs());
  SetNetworkInfo(controller_->GetSettings());
  ui->widget->graph()->data().clear();
  ui->widget->graph()->data().data()->clear();
  ui->widget->graph()->rescaleAxes();
  ui->widget->replot();
}

void MainWindow::OnTrainEpochProgress(std::size_t progress) {
  SetTrainProgress(progress);
}

void MainWindow::OnTrainEpochEnd(std::size_t epoch) {
  if (epoch > controller_->GetConfiguration().GetEpochs())
    epoch = controller_->GetConfiguration().GetEpochs();
  SetTrainEpochInformation(epoch, controller_->GetConfiguration().GetEpochs());
}

void MainWindow::OnTrainEpochTestStart() {
  auto str = ui->epoch_status->text();
  str.append(" (тестирование)");
  ui->epoch_status->setText(str);
}

void MainWindow::OnTrainEpochTestProgress(std::size_t progress) {
  SetTrainProgress(progress);
}

void MainWindow::OnTrainEpochTestEnd(std::size_t accuracy, std::size_t epoch) {
  ui->weights_accuracy->setText(QString::number(accuracy) + "%");
  ui->widget->graph(0)->addData(epoch, accuracy);
  ui->widget->graph(0)->rescaleAxes();
  ui->widget->replot();
}

void MainWindow::OnTrainEnd() {
  ShowMessage(QMessageBox::Icon::Information,
              QGuiApplication::applicationDisplayName(),
              tr("Обучение успешно завершено"));
  SetTrainEpochInformation(0, 0);
  SetTrainProgress(0);

  TrainUiUnlock();
}

void MainWindow::OnTrainError(const std::string &message) {
  ShowMessage(QMessageBox::Icon::Critical,
              QGuiApplication::applicationDisplayName(),
              tr("Ошибка при обучении: %1").arg(message.c_str()));

  TrainUiUnlock();
}

void MainWindow::SetTrainEpochInformation(std::size_t current_epoch,
                                          std::size_t total_epochs) {
  ui->epoch_status->setText(QString::fromStdString(
      std::to_string(current_epoch) + " / " + std::to_string(total_epochs)));
}

void MainWindow::SetTestProgress(std::size_t progress) {
  ui->testing_progress->setValue(progress);
}

void MainWindow::SetTrainProgress(std::size_t progress) {
  ui->learning_progress->setValue(progress);
}

void MainWindow::SetNetworkInfo(NetworkSettings settings, std::size_t epoch,
                                std::size_t accuracy) {
  ui->hidden_layers_count->setText(
      QString::number(settings.number_of_hidden_layers));
  ui->neurons_in_layer->setText(
      QString::number(settings.neurons_in_hidden_layer));
  static_cast<void>(epoch);
  if (accuracy != std::string::npos) {
    ui->weights_accuracy->setText(QString::number(accuracy) + "%");
  }
}

void MainWindow::TestUiLock() {
  ui->network_type->setEnabled(false);
  ui->save_weights->setEnabled(false);
  ui->load_weights->setEnabled(false);
  ui->open_train_button->setEnabled(false);
  ui->open_test_button->setEnabled(false);

  ui->tabWidget->tabBar()->setEnabled(false);
  ui->weight_mode->setEnabled(false);
  ui->cross_validation_mode->setEnabled(false);
  ui->sampe_part_box->setEnabled(false);
  ui->group_ammount_box->setEnabled(false);
  ui->begin_test_button->setEnabled(false);

  ui->end_test_button->setEnabled(true);
}

void MainWindow::TestUiUnlock() {
  ui->network_type->setEnabled(true);
  if (controller_->IsNetworkCreated()) ui->save_weights->setEnabled(true);
  ui->load_weights->setEnabled(true);
  ui->open_train_button->setEnabled(true);
  ui->open_test_button->setEnabled(true);

  ui->tabWidget->tabBar()->setEnabled(true);
  ui->weight_mode->setEnabled(true);
  ui->cross_validation_mode->setEnabled(true);
  ui->sampe_part_box->setEnabled(true);
  ui->group_ammount_box->setEnabled(true);
  ui->begin_test_button->setEnabled(true);

  ui->end_test_button->setEnabled(false);
}

void MainWindow::TrainUiLock() {
  ui->network_type->setEnabled(false);
  ui->save_weights->setEnabled(false);
  ui->load_weights->setEnabled(false);
  ui->open_train_button->setEnabled(false);
  ui->open_test_button->setEnabled(false);

  ui->tabWidget->tabBar()->setEnabled(false);
  ui->layers_slider->setEnabled(false);
  ui->epoch_box->setEnabled(false);
  ui->weight_saving_checkbox->setEnabled(false);
  ui->learning_speed->setEnabled(false);
  ui->begin_studying_box->setEnabled(false);

  ui->stop_studying_button->setEnabled(true);
}

void MainWindow::TrainUiUnlock() {
  ui->network_type->setEnabled(true);
  if (controller_->IsNetworkCreated()) ui->save_weights->setEnabled(true);
  ui->load_weights->setEnabled(true);
  ui->open_train_button->setEnabled(true);
  ui->open_test_button->setEnabled(true);

  ui->tabWidget->tabBar()->setEnabled(true);
  ui->layers_slider->setEnabled(true);
  ui->epoch_box->setEnabled(true);
  ui->weight_saving_checkbox->setEnabled(true);
  ui->learning_speed->setEnabled(true);
  ui->begin_studying_box->setEnabled(true);

  ui->stop_studying_button->setEnabled(false);
}

}  // namespace s21
