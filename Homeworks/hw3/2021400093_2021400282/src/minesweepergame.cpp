#include "MinesweeperGame.h"
#include "buttonmodified.h"
#include <QRandomGenerator>
#include <QMessageBox>
#include <iostream>
#include <QPixmap>
#include <QIcon>
#include <QDir>

// Constructor to initialize the game with given rows, cols, and mines
MinesweeperGame::MinesweeperGame(QWidget *parent, int rows, int cols, int mines)
    : QWidget(parent), rows(rows), cols(cols), mineCount(mines) {
    gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(0);
    setupGame();

    // Set the minimum size of the window
    this->setMinimumSize(15*cols, 15*rows + 40);
    gridLayout->setGeometry(QRect(0,0,15*cols, 15*rows));
}

MinesweeperGame::~MinesweeperGame() {
    // destructor. nothing special to clean up
}

// Function to set icons for buttons based on minesNum
void MinesweeperGame::setIconToButton(ButtonModified* button, int minesNum) {
    if (button == NULL) {
        qDebug() << "Button was not found!";
        return;
    }

    // Set icons based on the value of minesNum
    switch (minesNum) {
    case -1: // Flag
    {
        QPixmap pixmap("../../../../../assets/flag.png");
        pixmap = pixmap.scaled(20, 20);
        QIcon ButtonIcon(pixmap);
        button->setIcon(ButtonIcon);
        button->setIconSize(QSize(20,20));
        break;
    }
    case -2: // Clear icon
    {
        button->setIcon(QIcon());
        break;
    }
    case -3: // Mine
    {
        QPixmap pixmap("../../../../../assets/mine.png");
        pixmap = pixmap.scaled(20, 20);
        QIcon ButtonIcon(pixmap);
        button->setIcon(ButtonIcon);
        button->setIconSize(QSize(20,20));
        disconnect(button, &QPushButton::clicked, this, nullptr);
        break;
    }
    case -4: // Empty
    {
        QPixmap pixmap("../../../../../assets/empty.png");
        pixmap = pixmap.scaled(20, 20);
        QIcon ButtonIcon(pixmap);
        button->setIcon(ButtonIcon);
        button->setIconSize(QSize(20,20));
        break;
    }
    case -5 :// Num of mines
    {
        QPixmap pixmap("../../../../../assets/hint.png");
        pixmap = pixmap.scaled(20, 20);
        QIcon ButtonIcon(pixmap);
        button->setIcon(ButtonIcon);
        button->setIconSize(QSize(20,20));
        break;
    }
    default: //default checks the adjacent mies
        if (minesNum >= 0 && minesNum <= 8) {
            QPixmap pixmap(QString("../../../../../assets/%1.png").arg(minesNum));
            pixmap = pixmap.scaled(20, 20);
            QIcon ButtonIcon(pixmap);
            button->setIcon(ButtonIcon);
            button->setIconSize(QSize(20,20));
            disconnect(button, &QPushButton::clicked, this, nullptr);
        }
        break;
    }
}

