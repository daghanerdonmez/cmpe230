#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "minesweepergame.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , score(0)
{
    //MinesweeperGame *game = new MinesweeperGame(this);

    ui->setupUi(this); // setup the ui elements

    ui->label_2->setText(QString::number(score)); // Display the initial score

     // Calculate the window size based on the number of rows and columns in the game, hand calculated
    int rowresolution = ROWS*20 + 80;
    int colresolution = COLS*20 + 25;

    this->resize(colresolution, rowresolution); // Resize the window

    // Connect the countRevealed signal from MinesweeperGame to the setScore slot in MainWindow
    connect(ui->widget, &MinesweeperGame::countRevealed, this, &MainWindow::setScore);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Function to increment the score
void MainWindow::incrementScore(){
    score++;
    ui->label_2->setText(QString::number(score));
}

// Slot to set the score to a new value
void MainWindow::setScore(int newScore){
    score = newScore;
    ui->label_2->setText(QString::number(score));
}

// Slot for handling push button click event (im not using rn)
void MainWindow::on_pushButton_clicked()
{

}

