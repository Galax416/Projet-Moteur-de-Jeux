#include <common/Player.hpp>

Player::Player(std::string filename, Shader *shader, Camera camera) : Entity(filename, shader) {
    camera.setEditionMode(false);
    this->camera = camera;
	this->camera.init();
	this->camera.setPosition(glm::vec3(0.f, 90.f, 0.f));
	this->camera.setRotation(glm::quat(glm::vec3(0.f, 0.f, 0.f)));
	this->camera.setEditionMode(false);
}

void Player::updateInput(float _deltaTime, GLFWwindow* _window) {   
		float cameraTranslationSpeed = m_translationSpeed * _deltaTime;
		float cameraRotationSpeed = m_rotationSpeed * _deltaTime;

		bool isMouseLeftPressed = glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
		// Enable/disable the mouse cursor
		glfwSetInputMode(_window, GLFW_CURSOR, isMouseLeftPressed ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

		// Get the mouse position
		double mouseX, mouseY;
		glfwGetCursorPos(_window, &mouseX, &mouseY);

		// Compute the mouse movement
		static double lastMouseX = mouseX;
		static double lastMouseY = mouseY;
		double deltaX = mouseX - lastMouseX;
		double deltaY = mouseY - lastMouseY;
		lastMouseX = mouseX;
		lastMouseY = mouseY;

		// Compute the new rotation angles
		glm::vec3 eulerAngle = camera.getRotationEuler();
		if (isMouseLeftPressed) {
			eulerAngle.y -= (float)(deltaX) * m_rotationSpeed;
			eulerAngle.y = Camera::clipAngle180(eulerAngle.y); // Clamp the yaw angle between -180 and 180 degrees
			eulerAngle.x += (float)(deltaY) * m_rotationSpeed;
			eulerAngle.x = glm::clamp(eulerAngle.x, -89.9f, 89.9f); // Clamp the pitch angle between -90 and 90 degrees
		}


		// ZQSD movement
		glm::vec3 pos = transform.getLocalPosition();
		if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS) {
			pos += camera.getFront() * cameraTranslationSpeed;
		}
		if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS) {
			pos -= camera.getFront() * cameraTranslationSpeed;
		}
		if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS) {
			pos -= glm::normalize(glm::cross(camera.getFront(), VEC_UP)) * cameraTranslationSpeed;
		}
		if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS) {
			pos += glm::normalize(glm::cross(camera.getFront(), VEC_UP)) * cameraTranslationSpeed;
		}

		updatePlayer(pos, eulerAngle);

		// Weapon
		if (glfwGetKey(_window, GLFW_KEY_E) == GLFW_PRESS) { 
			glm::vec3 position = transform.getLocalPosition() + weapon->transform.getLocalPosition(); // weapon position
			weapon->shoot(camera.getPosition(), camera.getFront(), camera.getRotationEuler(), bulletOffset, 1000.0f, 2.0f);
		}
		if(glfwGetKey(_window, GLFW_KEY_R) == GLFW_PRESS) {
			weapon->reload();
		}

}

void Player::updatePlayer(glm::vec3 pos, glm::vec3 eulerAngle) {

	// Compute the camera position
    glm::quat rotationQuat = glm::angleAxis(glm::radians(eulerAngle.y), VEC_UP); // rotate around the Y axis
    glm::vec3 rotatedOffset = rotationQuat * cameraOffset;
    glm::vec3 finalCameraPosition = pos + rotatedOffset;

	// Update the camera position and rotation
    camera.setPosition(finalCameraPosition);
    camera.setRotation(eulerAngle);

	// Update the player position and rotation
	transform.setLocalRotation(glm::vec3(0.0f, eulerAngle.y, 0.0f)); // Only Y axis for the player model
	transform.setLocalPosition(pos);

	for (auto&& child : children) {
		// Rotate weapon around both X and Y axes
        child->transform.setLocalRotation(glm::vec3(eulerAngle.x - 90.0f, 0.0f, eulerAngle.z));
		// Set the weapon position
        child->transform.setLocalPosition(weaponOffset);

	}
}

void Player::initHUD(GLuint shaderID) {
	initText2D("../data/font/Holstein.tga", shaderID);
}

void Player::DrawHUD() {
	// Print the health
	char text[256];
	sprintf(text, "Health: %d", m_health);
	printText2D(text, 100, 500, 60);
}