// Set up the game grid and initialize buttons and mines
void MinesweeperGame::setupGame() {
    // Clear existing grid if re-setting the game
    QLayoutItem* item;
    while ((item = gridLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    // Clear the existing grid layout
    buttons.clear();
    mines.clear();
    revealed.clear();
    flagged.clear();
    visited.clear();
    knownMines.clear();
    knownSafes.clear();

    state = 1;

    gridLayout->setSpacing(0);         // No space between buttons
    gridLayout->setContentsMargins(0, 0, 0, 0); // No margins around the grid

    // Initialize the vectors with the grid dimensions
    buttons.resize(rows, QVector<ButtonModified*>(cols));
    mines.resize(rows, QVector<bool>(cols, false));
    revealed.resize(rows, QVector<bool>(cols, false));
    flagged.resize(rows, QVector<bool>(cols, false));
    visited.resize(rows, QVector<bool>(cols, false));
    knownMines.resize(rows, QVector<bool>(cols, false));
    knownSafes.resize(rows, QVector<bool>(cols, false));


    hintActive = false; //is there an active hint given on the board (a green box)
    hintCoord = 0; //coords of that green box

    // Create a visited matrix
    //for (int i=0; i<10; i++) {
    //    for (int j=0; j<10; j++) {
    //        visited[i][j] = false;
    //    }
    //}

    // Randomly place mines
    int minesPlaced = 0;
    while (minesPlaced < mineCount) {
        int row = QRandomGenerator::global()->bounded(rows);
        int col = QRandomGenerator::global()->bounded(cols);
        if (!mines[row][col]) {
            mines[row][col] = true;
            minesPlaced++;
        }
    }

    // Create buttons and add them to the grid layout
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            ButtonModified* button = new ButtonModified(this);
            setIconToButton(button, -4);  // Initialize all buttons with the empty icon
            button->setFixedSize(20, 20);  // Set button size
            button->setObjectName(QStringLiteral("button_%1_%2").arg(row).arg(col));  // Name each button for easier access
            button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
            button->setStyleSheet("QPushButton { margin: 0px; padding: 0px; border: none; }");
            gridLayout->addWidget(button, row, col);
            buttons[row][col] = button;

            connect(button, &QPushButton::clicked, [this, row, col]() {
                this->cellClicked(row, col);
            });
            connect(button, &ButtonModified::rightClicked, [this, row, col]() {
                this->handleRight(row, col);
            });
        }
    }

    // Update the layout and the parent widget
    this->setLayout(gridLayout);
    this->adjustSize();  // Adjust size to fit the new layout
    emit countRevealed(0);

    //printRevealedGrid();
}

void MinesweeperGame::cellClicked(int row, int col) {
    if(state == 0){
        return;
    }

    //qDebug() << "Cell clicked at: " << row << ", " << col;

    // Check if the cell is already revealed or flagged
    if (revealed[row][col]) {
        qDebug() << "Cell already revealed or flagged.";
        return;
    }

    // Reveal the cell
    revealed[row][col] = true;

    // Check if the cell contains a mine
    if (mines[row][col]) {
        qDebug() << "Mine hit!";
        setIconToButton(buttons[row][col], -3);
        emit countRevealed(calculateRevealedCount());
        gameOver(false);  // Trigger game over
    } else {
        // Calculate the number of adjacent mines
        int count = countAdjacentMines(row, col);
        setIconToButton(buttons[row][col], count);  // Update the button's icon
        if (count == 0) {
            // Optionally reveal adjacent cells if there are no adjacent mines
            //revealAdjacentCells(row, col);
            neighbourCells(row,col);
        }
    }

    // Check for win condition
    if (checkWinCondition()) {
        emit countRevealed(calculateRevealedCount());
        gameOver(true);  // Player wins the game
    }

    // Debug output the grid state
    //printRevealedGrid();

    emit countRevealed(calculateRevealedCount());
}

void MinesweeperGame::handleRight(int row, int col) {
    if(state == 0){
        return;
    }

    ButtonModified* button = buttons[row][col];

    if(revealed[row][col]){
        return;
    }

    bool flag = flagged[row][col];

    if (!flag) {
        setIconToButton(button, -1);
        disconnect(button, &QPushButton::clicked, this, nullptr); // Flag the cell
    }

    else {
        setIconToButton(button, -4); // Unflag the cell
        connect(button, &QPushButton::clicked, [this, row, col]() {
            this->cellClicked(row, col);
        });
    }

    flagged[row][col] = !flag;
}

int MinesweeperGame::countAdjacentMines(int row, int col) {
    int count = 0;
    // Loop over each of the nine blocks surrounding and including (row, col)
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int newRow = row + i;
            int newCol = col + j;
            // Check if the new row and col are within the bounds of the grid
            if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols) {
                // Check if this cell contains a mine
                if (mines[newRow][newCol]) {
                    count++;
                }
            }
        }
    }
    return count;
}

bool MinesweeperGame::checkWinCondition() {
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            // If there's a cell that doesn't contain a mine and hasn't been revealed, return false.
            if (!mines[row][col] && !revealed[row][col]) {
                return false;
            }
        }
    }
    return true;  // All non-mine cells have been revealed, player wins.
}

