#pragma once
#include <fstream>
#include "Utility.h"

typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG  biWidth;
    LONG  biHeight;
    WORD  biPlanes;
    WORD  biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG  biXPelsPerMeter;
    LONG  biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, * LPBITMAPINFOHEADER, * PBITMAPINFOHEADER;

#pragma pack(2) 
typedef struct tagBITMAPFILEHEADER {
    WORD  bfType;
    DWORD bfSize;
    WORD  bfReserved1;
    WORD  bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, * LPBITMAPFILEHEADER, * PBITMAPFILEHEADER;
#pragma pack() 

struct Environ
{
    std::string                         name;
    float                              airDensity;        // in kg/m^3
    std::string                         airDensityStr;
    DirectX::SimpleMath::Vector3        holePosition;
    float                              gravity;           // in m/s^2
    DirectX::SimpleMath::Vector3        gravityVec;
    std::string                         gravityStr;
    float                              landingFrictionScale;
    std::string                         landingFrictionScaleStr;
    float                              landingHardnessScale;
    std::string                         landingHardnessScaleStr;
    int                                 par;
    float                               scale;
    float                               teeDirection;       // start direction for first shot
    DirectX::SimpleMath::Vector3        teePosition;        // start position for first shot
    DirectX::XMVECTORF32                terrainColor;
    DirectX::SimpleMath::Vector3        wind;               // in m/s
    std::string                         windXStr;
    std::string                         windYStr;
    std::string                         windZStr;
};

enum class FixtureType
{
    FIXTURETYPE_FLAGSTICK,
    FIXTURETYPE_TEEBOX,
    FIXTURETYPE_BRIDGE,
    FIXTURETYPE_TREE01,
    FIXTURETYPE_TREE02,
    FIXTURETYPE_TREE03,
    FIXTURETYPE_TREE04,
    FIXTURETYPE_TREE05,
    FIXTURETYPE_TREE06,
    FIXTURETYPE_TREE07,
    FIXTURETYPE_TREE08,
    FIXTURETYPE_TREE09,
    FIXTURETYPE_TREE10,
};

struct Fixture
{
    unsigned int                    idNumber;
    DirectX::SimpleMath::Vector3    position;
    FixtureType                     fixtureType;  // think of a better name later
    float                           animationVariation;
    float                           distanceToCamera;
};


enum class EnvironmentType
{
    ENIVRONMENTTYPE_STARTSCREEN,
    ENVIRONMENTTYPE_GAMEPLAY,
};

struct HeightMap
{
    std::vector<DirectX::VertexPositionNormal> heightMap;
    std::vector<DirectX::VertexPositionNormal> terrainModel;
    float                         heightScale;

    EnvironmentType              mapType;
    float                        mapScale;
    float                        mapXtransform;
    float                        mapYtransform;
    float                        mapZtransform;

    int                          terrainHeight = 0;
    int                          terrainLength = 0;
    int                          terrainWidth = 0;

    float                        maxElevation = 0.0f;
    float                        xPosMax = 0.0f;
    float                        xPosMin = 0.0f;
    float                        zPosMax = 0.0f;
    float                        zPosMin = 0.0f;
};

struct Terrain
{
    EnvironmentType               environType;
    DirectX::VertexPositionNormalColor* terrainVertexArray;
    DirectX::VertexPositionNormalColor* terrainVertexArrayBase;
    int                           terrainVertexCount;
};

// Class to handle environment and gameplay world needs
class Environment
{
public:
    Environment();

    bool CheckTerrainTriangleHeight(DirectX::XMFLOAT3& aPos, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2) const;

    float GetAirDensity() const { return m_currentEnviron.airDensity; };
    std::string GetAirDensityString(const int aEnvironmentIndex) const { return m_environs[aEnvironmentIndex].airDensityStr; };
    float GetAirDensity(const int aEnvironmentIndex) const { return m_environs[aEnvironmentIndex].airDensity; };
    DirectX::XMVECTORF32 GetEnvironColor() const { return m_currentEnviron.terrainColor; };
    std::string GetEnvironName(const int aEnvironmentIndex) const { return m_environs[aEnvironmentIndex].name; };
    std::vector<Fixture> GetFixtureBucket() { return m_fixtureBucket; };

    float GetGravity() const { return m_currentEnviron.gravity; };
    DirectX::SimpleMath::Vector3 GetGravityVec() const { return m_currentEnviron.gravityVec; };
    std::string GetGravityString(const int aEnvironmentIndex) const { return m_environs[aEnvironmentIndex].gravityStr; };
    float GetGravity(const int aEnvironmentIndex) const { return m_environs[aEnvironmentIndex].gravity; };
    std::vector<DirectX::VertexPositionColor> GetFlagVertex() const { return m_flagVertex; };
    DirectX::SimpleMath::Vector3 GetHolePosition() const { return m_currentEnviron.holePosition; };
    float GetHoleRadius() const { return m_holeRadius * m_currentEnviron.scale; };
    std::vector<DirectX::VertexPositionColor> GetHoleVertex() const { return m_holeVertex; };
    float GetLandingHeight() const { return m_landingHeight; };
    float GetLauchHeight() const { return m_launchHeight; };
    int GetNumerOfEnvirons() const { return m_environsAvailable; };
    int GetNumberOfEnvironSelectDisplayVariables() const { return m_environSelectDisplayDataPoints; };
    int GetPar() const { return m_currentEnviron.par; };
    float GetScale() const { return m_currentEnviron.scale; };
    float GetTeeDirectionDegrees() const { return m_currentEnviron.teeDirection; };
    DirectX::SimpleMath::Vector3 GetTeePosition() const { return m_currentEnviron.teePosition; };


    bool GetIsPosInPlay(const DirectX::XMFLOAT3 aPos) const;
    float GetTerrainHeightAtPos(DirectX::XMFLOAT3 aPos) const;
    float GetTerrainHeightAtPos2(DirectX::XMFLOAT3 aPos) const;
    DirectX::SimpleMath::Vector3 GetTerrainNormal(DirectX::SimpleMath::Vector3 aPos) const;

    std::vector<DirectX::VertexPositionColor> GetTerrainColorVertex();
    std::vector<DirectX::VertexPositionNormalColor> GetTerrainPositionNormalColorVertex(EnvironmentType aEnvironType);

    bool GetVehicleUpdateData(DirectX::SimpleMath::Vector3 aPos, DirectX::SimpleMath::Vector3& aNorm, float& aHeight) const;

    float GetWindDirection() const;
    DirectX::SimpleMath::Vector3 GetWindVector() const { return m_currentEnviron.wind; };
    float GetWindX() const { return m_currentEnviron.wind.x; };
    std::string GetWindXString(const int aEnvironmentIndex) const { return m_environs[aEnvironmentIndex].windXStr; };
    float GetWindY() const { return m_currentEnviron.wind.y; };
    std::string GetWindYString(const int aEnvironmentIndex) const { return m_environs[aEnvironmentIndex].windYStr; };
    float GetWindZ() const { return m_currentEnviron.wind.z; };
    std::string GetWindZString(const int aEnvironmentIndex) const { return m_environs[aEnvironmentIndex].windZStr; };    

    bool InitializeTerrain(HeightMap& aMap, EnvironmentType aEnviron);

    void SortFixtureBucketByDistance();
    void UpdateEnvironment(const int aIndex);
    void UpdateFixtureDistanceToCamera(const DirectX::SimpleMath::Vector3 &aCameraPos);
    void UpdateFixtures(const DirectX::SimpleMath::Vector3 &aPos);

private:
    
    void BuildFlagVertex(DirectX::SimpleMath::Vector3 aPos);
    void BuildHoleVertex(DirectX::SimpleMath::Vector3 aPos);

    bool BuildTerrainModel(HeightMap& aMap);
    bool CalculateTerrainNormals(HeightMap& aMap);

    void CreateDataStrings();
    void InitializeHeightMapData();

    void LoadEnvironmentData();
    void LoadFixtureBucket();
    void LoadFixtureBucket12th();

    //bool LoadHeightMap(std::vector<DirectX::VertexPositionNormal>& aMap, EnvironmentType aEnviron);
    bool LoadHeightMap(HeightMap& aMap, EnvironmentType aEnviron);

 
    void SetLandingHeight(float aLandingHeight);
    void SetLauchHeight(float aLaunchHeight);

    void ScaleTerrain(HeightMap& aMap);
    
    Environ                             m_currentEnviron;
    std::vector<Environ>                m_environs;
    const int                           m_environsAvailable = 3;
    const int                           m_environSelectDisplayDataPoints = 5;

    std::vector<Fixture>                m_fixtureBucket;

    std::vector<DirectX::VertexPositionColor> m_flagVertex;
    std::vector<DirectX::VertexPositionColor> m_holeVertex;
    const int                           m_holeResolution = 30;          // number of vertices used to draw hole circle
    //const float                        m_holeRadius = 0.10795;              // Radius of the hole, future updates could include addition of "big cup" or "tiny cup" hole sizes
    const float                        m_holeRadius = .50795;

    float                              m_landingHeight = 0.0;     // in meters
    float                              m_launchHeight = 0.0;      // in meters

    // variables for bounce and roll functionality not yet implemented
    float                              m_landingFriction;
    float                              m_landingHardness;
    float                              m_landingXslope;
    float                              m_landingZslope;

    // min max consts
    const float                        m_minAirDensity = 0.0;
    const float                        m_maxAirDensity = 68.0; // just above the air density of Venus
    const float                        m_minGravity = 0.1;
    const float                        m_maxGravity = 28.0;    // approximate value for the mass of the sun
    const float                        m_minMaxHeight = 450.0; // Launch & Landing min/max heights is just above the largest elevation change (>400 meters) of any real golf course which is the Extreme 19 in Limpopo Province South Africa
    const float                        m_minMaxWind = 667.0;   // highest know wind speed on Neptune

    std::vector<DirectX::VertexPositionNormal> m_heightMap;
    std::vector<DirectX::VertexPositionNormal> m_heightMapStartScreen;

    int                                 m_terrainHeight = 0;
    int                                 m_terrainLength = 0;
    int                                 m_terrainWidth = 0;
    std::vector<DirectX::VertexPositionNormal> m_terrainModel;

    // scaling variables for different types of heightmaps
    //const float                         m_heightScale = 0.007f;
    const float                         m_heightScale = 1.009f;
    const float                        m_heightScaleStartScreen = 0.007f;

    const float                        m_elevationScaleStartScreen = 0.007f;
    const float                        m_mapScaleStartScreen = 0.2f;
    const float                        m_mapXtransformStartScreen = -2.101f;
    const float                        m_mapYtransformStartScreen = -0.01f;
    //const float                        m_mapZtransformStartScreen = -0.02f;
    const float                        m_mapZtransformStartScreen = -1.6f;
    
    const float                        m_elevationScaleGamePlay = 0.007f;
    const float                        m_mapScaleGamePlay = 0.000003f;
    const float                        m_mapXtransformGamePlay = -16.0f * m_mapScaleGamePlay;
    const float                        m_mapYtransformGamePlay = 0.0f;
    const float                        m_mapZtransformGamePlay = -16.0f * m_mapScaleGamePlay;

    HeightMap                          m_heightMapGamePlayData;
    HeightMap                          m_heightMapStartScreenData;
};

