#include "lights/spotLight.hpp"

SpotLight::SpotLight(glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 direction, float cutOff, float outerCutOff)
{
	this->position = position;
	this->constant = constant;
	this->linear = linear;
	this->quadratic = quadratic;

	this->direction = direction;
	this->cutOff = cutOff;
	this->outerCutOff = outerCutOff;
}

void SpotLight::sendToShader(unsigned int shaderProgramID, unsigned int index)
{

}