#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include "Shader.h"
#include "Vertice.h"
#include "Carro.h"
#include "Edificio.h"
#include "NaveStarFox.h"
#include "Misil.h"
#include "Bala.h"
#include "Meta.h"
#include "Meteorito.h"
#include "NaveEnemiga.h"
#include "Casa.h"
#include "Foco.h"
#include "Modelo.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
using namespace std;

// ======================================== Prototipos ========================================
void dibujar(), actualizar(), establecerVista(), establecerProyeccion(), inicializarModelos();
void teclado_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Inicialización de Modelos
void inicializarNaveStarFox(), inicializarNavesEnemigas(), inicializarCasas(), inicializarEdificios(),
inicializarFocos(), inicializarMeteoritos(), inicializarMisilBalaMeta();

// Establecimiento de Vista de Modelos
void vistaNaveStarFox(), vistaNavesEnemigas(), vistaCasas(), vistaEdificios(), vistaFocos(), vistaMeteoritos(),
vistaMisilBalaMeta();

// Dibujado de Modelos
void dibujarNaveStarFox(), dibujarNavesEnemigas(), dibujarCasas(), dibujarEdificios(), dibujarFocos(), 
dibujarMeteoritos(), dibujarMeta();

// ======================================== Variables ========================================
Shader* shader;
GLuint posicionID;
GLuint colorID;
GLuint modeloID;
GLuint vistaID;
GLuint proyeccionID;
GLfloat ancho = 1024.0f;
GLfloat alto = 768.0f;
vec3 posicionCamara = vec3(3.0f, 1.5f, 7.0f);
mat4 vista;
mat4 proyeccion;

bool rotado = false;
double tiempoAnterior = 0.0f;
double tiempoActual = 0.0f;
double tiempoDiferencial = 0.0f;
double tiempoAnteriorColision = 0.0f;
double tiempoDiferencialColision = 0.0f;

// Objetos
NaveStarFox* naveStarFox; Meta* meta; Misil* misil; Bala* bala;
Meteorito* meteorito; Meteorito* meteorito2; Meteorito* meteorito3; Meteorito* meteorito4; Meteorito* meteorito5; Meteorito* meteorito6;
NaveEnemiga* naveEnemiga; NaveEnemiga* naveEnemiga2; NaveEnemiga* naveEnemiga3;
Casa* casa; Casa* casa2; Casa* casa3; Casa* casa4;
Foco* foco; Foco* foco2; Foco* foco3; Foco* foco4; Foco* foco5; Foco* foco6; Foco* foco7; Foco* foco8; Foco* foco9; Foco* foco10; 
Foco* foco11; Foco* foco12;
Edificio* edificio; Edificio* edificio2; Edificio* edificio3; Edificio* edificio4;

// Declaramos apuntador de ventana
GLFWwindow* window;

