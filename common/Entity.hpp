#pragma once

#include <glm/glm.hpp> //glm::mat4
#include <list> //std::list
#include <glm/gtc/matrix_transform.hpp> //glm::translate, glm::rotate, glm::scale
#include <GL/glew.h> //GLuint

#include <common/Model.hpp>
#include <common/Transform.hpp>
#include <common/Shader.hpp>
#include <common/Camera.hpp>


class Entity {
public:
	// Hierarchy
	std::list<Entity*> children;
	Entity* parent;

	// Constructors
	Entity(Shader *shader) : model(""), shader(shader), parent(nullptr){}
	Entity(std::string filename, Shader *shader) : model(filename), shader(shader) ,parent(nullptr) {} // Entity with geometry

	// Geometry
	Model model;

	// Model Matrix
	Transform transform;

	// Shader
	Shader* shader;

	// Gravity
	bool isGravityEntity{ false };
	void setIsGravtityEntity(bool boolean) { isGravityEntity = boolean; }
	float gravity = 9.807f;
	glm::vec3 velocity{ 0.0f, 0.0f, 0.0f };
	bool isGrounded{ false };



	// Fonctions
	void addChild(Entity& child) {
		child.parent = this;
		children.push_back(&child);
	}

	// Update entity with dirty's flag
	void updateSelfAndChild() {
		if (transform.isDirty()) {
			forceUpdateSelfAndChild();
			return;
		}
		for (auto&& child : children) {
			child->updateSelfAndChild();
		}
	}

	// Update all entity
	void forceUpdateSelfAndChild() {
		if (parent)
			transform.computeModelMatrix(parent->transform.getModelMatrix());
		else
			transform.computeModelMatrix();

		for (auto&& child : children) {
			child->forceUpdateSelfAndChild();
		}
	}


	void render() {
		shader->use();
		glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "Model"), 1, GL_FALSE, &transform.getModelMatrix()[0][0]); // Model Matrix
		
		// Draw Mesh
		model.Draw(shader); // Render geometry

		for (auto&& child : children) {
			child->render();
		}
	}

	bool CheckCollisionWithEntity(Entity &other) {
		// Check collision with other
		if (CheckCollisionWithSingleEntity(other))
			return true;

		// And check collision with other child
		for (auto&& child : other.children) {
			if (CheckCollisionWithEntity(*child))
				return true;
		}

		return false;
	}

	bool CheckCollisionWithSingleEntity(Entity &other) { // AABB - AABB Collision
		AABB entityAABB = this->model.getBoundingBox();
		AABB otherAABB = other.model.getBoundingBox();

		// Update bounding box
		entityAABB.updateBoundingBox(this->transform.getModelMatrix());
		otherAABB.updateBoundingBox(other.transform.getModelMatrix());

		return entityAABB.intersects(otherAABB);

	}
};