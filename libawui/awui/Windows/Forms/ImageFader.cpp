// (c) Copyright 2024 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "ImageFader.h"

#include <awui/Drawing/ColorF.h>
#include <awui/Windows/Forms/Bitmap.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms;

ImageFader::ImageFader() {
	m_class = Classes::ImageFader;
	m_imageShowing = nullptr;
	m_state = State::FadeIn;
	m_percentage = 0.0f;
	m_speedFadeOut = 5.0f;
	m_speedFadeIn = 2.0f;
	m_color = Drawing::ColorF::FromArgb(1.0f, 1.0f, 1.0f, 1.0f);
}

bool ImageFader::IsClass(Classes objectClass) const {
	return (objectClass == Classes::ImageFader) || Control::IsClass(objectClass);
}

void ImageFader::SetColor(Drawing::ColorF color) {
	m_color = color;
}

void ImageFader::SetImage(Bitmap *image) {
	// Caso rápido para cuando se establece la misma imagen que se está mostrando actualmente.
	if (image == m_imageShowing) {
		if (m_state == State::FadeOut) {
			m_state = State::FadeIn;
		}
		// No hacer nada si la imagen es la misma y está en pausa o FadeIn.
		return;
	}

	// Asignar la nueva imagen a mostrar y cambiar el estado.
	m_imageToShow = image;

	if (m_imageShowing == nullptr) {
		// Si no hay imagen actualmente mostrándose, asignar directamente y pausar.
		m_imageShowing = image;
		m_state = State::FadeIn;
	} else {
		// Si hay una imagen mostrándose, iniciar el FadeOut para la transición.
		m_state = State::FadeOut;
	}
}

void ImageFader::OnTick(float deltaSeconds) {
	if (m_imageShowing) {
		m_imageShowing->SetSize(GetWidth(), GetHeight());
	}

	switch (m_state) {
		case State::Paused:
			break;
		case State::FadeOut:
			m_percentage -= m_speedFadeOut * deltaSeconds;
			if (m_percentage <= 0.0f) {
				m_percentage = 0.0f;
				m_state = State::FadeIn;
				m_imageShowing = m_imageToShow;
				m_imageToShow = nullptr;
			}
			break;
		case State::FadeIn:
			m_percentage += m_speedFadeIn * deltaSeconds;
			if (m_percentage >= 1.0f) {
				m_percentage = 1.0f;
				m_state = State::Paused;
			}
			break;
	}
}

void ImageFader::OnPaint(GL *gl) {
	if (m_imageShowing) {
		m_imageShowing->SetColor(ColorF::FromArgb(m_percentage * m_color.GetA(), m_color.GetR(), m_color.GetG(), m_color.GetB()));
		m_imageShowing->OnPaint(gl);
	}
}
