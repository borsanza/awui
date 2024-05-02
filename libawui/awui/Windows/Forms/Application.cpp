/**
 * awui/Windows/Forms/Application.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Application.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>
#include <awui/ChronoLap.h>
#include <awui/Console.h>
#include <awui/Convert.h>
#include <awui/GOB/Engine/Materials/MeshBasicMaterial.h>
#include <awui/GOB/Engine/Math/Matrix4.h>
#include <awui/GOB/Engine/Shaders/Shader.h>
#include <awui/GOB/Engine/Textures/Texture.h>
#include <awui/Math.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/Joystick/Controller.h>
#include <awui/Windows/Forms/Statistics/Stats.h>
#include <iostream>

using namespace awui;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms;
using namespace awui::GOB::Engine;
using namespace awui::Windows::Forms::Statistics;

int Application::quit = 0;

Application::Application() {
	m_class = Classes::Application;
}

bool Application::IsClass(Classes objectClass) const {
	return (objectClass == Classes::Application) || Object::IsClass(objectClass);
}

void Application::Quit() {
	Application::quit = 1;
}

void Application::Run(Form *form = NULL) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
		SDL_Log("[ERROR] SDL_Init failed: %s", SDL_GetError());
		return;
	}

	if (!IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG)) {
		SDL_Log("SDL_image could not initialize! SDL_image Error: %s", IMG_GetError());
		return;
	}

	// Me he quedado en OpenGL 3.1, hay que ver de ir a 3.2
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	Joystick::Controller::Refresh();

	form->Init();

	atexit(SDL_Quit);

	Stats *stats = Stats::Instance();

	ChronoLap chronoLap;

	chronoLap.Start();

	glEnable(GL_MULTISAMPLE);

	// Lo inicializo en una frecuencia de 60Hz
	float lastDeltaSeconds = 1.0f / 60.0f;
	while (!Application::quit) {
		ProcessEvents();

		chronoLap.Lap();
		float deltaSeconds = chronoLap.GetLapDuration();

		// Se comporta mejor en fullscreen si amortiguo el deltaseconds
		lastDeltaSeconds = Math::Interpolate(lastDeltaSeconds, deltaSeconds, 0.2, false);
		// lastDeltaSeconds = 1.0 / 60.0;

		form->OnTickPre(lastDeltaSeconds);

		glViewport(0, 0, form->GetWidth(), form->GetHeight());
		glClearColor(form->GetBackColor().GetR() / 255.0f, form->GetBackColor().GetG() / 255.0f, form->GetBackColor().GetB() / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		form->OnPaintForm();

		/*
				int top = 640;
				for (int i = -top; i < 0; i++) {
					for (int j = -top; j < 0; j++) {
						OpenGL::GL::FillRectangle(i << 3, j << 3, (i << 3) + 5, (j << 3) + 5);
					}
				}
		*/

		stats->SetTimeBeforeIddle();
		form->SwapGL();
		stats->SetTimeAfterIddle();
	}

	Joystick::Controller::CloseAll();

	SDL_Quit();
}

int Application::Test() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) { // Inicializar SDL2
		std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	if (!IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG)) {
		std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
		return -1;
	}

	// Configurar SDL para usar OpenGL 3.3 core profile
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	// Crear ventana
	SDL_Window *window = SDL_CreateWindow("SDL2 OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!window) {
		std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return -1;
	}

	// Crear contexto OpenGL
	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	if (!glContext) {
		std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	// Bucle principal (puede mantener la ventana abierta hasta que el usuario cierra la ventana)
	bool quit = false;
	SDL_Event e;

	glewInit();

	SDL_GL_SetSwapInterval(true);

	glEnable(GL_MULTISAMPLE);

	// Shader *shader = new Shader("shader-330.vs", "shader-330.fs");
	float vertices[] = {
		// Posiciones    // UVs
		0.0f,  0.5f,  0.0f, 0.5f, 1.0f, // Top
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // Left
		0.5f,  -0.5f, 0.0f, 1.0f, 0.0f	// Right
	};

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind the Vertex Array Object
	glBindVertexArray(VAO);

	// Bind and set vertex buffer(s)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
	glEnableVertexAttribArray(0);

	// Texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Unbind VBO and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Texture *test = new Texture("./images/block-dirt.png", Texture::TEXTURE_NEAREST, Texture::TEXTURE_NEAREST);
	MeshBasicMaterial *m_basic = new MeshBasicMaterial(test, false);

	m_basic->ApplyMaterial();

	Matrix4 identity = Matrix4::Identity();
	GLuint shaderProgram = test->GetProgram();
	GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, identity.data());
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, identity.data());
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, identity.data());

	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
		}

		// Limpia el color y el buffer de profundidad
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Dibujar el triángulo
		// shader->Use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		// shader->Unuse();

		// Intercambia los buffers delantero y trasero
		SDL_GL_SwapWindow(window); // Intercambiar buffers
	}
	m_basic->UnApplyMaterial();

	// Limpiar
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

