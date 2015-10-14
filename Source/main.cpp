/* TODOS */
// - Memory management: Create a custom heap? Track the memory in some way?

#include "render.h"


int main()
{

	EUTS_Window window;
	initWindow(&window);

	EUTS_RenderState renderState;
	initD3D11(&window, &renderState);
	
	EUTS_Mesh mesh;

	EUTS_Mesh_initialize(&mesh, &renderState);
	EUTS_Mesh_bind(&mesh, &renderState);
	EUTS_Mesh_finalize(&mesh);

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
			render(&renderState);
		}

	}

	finalizeD3D11(&renderState);

	return 0;
}