/*
 * Copyright 2009-2011 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */
#ifdef WIN32 // win32 only

#include "D3D9WPFRenderer.h"
#include "SamplePlatform.h"

#define APP_NAME "D3D9WPFRenderer"

namespace SampleRenderer
{
	

	static volatile LONG classRefCount = 0;

	static LONG incrementClassReference()
	{
		return InterlockedIncrement( &classRefCount );
	}
	static LONG decrementClassReference()
	{
		return InterlockedDecrement( &classRefCount );
	}
		
	D3D9WPFRenderer::D3D9WPFRenderer(  HWND hWnd
									, IDirect3D9*	inDirect3d
									, IDirect3DDevice9* inDevice
									, const char* devName
									, PxU32 dispWidth
									, PxU32 dispHeight
									, const char* assetDir
									, bool lockable
									, bool isDeviceEx )
		: D3D9Renderer( inDirect3d, devName, 0, 0, inDevice, isDeviceEx, assetDir )
		, mHwnd( hWnd )
		, mDesiredWidth( dispWidth )
		, mDesiredHeight( dispHeight )
		, mCanonicalSurface( NULL )
		, mLockable( lockable )
		, mListener( NULL )
	{
		SampleFramework::createPlatform(NULL);
	}

	D3D9WPFRenderer::~D3D9WPFRenderer()
	{
		releaseSurface();
		DestroyWindow(mHwnd); 
		if ( mListener ) mListener->release(); mListener = NULL;
		if ( decrementClassReference() == 0 )
			UnregisterClass(TEXT(APP_NAME), NULL);
	}
	
	WPFRendererListener* D3D9WPFRenderer::getListener() { return mListener; }
	void D3D9WPFRenderer::setListener( WPFRendererListener* listener )
	{
		if ( mListener != listener )
		{
			if ( mListener ) mListener->release(); mListener = NULL;
			mListener = listener;
			if ( mCanonicalSurface && mListener ) mListener->afterSurfaceCreated( mCanonicalSurface );
		}
	}

	void D3D9WPFRenderer::onResize( PxU32 newWidth, PxU32 newHeight )
	{
		mDesiredWidth = newWidth;
		mDesiredHeight = newHeight;
		checkResize( false );
	}

	bool D3D9WPFRenderer::swapBuffers()
	{
		return true; //does nothing, that is up for the parent context to do.
	}

	bool D3D9WPFRenderer::isOk() const
	{
		return true;}
	
	void D3D9WPFRenderer::onDeviceLost()
	{
		releaseSurface();
		D3D9Renderer::onDeviceLost();
	}

	void D3D9WPFRenderer::onDeviceReset()
	{
		allocateSurface();
		D3D9Renderer::onDeviceReset();
	}

	void D3D9WPFRenderer::releaseSurface()
	{
		if( mListener ) mListener->beforeSurfaceRelease( mCanonicalSurface );
		if ( mCanonicalSurface ) mCanonicalSurface->Release();
		mCanonicalSurface = NULL;
	}

	void D3D9WPFRenderer::allocateSurface()
	{
		PX_ASSERT( mCanonicalSurface == NULL );

		HRESULT result = m_d3dDevice->CreateRenderTarget(
				m_displayWidth
				, m_displayHeight 
				, D3DFMT_X8R8G8B8
				, D3DMULTISAMPLE_NONE
				, 0
				, mLockable
				, &mCanonicalSurface
				, NULL );
		PX_ASSERT( mCanonicalSurface );
		PX_ASSERT( result == D3D_OK );

		if ( mCanonicalSurface != NULL )
			m_d3dDevice->SetRenderTarget(0, mCanonicalSurface);

		if ( mListener ) mListener->afterSurfaceCreated( mCanonicalSurface );
	}

	bool D3D9WPFRenderer::checkResize( bool isDeviceLost )
	{
		bool isDeviceReset = false;
		//resize the system if the desired width or height is more than we can support.
		if ( mDesiredWidth > m_displayWidth
			|| mDesiredHeight > m_displayHeight
			|| isDeviceLost )
		{
			m_displayWidth = PxMax( mDesiredWidth, m_displayWidth );
			m_displayHeight = PxMax( mDesiredHeight, m_displayHeight );
			if ( isDeviceLost )
			{
				physx::PxU64 res = m_d3dDevice->TestCooperativeLevel();
				if(res == D3D_OK || res == D3DERR_DEVICENOTRESET)	//if device is lost, device has to be ready for reset
				{
					isDeviceReset = true;
					onDeviceLost();
					onDeviceReset();
				}
			}
			else
			{
				releaseSurface();
				releaseDepthStencilSurface();
				allocateSurface();
				buildDepthStencilSurface();
				// set out initial states...
				m_d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
				m_d3dDevice->SetRenderState(D3DRS_LIGHTING, 0);
				m_d3dDevice->SetRenderState(D3DRS_ZENABLE,  1);
			}
		}
		//else just mess with the viewport so we only render pixel-by-pixel
		D3DVIEWPORT9 viewport = {0};
		m_d3dDevice->GetViewport(&viewport);
		if ( viewport.Width != mDesiredWidth
			|| viewport.Height != mDesiredHeight )
		{
			viewport.X = 0;
			viewport.Y = 0;
			viewport.Width  = (DWORD)mDesiredWidth;
			viewport.Height = (DWORD)mDesiredHeight;
			viewport.MinZ =  0.0f;
			viewport.MaxZ =  1.0f;
			m_d3dDevice->SetViewport(&viewport);
		}
		return isDeviceReset;
	}
	
