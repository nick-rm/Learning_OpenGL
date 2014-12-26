#pragma comment (lib,"opengl32.lib")
#include <windows.h> 
#include <glew.h>
#include <wglew.h>
#include <stdio.h>
#include <glm.hpp>
#include <ext.hpp>
#include "shaders.h"
#include "meshobj.h"


HINSTANCE               g_hInst = NULL;
HWND                    g_hWnd = NULL;
HGLRC					hRC = NULL;

MeshFromObj*			Mesh = NULL;

bool keys[256];
float dorbit = 0.0f;
float dorbit1 = 0.0f;
bool doAnimation = false;
int polygonMode = GL_LINE;
wchar_t filename[MAX_PATH]=L"";
wchar_t filename1[MAX_PATH]=L"";
wchar_t filename2[MAX_PATH]=L"";
bool isLoaded1 = false;
bool isLoaded2 = false;

//Functions declaration
HRESULT WindowInit( HINSTANCE hInstance, int nCmdShow );
HRESULT CompatContextInit();
HRESULT ContextInit();
void GeometrySet();
void ContextClean();
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
void Render();
void FileName(wchar_t * fname);


//Main Program's function
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

    if( FAILED( WindowInit( hInstance, nCmdShow ) ) )
        return 0;

	if( FAILED( CompatContextInit() ) )
		return 0;

	if( FAILED( ContextInit() ) )
    {
        ContextClean();
        return 0;
    }
		GeometrySet();
	
    //Messages' processing loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
		else if (isLoaded1 && isLoaded2)
		{
			GeometrySet();
			isLoaded2 = isLoaded1 = false;
		}
        else
        {
			Render();
			
			if(!keys[VK_LEFT])								
				dorbit = 0.0f;

			if(!keys[VK_RIGHT])								
				dorbit = 0.0f;								
			
			if(keys[VK_RIGHT])								
				dorbit = -0.01f;								

			if(keys[VK_LEFT])								
				dorbit = 0.01f;
			//------------------
			if(keys[VK_DOWN])								
				dorbit1 += 0.01f;								

			if(keys[VK_UP])								
				dorbit1 -= 0.01f;
			//------------------
			if(keys[VK_SPACE])
				doAnimation = true;

			if(keys['W'])
				polygonMode = GL_LINE;

			if(keys['S'])
				polygonMode = GL_FILL;

        }
    }

    ContextClean();

    return ( int )msg.wParam;
}


//Window creation
HRESULT WindowInit( HINSTANCE hInstance, int nCmdShow )
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = 0; // LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"Task1";
    wcex.hIconSm = 0; // LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    if( !RegisterClassEx( &wcex ) )
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, 533, 400 };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow( L"Task1", L"Task1. Plane Morphing", WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
                           NULL );
    if( !g_hWnd )
        return E_FAIL;

    ShowWindow( g_hWnd, nCmdShow );

	HMENU FileMenu;
	HMENU FileSubMenu;
	FileMenu = CreateMenu();
	FileSubMenu = CreateMenu();
	AppendMenu(FileMenu,MF_POPUP,(UINT_PTR)FileSubMenu,L"File");
	AppendMenu(FileSubMenu, MF_STRING, 1, L"Open File1");
	AppendMenu(FileSubMenu, MF_STRING, 2, L"Open File2");

    AppendMenu(FileMenu, MF_STRING, 3, L"Help");
	SetMenu(g_hWnd, FileMenu);

    return S_OK;
}

