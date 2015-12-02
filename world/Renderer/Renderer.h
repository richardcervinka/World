#pragma once

#include <d3d11.h>
#include "..\framework\Array.h"
#include "RenderInterface.h"

// forward deklarace
class Window;
class WinWindow;

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
// TENTO KOD JE POZUSTATEK Z PROTOTYPU, POTREBA KOMPLETRNE PREPSAT !
//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// parametry funkce Initialize()
struct RendererInitializeParams {
};

class Renderer {
public:
	Renderer();
	~Renderer();

	// Inicializace, je nutne volat pred prvnim pouzitim rendereru.
	bool Initialize( const RendererInitializeParams &params );

	// vytvori back buffer pro okno predane v parametru
	RenderInterface::BackBuffer *CreateWindowBackBuffer( Window &window );
	/*
	Zmeni velikost vsech G-Bufferu, depth stencil bufferu a back bufferu.
	Tuto funkci je nutne volat vzdy pri zmene velikosti okna.
	*/
	void ResizeBuffers( Window &window, const int width, const int height );

private:
	RenderInterface::Device *device;
	RenderInterface::CommandInterface *immediateCommandInterface;
	
	// G-Bffery deferred rendereru, color, normal
	enum {
		GBUFFER_DIFUSE,
		GBUFFER_NORMAL,
		GBUFFER_SPECULAR,
		GBUFFERS_COUNT
	};
	//RenderInterface:: *gbuffers[ GBUFFERS_COUNT ];

	// engine poskytuje pevnou sadu texture sampleru, defaultni je mozne modifikovat nastavenim
	enum {
		SAMPLER_DEFAULT,
		SAMPLER_POINT,
		SAMPLER_LINEAR,
		SAMPLERS_COUNT
	};
	RenderInterface::Sampler *samplers[ SAMPLERS_COUNT ];
};

/*
#include <d3d11.h>
#include "Core.h"
#include "VertexBuffer.h"
#include "Material.h"
#include "Texture.h"
#include "Vector.h"
#include "Matrix.h"
#include "Light.h"
#include "String.h"
#include "Shader.h"
#include "Mesh.h"
#include "ScreenPlanes.h"
#include "containers\Array.h"
*/