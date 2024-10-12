#include <iostream>
#include <memory>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <cctype>

using namespace std;

class Figure {
protected:
    int x;
    int y;
    std::string color;
    bool filled;
    int id;

public:
    Figure(int x_coordinate, int y_coordinate, const std::string& color_value, bool is_filled)
        : x(x_coordinate), y(y_coordinate), color(color_value), filled(is_filled), id(0) {}

    virtual void draw(std::vector<std::vector<char>>& grid) = 0;
    virtual std::string info() = 0;
    virtual ~Figure() = default;
    virtual bool isWithinBoard(int boardWidth, int boardHeight) = 0;
    virtual bool isEqual(const Figure& other) const = 0;
    virtual bool containsPoint(int px, int py) const = 0;

    int getId() const { return id; }
    void setId(int newId) { id = newId; }
    void setPosition(int newX, int newY) { x = newX; y = newY; }
    void setColor(const std::string& newColor) { color = newColor; }
};

class Circle : public Figure {
protected:
    int radius;

public:
    Circle(int x_coordinate, int y_coordinate, int radius_circle, const std::string& color_value, bool is_filled)
        : Figure(x_coordinate, y_coordinate, color_value, is_filled), radius(radius_circle) {}

    void draw(std::vector<std::vector<char>>& grid) override {
        char drawChar = color.empty() ? 'O' : tolower(color[0]);
        int grid_height = grid.size();
        int grid_width = grid[0].size();
        for (int i = 0; i < grid_height; i++) {
            for (int j = 0; j < grid_width; ++j) {
                double dist = sqrt(pow(j - x, 2) + pow(i - y, 2));
                if (filled) {
                    if (dist <= radius) {
                        grid[i][j] = drawChar;
                    }
                } else {
                    if (abs(dist - radius) < 0.5) {
                        grid[i][j] = drawChar;
                    }
                }
            }
        }
    }

    std::string info() override {
        return "Circle " + std::to_string(id) + " " + std::to_string(x) + " " + std::to_string(y) + " "
            + std::to_string(radius) + " " + (filled ? "fill" : "frame") + " " + color;
    }

    bool isWithinBoard(int boardWidth, int boardHeight) override {
        return (x + radius >= 0 && x - radius < boardWidth && y + radius >= 0 && y - radius < boardHeight);
    }

    bool isEqual(const Figure& other) const override {
        const auto* otherCircle = dynamic_cast<const Circle*>(&other);
        if (otherCircle == nullptr) return false;
        return x == otherCircle->x && y == otherCircle->y && radius == otherCircle->radius &&
               color == otherCircle->color && filled == otherCircle->filled;
    }

    bool containsPoint(int px, int py) const override {
        double dist = sqrt(pow(px - x, 2) + pow(py - y, 2));
        if (filled) {
            return dist <= radius;
        } else {
            return abs(dist - radius) < 0.5;
        }
    }

    void setRadius(int newRadius) {
        radius = newRadius;
    }
};

class Rectangle : public Figure {
protected:
    int width;
    int height;

public:
    Rectangle(int x_coordinate, int y_coordinate, int rect_width, int rect_height, const std::string& color_value, bool is_filled)
        : Figure(x_coordinate, y_coordinate, color_value, is_filled), width(rect_width), height(rect_height) {}

    void draw(std::vector<std::vector<char>>& grid) override {
        char drawChar = color.empty() ? '#' : tolower(color[0]);
        int grid_height = grid.size();
        int grid_width = grid[0].size();
        for (int i = y; i < y + height && i < grid_height; ++i) {
            if (i < 0) continue;
            for (int j = x; j < x + width && j < grid_width; ++j) {
                if (j < 0) continue;
                if (filled) {
                    grid[i][j] = drawChar;
                } else {
                    if (i == y || i == y + height - 1 || j == x || j == x + width -1) {
                        grid[i][j] = drawChar;
                    }
                }
            }
        }
    }

    std::string info() override {
        return "Rectangle " + std::to_string(id) + " " + std::to_string(x) + " " + std::to_string(y) + " "
            + std::to_string(width) + " " + std::to_string(height) + " " + (filled ? "fill" : "frame") + " " + color;
    }

    bool isWithinBoard(int boardWidth, int boardHeight) override {
        return (x < boardWidth && x + width > 0 && y < boardHeight && y + height > 0);
    }