//Processng of Windows messages 
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

	OPENFILENAME of;
	wchar_t* buf;
	HANDLE hf;
	DWORD len,len1;

	ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lStructSize=OPENFILENAME_SIZE_VERSION_400A;
    of.hwndOwner=hWnd;
    of.lpstrFilter=L".obj Files (*.obj)\0*.obj\0";
    of.lpstrCustomFilter=NULL; of.nMaxCustFilter=0;
    of.nFilterIndex=1;
    of.lpstrFile=(LPWSTR)filename; of.nMaxFile=MAX_PATH;
    of.lpstrFileTitle=NULL; of.nMaxFileTitle=0;
    of.lpstrInitialDir=NULL;
    of.Flags=OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY;

    switch( message )
    {
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

		case WM_KEYDOWN:										
		{
			keys[wParam] = TRUE;								
			return 0;											
		}

		case WM_KEYUP:											
		{
			keys[wParam] = FALSE;								
			return 0;											
		}

		case WM_COMMAND:
		{
			if (wParam == 1)
            {
				if (!GetOpenFileName(&of)) return 0;
				hf=CreateFile((LPWSTR)filename,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);

				if (hf==INVALID_HANDLE_VALUE) {
				 MessageBox(hWnd,L"Open failed", L"Error",MB_ICONHAND|MB_OK);
				 break;
				}
				len=GetFileSize(hf,NULL);
				buf=(wchar_t*)malloc(len+1);
				if (!buf) {
				 MessageBox(hWnd,L"Mem allocation failed",L"Error",MB_ICONHAND|MB_OK);
				 break;
				}
				wcscpy(filename1, filename);
				FileName(filename1);
				isLoaded1 = true;
				CloseHandle(hf);
				free(buf);
				return 0;
            }

			if (wParam == 2)
             {
               if (!GetOpenFileName(&of)) return 0;
			   hf=CreateFile((LPWSTR)filename,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);

				if (hf==INVALID_HANDLE_VALUE) 
				{
				 MessageBox(hWnd,L"Open failed", L"Error",MB_ICONHAND|MB_OK);
				 break;
				}
				len=GetFileSize(hf,NULL);
				buf=(wchar_t*)malloc(len+1);
				if (!buf) {
				 MessageBox(hWnd,L"Mem allocation failed",L"Error",MB_ICONHAND|MB_OK);
				 break;
				}
				wcscpy(filename2, filename);
				FileName(filename2);
				isLoaded2 = true;
				CloseHandle(hf);
				free(buf);
				return 0;
             }
			
			if (wParam == 3)
             {
               MessageBox(hWnd,
				   L"Controls:\n\n* Rotattion of the scene: Arrows <Left>, <Right>, <Up>, <Down>.\n* Type of view: Button <W> - Wireframe, Button <S> - Solid.",
				   L"Help: Controls Information",MB_OK|MB_ICONINFORMATION);
			   return 0;
             }
		}

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}

LRESULT CALLBACK WndProc2( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

	OPENFILENAME of;
	wchar_t* buf;
	HANDLE hf;
	DWORD len,len1;

	ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lStructSize=OPENFILENAME_SIZE_VERSION_400A;
    of.hwndOwner=hWnd;
    of.lpstrFilter=L".obj Files (*.obj)\0*.obj\0";
    of.lpstrCustomFilter=NULL; of.nMaxCustFilter=0;
    of.nFilterIndex=1;
    of.lpstrFile=(LPWSTR)filename; of.nMaxFile=MAX_PATH;
    of.lpstrFileTitle=NULL; of.nMaxFileTitle=0;
    of.lpstrInitialDir=NULL;
    of.Flags=OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY;

    switch( message )
    {
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;
	
        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

			case WM_KEYDOWN:										
		{
			keys[wParam] = TRUE;								
			return 0;											
		}

		case WM_KEYUP:											
		{
			keys[wParam] = FALSE;								
			return 0;											
		}

		case WM_COMMAND:
		{
			if (wParam == 1)
            {
				if (!GetOpenFileName(&of)) return 0;
				hf=CreateFile((LPWSTR)filename,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);

				if (hf==INVALID_HANDLE_VALUE) {
				 MessageBox(hWnd,L"Open failed", L"Error",MB_ICONHAND|MB_OK);
				 break;
				}
				len=GetFileSize(hf,NULL);
				buf=(wchar_t*)malloc(len+1);
				if (!buf) {
				 MessageBox(hWnd,L"Mem allocation failed",L"Error",MB_ICONHAND|MB_OK);
				 break;
				}
				wcscpy(filename1, filename);
				FileName(filename1);
				isLoaded1 = true;
				CloseHandle(hf);
				free(buf);
				return 0;
            }

			if (wParam == 2)
             {
               if (!GetOpenFileName(&of)) return 0;
			   hf=CreateFile((LPWSTR)filename,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);

				if (hf==INVALID_HANDLE_VALUE) 
				{
				 MessageBox(hWnd,L"Open failed", L"Error",MB_ICONHAND|MB_OK);
				 break;
				}
				len=GetFileSize(hf,NULL);
				buf=(wchar_t*)malloc(len+1);
				if (!buf) {
				 MessageBox(hWnd,L"Mem allocation failed",L"Error",MB_ICONHAND|MB_OK);
				 break;
				}
				wcscpy(filename2, filename);
				FileName(filename2);
				isLoaded2 = true;
				CloseHandle(hf);
				free(buf);
				return 0;
             }
			
			if (wParam == 3)
             {
               MessageBox(hWnd,
				   L"Controls:\n\n* Rotattion of the scene: Arrows <Left>, <Right>, <Up>, <Down>.\n* Type of view: Button <W> - Wireframe, Button <S> - Solid.",
				   L"Help: Controls Information",MB_OK|MB_ICONINFORMATION);
			   return 0;
             }
		}

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }
	
    return 0;
}