	void D3D9WPFRenderer::setupAlphaBlendState()
	{
		m_d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,		1);
		m_d3dDevice->SetRenderState(D3DRS_SRCBLEND,				D3DBLEND_ONE);
		m_d3dDevice->SetRenderState(D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA);
	}

	void D3D9WPFRenderer::restoreAlphaBlendState()
	{
		m_d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,  0);
	}

	void D3D9WPFRenderer::disableZWrite()
	{
		m_d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,  0);
	}
	void D3D9WPFRenderer::enableZWrite()
	{
		m_d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,  1);
	}
	
	static DWORD GetVertexProcessingCaps(IDirect3D9* d3d)
	{
		D3DCAPS9 caps;
		DWORD dwVertexProcessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		if (SUCCEEDED(d3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
		{
			if ((caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == D3DDEVCAPS_HWTRANSFORMANDLIGHT)
			{
				dwVertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
			}
		}
		return dwVertexProcessing;
	}


	HRESULT InitializeD3D(HWND hWnd, D3DPRESENT_PARAMETERS d3dpp, IDirect3D9*& d3d, IDirect3DDevice9*& d3dDevice )
	{
		d3d = NULL;
		d3dDevice = NULL;
		// initialize Direct3D
		d3d = Direct3DCreate9(D3D_SDK_VERSION);
		if ( d3d != NULL )
		{
			// determine what type of vertex processing to use based on the device capabilities
			DWORD dwVertexProcessing = GetVertexProcessingCaps(d3d);
			// create the D3D device
			return d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
									dwVertexProcessing | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
									&d3dpp, &d3dDevice);
		}
		return -1;
	}

	typedef HRESULT  (WINAPI *D3DEXCreateFn)( DWORD version, IDirect3D9Ex** result );


	HRESULT InitializeD3DEx(HWND hWnd, D3DPRESENT_PARAMETERS d3dpp, IDirect3D9*& d3d, IDirect3DDevice9*& d3dDevice, D3DEXCreateFn createFn )
	{
		d3d = NULL;
		d3dDevice = NULL;
		IDirect3D9Ex* d3dEx = NULL;
		// initialize Direct3D using the Ex function
		HRESULT result = createFn( D3D_SDK_VERSION, &d3dEx );
		if ( FAILED( result ) ) return result;

		result = d3dEx->QueryInterface(__uuidof(IDirect3D9), reinterpret_cast<void **>(&d3d) );	
		if ( FAILED( result ) ) { d3dEx->Release(); return result; }
	
		// determine what type of vertex processing to use based on the device capabilities
		DWORD dwVertexProcessing = GetVertexProcessingCaps(d3d);

		// create the D3D device using the Ex function
		IDirect3DDevice9Ex* deviceEx = NULL;
		result = d3dEx->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
				dwVertexProcessing | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
				&d3dpp, NULL, &deviceEx);

		if ( FAILED(result) ) { d3dEx->Release(); d3d->Release();  return result; }
		
		// obtain the standard D3D device interface
		deviceEx->QueryInterface(__uuidof(IDirect3DDevice9), reinterpret_cast<void **>(&d3dDevice) );

		deviceEx->Release();
		d3dEx->Release();

		return 0;
	}

	D3D9WPFRenderer* D3D9WPFRenderer::createWPFRenderer( const char* inAssetDir, PxU32 initialWidth, PxU32 initialHeight )
	{
		if ( incrementClassReference() == 1 )
		{
			WNDCLASS wndclass;
			memset( &wndclass, 0, sizeof( WNDCLASS ) );

			wndclass.style = CS_HREDRAW | CS_VREDRAW;
			wndclass.lpfnWndProc = DefWindowProc;
			wndclass.lpszClassName = TEXT(APP_NAME);

			RegisterClass(&wndclass);
		}

        HWND hWnd = CreateWindow(TEXT(APP_NAME),
                            TEXT(APP_NAME),
                            WS_OVERLAPPEDWINDOW,
                            0,                   // Initial X
                            0,                   // Initial Y
                            0,                   // Width
                            0,                   // Height
                            NULL,
                            NULL,
                            NULL,
                            NULL);
		if ( hWnd == 0 ) return NULL;

		HMODULE hD3D9 = LoadLibrary(TEXT("d3d9.dll"));
		D3DEXCreateFn exCreateFn = reinterpret_cast<D3DEXCreateFn>( GetProcAddress(hD3D9, "Direct3DCreate9Ex") );
		bool is9x = exCreateFn != NULL;
		IDirect3D9*			d3d = NULL;
		IDirect3DDevice9*	d3dDevice = NULL;
		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.Windowed = TRUE;
		d3dpp.BackBufferHeight = 1;
		d3dpp.BackBufferWidth = 1;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
		d3dpp.hDeviceWindow = hWnd;
		HRESULT result;

		if (is9x)
		{
			result = InitializeD3DEx(NULL, d3dpp, d3d, d3dDevice, exCreateFn);
		}
		else
		{
			result = InitializeD3D(NULL, d3dpp, d3d, d3dDevice);
		}
		PX_ASSERT( result >= 0 );
		if ( result >= 0 )
		{
			bool lockable = !is9x; 
			//We only lock earlier interface, not later ones because the later ones use a hardware transfer mechanism
			//where you seem to not need to lock according to some...
			D3D9WPFRenderer* retval = new D3D9WPFRenderer( hWnd, d3d, d3dDevice, "Direct3d", initialWidth, initialHeight, inAssetDir, !is9x, is9x );
			retval->checkResize( false );	//setup viewport.
			//Can we create a vertex buffer here?
			PX_ASSERT( result == D3D_OK );
			return retval;
		}
		return NULL;
	}
}

#endif