#include <iostream>
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <limits>

using namespace std;

class Figure {
protected:
    int x;
    int y;
public:
    Figure(int x_coordinate, int y_coordinate) : x(x_coordinate), y(y_coordinate) {}
    virtual void draw(std::vector<std::vector<char>>& grid) = 0;
    virtual std::string info() = 0;
    virtual ~Figure() = default;

    virtual bool isWithinBoard(int boardWidth, int boardHeight) = 0;

    virtual std::string getParameters() = 0;

    virtual bool isEqual(const Figure& other) const = 0; // Added this line
};

class Circle : public Figure {
protected:
    int radius;
public:
    Circle(int x_coordinate, int y_coordinate, int radius_circle)
        : Figure(x_coordinate, y_coordinate), radius(radius_circle) {}

    void draw(std::vector<std::vector<char>>& grid) override {
        int grid_height = grid.size();
        int grid_width = grid[0].size();
        for (int i = 0; i < grid_height; i++) {
            for (int j = 0; j < grid_width; ++j) {
                double dist = sqrt(pow(j - x, 2) + pow(i - y, 2));
                if (abs(dist - radius) < 0.5) {
                    grid[i][j] = 'O';
                }
            }
        }
    }

    std::string info() override {
        return "Circle " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(radius);
    }

    bool isWithinBoard(int boardWidth, int boardHeight) override {
        return (x + radius >= 0 && x - radius < boardWidth && y + radius >= 0 && y - radius < boardHeight);
    }

    std::string getParameters() override {
        return "Circle " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(radius);
    }

    bool isEqual(const Figure& other) const override {
        const Circle* otherCircle = dynamic_cast<const Circle*>(&other);
        if (otherCircle == nullptr) return false;
        return x == otherCircle->x && y == otherCircle->y && radius == otherCircle->radius;
    }
};

class Rectangle : public Figure {
protected:
    int width;
    int height;
public:
    Rectangle(int x_coordinate, int y_coordinate, int width_rect, int height_rect)
        : Figure(x_coordinate, y_coordinate), width(width_rect), height(height_rect) {}

    void draw(std::vector<std::vector<char>>& grid) override {
        int grid_height = grid.size();
        int grid_width = grid[0].size();

        for (int j = x; j < x + width && j < grid_width; ++j) {
            if (y >= 0 && y < grid_height && j >= 0) {
                grid[y][j] = '#';
            }
            int bottomY = y + height - 1;
            if (bottomY >= 0 && bottomY < grid_height && j >= 0) {
                grid[bottomY][j] = '#';
            }
        }

        for (int i = y; i < y + height && i < grid_height; ++i) {
            if (x >= 0 && x < grid_width && i >= 0) {
                grid[i][x] = '#';
            }
            int rightX = x + width - 1;
            if (rightX >= 0 && rightX < grid_width && i >= 0) {
                grid[i][rightX] = '#';
            }
        }
    }

    std::string info() override {
        return "Rectangle " + std::to_string(x) + " " + std::to_string(y) + " "
            + std::to_string(width) + " " + std::to_string(height);
    }

    bool isWithinBoard(int boardWidth, int boardHeight) override {
        return (x < boardWidth && x + width > 0 && y < boardHeight && y + height > 0);
    }

    std::string getParameters() override {
        return "Rectangle " + std::to_string(x) + " " + std::to_string(y) + " "
            + std::to_string(width) + " " + std::to_string(height);
    }

    bool isEqual(const Figure& other) const override {
        const Rectangle* otherRect = dynamic_cast<const Rectangle*>(&other);
        if (otherRect == nullptr) return false;
        return x == otherRect->x && y == otherRect->y && width == otherRect->width && height == otherRect->height;
    }
};

class Line : public Figure {
protected:
    int length;
    char direction;
public:
    Line(int x_coordinate, int y_coordinate, int length_line, char dir)
        : Figure(x_coordinate, y_coordinate), length(length_line), direction(dir) {}

