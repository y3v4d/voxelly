#pragma once

struct Entity {
    int id;
};

class Component {
public:
    int id;

private:
    Entity* owner;
};

class Scene {

};