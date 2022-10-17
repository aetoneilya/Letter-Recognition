#include <gtest/gtest.h>

#include "model/model.h"

TEST(s21_graph_network, neuron_1) {
  s21::Neuron neuron;
  double out = 1;
  neuron.SetOutput(out);
  EXPECT_DOUBLE_EQ(neuron.GetOutput(), out);
}

TEST(s21_graph_network, neuron_2) {
  s21::Neuron input;
  s21::Neuron output;
  double in = 1;
  input.SetOutput(in);

  output.AddRandomWeight(&input);
  EXPECT_TRUE(output.GetConnections().find(&input) !=
              output.GetConnections().end());
  output.CalcOutput();
  double expected_output = s21::utility::ActivationFunc(
      output.GetConnections().at(&input) * input.GetOutput());
  EXPECT_DOUBLE_EQ(output.GetOutput(), expected_output);
}

TEST(s21_graph_network, neuron_3) {
  s21::Neuron input_1, input_2, input_3, input_4;
  s21::Neuron output;
  double in_1 = 1;
  double in_2 = -1;
  double in_3 = 0.5;
  double in_4 = -0.7;
  input_1.SetOutput(in_1);
  input_2.SetOutput(in_2);
  input_3.SetOutput(in_3);
  input_4.SetOutput(in_4);

  output.AddRandomWeight(&input_1);
  output.AddRandomWeight(&input_2);
  output.AddRandomWeight(&input_3);
  output.AddRandomWeight(&input_4);

  output.CalcOutput();
  std::map<s21::Neuron*, double> connections = output.GetConnections();
  double expected_output = s21::utility::ActivationFunc(
      connections.at(&input_1) * input_1.GetOutput() +
      connections.at(&input_2) * input_2.GetOutput() +
      connections.at(&input_3) * input_3.GetOutput() +
      connections.at(&input_4) * input_4.GetOutput());
  EXPECT_DOUBLE_EQ(output.GetOutput(), expected_output);
}

TEST(s21_graph_network, layer_constructor) {
  size_t size_1 = 1;
  size_t size_2 = 2;
  size_t size_3 = 3;
  std::unique_ptr<s21::Layer> input = std::make_unique<s21::Layer>(size_1);
  std::unique_ptr<s21::Layer> hidden =
      std::make_unique<s21::Layer>(size_2, input);
  std::unique_ptr<s21::Layer> output =
      std::make_unique<s21::Layer>(size_3, hidden);
  EXPECT_EQ(input->GetLayerType(), s21::LayerType::kInput);
  EXPECT_EQ(hidden->GetLayerType(), s21::LayerType::kHidden);
  EXPECT_EQ(output->GetLayerType(), s21::LayerType::kOutput);
  EXPECT_EQ(input->Neurons().size(), size_1);
  EXPECT_EQ(hidden->Neurons().size(), size_2);
  EXPECT_EQ(output->Neurons().size(), size_3);
}

TEST(s21_graph_network, s21_layer_set_output) {
  size_t size = 2;
  std::unique_ptr<s21::Layer> layer = std::make_unique<s21::Layer>(size);

  double exp_val = 1;
  std::vector<double> val(size, exp_val);
  layer->SetOutput(val);

  for (s21::Neuron neuron : layer->Neurons()) {
    EXPECT_EQ(neuron.GetOutput(), exp_val);
  }
}

TEST(s21_graph_network, layer_error) {
  size_t size = 3;
  std::unique_ptr<s21::Layer> input = std::make_unique<s21::Layer>(size);
  std::unique_ptr<s21::Layer> hidden =
      std::make_unique<s21::Layer>(size, input);
  std::unique_ptr<s21::Layer> output =
      std::make_unique<s21::Layer>(size, hidden);

  double exp_val = 1;
  std::vector<double> exp_values(size, exp_val);

  std::vector<double> out_error = output->Error(exp_values);
  for (size_t i = 0; i < out_error.size(); i++) {
    EXPECT_DOUBLE_EQ(out_error[i], -1);
  }
}

TEST(s21_graph_network, gn_forward_and_back_propagation) {
  s21::NetworkSettings settings;
  settings.neurons_in_input_layer = 2;
  settings.neurons_in_hidden_layer = 3;
  settings.neurons_in_output_layer = 1;
  settings.number_of_hidden_layers = 1;

  s21::GraphNetwork gn(settings);

  std::vector<double> weights({0.5, 0.3, 0.2, 0.1, 0.6, 0.4, 0.2, 0.3, 0.4});
  gn.LoadWeights(weights);
  gn.SetInput({1, 0.5});
  gn.ForwardPropagation();

  double exp_result = 0.64015681610605701;

  EXPECT_DOUBLE_EQ(gn.GetOutput()[0], exp_result);

  gn.BackPropagation({1}, 0.3);
  gn.ForwardPropagation();

  EXPECT_TRUE(gn.GetOutput()[0] > exp_result);
}

TEST(s21_matrix_network, mn_forward_and_back_propagation) {
  s21::NetworkSettings settings;
  settings.neurons_in_input_layer = 2;
  settings.neurons_in_hidden_layer = 3;
  settings.neurons_in_output_layer = 1;
  settings.number_of_hidden_layers = 1;

  s21::MatrixNetwork mn(settings);

  std::vector<double> weights({0.5, 0.3, 0.2, 0.1, 0.6, 0.4, 0.2, 0.3, 0.4});
  mn.LoadWeights(weights);

  mn.SetInput({1, 0.5});
  mn.ForwardPropagation();
  double exp_result = 0.64015681610605701;

  EXPECT_DOUBLE_EQ(mn.GetOutput()[0], exp_result);

  mn.BackPropagation({1}, 0.3);
  mn.ForwardPropagation();

  EXPECT_TRUE(mn.GetOutput()[0] > exp_result);
}

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