    void draw(std::vector<std::vector<char>>& grid) override {
        int grid_height = grid.size();
        int grid_width = grid[0].size();

        if (direction == 'H') {
            for (int j = x; j < x + length && j < grid_width; ++j) {
                if (y >= 0 && y < grid_height && j >= 0) {
                    grid[y][j] = '-';
                }
            }
        }
        else if (direction == 'V') {
            for (int i = y; i < y + length && i < grid_height; ++i) {
                if (x >= 0 && x < grid_width && i >= 0) {
                    grid[i][x] = '|';
                }
            }
        }
    }

    std::string info() override {
        return "Line " + std::to_string(x) + " " + std::to_string(y) + " " +
            std::to_string(length) + " " + direction;
    }

    bool isWithinBoard(int boardWidth, int boardHeight) override {
        if (direction == 'H') {
            return (x < boardWidth && x + length > 0 && y >= 0 && y < boardHeight);
        }
        else if (direction == 'V') {
            return (y < boardHeight && y + length > 0 && x >= 0 && x < boardWidth);
        }
        return false;
    }

    std::string getParameters() override {
        return "Line " + std::to_string(x) + " " + std::to_string(y) + " " +
            std::to_string(length) + " " + direction;
    }

    bool isEqual(const Figure& other) const override {
        const Line* otherLine = dynamic_cast<const Line*>(&other);
        if (otherLine == nullptr) return false;
        return x == otherLine->x && y == otherLine->y && length == otherLine->length && direction == otherLine->direction;
    }
};

class Triangle : public Figure {
protected:
    int height;
public:
    Triangle(int x_coordinate, int y_coordinate, int height_triangle)
        : Figure(x_coordinate, y_coordinate), height(height_triangle) {}

    void draw(std::vector<std::vector<char>>& grid) override {
        int grid_height = grid.size();
        int grid_width = grid[0].size();

        for (int i = 0; i < height; ++i) {
            int row = y + i;
            if (row < 0 || row >= grid_height) continue;

            int leftCol = x - i;
            int rightCol = x + i;

            if (leftCol >= 0 && leftCol < grid_width) {
                grid[row][leftCol] = '*';
            }
            if (rightCol >= 0 && rightCol < grid_width) {
                grid[row][rightCol] = '*';
            }
        }

        int baseRow = y + height - 1;
        for (int col = x - height + 1; col <= x + height - 1; ++col) {
            if (baseRow >= 0 && baseRow < grid_height && col >= 0 && col < grid_width) {
                grid[baseRow][col] = '*';
            }
        }
    }

    std::string info() override {
        return "Triangle " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(height);
    }

    bool isWithinBoard(int boardWidth, int boardHeight) override {
        return (x - height + 1 < boardWidth && x + height - 1 >= 0 && y < boardHeight && y + height > 0);
    }

    std::string getParameters() override {
        return "Triangle " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(height);
    }

    bool isEqual(const Figure& other) const override {
        const Triangle* otherTriangle = dynamic_cast<const Triangle*>(&other);
        if (otherTriangle == nullptr) return false;
        return x == otherTriangle->x && y == otherTriangle->y && height == otherTriangle->height;
    }
};

class Blackboard {
protected:
    const int BOARD_WIDTH = 80;
    const int BOARD_HEIGHT = 25;
    std::vector<std::vector<char>> grid;
    std::vector<std::unique_ptr<Figure>> figures;

public:
    Blackboard() : grid(BOARD_HEIGHT, std::vector<char>(BOARD_WIDTH, ' ')) {}

    void print() {
        for (auto& row : grid) {
            for (char c : row) {
                std::cout << c;
            }
            std::cout << "\n";
        }
    }

    void clear() {
        grid.assign(BOARD_HEIGHT, std::vector<char>(BOARD_WIDTH, ' '));
    }