    bool isEqual(const Figure& other) const override {
        const Rectangle* otherRect = dynamic_cast<const Rectangle*>(&other);
        if (otherRect == nullptr) return false;
        return x == otherRect->x && y == otherRect->y && width == otherRect->width && height == otherRect->height
            && color == otherRect->color && filled == otherRect->filled;
    }

    bool containsPoint(int px, int py) const override {
        if (px >= x && px < x + width && py >= y && py < y + height) {
            if (filled) {
                return true;
            } else {
                return (px == x || px == x + width - 1 || py == y || py == y + height -1);
            }
        }
        return false;
    }

    void setWidth(int newWidth) {
        width = newWidth;
    }
    void setHeight(int newHeight) {
        height = newHeight;
    }
};

class Line : public Figure {
protected:
    int length;
    char direction;

public:
    Line(int x_coordinate, int y_coordinate, int length_line, char dir, const std::string& color_value, bool is_filled)
        : Figure(x_coordinate, y_coordinate, color_value, is_filled), length(length_line), direction(dir) {}

    void draw(std::vector<std::vector<char>>& grid) override {
        char drawChar = color.empty() ? '-' : tolower(color[0]);
        int grid_height = grid.size();
        int grid_width = grid[0].size();
        if (direction == 'H') {
            for (int j = x; j < x + length && j < grid_width; ++j) {
                if (j < 0 || y < 0 || y >= grid_height) continue;
                grid[y][j] = drawChar;
            }
        } else if (direction == 'V') {
            for (int i = y; i < y + length && i < grid_height; ++i) {
                if (i < 0 || x < 0 || x >= grid_width) continue;
                grid[i][x] = drawChar;
            }
        }
    }

    std::string info() override {
        return "Line " + std::to_string(id) + " " + std::to_string(x) + " " + std::to_string(y) + " "
            + std::to_string(length) + " " + direction + " " + (filled ? "fill" : "frame") + " " + color;
    }

    bool isWithinBoard(int boardWidth, int boardHeight) override {
        if (direction == 'H') {
            return (x < boardWidth && x + length > 0 && y >= 0 && y < boardHeight);
        } else if (direction == 'V') {
            return (y < boardHeight && y + length > 0 && x >= 0 && x < boardWidth);
        }
        return false;
    }

    bool isEqual(const Figure& other) const override {
        const Line* otherLine = dynamic_cast<const Line*>(&other);
        if (otherLine == nullptr) return false;
        return x == otherLine->x && y == otherLine->y && length == otherLine->length && direction == otherLine->direction
            && color == otherLine->color && filled == otherLine->filled;
    }

    bool containsPoint(int px, int py) const override {
        if (direction == 'H') {
            return (py == y && px >= x && px < x + length);
        } else if (direction == 'V') {
            return (px == x && py >= y && py < y + length);
        }
        return false;
    }

    void setLength(int newLength) {
        length = newLength;
    }

    void setDirection(char newDirection) {
        direction = newDirection;
    }
};


class Triangle : public Figure {
protected:
    int height;

public:
    Triangle(int x_coordinate, int y_coordinate, int triangle_height, const std::string& color_value, bool is_filled)
        : Figure(x_coordinate, y_coordinate, color_value, is_filled), height(triangle_height) {}

    void draw(std::vector<std::vector<char>>& grid) override {
        char drawChar = color.empty() ? '*' : tolower(color[0]);
        int grid_height = grid.size();
        int grid_width = grid[0].size();
        for (int i = 0; i < height; ++i) {
            int row = y + i;
            if (row < 0 || row >= grid_height) continue;
            int leftCol = x - i;
            int rightCol = x + i;
            if (filled) {
                for (int col = leftCol; col <= rightCol; ++col) {
                    if (col >= 0 && col < grid_width) {
                        grid[row][col] = drawChar;
                    }
                }
            } else {
                if (leftCol >= 0 && leftCol < grid_width) {
                    grid[row][leftCol] = drawChar;
                }
                if (rightCol >= 0 && rightCol < grid_width) {
                    grid[row][rightCol] = drawChar;
                }
                if (i == height - 1) {
                    for (int col = leftCol; col <= rightCol; ++col) {
                        if (col >= 0 && col < grid_width) {
                            grid[row][col] = drawChar;
                        }
                    }
                }
            }
        }
    }