//OpenGL Compatible Context Initialization
HRESULT CompatContextInit()
{
	int iMajorVersion=0;
	int iMinorVersion=0;

	HDC hDC = GetDC(g_hWnd);

	// Setting the parameters of context's surface
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize		= sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion   = 1;
	pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA; //pixels type
	pfd.cColorBits = 32;		    //color type
	pfd.cDepthBits = 24;            //z-buffer type
	pfd.iLayerType = PFD_MAIN_PLANE;
 
	int iPixelFormat = ChoosePixelFormat(hDC, &pfd);
	if (iPixelFormat == 0) return false;

	if(!SetPixelFormat(hDC, iPixelFormat, &pfd)) return false;

	// Creating of compatible context
	HGLRC hRCCompatible = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRCCompatible);

	bool bResult = true;

	if(glewInit() != GLEW_OK)
	{
			MessageBoxA(g_hWnd, "Couldn't initialize GLEW!", "Fatal Error", MB_ICONERROR);
			bResult = false;
	}

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRCCompatible);

	return bResult;
}

//OpenGL Main context Initialization
HRESULT ContextInit()
{
	int iMajorVersion=4;
	int iMinorVersion=0;

	HDC hDC = GetDC(g_hWnd);

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd,sizeof(pfd)); 

	bool bError=false;
	
	// Setting the parameters of context's surface
	if(WGLEW_ARB_create_context && WGLEW_ARB_pixel_format)
	{
		const int iPixelFormatAttribList[] =
		{
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB, //pixels type
			WGL_COLOR_BITS_ARB, 32,			       //color type
			WGL_DEPTH_BITS_ARB, 24,                //z-buffer type
			WGL_STENCIL_BITS_ARB, 8,
			0 // End of attributes list
		};

		int iContextAttribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, iMajorVersion,
			WGL_CONTEXT_MINOR_VERSION_ARB, iMinorVersion,
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			0 // End of attributes list
		};

		int iPixelFormat, iNumFormats;
		wglChoosePixelFormatARB(hDC, iPixelFormatAttribList, NULL, 1, &iPixelFormat, (UINT*)&iNumFormats);
    
		// Creating of main context
		hRC = wglCreateContextAttribsARB(hDC, 0, iContextAttribs);
		// If everything went OK
		if(hRC) wglMakeCurrent(hDC, hRC);
		else bError = true;

	}
	else bError = true;
	
	if(bError)
	{
		// Generate error messages
		char sErrorMessage[255], sErrorTitle[255];
		sprintf(sErrorMessage, "OpenGL %d.%d is not supported! Please download latest GPU drivers!", iMajorVersion, iMinorVersion);
		sprintf(sErrorTitle, "OpenGL %d.%d Not Supported", iMajorVersion, iMinorVersion);
		MessageBoxA(g_hWnd, sErrorMessage, sErrorTitle, MB_ICONINFORMATION);
		return false;
	}

   return S_OK;
}

//Shaders setting
ShaderClass SVertex, SFragment; 
ShaderProgClass SProg; 

//Geometry Initialization
void GeometrySet() 
{ 
   // Load shaders and create shader program
   SVertex.loadShader("data\\shaders\\shader.vert", GL_VERTEX_SHADER); 
   SFragment.loadShader("data\\shaders\\shader.frag", GL_FRAGMENT_SHADER); 

   SProg.createProgram(); 
   SProg.addShaderToProgram(&SVertex); 
   SProg.addShaderToProgram(&SFragment); 

   SProg.linkProgram(); 
   SProg.useProgram(); 

   wglSwapIntervalEXT(1);

   //Enable Z-Buffer
   glEnable(GL_DEPTH_TEST);
   glClearDepth(1.0f);

   //Loading Mesh Objects
    Mesh=new MeshFromObj(filename1, filename2);
}