    void clearAll() {
        clear();
        figures.clear();
    }

    void addFigure(std::unique_ptr<Figure> figure) {

        if (!figure->isWithinBoard(BOARD_WIDTH, BOARD_HEIGHT)) {
            std::cout << "Shape is out of board bounds or too big. Cannot add it.\n";
            return;
        }

        for (const auto& existingFigure : figures) {
            if (existingFigure->isEqual(*figure)) {
                std::cout << "Shape with the same parameters already exists at the same position.\n";
                return;
            }
        }

        figure->draw(grid);
        figures.push_back(std::move(figure));
    }

    void removeLastFigure() {
        if (!figures.empty()) {
            figures.pop_back();
            redraw();
        } else {
            std::cout << "No shapes to remove.\n";
        }
    }

    void save(const std::string& filePath) {
        std::ofstream file(filePath);
        if (!file) {
            std::cerr << "Error saving to file.\n";
            return;
        }
        for (const auto& figure : figures) {
            file << figure->info() << std::endl;
        }
    }

    void load(const std::string& filePath) {
        clearAll();
        std::ifstream file(filePath);
        if (!file) {
            std::cerr << "Error loading from file.\n";
            return;
        }
        std::string line;
        while (getline(file, line)) {
            std::istringstream ss(line);
            std::string figureType;
            ss >> figureType;
            if (figureType == "Circle") {
                int x, y, radius;
                ss >> x >> y >> radius;
                if (ss.fail()) {
                    std::cerr << "Invalid Circle data in file.\n";
                    clearAll();
                    return;
                }
                addFigure(std::make_unique<Circle>(x, y, radius));
            }
            else if (figureType == "Rectangle") {
                int x, y, width, height;
                ss >> x >> y >> width >> height;
                if (ss.fail()) {
                    std::cerr << "Invalid Rectangle data in file.\n";
                    clearAll();
                    return;
                }
                addFigure(std::make_unique<Rectangle>(x, y, width, height));
            }
            else if (figureType == "Triangle") {
                int x, y, height;
                ss >> x >> y >> height;
                if (ss.fail()) {
                    std::cerr << "Invalid Triangle data in file.\n";
                    clearAll();
                    return;
                }
                addFigure(std::make_unique<Triangle>(x, y, height));
            }
            else if (figureType == "Line") {
                int x, y, length;
                char dir;
                ss >> x >> y >> length >> dir;
                if (ss.fail() || (dir != 'H' && dir != 'V')) {
                    std::cerr << "Invalid Line data in file.\n";
                    clearAll();
                    return;
                }
                addFigure(std::make_unique<Line>(x, y, length, dir));
            }
            else {
                std::cerr << "Unknown shape type in file.\n";
                clearAll();
                return;
            }
        }
        redraw();
    }

    void listAvailableShapes() {
        std::cout << "Available shapes and usage:\n";
        std::cout << "- circle x y radius\n";
        std::cout << "- rectangle x y width height\n";
        std::cout << "- line x y length direction(H/V)\n";
        std::cout << "- triangle x y height\n";
    }

    void listFigures() {
        if (figures.empty()) {
            std::cout << "No shapes on the board.\n";
            return;
        }
        std::cout << "Figures on the board:\n";
        for (size_t i = 0; i < figures.size(); ++i) {
            std::cout << i + 1 << ". " << figures[i]->info() << "\n";
        }
    }

private:
    void redraw() {
        clear();
        for (const auto& figure : figures) {
            figure->draw(grid);
        }
    }
};
void handleAddCommand(Blackboard& board);
void handleDrawCommand(Blackboard& board);
void handleUndoCommand(Blackboard& board);
void handleClearCommand(Blackboard& board);
void handleSaveCommand(Blackboard& board);
void handleLoadCommand(Blackboard& board);
void handleShapesCommand(Blackboard& board);
void handleListCommand(Blackboard& board);
void handleHelpCommand();