// =================================== Main Source Code ==================================
int main()
{
	// Si no se pudo iniciar glfw, terminamos ejecución
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	// Si se pudo iniciar GLFW, entonces inicializamos la ventana
	window = glfwCreateWindow(1024, 768, "Ventana", NULL, NULL);

	// Si no podemos iniciar la ventana, entonces terminamos ejecucion
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Establecemos el contexto
	glfwMakeContextCurrent(window);

	// Una vez establecido  el contexto, activamos funciones modernas
	glewExperimental = true;

	GLenum errorGlew = glewInit();

	if (errorGlew != GLEW_OK) {
		cout << glewGetErrorString(errorGlew);
	}

	const GLubyte* versionGL = glGetString(GL_VERSION);
	cout << "Version OpenGL: " << versionGL;

	const char* rutaVertex = "VertexShader.shader";
	const char* rutaFragment = "FragmentShader.shader";

	shader = new Shader(rutaVertex, rutaFragment);

	// Mapeo de atributos
	posicionID = glGetAttribLocation(shader->getID(), "posicion");
	colorID = glGetAttribLocation(shader->getID(), "color");
	modeloID = glGetUniformLocation(shader->getID(), "modelo");
	vistaID = glGetUniformLocation(shader->getID(), "vista");
	proyeccionID = glGetUniformLocation(shader->getID(), "proyeccion");

	// Desenlazar el shader
	shader->desenlazar();

	establecerProyeccion();
	inicializarModelos();

	tiempoAnterior = glfwGetTime();

	// Ciclo de dibujo (Draw loop)
	while (!glfwWindowShouldClose(window)) {
		// Establecer region de dibujo
		glViewport(0, 0, 1024, 768);

		// Establece el color de borrado
		glClearColor(1, 0.2, 0.5, 1);

		// Borramos
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Rutina de dibujo
		dibujar();
		actualizar();

		// Cambiar los buffers
		glfwSwapBuffers(window);

		// Reconocer si hay entradas
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void actualizar() {
	tiempoActual = glfwGetTime();
	tiempoDiferencial = tiempoActual - tiempoAnterior;

	naveEnemiga->avanzar();
	naveStarFox->avanzar();
	establecerVista();

	tiempoAnterior = tiempoActual;

	int estadoEspacio = glfwGetKey(window, GLFW_KEY_SPACE);
	if (estadoEspacio == GLFW_PRESS) {
		naveStarFox->rotar();
	}

	int estadoIzquierda = glfwGetKey(window, GLFW_KEY_LEFT);
	if (estadoIzquierda == GLFW_PRESS) {
		naveStarFox->moverIzquierda();
	}

	int estadoDerecha = glfwGetKey(window, GLFW_KEY_RIGHT);
	if (estadoDerecha == GLFW_PRESS) {
		naveStarFox->moverDerecha();
	}

	int estadoAtras = glfwGetKey(window, GLFW_KEY_DOWN);
	if (estadoAtras == GLFW_PRESS) {
		naveStarFox->retroceder();
	}

	int estadoP = glfwGetKey(window, GLFW_KEY_P);
	if (estadoP == GLFW_PRESS) {
		naveStarFox->rapidez();
	}

	int estadoDisparar = glfwGetKey(window, GLFW_KEY_Z);
	if (estadoDisparar == GLFW_PRESS)
	{
		bala->aparecer(naveStarFox->getCoordenadas());
		bala->dibujar(GL_QUADS);
		bala->avanzar();
	}
}

void dibujar() {
	dibujarNaveStarFox();
	dibujarNavesEnemigas();
	dibujarCasas();
	dibujarEdificios();
	dibujarFocos();
	dibujarMeteoritos();
	dibujarMeta();
}

// MVP
void establecerVista() {
	vec3 posicionCamara = naveStarFox->getCoordenadas();
	posicionCamara.z -= 2.5;
	posicionCamara.y += 3.7;

	vista = lookAt(
		posicionCamara,						//Posicion de la camara
		naveStarFox->getCoordenadas(),		//Posicion del objetivo
		vec3(0.0f, 1.0f, 0.0f)				//Vector hacia arriba
	);
	vistaNaveStarFox();
	vistaNavesEnemigas();
	vistaCasas();
	vistaEdificios();
	vistaFocos();
	vistaMeteoritos();
	vistaMisilBalaMeta();
}
void establecerProyeccion() {
	proyeccion = perspective(
		1.83259416667f,					//Campo de vision (FoV)
		ancho / alto,					//Relación de aspecto (Aspect Ratio)
		0.1f,							//Near clipping (Desde donde renderea)
		100.0f							//Far clipping (Hasta donde renderea)
	);
}
void inicializarModelos()
{
	inicializarNaveStarFox();
	inicializarNavesEnemigas();
	inicializarCasas();
	inicializarEdificios();
	inicializarFocos();
	inicializarMeteoritos();
	inicializarMisilBalaMeta();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

// Inicializaciones
void inicializarNaveStarFox()
{
	naveStarFox = new NaveStarFox();
	naveStarFox->proyeccion = proyeccion;
	naveStarFox->shader = shader;
	naveStarFox->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	establecerVista();
	naveStarFox->vista = vista;
	establecerVista();
	naveStarFox->vista = vista;
}
void inicializarNavesEnemigas()
{
	//inicializar nave enemiga 1
	naveEnemiga = new NaveEnemiga();
	naveEnemiga->vista = vista;
	naveEnemiga->proyeccion = proyeccion;
	naveEnemiga->shader = shader;
	naveEnemiga->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializar nave enemiga 2
	naveEnemiga2 = new NaveEnemiga();
	naveEnemiga2->vista = vista;
	naveEnemiga2->proyeccion = proyeccion;
	naveEnemiga2->shader = shader;
	naveEnemiga2->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializar nave enemiga 3
	naveEnemiga3 = new NaveEnemiga();
	naveEnemiga3->vista = vista;
	naveEnemiga3->proyeccion = proyeccion;
	naveEnemiga3->shader = shader;
	naveEnemiga3->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);
}
void inicializarCasas()
{
	//inicializacion casa 1
	casa = new Casa();
	casa->vista = vista;
	casa->proyeccion = proyeccion;
	casa->shader = shader;
	casa->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializacion casa 2
	casa2 = new Casa();
	casa2->vista = vista;
	casa2->proyeccion = proyeccion;
	casa2->shader = shader;
	casa2->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializacion casa 3
	casa3 = new Casa();
	casa3->vista = vista;
	casa3->proyeccion = proyeccion;
	casa3->shader = shader;
	casa3->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializacion casa 4
	casa4 = new Casa();
	casa4->vista = vista;
	casa4->proyeccion = proyeccion;
	casa4->shader = shader;
	casa4->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);
}
void inicializarEdificios()
{
	//inicializarEdificio 1
	edificio = new Edificio();
	edificio->vista = vista;
	edificio->proyeccion = proyeccion;
	edificio->shader = shader;
	edificio->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializarEdificio 2
	edificio2 = new Edificio();
	edificio2->vista = vista;
	edificio2->proyeccion = proyeccion;
	edificio2->shader = shader;
	edificio2->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializarEdificio 3
	edificio3 = new Edificio();
	edificio3->vista = vista;
	edificio3->proyeccion = proyeccion;
	edificio3->shader = shader;
	edificio3->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializarEdificio 4
	edificio4 = new Edificio();
	edificio4->vista = vista;
	edificio4->proyeccion = proyeccion;
	edificio4->shader = shader;
	edificio4->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);
}
void inicializarFocos()
{
	//inicializarFoco 1
	foco = new Foco();
	foco->vista = vista;
	foco->proyeccion = proyeccion;
	foco->shader = shader;
	foco->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializarFoco 2
	foco2 = new Foco();
	foco2->vista = vista;
	foco2->proyeccion = proyeccion;
	foco2->shader = shader;
	foco2->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializarFoco 3
	foco3 = new Foco();
	foco3->vista = vista;
	foco3->proyeccion = proyeccion;
	foco3->shader = shader;
	foco3->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializarFoco 4
	foco4 = new Foco();
	foco4->vista = vista;
	foco4->proyeccion = proyeccion;
	foco4->shader = shader;
	foco4->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializarFoco 5
	foco5 = new Foco();
	foco5->vista = vista;
	foco5->proyeccion = proyeccion;
	foco5->shader = shader;
	foco5->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializarFoco 6
	foco6 = new Foco();
	foco6->vista = vista;
	foco6->proyeccion = proyeccion;
	foco6->shader = shader;
	foco6->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializarFoco 7
	foco7 = new Foco();
	foco7->vista = vista;
	foco7->proyeccion = proyeccion;
	foco7->shader = shader;
	foco7->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializarFoco 8
	foco8 = new Foco();
	foco8->vista = vista;
	foco8->proyeccion = proyeccion;
	foco8->shader = shader;
	foco8->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializarFoco 9
	foco9 = new Foco();
	foco9->vista = vista;
	foco9->proyeccion = proyeccion;
	foco9->shader = shader;
	foco9->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializarFoco 10
	foco10 = new Foco();
	foco10->vista = vista;
	foco10->proyeccion = proyeccion;
	foco10->shader = shader;
	foco10->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializarFoco 11
	foco11 = new Foco();
	foco11->vista = vista;
	foco11->proyeccion = proyeccion;
	foco11->shader = shader;
	foco11->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializarFoco 12
	foco12 = new Foco();
	foco12->vista = vista;
	foco12->proyeccion = proyeccion;
	foco12->shader = shader;
	foco12->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);
}
void inicializarMeteoritos()
{
	//inicializacion meteorito 1
	meteorito = new Meteorito();
	meteorito->vista = vista;
	meteorito->proyeccion = proyeccion;
	meteorito->shader = shader;
	meteorito->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializacion meteorito 2
	meteorito2 = new Meteorito();
	meteorito2->vista = vista;
	meteorito2->proyeccion = proyeccion;
	meteorito2->shader = shader;
	meteorito2->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializacion meteorito 3
	meteorito3 = new Meteorito();
	meteorito3->vista = vista;
	meteorito3->proyeccion = proyeccion;
	meteorito3->shader = shader;
	meteorito3->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializacion meteorito 4
	meteorito4 = new Meteorito();
	meteorito4->vista = vista;
	meteorito4->proyeccion = proyeccion;
	meteorito4->shader = shader;
	meteorito4->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializacion meteorito 5
	meteorito5 = new Meteorito();
	meteorito5->vista = vista;
	meteorito5->proyeccion = proyeccion;
	meteorito5->shader = shader;
	meteorito5->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializacion meteorito 6
	meteorito6 = new Meteorito();
	meteorito6->vista = vista;
	meteorito6->proyeccion = proyeccion;
	meteorito6->shader = shader;
	meteorito6->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);
}
void inicializarMisilBalaMeta()
{
	//inicializacion misil
	misil = new Misil();
	misil->vista = vista;
	misil->proyeccion = proyeccion;
	misil->shader = shader;
	misil->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializacion bala 
	bala = new Bala();
	bala->vista = vista;
	bala->proyeccion = proyeccion;
	bala->shader = shader;
	bala->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);

	//inicializacion meta
	meta = new Meta();
	meta->vista = vista;
	meta->proyeccion = proyeccion;
	meta->shader = shader;
	meta->inicializarVertexArray(posicionID, colorID, modeloID, vistaID, proyeccionID);
}