    std::string info() override {
        return "Triangle " + std::to_string(id) + " " + std::to_string(x) + " " + std::to_string(y) + " "
            + std::to_string(height) + " " + (filled ? "fill" : "frame") + " " + color;
    }

    bool isWithinBoard(int boardWidth, int boardHeight) override {
        int leftMost = x - height + 1;
        int rightMost = x + height - 1;
        int bottomMost = y + height -1;
        return (leftMost < boardWidth && rightMost >=0 && y >= 0 && bottomMost < boardHeight);
    }

    bool isEqual(const Figure& other) const override {
        const Triangle* otherTriangle = dynamic_cast<const Triangle*>(&other);
        if (otherTriangle == nullptr) return false;
        return x == otherTriangle->x && y == otherTriangle->y && height == otherTriangle->height
            && color == otherTriangle->color && filled == otherTriangle->filled;
    }

    bool containsPoint(int px, int py) const override {
        int dy = py - y;
        if (dy < 0 || dy >= height) return false;
        int minX = x - dy;
        int maxX = x + dy;
        if (filled) {
            return px >= minX && px <= maxX;
        } else {
            return (px == minX || px == maxX || py == y + height - 1);
        }
    }

    void setHeight(int newHeight) {
        height = newHeight;
    }
};

class Blackboard {
protected:
    const int BOARD_WIDTH = 80;
    const int BOARD_HEIGHT = 25;
    std::vector<std::vector<char>> grid;
    std::vector<std::unique_ptr<Figure>> figures;
    int nextId = 1;
    int selectedId = -1;

public:
    virtual ~Blackboard() = default;
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
        selectedId = -1;
        nextId = 1;
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
        figure->setId(nextId++);
        figure->draw(grid);
        figures.push_back(std::move(figure));
    }

    void removeFigure(int id) {
        auto it = std::remove_if(figures.begin(), figures.end(),
            [id](const std::unique_ptr<Figure>& fig) { return fig->getId() == id; });
        if (it != figures.end()) {
            figures.erase(it, figures.end());
            redraw();
            std::cout << "Shape with ID " << id << " removed.\n";
        } else {
            std::cout << "Shape with ID " << id << " not found.\n";
        }
    }

    void removeSelectedFigure() {
        if (selectedId == -1) {
            std::cout << "No shape selected.\n";
            return;
        }
        removeFigure(selectedId);
        selectedId = -1;
    }

    void deselectFigure() {
        selectedId = -1;
    }

    void selectFigure(int id) {
        for (const auto& fig : figures) {
            if (fig->getId() == id) {
                selectedId = id;
                std::cout << "Selected shape: " << fig->info() << "\n";
                return;
            }
        }
        std::cout << "Shape with ID " << id << " not found.\n";
    }

    void selectFigureAt(int x, int y) {
        for (auto it = figures.rbegin(); it != figures.rend(); ++it) {
            if ((*it)->containsPoint(x, y)) {
                selectedId = (*it)->getId();
                std::cout << "Selected shape: " << (*it)->info() << "\n";
                return;
            }
        }
        std::cout << "No shape found at the given coordinates.\n";
    }

    void moveSelectedFigure(int newX, int newY) {
        if (selectedId == -1) {
            std::cout << "No shape selected.\n";
            return;
        }
        for (auto it = figures.begin(); it != figures.end(); ++it) {
            if ((*it)->getId() == selectedId) {
                (*it)->setPosition(newX, newY);
                if (!(*it)->isWithinBoard(BOARD_WIDTH, BOARD_HEIGHT)) {
                    std::cout << "Cannot move shape out of bounds.\n";
                    return;
                }
                std::unique_ptr<Figure> temp = std::move(*it);
                figures.erase(it);
                figures.push_back(std::move(temp));
                redraw();
                std::cout << "Shape moved to (" << newX << ", " << newY << ").\n";
                return;
            }
        }
        std::cout << "Selected shape not found.\n";
    }

    void paintSelectedFigure(const std::string& newColor) {
        if (selectedId == -1) {
            std::cout << "No shape selected.\n";
            return;
        }
        for (auto& fig : figures) {
            if (fig->getId() == selectedId) {
                fig->setColor(newColor);
                redraw();
                std::cout << "Shape color changed to " << newColor << ".\n";
                return;
            }
        }
        std::cout << "Selected shape not found.\n";
    }

    void editSelectedFigureParameters(const std::vector<std::string>& params) {
        if (selectedId == -1) {
            std::cout << "No shape selected.\n";
            return;
        }
        for (auto& fig : figures) {
            if (fig->getId() == selectedId) {
                Circle* circle = dynamic_cast<Circle*>(fig.get());
                if (circle) {
                    if (params.size() != 1) {
                        std::cout << "Invalid number of parameters for Circle.\n";
                        return;
                    }
                    int newRadius = std::stoi(params[0]);
                    if (newRadius <= 0) {
                        std::cout << "Invalid radius.\n";
                        return;
                    }
                    circle->setRadius(newRadius);
                    if (!circle->isWithinBoard(BOARD_WIDTH, BOARD_HEIGHT)) {
                        std::cout << "Shape will go out of the board.\n";
                        return;
                    }
                    redraw();
                    std::cout << "Circle radius changed to " << newRadius << ".\n";
                    return;
                }
                Rectangle* rectangle = dynamic_cast<Rectangle*>(fig.get());
                if (rectangle) {
                    if (params.size() != 2) {
                        std::cout << "Invalid number of parameters for Rectangle.\n";
                        return;
                    }
                    int newWidth = std::stoi(params[0]);
                    int newHeight = std::stoi(params[1]);
                    if (newWidth <= 0 || newHeight <= 0) {
                        std::cout << "Invalid dimensions.\n";
                        return;
                    }
                    rectangle->setWidth(newWidth);
                    rectangle->setHeight(newHeight);
                    if (!rectangle->isWithinBoard(BOARD_WIDTH, BOARD_HEIGHT)) {
                        std::cout << "Shape will go out of the board.\n";
                        return;
                    }
                    redraw();
                    std::cout << "Rectangle dimensions changed to width=" << newWidth << ", height=" << newHeight << ".\n";
                    return;
                }
                Line* line = dynamic_cast<Line*>(fig.get());
                if (line) {
                    if (params.size() != 2) {
                        std::cout << "Invalid number of parameters for Line.\n";
                        return;
                    }
                    int newLength = std::stoi(params[0]);
                    char newDirection = toupper(params[1][0]);
                    if (newLength <= 0 || (newDirection != 'H' && newDirection != 'V')) {
                        std::cout << "Invalid parameters for Line.\n";
                        return;
                    }
                    line->setLength(newLength);
                    line->setDirection(newDirection);
                    if (!line->isWithinBoard(BOARD_WIDTH, BOARD_HEIGHT)) {
                        std::cout << "Shape will go out of the board.\n";
                        return;
                    }
                    redraw();
                    std::cout << "Line length changed to " << newLength << ", direction=" << newDirection << ".\n";
                    return;
                }
                Triangle* triangle = dynamic_cast<Triangle*>(fig.get());
                if (triangle) {
                    if (params.size() != 1) {
                        std::cout << "Invalid number of parameters for Triangle.\n";
                        return;
                    }
                    int newHeight = std::stoi(params[0]);
                    if (newHeight <= 0) {
                        std::cout << "Invalid height.\n";
                        return;
                    }
                    triangle->setHeight(newHeight);
                    if (!triangle->isWithinBoard(BOARD_WIDTH, BOARD_HEIGHT)) {
                        std::cout << "Shape will go out of the board.\n";
                        return;
                    }
                    redraw();
                    std::cout << "Triangle height changed to " << newHeight << ".\n";
                    return;
                }
                std::cout << "Unknown shape type.\n";
                return;
            }
        }
        std::cout << "Selected shape not found.\n";
    }

    void redraw() {
        clear();
        for (const auto& figure : figures) {
            figure->draw(grid);
        }
    }

    void listFigures() {
        if (figures.empty()) {
            std::cout << "No shapes on the board.\n";
            return;
        }
        std::cout << "Figures on the board:\n";
        for (const auto& figure : figures) {
            std::cout << figure->info() << "\n";
        }
    }
};

