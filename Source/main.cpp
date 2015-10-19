/* TODOS */
// - Separate shader loading and creation from input layout initialization.
// - Some shader checking (i.e, not being able to set parameters of one shader type to a shader of another type)
// - Model matrix will belong to an entity, not global to render state
// - Memory management: Create a custom heap? Track the memory in some way?

#include "render.h"
#include "mesh.h"
#include "shader.h"

int main()
{

	EUTS_Window window;
	initWindow(&window);

	EUTS_RenderState renderState;
	initD3D11(&window, &renderState);
	
	EUTS_Mesh mesh;
	EUTS_Mesh_initialize(&mesh, &renderState);
	
	EUTS_Shader shader;
	EUTS_Shader_initialize(&shader, &renderState, L"../../../Resources/Shaders/ColorVS.hlsl", L"../../../Resources/Shaders/ColorPS.hlsl", sizeof(EUTS_VSMatrixConstantBuffer));
	
	EUTS_Camera camera;
	EUTS_Camera_setPosition(&camera, 0.0f, 0.0, -5.0f);
	EUTS_Camera_setRotation(&camera, 0.0f, 0.0f, 0.0f);

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
			beginScene(&renderState);
			EUTS_Camera_update(&camera);

			EUTS_Mesh_bind(&mesh, &renderState);
			EUTS_Shader_bind(&shader, &renderState);
			EUTS_Shader_Color_setParameters(&shader, &renderState, &(renderState.modelMatrix), &(camera.viewMatrix), &(renderState.projectionMatrix));

			renderState.deviceContext->DrawIndexed(mesh.indexCount, 0, 0);
			endScene(&renderState);
		}

	}

	EUTS_Mesh_finalize(&mesh);
	EUTS_Shader_finalize(&shader);
	finalizeD3D11(&renderState);

	return 0;
}