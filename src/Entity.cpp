#include "Entity.h"

Entity::Entity(sf::Vector2f startPos, float hp, float speed) {
    position = startPos;
    this->hp = hp; //permet de dire au compilateur que c'est le hp de l'attribut de l'objet et pas du paramètre reçu
    this->maxHp = hp;
    this->speed = speed;
}

sf::Vector2f Entity::getPosition() const {
    return position; //Renvoie la valeur de l'attribut
}

float Entity::getHp() const {
    return hp; //Renvoie la valeur de l'attribut
}

bool Entity::isAlive() const {
    return hp > 0; //Renvoie true si hp > 0, false sinon
}

void Entity::takeDamage(float amount) {
    hp -= amount;
    if (hp < 0) hp = 0; //Soustrait les dégats des PV
}