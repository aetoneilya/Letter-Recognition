# Multilayer Perceptron

Implementation of a basic multilayer perceptron.


You need to implement the MultilayerPerceptron project:

The program must be developed in C++ language of C++17 standard
The program code must be located in the src folder
When writing code it is necessary to follow the Google style
Prepare full coverage of modules related to calculations with unit-tests using the GTest library
The program must be built with Makefile which contains standard set of targets for GNU-programs: all, install, uninstall, clean, dvi, dist, tests. Installation directory should be arbitrary, except the building one
GUI implementation, based on any GUI library with API for C++17: Qt, SFML, GTK+, Nanogui, Nngui, etc.
The program must be implemented using the MVC pattern, and also:

there should be no business code in the view code
there should be no interface code in the controller and the model
controllers must be thin


The program should provide the ability to form and train neural network models to classify handwritten Latin letters
The perceptron must:

classify images with handwritten letters of the Latin alphabet
have from 2 to 5 hidden layers
use a sigmoid activation function for each hidden layer
be able to learn on an open dataset (e.g. EMNIST-letters presented in the datasets directory). The ratio of the test sample to the training one should be no more than 2:8, i.e. the test sample makes no more than 20% of the initial dataset
show accuracy on a test sample over 70 percent

be trained using the backpropagation method


It is not allowed to train the perceptron on the test sample
-Implement the perceptron in two ways:

in matrix form (all layers are represented as weight matrices)
in graph form (each neuron is represented as some node object connected to other nodes by refs)


The input data must be normalized (by size and color) before neural network execution, in order to match the format of the emnist sample;
The interface of the program must provide the ability to:

run the experiment on the test sample or on a part of it, given by a floating point number between 0 and 1 (where 0 is the empty sample - the degenerate situation, and 1 is the whole test sample). After the experiment, there should be an average accuracy, precision, recall, f-measure and total time spent on the experiment displayed on the screen
load BMP images (image size can be up to 512x512) with Latin letters and classify them
draw two-color square images by hand in a separate window
start the real-time training process for a user-defined number of epochs with displaying the error control values for each training epoch. Make a report as a graph of the error change calculated on the test sample for each training epoch
run the training process using cross-validation for a given number of groups k

switch perceptron implementation (matrix or graph)
switch the number of perceptron hidden layers (from 2 to 5)
save to a file and load weights of perceptron from a file