int main() {
    Blackboard board;
    string command;
    cout << "Welcome to the Blackboard application. Type 'help' to see available commands.\n";
    while (true) {
        cout << "> ";
        cin >> command;
        if (command == "draw") {
            handleDrawCommand(board);
        }
        else if (command == "add") {
            handleAddCommand(board);
        }
        else if (command == "undo") {
            handleUndoCommand(board);
        }
        else if (command == "clear") {
            handleClearCommand(board);
        }
        else if (command == "save") {
            handleSaveCommand(board);
        }
        else if (command == "load") {
            handleLoadCommand(board);
        }
        else if (command == "shapes") {
            handleShapesCommand(board);
        }
        else if (command == "list") {
            handleListCommand(board);
        }
        else if (command == "help") {
            handleHelpCommand();
        }
        else if (command == "exit") {
            cout << "Exiting the application. Goodbye!\n";
            break;
        }
        else {
            cout << "Unknown command. Type 'help' to see available commands.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    return 0;
}

void handleAddCommand(Blackboard& board) {
    string shapeType;
    cin >> shapeType;
    if (shapeType == "circle") {
        int x, y, radius;
        cin >> x >> y >> radius;
        if (cin.fail() || radius <= 0) {
            cout << "Invalid parameters for Circle. Usage: add circle x y radius\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }
        board.addFigure(make_unique<Circle>(x, y, radius));
    }
    else if (shapeType == "rectangle") {
        int x, y, width, height;
        cin >> x >> y >> width >> height;
        if (cin.fail() || width <= 0 || height <= 0) {
            cout << "Invalid parameters for Rectangle. Usage: add rectangle x y width height\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }
        board.addFigure(make_unique<Rectangle>(x, y, width, height));
    }
    else if (shapeType == "line") {
        int x, y, length;
        char dir;
        cin >> x >> y >> length >> dir;
        if (cin.fail() || length <= 0 || (dir != 'H' && dir != 'V')) {
            cout << "Invalid parameters for Line. Usage: add line x y length direction(H/V)\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }
        board.addFigure(make_unique<Line>(x, y, length, dir));
    }
    else if (shapeType == "triangle") {
        int x, y, height;
        cin >> x >> y >> height;
        if (cin.fail() || height <= 0) {
            cout << "Invalid parameters for Triangle. Usage: add triangle x y height\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }
        board.addFigure(make_unique<Triangle>(x, y, height));
    }
    else {
        cout << "Unknown shape type: " << shapeType << ". Type 'shapes' to see available shapes.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void handleDrawCommand(Blackboard& board) {
    board.print();
}

void handleUndoCommand(Blackboard& board) {
    board.removeLastFigure();
}

void handleClearCommand(Blackboard& board) {
    board.clearAll();
    cout << "Blackboard cleared.\n";
}

void handleSaveCommand(Blackboard& board) {
    string filePath;
    cin >> filePath;
    board.save(filePath);
}

void handleLoadCommand(Blackboard& board) {
    string filePath;
    cin >> filePath;
    board.load(filePath);
}

void handleShapesCommand(Blackboard& board) {
    board.listAvailableShapes();
}

void handleListCommand(Blackboard& board) {
    board.listFigures();
}

void handleHelpCommand() {
    cout << "Available commands:\n";
    cout << "- add [shape]: Adds a shape to the blackboard. Type 'shapes' to see available shapes and usage.\n";
    cout << "- draw: Prints the current blackboard.\n";
    cout << "- undo: Removes the last added shape.\n";
    cout << "- clear: Clears the blackboard.\n";
    cout << "- save [filename]: Saves the current blackboard to a file.\n";
    cout << "- load [filename]: Loads the blackboard from a file.\n";
    cout << "- shapes: Lists available shapes and their usage.\n";
    cout << "- list: Lists the shapes currently on the blackboard.\n";
    cout << "- exit: Exits the application.\n";
}