// Establecimiento de Vistas
void vistaNaveStarFox()
{
	if (naveStarFox != NULL) {
		naveStarFox->vista = vista;
	}
}
void vistaNavesEnemigas()
{
	if (naveEnemiga != NULL) {
		naveEnemiga->vista = vista;
	}
	if (naveEnemiga2 != NULL) {
		naveEnemiga2->vista = vista;
	}
	if (naveEnemiga3 != NULL) {
		naveEnemiga3->vista = vista;
	}
}
void vistaCasas()
{
	if (casa != NULL) {
		casa->vista = vista;
	}
	if (casa2 != NULL) {
		casa2->vista = vista;
	}
	if (casa3 != NULL) {
		casa3->vista = vista;
	}
	if (casa4 != NULL) {
		casa4->vista = vista;
	}
}
void vistaEdificios()
{
	if (edificio != NULL) {
		edificio->vista = vista;
	}
	if (edificio2 != NULL) {
		edificio2->vista = vista;
	}
	if (edificio3 != NULL) {
		edificio3->vista = vista;
	}
	if (edificio4 != NULL) {
		edificio4->vista = vista;
	}
}
void vistaFocos()
{
	if (foco != NULL) {
		foco->vista = vista;
	}
	if (foco2 != NULL) {
		foco2->vista = vista;
	}
	if (foco3 != NULL) {
		foco3->vista = vista;
	}
	if (foco4 != NULL) {
		foco4->vista = vista;
	}
	if (foco5 != NULL) {
		foco5->vista = vista;
	}
	if (foco6 != NULL) {
		foco6->vista = vista;
	}
	if (foco7 != NULL) {
		foco7->vista = vista;
	}
	if (foco8 != NULL) {
		foco8->vista = vista;
	}
	if (foco9 != NULL) {
		foco9->vista = vista;
	}
	if (foco10 != NULL) {
		foco10->vista = vista;
	}
	if (foco11 != NULL) {
		foco11->vista = vista;
	}
	if (foco12 != NULL) {
		foco12->vista = vista;
	}
}
void vistaMeteoritos()
{
	if (meteorito != NULL) {
		meteorito->vista = vista;
	}
	if (meteorito2 != NULL) {
		meteorito2->vista = vista;
	}
	if (meteorito3 != NULL) {
		meteorito3->vista = vista;
	}
	if (meteorito4 != NULL) {
		meteorito4->vista = vista;
	}
	if (meteorito5 != NULL) {
		meteorito5->vista = vista;
	}
	if (meteorito6 != NULL) {
		meteorito6->vista = vista;
	}
}
void vistaMisilBalaMeta()
{
	if (misil != NULL) {
		misil->vista = vista;
	}
	if (bala != NULL) {
		bala->vista = vista;
	}
	if (meta != NULL) {
		meta->vista = vista;
	}
}

