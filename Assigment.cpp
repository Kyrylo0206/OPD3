#include <iostream>
#include <memory>
#include <vector>
#include <fstream>
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
        const Circle* otherCircle = dynamic_cast<const Circle*>(&other);
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
                figures.erase(it); // Erase before moving to avoid invalid iterator
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

    void editSelectedFigureParameters(const std::vector<int>& params) {
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
                    int newRadius = params[0];
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
        std::vector<int> params;
        int param;
        while (iss >> param) {
            params.push_back(param);
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
        std::cout << "select [id|x y] - Select a shape by id or coordinates\n";
        std::cout << "remove - Remove the selected shape\n";
        std::cout << "move [x y] - Move the selected shape to new coordinates\n";
        std::cout << "paint [color] - Change the color of the selected shape\n";
        std::cout << "edit [parameters] - Edit parameters of the selected shape\n";
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
