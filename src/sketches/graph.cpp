#include <sketches/graph.hpp>
#include <cmath>

// Node functions
Node::Node(StateManager *applicationStateManager, int value, float x, float y, float radius, float sketchWidth, float sketchHeight)
    : StateElement(applicationStateManager, x, y, radius * 2, radius * 2)
{
    this->x = x;
    this->y = y;
    this->value = value;

    this->areaWidth = sketchWidth;
    this->areaHeight = sketchHeight;
    this->radius = radius;

    // Set up Node shape
    this->nodeColor = sf::Color::Black;
    this->nodeShape.setPosition(sf::Vector2f(x, y));
    this->nodeShape.setRadius(this->radius);
    this->nodeShape.setFillColor(this->nodeColor);
    this->nodeShape.setOrigin(this->x + this->radius, this->y + this->radius);

    // Set up text
    this->text->setString(std::to_string(this->value));
    this->text->setCharacterSize(10);
    this->centerTextOnShape();

    // Rearrange the serial of drawables
    this->drawableList.pop_back();
    this->drawableList.push_back(&(this->nodeShape));
    this->drawableList.push_back(this->text);
}

Node::~Node() {}

void Node::setColor(sf::Color newColor)
{
    this->nodeColor = newColor;
}

sf::Vector2f Node::getCenter()
{
    return this->nodeShape.getOrigin();
}

float Node::getRadius()
{
    return this->radius;
}

sf::Vector2i Node::sendMousePosition()
{
    return this->getMousePosition();
}

sf::Color Node::getColor()
{
    return this->nodeColor;
}

void Node::setPosition(sf::Vector2i newPosition)
{
    this->x = newPosition.x;
    this->y = newPosition.y;
}

sf::CircleShape *Node::getNodeShape()
{
    return &(this->nodeShape);
}

sf::Vector2f Node::getPosition()
{
    return sf::Vector2f(x, y);
}

void Node::update()
{
    if (this->clicked())
        this->setPosition(getMousePosition());
}

// Edge functions
Edge::Edge(Node *firstNode, Node *secondNode)
{
    this->firstNode = firstNode;
    this->secondNode = secondNode;

    this->color = sf::Color::Black;
    this->edgeWidth = 10;

    // Position on the first node
    this->edgeShape.setPosition(firstNode->getCenter().x - this->edgeWidth / 2.0, firstNode->getCenter().y - this->edgeWidth / 2.0);
    this->edgeShape.setOrigin(firstNode->getPosition().x + this->edgeWidth / 2.0, firstNode->getPosition().y + this->edgeWidth / 2.0);

    // Scale based on the second node
    this->edgeLength = getNewLength(firstNode->getCenter(), secondNode->getCenter());
    this->edgeShape.setSize(sf::Vector2f(this->edgeLength, this->edgeWidth));

    // Set up for the second node
    this->update();
}

Edge::~Edge()
{
    delete this->firstNode;
    delete this->secondNode;
}

void Edge::setColor(sf::Color newColor)
{
    this->color = newColor;
}

float Edge::getNewAngle(sf::Vector2f firstPoint, sf::Vector2f secondPoint)
{
    return std::atan((secondPoint.y - firstPoint.y) / (secondPoint.x - firstPoint.x));
}

float Edge::getNewLength(sf::Vector2f firstPoint, sf::Vector2f secondPoint)
{
    float distance = sqrt((secondPoint.x - firstPoint.x) * (secondPoint.x - firstPoint.x) + (secondPoint.y - firstPoint.y) * (secondPoint.y - firstPoint.y));
    distance += this->edgeWidth;
    return distance;
}

sf::RectangleShape *Edge::getEdgeShape()
{
    return &(this->edgeShape);
}

void Edge::update()
{
    // Rotate based on the second node
    this->edgeAngle = getNewAngle(firstNode->getCenter(), secondNode->getCenter());
    this->edgeShape.rotate(this->edgeAngle);
}

// Graph functions

Graph::Graph(StateManager *applicationStateManager, float x, float y, float width, float height, int size, Action action)
    : SketchContainer(applicationStateManager, x, y, width, height)
{
    std::cout << "Graph Loading" << std::endl;

    this->action = action;
    this->size = size;

    this->x = x;
    this->y = y;

    this->graphWidth = width;
    this->graphHeight = height;

    std::cout << "Graph loaded" << std::endl;
}

Graph::~Graph()
{
    for (auto node : nodeList)
        delete node;
    for (auto edge : edgeList)
        delete edge;
}

void Graph::reset()
{
    if (this->paused)
    {
        this->completed = false;

        this->nodeList.clear();

        for (int i = 0; i < numberOfNodes; i++)
            nodeList.push_back(new Node(this->stateManager, i + 1, 400, 400, 30, this->graphWidth, this->graphHeight));
    }
}

void Graph::createDrawableList()
{
    temporaryDrawableList.clear();

    for (int i = 0; i < this->numberOfNodes; i++)
    {
        for (auto edge : edgeList)
        {
            temporaryDrawableList.push_back(edge->getEdgeShape());
        }
        for (auto node : nodeList)
        {
            temporaryDrawableList.push_back(node->getNodeShape());
        }
    }

    // Color the nodes
}

void Graph::update()
{
    for (auto node : nodeList)
    {
        node->update();
    }
    for (auto edge : edgeList)
    {
        edge->update();
    }

    createDrawableList();

    // Select corresponding algorithm
    if (!this->paused and this->action == Action::DepthFirstSearch)
        this->depthFirstSearch();
    if (!this->paused and this->action == Action::BreadthFirstSearch)
        this->breadthFirstSearch();
    if (!this->paused and this->action == Action::Dijkstra)
        this->dijkstra();
    if (this->completed)
        this->paused = true;
}