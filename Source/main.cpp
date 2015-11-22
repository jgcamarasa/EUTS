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
#include <imgui.h>
#include "imgui_impl_dx11.h"

int main()
{

	EUTS_Window window;
	initWindow(&window);

	EUTS_RenderState renderState;
	initD3D11(&window, &renderState);
	
	EUTS_Mesh islandMesh;
	EUTS_Mesh_load(&islandMesh, &renderState, "../../../Resources/Meshes/island.mesh");
	EUTS_Mesh treeMesh;
	EUTS_Mesh_load(&treeMesh, &renderState, "../../../Resources/Meshes/tree.mesh");
	EUTS_Mesh quadMesh;
	EUTS_Mesh_loadQuad(&quadMesh, &renderState);
	
	EUTS_ShaderConstants shaderConstants;
	EUTS_ShaderConstants_initialize(&shaderConstants, &renderState);

	EUTS_Shader shader;
	EUTS_Shader_initialize(&shader, &renderState, L"../../../Resources/Shaders/TextureVS.hlsl", L"../../../Resources/Shaders/TexturePS.hlsl", SHADER_FLAG_TEXTURE);
	EUTS_Shader blurHShader;
	EUTS_Shader_initialize(&blurHShader, &renderState, L"../../../Resources/Shaders/PostVS.hlsl", L"../../../Resources/Shaders/BlurHPS.hlsl", SHADER_FLAG_TEXTURE);
	EUTS_Shader blurVShader;
	EUTS_Shader_initialize(&blurVShader, &renderState, L"../../../Resources/Shaders/PostVS.hlsl", L"../../../Resources/Shaders/BlurVPS.hlsl", SHADER_FLAG_TEXTURE);
	
	EUTS_Texture islandTexture;
	EUTS_Texture_load(&islandTexture, &renderState, "../../../Resources/Textures/island.png");
	EUTS_Texture treeTexture;
	EUTS_Texture_load(&treeTexture, &renderState, "../../../Resources/Textures/tree.png");
	
	EUTS_Camera camera;
	EUTS_Camera_setDistance(&camera, 40.0f);
	EUTS_Camera_setTarget(&camera, 0.0f, 0.0f, 0.0f);
	EUTS_Camera_setAngles(&camera, 1.0f, 0.7f);

	EUTS_RenderTarget renderTarget;
	EUTS_RenderTarget_initialize(&renderTarget, &renderState, SCREEN_WIDTH, SCREEN_HEIGHT);
	EUTS_RenderTarget blurTarget;
	EUTS_RenderTarget_initialize(&blurTarget, &renderState, BLUR_WIDTH, BLUR_HEIGHT);

	EUTS_DebugRender_initialize(&renderState);

	XMFLOAT4 ambient(0.5, 0.5f, 0.8f, 1.0f);
	XMFLOAT4 sunDirection(0.0f, 1.f, -1.f, 1.0f);
	XMFLOAT4 sunColor(1.0f, 1.0f, 1.0f, 1.0f);

	float guiSunDirection[3];
	float guiSunColor[3];
	guiSunDirection[0] = sunDirection.x;
	guiSunDirection[1] = sunDirection.y;
	guiSunDirection[2] = sunDirection.z;
	guiSunColor[0] = sunColor.x;
	guiSunColor[1] = sunColor.y;
	guiSunColor[2] = sunColor.z;

	bool showDebugGui = true;
	bool depthOfField = true;

	// Loop
	MSG msg;
	bool done;

	ImGui_ImplDX11_Init(window.handler, renderState.device, renderState.deviceContext);

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

			static float cameraDistance = 50.0f;
			static float cameraHeight = 0.7f;
			static float cameraRotation = 1.0f;
			static bool  cameraAutoRotate = true;
			EUTS_Camera_setAngles(&camera, cameraRotation, cameraHeight);
			EUTS_Camera_setDistance(&camera, cameraDistance);
			if (cameraAutoRotate)
			{
				cameraRotation += 0.005f;
				if (cameraRotation > 3.14159)
				{
					cameraRotation = -3.14159;
				}
			}

			float color[4] = { 0.5f, 0.8f, 1.0f, 1.0f };
			// Main render target
			if (depthOfField)
			{
				EUTS_Render_setRenderTarget(&renderState, &renderTarget);
			}
			else
			{
				EUTS_Render_setDefaultRenderTarget(&renderState);
			}
			EUTS_ShaderConstants_setRenderTargetParameters(&shaderConstants, &renderState, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
			EUTS_Render_beginFrame(&renderState);
			EUTS_RenderTarget_clear(&renderTarget, &renderState, 0.5f, 0.8f, 1.0f, 1.0f);

			EUTS_Camera_update(&camera);

			EUTS_Shader_bind(&shader, &renderState);
			EUTS_ShaderConstants_setSceneMatrices(&shaderConstants, &renderState, &(camera.viewMatrix), &(renderState.projectionMatrix));
			XMVECTOR rotVector = XMLoadFloat3(&XMFLOAT3(1.0f, 0.0f, 0.0f));
			static float rot = 0.0f;
			XMMATRIX modelMatrix = XMMatrixRotationAxis(rotVector, rot);
			rot += 0.00f;
			EUTS_ShaderConstants_setModelMatrix(&shaderConstants, &renderState, &(modelMatrix));
			EUTS_ShaderConstants_setLightParameters(&shaderConstants, &renderState, &sunDirection, &sunColor, &ambient);

			EUTS_Mesh_bind(&treeMesh, &renderState);
			EUTS_Render_bindTexture(&renderState, treeTexture.textureView, 0);
			renderState.deviceContext->DrawIndexed(treeMesh.indexCount, 0, 0);
			EUTS_Mesh_bind(&islandMesh, &renderState);
			EUTS_Render_bindTexture(&renderState, islandTexture.textureView, 0);
			renderState.deviceContext->DrawIndexed(islandMesh.indexCount, 0, 0);

			EUTS_ShaderConstants_setModelMatrix(&shaderConstants, &renderState, &XMMatrixIdentity());
			EUTS_ShaderConstants_setColor(&shaderConstants, &renderState, &XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
			EUTS_DebugRender_drawLine(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(6.0f, 0.0f, 0.0f), &renderState);
			EUTS_ShaderConstants_setColor(&shaderConstants, &renderState, &XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
			EUTS_DebugRender_drawLine(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 6.0f, 0.0f), &renderState);
			EUTS_ShaderConstants_setColor(&shaderConstants, &renderState, &XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
			EUTS_DebugRender_drawLine(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 6.0f), &renderState);

			if (depthOfField)
			{
				EUTS_Render_setRenderTarget(&renderState, &blurTarget);
				EUTS_ShaderConstants_setRenderTargetParameters(&shaderConstants, &renderState, (float)BLUR_WIDTH, (float)BLUR_HEIGHT);
				EUTS_RenderTarget_clear(&blurTarget, &renderState, 0.5f, 0.8f, 1.0f, 1.0f);
				EUTS_Mesh_bind(&quadMesh, &renderState);
				EUTS_Shader_bind(&blurVShader, &renderState);
				EUTS_Render_bindTexture(&renderState, renderTarget.shaderResourceView, 0);
				renderState.deviceContext->DrawIndexed(quadMesh.indexCount, 0, 0);

				EUTS_Render_setDefaultRenderTarget(&renderState);
				//renderState.deviceContext->RSSetState(renderState.rasterState);
				//renderState.deviceContext->OMSetDepthStencilState(renderState.depthStencilState, 1);
				renderState.deviceContext->ClearRenderTargetView(renderState.renderTargetView, color);
				renderState.deviceContext->ClearDepthStencilView(renderState.depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
				EUTS_Shader_bind(&blurHShader, &renderState);

				EUTS_Render_bindTexture(&renderState, blurTarget.shaderResourceView, 0);
				EUTS_Render_bindTexture(&renderState, renderTarget.shaderResourceView, 1);
				renderState.deviceContext->DrawIndexed(quadMesh.indexCount, 0, 0);
				EUTS_Render_unbindTexture(&renderState, 1);
			}

			if (showDebugGui)
			{
				ImGui_ImplDX11_NewFrame();
				{
					ImGui::Begin("Camera", 0, 0);
					ImGui::SliderFloat("Camera Distance", &cameraDistance, 5.0f, 60.0f);
					ImGui::SliderAngle("Camera Rotation", &cameraRotation, -180.0f, 180.0f);
					ImGui::SliderAngle("Camera Height", &cameraHeight, -80.0f, 80.0f);
					ImGui::Checkbox("Auto Rotate", &cameraAutoRotate);
					ImGui::End();
				}
				{
					ImGui::Begin("Graphics", 0, 0);
					ImGui::SliderFloat3("Sun Direction", guiSunDirection, -1.0f, 1.0f);
					ImGui::ColorEdit3("Sun Color", guiSunColor);
					ImGui::Checkbox("Depth Of Field", &depthOfField);
					ImGui::End();
				}
				{
					ImGui::Begin("Profiling", 0, 0);
					ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
					ImGui::Text("Total Video Memory: %f MB", renderState.videoMemoryInMB);
					ImGui::End();
				}
			}
			sunDirection.x = guiSunDirection[0];
			sunDirection.y = guiSunDirection[1];
			sunDirection.z = guiSunDirection[2];
			sunColor.x = guiSunColor[0];
			sunColor.y = guiSunColor[1];
			sunColor.z = guiSunColor[2];
			

			ImGui::Render();
			EUTS_Render_endFrame(&renderState);
		}

	}

	EUTS_DebugRender_finalize();
	EUTS_RenderTarget_finalize(&renderTarget);
	EUTS_RenderTarget_finalize(&blurTarget);
	EUTS_Texture_delete(&treeTexture);
	EUTS_Texture_delete(&islandTexture);
	EUTS_Mesh_finalize(&quadMesh);
	EUTS_Mesh_finalize(&treeMesh);
	EUTS_Mesh_finalize(&islandMesh);
	EUTS_Shader_finalize(&shader);
	EUTS_Shader_finalize(&blurHShader);
	EUTS_Shader_finalize(&blurVShader);
	EUTS_ShaderConstants_finalize(&shaderConstants);
	finalizeD3D11(&renderState);

	return 0;
}