#pragma once

#include "Modelo.h"
#include "glm/gtx/transform.hpp"
#include "glm/glm.hpp"

class Meteorito : public Modelo {

public:
	Meteorito();
	void avanzar();
	void actualizarMatrizModelo();
	vec3 coordenadas = vec3(0.0f, 0.0f, 0.0f);
	bool vida = true;
	vec3 getCoordenadas();
	
private:

};