void handleCommand(const std::string& inputLine, Blackboard& board) {
    std::istringstream iss(inputLine);
    std::string command;
    iss >> command;
    if (command == "add") {
        std::string fillModeStr, color, shapeType;
        iss >> fillModeStr >> color >> shapeType;
        bool filled = (fillModeStr == "fill");
        if (shapeType == "circle") {
            int x, y, radius;
            iss >> x >> y >> radius;
            if (iss.fail()) {
                std::cout << "Invalid parameters for Circle.\n";
                return;
            }
            board.addFigure(std::make_unique<Circle>(x, y, radius, color, filled));
        } else if (shapeType == "rectangle") {
            int x, y, width, height;
            iss >> x >> y >> width >> height;
            if (iss.fail()) {
                std::cout << "Invalid parameters for Rectangle.\n";
                return;
            }
            board.addFigure(std::make_unique<Rectangle>(x, y, width, height, color, filled));
        } else if (shapeType == "line") {
            int x, y, length;
            char direction;
            iss >> x >> y >> length >> direction;
            direction = toupper(direction);
            if (iss.fail() || (direction != 'H' && direction != 'V')) {
                std::cout << "Invalid parameters for Line.\n";
                return;
            }
            board.addFigure(std::make_unique<Line>(x, y, length, direction, color, filled));
        } else if (shapeType == "triangle") {
            int x, y, height;
            iss >> x >> y >> height;
            if (iss.fail()) {
                std::cout << "Invalid parameters for Triangle.\n";
                return;
            }
            board.addFigure(std::make_unique<Triangle>(x, y, height, color, filled));
        } else {
            std::cout << "Unknown shape type.\n";
        }
    }
    else if (command == "select") {
        std::string param;
        iss >> param;
        if (isdigit(param[0]) || (param[0] == '-' && isdigit(param[1]))) {
            int id = std::stoi(param);
            board.selectFigure(id);
        } else {
            int x = std::stoi(param);
            int y;
            iss >> y;
            board.selectFigureAt(x, y);
        }
    }
    else if (command == "remove") {
        board.removeSelectedFigure();
    }
    else if (command == "move") {
        int newX, newY;
        iss >> newX >> newY;
        board.moveSelectedFigure(newX, newY);
    }
    else if (command == "paint") {
        std::string newColor;
        iss >> newColor;
        board.paintSelectedFigure(newColor);
    }
    else if (command == "edit") {
        std::vector<std::string> params;
        std::string paramStr;
        while (iss >> paramStr) {
            params.push_back(paramStr);
        }
        board.editSelectedFigureParameters(params);
    }
    else if (command == "draw") {
        board.print();
    }
    else if (command == "list") {
        board.listFigures();
    }
    else if (command == "clear") {
        board.clearAll();
        std::cout << "Blackboard cleared.\n";
    }
    else if (command == "help") {
        std::cout << "Available commands:\n";
        std::cout << "add [fill|frame] [color] [shape] [parameters] - Add a new shape\n";
        std::cout << "  Shapes and parameters:\n";
        std::cout << "    circle x y radius\n";
        std::cout << "    rectangle x y width height\n";
        std::cout << "    line x y length direction(H/V)\n";
        std::cout << "    triangle x y height\n";
        std::cout << "select [id|x y] - Select a shape by id or coordinates\n";
        std::cout << "remove - Remove the selected shape\n";
        std::cout << "move [x y] - Move the selected shape to new coordinates\n";
        std::cout << "paint [color] - Change the color of the selected shape\n";
        std::cout << "edit [parameters] - Edit parameters of the selected shape\n";
        std::cout << "  Parameters depend on shape type:\n";
        std::cout << "    Circle: radius\n";
        std::cout << "    Rectangle: width height\n";
        std::cout << "    Line: length direction(H/V)\n";
        std::cout << "    Triangle: height\n";
        std::cout << "draw - Draw the blackboard\n";
        std::cout << "list - List all shapes\n";
        std::cout << "clear - Clear the blackboard\n";
        std::cout << "help - Show this help message\n";
        std::cout << "exit - Exit the application\n";
    }
    else {
        std::cout << "Unknown command.\n";
    }
}

int main() {
    Blackboard board;
    std::string inputLine;
    std::cout << "Welcome to the Extended Blackboard application. Type 'help' to see available commands.\n";
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, inputLine);
        if (inputLine == "exit") {
            std::cout << "Exiting the application. Goodbye!\n";
            break;
        }
        handleCommand(inputLine, board);
    }
    return 0;
}
