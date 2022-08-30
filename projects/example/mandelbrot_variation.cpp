
#include "yoshix.h"

#include <math.h>

using namespace gfx;

struct SVertexBuffer
{
    float m_ScreenMatrix[16];
};

// -----------------------------------------------------------------------------

struct SPixelBuffer
{
    float m_Time;
    float m_Resolution[2];
    float m_Padding01;
};

// -----------------------------------------------------------------------------

class CApplication : public IApplication
{
    public:

        CApplication();
        virtual ~CApplication();

    private:

        float m_Time = 0.0f;

        float m_Resolution[2] = { 800.0f, 450.0f };

    private:

        float   m_FieldOfViewY;             
        BHandle m_pVertexConstantBuffer;    
        BHandle m_pPixelConstantBuffer;    
        BHandle m_pVertexShader;            
        BHandle m_pPixelShader;          
        BHandle m_pMaterial;                
        BHandle m_pMesh;                    

    public:

        float GetResolution(int _Index);
        void AddTime(float _Time);

    private:

        virtual bool InternOnCreateConstantBuffers();
        virtual bool InternOnReleaseConstantBuffers();
        virtual bool InternOnCreateShader();
        virtual bool InternOnReleaseShader();
        virtual bool InternOnCreateMaterials();
        virtual bool InternOnReleaseMaterials();
        virtual bool InternOnCreateMeshes();
        virtual bool InternOnReleaseMeshes();
        virtual bool InternOnResize(int _Width, int _Height);
        virtual bool InternOnUpdate();
        virtual bool InternOnFrame();
};

// -----------------------------------------------------------------------------

CApplication::CApplication()
    : m_FieldOfViewY         (60.0f)        
    , m_pVertexConstantBuffer(nullptr)
    , m_pPixelConstantBuffer (nullptr)
    , m_pVertexShader        (nullptr)
    , m_pPixelShader         (nullptr)
    , m_pMaterial            (nullptr)
    , m_pMesh                (nullptr)
{
}

// -----------------------------------------------------------------------------

CApplication::~CApplication()
{
}

// -----------------------------------------------------------------------------

float CApplication::GetResolution(int _Index)
{
    return m_Resolution[_Index];
}

// -----------------------------------------------------------------------------

void CApplication::AddTime(float _Time)
{
    m_Time += _Time;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnCreateConstantBuffers()
{
    CreateConstantBuffer(sizeof(SVertexBuffer), &m_pVertexConstantBuffer);
    CreateConstantBuffer(sizeof(SPixelBuffer), &m_pPixelConstantBuffer);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseConstantBuffers()
{
    ReleaseConstantBuffer(m_pVertexConstantBuffer);
    ReleaseConstantBuffer(m_pPixelConstantBuffer);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnCreateShader()
{
    CreateVertexShader("..\\data\\shader\\mandelbrot.fx", "VSShader", &m_pVertexShader);
    CreatePixelShader("..\\data\\shader\\mandelbrot.fx", "PSShader", &m_pPixelShader);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseShader()
{
    ReleaseVertexShader(m_pVertexShader);
    ReleasePixelShader(m_pPixelShader);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnCreateMaterials()
{
    SMaterialInfo MaterialInfo;

    MaterialInfo.m_NumberOfTextures = 0;                           
    MaterialInfo.m_NumberOfVertexConstantBuffers = 1;                      
    MaterialInfo.m_pVertexConstantBuffers[0] = m_pVertexConstantBuffer;   
    MaterialInfo.m_NumberOfPixelConstantBuffers = 1;                    
    MaterialInfo.m_pPixelConstantBuffers[0] = m_pPixelConstantBuffer;   
    MaterialInfo.m_pVertexShader = m_pVertexShader;          
    MaterialInfo.m_pPixelShader = m_pPixelShader;             
    MaterialInfo.m_NumberOfInputElements = 1;                           
    MaterialInfo.m_InputElements[0].m_pName = "POSITION";                
    MaterialInfo.m_InputElements[0].m_Type = SInputElement::Float3;  

    CreateMaterial(MaterialInfo, &m_pMaterial);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseMaterials()
{
    ReleaseMaterial(m_pMaterial);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnCreateMeshes()
{
    float Vertices[][3] =
    {
        { 0.0f, 1.0f, 0.0f, },
        { 1.0f, 1.0f, 0.0f, },
        { 1.0f, 0.0f, 0.0f, },
        { 0.0f, 0.0f, 0.0f, },
    };

    int Indices[][3] =
    {
        { 0, 1, 2, },
        { 0, 2, 3, },
    };

    SMeshInfo MeshInfo;

    MeshInfo.m_pVertices = &Vertices[0][0];                    
    MeshInfo.m_NumberOfVertices = 4;                          
    MeshInfo.m_pIndices = &Indices[0][0];                       
    MeshInfo.m_NumberOfIndices = 6;                            
    MeshInfo.m_pMaterial = m_pMaterial;                         

    CreateMesh(MeshInfo, &m_pMesh);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnReleaseMeshes()
{
    ReleaseMesh(m_pMesh);

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnResize(int _Width, int _Height)
{
    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnUpdate()
{
    float Eye[3];
    float At[3];
    float Up[3];

    Eye[0] =   0.0f; At[0] = 0.0f; Up[0] = 0.0f;
    Eye[1] =   0.0f; At[1] = 0.0f; Up[1] = 1.0f;
    Eye[2] = -30.0f; At[2] = 0.0f; Up[2] = 0.0f;

    return true;
}

// -----------------------------------------------------------------------------

bool CApplication::InternOnFrame()
{
    SVertexBuffer VertexBuffer;

    AddTime(0.01f);

    GetScreenMatrix(VertexBuffer.m_ScreenMatrix);

    UploadConstantBuffer(&VertexBuffer, m_pVertexConstantBuffer);


    SPixelBuffer PixelBuffer;

    PixelBuffer.m_Time = m_Time;
    PixelBuffer.m_Resolution[0] = m_Resolution[0];
    PixelBuffer.m_Resolution[1] = m_Resolution[1];

    UploadConstantBuffer(&PixelBuffer, m_pPixelConstantBuffer);

    DrawMesh(m_pMesh);

    return true;
}

// -----------------------------------------------------------------------------

void main()
{
    CApplication Application;

    RunApplication(Application.GetResolution(0), Application.GetResolution(1), "Mandelbrot | Dirk Hofmann", &Application);
}