void FileName(wchar_t * fname)
{
	wchar_t temp[MAX_PATH];
	int i = 0, j = 0;
	
	while (fname[j])
	{
		if (fname[j] == L'\\')
		{
			temp[i] = fname[j];
			i++;
			temp[i] = L'\\';
			i++;
			j++;
		}
		else 
		{
			temp[i] = fname[j];
			i++;
			j++;
		}
	}
	temp[i] = L'\0';
	wcscpy(fname, temp);
}

float phi = 2.1199985f;
float theta = 0.0f;
float orbit = 0.0f;
float dtime = 0.0f;
float ptime = 0.002f;

//Renderer
void Render()
{
    //Render-target clean-up 
	HDC hDC = GetDC(g_hWnd);

	//Background color
	glClearColor(0.0f, 0.9f, 0.5f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    //Camera rotation Actions
	theta = 0.0f;

    float offset = 0.0f;

	float radius=6.0f;	
    float pi= 3.141593f;
    phi += dorbit;
    theta += dorbit1;

    theta += pi / 2;

    int quarter = (int)(theta / pi);
    int quarterTotal = quarter % 2;
    if (quarterTotal == 0)  //  pare quarter
    {

    }
    else        //  non pare quarter
    {
        offset = pi;
    }

    orbit = phi + offset;

    theta -= pi / 2;
    if (quarterTotal == 0)
    {

    }
    else
    {
        theta = pi - theta;
    }

    float thetaF = pi*0.5f-theta;
    float radius1 = radius*sin(thetaF); 
    glm::vec3   Eye( cos(orbit)*radius1, radius*cos(thetaF), sin(orbit)*radius1 );
    glm::vec3   Up(sin(orbit)*cos(thetaF), sin(thetaF), cos(orbit)*cos(thetaF));

	//Setting matrices for Camera
	glm::mat4	mWorld;
	glm::mat4	mView;
	glm::mat4	mProjection;
	mWorld=glm::mat4(1.0f,0,0,0, 0,1.0f,0,0, 0,0,1.0f,0, 0,0,0,1.0f);

	//Setting time variable (for plane morphing)
	if(doAnimation) dtime += ptime;
	
	if (dtime > 1.0f || dtime < 0.0f) 
	{
		dtime -= ptime; 	
		ptime *= -1.0f;
		doAnimation = false;
	}

	mWorld=		glm::translate(0,0,0);
	mView=		glm::lookAt(Eye,glm::vec3(0,0,0),Up);
	mProjection=glm::perspectiveFov(90.0f,533.0f,400.0f,0.001f,1000.0f);

	//Matrices shader constants
	int iWorld=	glGetUniformLocation(SProg.getProgramID(),"mWorld");
	int iView=	glGetUniformLocation(SProg.getProgramID(),"mView");
	int iProjection=	glGetUniformLocation(SProg.getProgramID(),"mProjection");
	int iTime =	glGetUniformLocation(SProg.getProgramID(),"time");

	glUniformMatrix4fv(iWorld,1,GL_FALSE,glm::value_ptr(mWorld));
	glUniformMatrix4fv(iView,1,GL_FALSE,glm::value_ptr(mView));
	glUniformMatrix4fv(iProjection,1,GL_FALSE,glm::value_ptr(mProjection));
	glUniform1f(iTime, dtime);

	// Renderer
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
	mWorld=		glm::translate(0.0f,0.0f,0.0f);
	glUniformMatrix4fv(iWorld,1,GL_FALSE,glm::value_ptr(mWorld));

	Mesh->Draw();
	
	SwapBuffers(hDC);
}

//Cleaning the Context
void ContextClean()
{
	if (Mesh!=NULL) delete Mesh;
	
	SProg.deleteProgram();
	SVertex.deleteShader();
	SFragment.deleteShader();


	HDC hDC = GetDC(g_hWnd);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(g_hWnd, hDC);

	g_hWnd = NULL;
}


