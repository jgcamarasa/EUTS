/* TODOS */
// - Shader management
// - Mesh management
// - Texture management
// - Model matrix will belong to an entity, not global to render state
// - Debug drawing (grid... axes...)
// - Memory management: Create a custom heap? Track the memory in some way?

#include "render.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "debug_render.h"

int main()
{

	EUTS_Window window;
	initWindow(&window);

	EUTS_RenderState renderState;
	initD3D11(&window, &renderState);
	
	EUTS_Mesh mesh;
	EUTS_Mesh_load(&mesh, &renderState, "../../../Resources/Meshes/cube.mesh");
	
	EUTS_ShaderConstants shaderConstants;
	EUTS_ShaderConstants_initialize(&shaderConstants, &renderState);

	EUTS_Shader shader;
	EUTS_Shader_initialize(&shader, &renderState, L"../../../Resources/Shaders/TextureVS.hlsl", L"../../../Resources/Shaders/TexturePS.hlsl", SHADER_FLAG_TEXTURE);
	EUTS_Texture texture;
	EUTS_Texture_load(&texture, &renderState, "../../../Resources/Textures/bird.png");
	
	EUTS_Camera camera;
	EUTS_Camera_setPosition(&camera, 6.0f, 6.0, -10.0f);
	EUTS_Camera_setRotation(&camera, 0.5f, -0.5f, 0.0f);

	EUTS_DebugRender_initialize(&renderState);

	// Loop
	MSG msg;
	bool done;


	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			EUTS_Render_beginFrame(&renderState);
			EUTS_Camera_update(&camera);

			EUTS_Mesh_bind(&mesh, &renderState);
			EUTS_Shader_bind(&shader, &renderState);
			EUTS_ShaderConstants_setSceneMatrices(&shaderConstants, &renderState, &(camera.viewMatrix), &(renderState.projectionMatrix));
			EUTS_ShaderConstants_setModelMatrix(&shaderConstants, &renderState, &(modelMatrix));
			EUTS_Render_setTexture(&renderState, texture.textureView);
			renderState.deviceContext->DrawIndexed(mesh.indexCount, 0, 0);

			EUTS_ShaderConstants_setModelMatrix(&shaderConstants, &renderState, &XMMatrixIdentity());
			EUTS_ShaderConstants_setColor(&shaderConstants, &renderState, &XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
			EUTS_DebugRender_drawLine(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(6.0f, 0.0f, 0.0f), &renderState);
			EUTS_ShaderConstants_setColor(&shaderConstants, &renderState, &XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
			EUTS_DebugRender_drawLine(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 6.0f, 0.0f), &renderState);
			EUTS_ShaderConstants_setColor(&shaderConstants, &renderState, &XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
			EUTS_DebugRender_drawLine(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 6.0f), &renderState);
			EUTS_Render_endFrame(&renderState);
		}

	}

	EUTS_DebugRender_finalize();
	EUTS_Texture_delete(&texture);
	EUTS_Mesh_finalize(&mesh);
	EUTS_Shader_finalize(&shader);
	EUTS_ShaderConstants_finalize(&shaderConstants);
	finalizeD3D11(&renderState);

	return 0;
}