void Application::ProcessEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		bool ret = false;
		// Console::WriteLine(String("Event [") + Convert::ToString((int)event.type) + "]");
		for (int i = 0; i < Form::m_formsList->GetCount(); i++) {
			Form *formW = (Form *) Form::m_formsList->Get(i);
			switch (event.type) {
				case SDL_JOYDEVICEADDED:
				case SDL_JOYDEVICEREMOVED:
					ret = true;
					break;
				case SDL_CONTROLLERDEVICEADDED:
				case SDL_CONTROLLERDEVICEREMOVED:
					Joystick::Controller::Refresh();
					ret = true;
					break;
				case SDL_CONTROLLERAXISMOTION: {
					// Console::WriteLine(String("SDL_CONTROLLERAXISMOTION [") + Convert::ToString((int)event.caxis.axis) + " - " + Convert::ToString((int)event.caxis.value) + "]");
					Joystick::Controller *controller = Joystick::Controller::GetByWhich(event.cbutton.which);
					if (controller) {
						controller->OnAxisMotion(event.caxis.axis, event.caxis.value);
						formW->OnJoystickAxisMotionPre(controller->GetOrder(), controller->GetAxisX(), controller->GetAxisY());
						ret = true;
					}
					break;
				}
				case SDL_CONTROLLERBUTTONDOWN: {
					Joystick::Controller *controller = Joystick::Controller::GetByWhich(event.cbutton.which);
					if (controller) {
						controller->OnButtonDown(2 << event.cbutton.button);
						formW->OnJoystickButtonDownPre(controller->GetOrder(), 2 << event.cbutton.button, controller->GetButtons(), controller->GetPrevButtons());
						ret = true;
					}
					break;
				}
				case SDL_CONTROLLERBUTTONUP: {
					Joystick::Controller *controller = Joystick::Controller::GetByWhich(event.cbutton.which);
					if (controller) {
						controller->OnButtonUp(2 << event.cbutton.button);
						formW->OnJoystickButtonUpPre(controller->GetOrder(), 2 << event.cbutton.button, controller->GetButtons(), controller->GetPrevButtons());
						ret = true;
					}
					break;
				}
				// TODO: Mostrar la bateria del Joystick
				case SDL_JOYBATTERYUPDATED:
					ret = true;
					break;
				// Ya las he implementado como Controllers
				case SDL_JOYHATMOTION:
				case SDL_JOYAXISMOTION:
				case SDL_JOYBUTTONDOWN:
				case SDL_JOYBUTTONUP:
					ret = true;
					break;
			}
		}

		if (ret) {
			continue;
		}

		Uint32 windowID = 0;
		switch (event.type) {
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				windowID = event.key.windowID;
				break;
			case SDL_MOUSEWHEEL:
				windowID = event.wheel.windowID;
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				windowID = event.button.windowID;
				break;
			case SDL_MOUSEMOTION:
				windowID = event.motion.windowID;
				break;
			case SDL_WINDOWEVENT:
				windowID = event.window.windowID;
				break;
		}

		if (windowID != 0) {
			for (int i = 0; i < Form::m_formsList->GetCount(); i++) {
				Form *formW = (Form *) Form::m_formsList->Get(i);
				if (windowID == formW->GetWindowID()) {
					formW->ProcessEvents(&event);
				}
			}

			continue;
		}

		switch (event.type) {
			case SDL_QUIT:
				Application::Quit();
				break;

			default:
				// Console::WriteLine("Event [%d]", event.type);
				break;
		}
	}
}