// Dibujado
void dibujarNaveStarFox()
{
	naveStarFox->dibujar(GL_QUADS);
}
void dibujarNavesEnemigas()
{
	//Nave izquierda abajo
	naveEnemiga->dibujar(GL_QUADS);
	naveEnemiga->coordenadas = vec3(-5.0f, 0.0f, -30.0f);
	naveEnemiga->actualizarMatrizModelo();

	//Nave izquierda arriba
	naveEnemiga2->dibujar(GL_QUADS);
	naveEnemiga2->coordenadas = vec3(-10.0f, 0.0f, -20.0f);
	naveEnemiga2->actualizarMatrizModelo();

	//Nave izquierda abajo
	naveEnemiga3->dibujar(GL_QUADS);
	naveEnemiga3->coordenadas = vec3(5.0f, 0.0f, -10.0f);
	naveEnemiga3->actualizarMatrizModelo();
}
void dibujarCasas()
{
	//casa der abajo
	casa->dibujar(GL_QUADS);
	casa->coordenadas = vec3(-10.0f, -2.0f, 10.0f);
	casa->actualizarMatrizModelo();

	//casa derecha arriba
	casa2->dibujar(GL_QUADS);
	casa2->coordenadas = vec3(10.0f, -2.0f, 60.0f);
	casa2->actualizarMatrizModelo();

	//casa izq abajo
	casa3->dibujar(GL_QUADS);
	casa3->coordenadas = vec3(-10.0f, -2.0f, 60.0f);
	casa3->actualizarMatrizModelo();

	//casa izq abajo
	casa4->dibujar(GL_QUADS);
	casa4->coordenadas = vec3(10.0f, -2.0f, 10.0f);
	casa4->actualizarMatrizModelo();
}
void dibujarEdificios()
{
	//Edificio izq abajo
	edificio->dibujar(GL_QUADS);
	edificio->coordenadas = vec3(-10.0f, -2.0f, -60.0f);
	edificio->actualizarMatrizModelo();

	//Edificio derecha abajo
	edificio2->dibujar(GL_QUADS);
	edificio2->coordenadas = vec3(10.0f, -2.0f, -40.0f);
	edificio2->actualizarMatrizModelo();

	//Edificio derecha arriba
	edificio3->dibujar(GL_QUADS);
	edificio3->coordenadas = vec3(10.0f, -2.0f, 40.0f);
	edificio3->actualizarMatrizModelo();

	//Edificio izq arriba
	edificio4->dibujar(GL_QUADS);
	edificio4->coordenadas = vec3(-10.0f, -2.0f, 40.0f);
	edificio4->actualizarMatrizModelo();
}
void dibujarFocos()
{
	foco->dibujar(GL_QUADS);
	foco->coordenadas = vec3(-10.0f, 0.4f, -60.0f);
	foco->actualizarMatrizModelo();

	foco2->dibujar(GL_QUADS);
	foco2->coordenadas = vec3(10.0f, 0.4f, -60.0f);
	foco2->actualizarMatrizModelo();

	foco3->dibujar(GL_QUADS);
	foco3->coordenadas = vec3(-10.0f, 0.4f, -40.0f);
	foco3->actualizarMatrizModelo();

	foco4->dibujar(GL_QUADS);
	foco4->coordenadas = vec3(10.0f, 0.4f, -40.0f);
	foco4->actualizarMatrizModelo();

	foco5->dibujar(GL_QUADS);
	foco5->coordenadas = vec3(-10.0f, 0.4f, -20.0f);
	foco5->actualizarMatrizModelo();

	foco6->dibujar(GL_QUADS);
	foco6->coordenadas = vec3(10.0f, 0.4f, -20.0f);
	foco6->actualizarMatrizModelo();

	foco7->dibujar(GL_QUADS);
	foco7->coordenadas = vec3(-10.0f, 0.4f, 0.0f);
	foco7->actualizarMatrizModelo();

	foco8->dibujar(GL_QUADS);
	foco8->coordenadas = vec3(10.0f, 0.4f, 0.0f);
	foco8->actualizarMatrizModelo();

	foco9->dibujar(GL_QUADS);
	foco9->coordenadas = vec3(-10.0f, 0.4f, 20.0f);
	foco9->actualizarMatrizModelo();

	foco10->dibujar(GL_QUADS);
	foco10->coordenadas = vec3(10.0f, 0.4f, 20.0f);
	foco10->actualizarMatrizModelo();

	foco11->dibujar(GL_QUADS);
	foco11->coordenadas = vec3(-10.0f, 0.4f, 40.0f);
	foco11->actualizarMatrizModelo();

	foco12->dibujar(GL_QUADS);
	foco12->coordenadas = vec3(10.0f, 0.4f, 40.0f);
	foco12->actualizarMatrizModelo();
}
void dibujarMeteoritos()
{
	//Meteorito medio
	meteorito->dibujar(GL_QUADS);
	meteorito->coordenadas = vec3(0.0f, 0.0f, 0.0f);
	meteorito->actualizarMatrizModelo();

	//Meteorito izq abajo
	meteorito2->dibujar(GL_QUADS);
	meteorito2->coordenadas = vec3(-7.0f, 0.0f, 5.0f);
	meteorito2->actualizarMatrizModelo();

	//Meteorito derecha abajo
	meteorito3->dibujar(GL_QUADS);
	meteorito3->coordenadas = vec3(7.0f, 0.0f, 7.0f);
	meteorito3->actualizarMatrizModelo();

	//Meteorito izq medioo
	meteorito4->dibujar(GL_QUADS);
	meteorito4->coordenadas = vec3(-5.0f, 0.0f, 10.0f);
	meteorito4->actualizarMatrizModelo();

	//Meteorito derecha arriba
	meteorito5->dibujar(GL_QUADS);
	meteorito5->coordenadas = vec3(5.0f, 0.0f, 15.0f);
	meteorito5->actualizarMatrizModelo();

	//Meteorito izq arriba
	meteorito6->dibujar(GL_QUADS);
	meteorito6->coordenadas = vec3(-10.0f, 0.0f, 17.0f);
	meteorito6->actualizarMatrizModelo();
}
void dibujarMeta()
{
	meta->dibujar(GL_QUADS);
	meta->coordenadas = vec3(0.0f, 2.5f, 70.0f);
	meta->actualizarMatrizModelo();
}

// Cerrado del juego al presionar escape
void teclado_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}