// Reveal adjacent cells if no adjacent mines
void MinesweeperGame::neighbourCells(int row, int col){
    if (visited[row][col] == false) {
        visited[row][col] = true;
        int minesNum = countAdjacentMines(row, col);
        setIconToButton(buttons[row][col], minesNum);
        revealed[row][col] = true;

        if (minesNum == 0) {
            for (int i=-1; i<2; i++) {
                for (int j=-1; j<2; j++) {
                    int newrow = row+i;
                    int newcol = col+j;

                    if (newrow >= 0 && newrow < rows && newcol >= 0 && newcol < cols) {
                        if (i != 0 || j != 0) {
                            neighbourCells(newrow, newcol);

                        }
                    }
                }
            }
        }
    }
}

// Calculate the number of revealed cells
int MinesweeperGame::calculateRevealedCount(){
    int count = 0;
    for (int row = 0; row < rows; row++){
        for (int col = 0; col < cols; col++){
            if (revealed[row][col] == true && !mines[row][col]){
                count++;
            }
        }
    }

    return count;
}

// Handle game over logic
void MinesweeperGame::gameOver(bool won) {
    // Handle game over logic
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            if(mines[row][col]){
                setIconToButton(buttons[row][col], -3);
            }
        }
    }
    state = 0;
    QMessageBox::information(this, "Game Over", won ? "You win!" : "You lose!");
}

void MinesweeperGame::printRevealedGrid() {
    std::cout << "\nRevealed Grid State:\n";
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            if (mines[row][col]) {
                std::cout << "* ";  // Mine
            } else if (flagged[row][col]) {
                std::cout << "F ";  // Flagged
            } else if (revealed[row][col]) {
                std::cout << "R ";  // Revealed
            } else {
                std::cout << ". ";  // Unrevealed
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

// Print the known mines grid state for debugging
void MinesweeperGame::printKnownMinesGrid() {
    std::cout << "\nKnown Mines State:\n";
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            if (knownMines[row][col]) {
                std::cout << "* ";  // Mine
            } else {
                std::cout << "_ ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

// Print the known safe grid state for debugging
void MinesweeperGame::printKnownSafeGrid() {
    std::cout << "\nKnown Safe State:\n";
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            if (knownSafes[row][col]) {
                std::cout << "S ";  // Safe
            } else {
                std::cout << "_ ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}



// Function to copy a matrix
QVector<QVector<bool>> MinesweeperGame::copyMatrix(const QVector<QVector<bool>>& matrix) {
    return matrix;  // QVector's copy constructor does a deep copy
}

// Function to compare two matrices
bool MinesweeperGame::matricesAreEqual(const QVector<QVector<bool>>& a, const QVector<QVector<bool>>& b) {
    if (a.size() != b.size()) return false;
    for (int i = 0; i < a.size(); ++i) {
        if (a[i].size() != b[i].size()) return false;
        for (int j = 0; j < a[i].size(); ++j) {
            if (a[i][j] != b[i][j]) return false;
        }
    }
    return true;
}

void MinesweeperGame::giveHint() {

    if (hintActive){ // If a hint is already active, reveal the hinted cell
        cellClicked(hintCoord/COLS, hintCoord%COLS);
        hintActive = false;
    }else {
        //printf("en baş\n");
        // Try to find a cell that can be hinted at
        int a = canHint();
        if (a != -1) {
            executeHint(a/COLS, a%COLS);
            hintActive = true;
            hintCoord = a;
        } else {
            // Update known mines and safe cells and try to find a hintable cell again
            updateKnowns();
            a = canHint();
            if (a != -1) {
                executeHint(a/COLS, a%COLS);
                hintActive = true;
                hintCoord = a;
            } else {
                //printf("bulamadım\n");
                // No hintable cells found
            }
        }
    }
}

// Function to execute a hint by setting a hint icon
void MinesweeperGame::executeHint(int row, int col) {
    setIconToButton(buttons[row][col], -5);
}

// Function to reveal a hint for a cell (currently not used)
void MinesweeperGame::revealHint(int row, int col) {
    int count = countAdjacentMines(row, col);
    setIconToButton(buttons[row][col], count);
}

// Function to check if there are any safe cells that can be hinted at
int MinesweeperGame::canHint() {
    std::vector<int> canHints; // Vector to store the indices of hintable cells
    int canHintsSize = 0; // Size of the canHints vector
    bool found = false; // Flag to indicate if any hintable cells are found

    // Loop through all the cells in the game board
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            // Check if the cell is not revealed, is known to be safe, and is not a mine
            if (!revealed[i][j] && knownSafes[i][j] && !mines[i][j]) {
                canHints.push_back(i * COLS + j); // Add the cell index to the canHints vector
                canHintsSize++; // Increment the size of the canHints vector
                found = true; // Set the found flag to true
            }
        }
    }

    // If any hintable cells are found, return a random hintable cell index
    if (found) {
        int index = QRandomGenerator::global()->bounded(canHintsSize); // Generate a random index within the canHintsSize
        return canHints[index]; // Return the randomly selected hintable cell index
    }
    return -1; // If no hintable cells are found, return -1
}


// Function to update known mines and safe cells
void MinesweeperGame::updateKnowns() {

    while (true) {
        // Create copies of the current known mines and safe cells
        QVector<QVector<bool>> oldKnownMines = copyMatrix(knownMines);
        QVector<QVector<bool>> oldKnownSafes = copyMatrix(knownSafes);

        // Loop through all cells in the grid
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                // If the cell is revealed and has adjacent mines
                if (revealed[i][j] && countAdjacentMines(i, j) != 0) {
                    // Mark known mines and safe cells around this cell
                    markKnownMines(i, j);
                    markKnownSafes(i, j);
                }
            }
        }

        // Break the loop if no changes were made to known mines or safe cells
        if (matricesAreEqual(oldKnownMines, knownMines) && matricesAreEqual(oldKnownSafes, knownSafes)) {
            break;
        }
    }
}

// Function to mark known mines around a revealed cell
void MinesweeperGame::markKnownMines(int x, int y) {
    int minesNum = countAdjacentMines(x, y); // Number of adjacent mines
    int unsureCount = 0; // Counter for unsure cells
    int unsures[8][2]; // Array to store unsure cells' coordinates

    // Loop through the adjacent cells
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            int newx = x + i;
            int newy = y + j;

            // Check if the new coordinates are within bounds
            if (newx >= 0 && newx < COLS && newy >= 0 && newy < ROWS) {
                // Check if the cell is not revealed and not known to be safe
                if (!revealed[newx][newy] && !knownSafes[newx][newy]) {
                    unsures[unsureCount][0] = newx;
                    unsures[unsureCount][1] = newy;
                    unsureCount++;
                }
            }
        }
    }

    // If the number of unsure cells equals the number of adjacent mines, mark them as known mines
    if (minesNum == unsureCount) {
        for (int i = 0; i < unsureCount; i++) {
            int nX = unsures[i][0];
            int nY = unsures[i][1];
            knownMines[nX][nY] = true;
        }
    }

    // Print the known mines grid for debugging
    //printKnownMinesGrid();
}

