#ifndef MINESWEEPERGAME_H
#define MINESWEEPERGAME_H

#include "buttonmodified.h"
#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QMouseEvent>
#include <QVector>

// Game settings
#define ROWS 32
#define COLS 32
#define MINES 50

// Main Minesweeper class inheriting from qwidget
class MinesweeperGame : public QWidget {
    Q_OBJECT

public:
    // Constructor and destructor
    explicit MinesweeperGame(QWidget *parent = nullptr, int rows = ROWS, int cols = COLS, int mines = MINES);
    ~MinesweeperGame();

public slots:
    // Slot to set up the game baord
    void setupGame();
    // Slot to provide hints
    void giveHint();

signals:
    // Signal to count the number of revealed cells
    void countRevealed(int count);

private:
    // Check if the player has won
    bool checkWinCondition();
    // End the game, with a flag indicating win or lose
    void gameOver(bool won);
    // Set an icon on a button based on the number of adjacent mines
    void setIconToButton(ButtonModified *button, int minesNum);
    // Debugging method to print the revealed grid to console
    void printRevealedGrid();
    // Count adjacent mines for a given cell
    int countAdjacentMines(int row, int col);
    // Handle left-click event on a cell
    void cellClicked(int row, int col);
    // Handle right-click event on a cell
    void handleRight(int row, int col);
    // Reveal neighboring cells recursively
    void neighbourCells(int row, int col);
    // Calculate the number of revealed cells
    int calculateRevealedCount();

    // Hint-related methods
    int canHint();
    void updateKnowns();
    void markKnownMines(int x, int y);
    void markKnownSafes(int x, int y);
    void executeHint(int row, int col);
    void revealHint(int row, int col);

    // Debugging methods to print known mines and safes (can delete later)
    void printKnownMinesGrid();
    void printKnownSafeGrid();

    // Utility methods for matrix operations
    QVector<QVector<bool>> copyMatrix(const QVector<QVector<bool>>& matrix);
    bool matricesAreEqual(const QVector<QVector<bool>>& a, const QVector<QVector<bool>>& b);

    // Layout and button grid
    QGridLayout *gridLayout;
    QVector<QVector<ButtonModified*>> buttons;
    // Game state matrices
    QVector<QVector<bool>> mines;
    QVector<QVector<bool>> revealed;
    QVector<QVector<bool>> flagged;
    QVector<QVector<bool>> visited;
    QVector<QVector<bool>> knownSafes;
    QVector<QVector<bool>> knownMines;

    // Game settings
    int rows, cols, mineCount, state;
    // Hint state variables
    bool hintActive;
    int hintCoord;
};

#endif // MINESWEEPERGAME_H