// Function to mark known safe cells around a revealed cell
void MinesweeperGame::markKnownSafes(int x, int y) {
    int minesNum = countAdjacentMines(x, y); // Number of adjacent mines
    int unsureCount = 0; // Counter for unsure cells
    int sureMineCount = 0; // Counter for known mines
    int unsures[8][2]; // Array to store unsure cells' coordinates

    // Loop through the adjacent cells
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            int newx = x + i;
            int newy = y + j;

            // Check if the new coordinates are within bounds
            if (newx >= 0 && newx < COLS && newy >= 0 && newy < ROWS) {
                // Check if the cell is not revealed
                if (!revealed[newx][newy]) {
                    if (!knownMines[newx][newy]) {
                        unsures[unsureCount][0] = newx;
                        unsures[unsureCount][1] = newy;
                        unsureCount++;
                    } else {
                        sureMineCount++;
                    }
                }
            }
        }
    }

    // If the number of adjacent mines equals the number of known mines, mark the unsure cells as safe
    if (minesNum == sureMineCount) {
        for (int i = 0; i < unsureCount; i++) {
            int nX = unsures[i][0];
            int nY = unsures[i][1];
            knownSafes[nX][nY] = true;
        }
    }

    // Print the known safe grid for debugging
    //printKnownSafeGrid();
}
