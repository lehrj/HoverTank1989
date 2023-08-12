#include "pch.h"
#include "Environment.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>


Environment::Environment()
{
    InitializeHeightMapData();
    bool result = InitializeTerrain(m_heightMapGamePlayData);
    if (result == false)
    {
        // ToDo: add error handling
    }
    result = InitializeTerrain(m_heightMapStartScreenData);
    if (result == false)
    {
        // ToDo: add error handling
    }

    LoadEnvironmentData();
    CreateDataStrings();
    const int startEnviron = 0;  // ToDo: add error checking 
    m_currentEnviron = m_environs[startEnviron];
}

bool Environment::BuildTerrainModel(HeightMap& aMap)
{
    aMap.terrainModel.clear();
    aMap.terrainModel.resize((aMap.terrainHeight - 1) * (aMap.terrainWidth - 1) * 6);
    if (aMap.terrainModel.size() < 1)
    {
        return false;
    }

    int index = 0;

    for (int j = 0; j < (aMap.terrainHeight - 1); ++j)
    {
        for (int i = 0; i < (aMap.terrainWidth - 1); ++i)
        {
            // Get the indexes to the four points of the quad
            int index1 = (aMap.terrainWidth * j) + i;          // Upper left.
            int index2 = (aMap.terrainWidth * j) + (i + 1);      // Upper right.
            int index3 = (aMap.terrainWidth * (j + 1)) + i;      // Bottom left.
            int index4 = (aMap.terrainWidth * (j + 1)) + (i + 1);  // Bottom right.

            // Now create two triangles for that quad
            // Triangle 1 - Upper left
            aMap.terrainModel[index].position = aMap.heightMap[index1].position;
            aMap.terrainModel[index].normal = aMap.heightMap[index1].normal;
            ++index;

            // Triangle 1 - Upper right
            aMap.terrainModel[index].position = aMap.heightMap[index2].position;
            aMap.terrainModel[index].normal = aMap.heightMap[index2].normal;
            ++index;

            // Triangle 1 - Bottom left
            aMap.terrainModel[index].position = aMap.heightMap[index3].position;
            aMap.terrainModel[index].normal = aMap.heightMap[index3].normal;
            ++index;

            // Triangle 2 - Bottom left
            aMap.terrainModel[index].position = aMap.heightMap[index3].position;
            aMap.terrainModel[index].normal = aMap.heightMap[index3].normal;
            ++index;

            // Triangle 2 - Upper right.
            aMap.terrainModel[index].position = aMap.heightMap[index2].position;
            aMap.terrainModel[index].normal = aMap.heightMap[index2].normal;
            ++index;

            // Triangle 2 - Bottom right.
            aMap.terrainModel[index].position = aMap.heightMap[index4].position;
            aMap.terrainModel[index].normal = aMap.heightMap[index4].normal;
            ++index;
        }
    }

    // to do: m_heightMap.clear(); once testing is complete
    return true;
}

bool Environment::CalculateTerrainNormals(HeightMap& aMap)
{
    std::vector<DirectX::SimpleMath::Vector3> normals;
    normals.clear();
    normals.resize((aMap.terrainHeight - 1) * (aMap.terrainWidth - 1));
    if (normals.size() < 1)
    {
        return false;
    }

    for (int j = 0; j < (aMap.terrainHeight - 1); ++j)
    {
        for (int i = 0; i < (aMap.terrainWidth - 1); ++i)
        {
            int index1 = ((j + 1) * aMap.terrainWidth) + i;      // Bottom left vertex.
            int index2 = ((j + 1) * aMap.terrainWidth) + (i + 1);  // Bottom right vertex.
            int index3 = (j * aMap.terrainWidth) + i;          // Upper left vertex.

            // Get three vertices from the face.
            DirectX::SimpleMath::Vector3 vertex1 = aMap.heightMap[index1].position;
            DirectX::SimpleMath::Vector3 vertex2 = aMap.heightMap[index2].position;
            DirectX::SimpleMath::Vector3 vertex3 = aMap.heightMap[index3].position;

            // Calculate the two vectors for this face.
            DirectX::SimpleMath::Vector3 vector1 = vertex2 - vertex1;
            DirectX::SimpleMath::Vector3 vector2 = vertex3 - vertex1;


            int index = (j * (aMap.terrainWidth - 1)) + i;
            normals[index] = DirectX::XMVector3Cross(vector1, vector2);
            normals[index].Normalize();
        }
    }

    // Now go through all the vertices and take a sum of the face normals that touch this vertex.
    for (int j = 0; j < aMap.terrainHeight; j++)
    {
        for (int i = 0; i < aMap.terrainWidth; i++)
        {
            DirectX::SimpleMath::Vector3 sum = DirectX::SimpleMath::Vector3::Zero;

            // Bottom left face.
            if (((i - 1) >= 0) && ((j - 1) >= 0))
            {
                int index = ((j - 1) * (aMap.terrainWidth - 1)) + (i - 1);
                sum += normals[index];
            }
            // Bottom right face.
            if ((i < (aMap.terrainWidth - 1)) && ((j - 1) >= 0))
            {
                int index = ((j - 1) * (aMap.terrainWidth - 1)) + i;
                sum += normals[index];
            }
            // Upper left face.
            if (((i - 1) >= 0) && (j < (aMap.terrainHeight - 1)))
            {
                int index = (j * (aMap.terrainWidth - 1)) + (i - 1);
                sum += normals[index];
            }
            // Upper right face.
            if ((i < (aMap.terrainWidth - 1)) && (j < (aMap.terrainHeight - 1)))
            {
                int index = (j * (aMap.terrainWidth - 1)) + i;
                sum += normals[index];
            }

            int index = (j * aMap.terrainWidth) + i;

            sum.Normalize();
            aMap.heightMap[index].normal = sum;
        }
    }

    return true;
}

bool Environment::CheckTerrainTriangleHeight(DirectX::XMFLOAT3& aPos, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2) const
{
    // Starting position of the ray that is being cast
    DirectX::XMFLOAT3 startVector(aPos.x, 0.0f, aPos.z);

    // The direction the ray is being cast
    DirectX::XMFLOAT3 directionVector(0.0f, -1.0f, 0.0f);

    // Calculate the two edges from the three points given
    DirectX::SimpleMath::Vector3 edge1(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
    DirectX::SimpleMath::Vector3 edge2(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z);

    // Calculate the normal of the triangle from the two edges // ToDo use cross prod funcs
    DirectX::SimpleMath::Vector3 normal;

    edge1.Cross(edge2, normal);
    normal.Normalize();

    // Find the distance from the origin to the plane.
    float distance = ((-normal.x * v0.x) + (-normal.y * v0.y) + (-normal.z * v0.z));

    // Get the denominator of the equation.
    float denominator = ((normal.x * directionVector.x) + (normal.y * directionVector.y) + (normal.z * directionVector.z));

    // Make sure the result doesn't get too close to zero to prevent divide by zero.
    if (fabs(denominator) < 0.0001f)
    {
        return false;
    }

    // Get the numerator of the equation.
    float numerator = -1.0f * (((normal.x * startVector.x) + (normal.y * startVector.y) + (normal.z * startVector.z)) + distance);

    // Calculate where we intersect the triangle.
    float t = numerator / denominator;

    // Find the intersection vector.
    DirectX::SimpleMath::Vector3 Q;
    Q.x = startVector.x + (directionVector.x * t);
    Q.y = startVector.y + (directionVector.y * t);
    Q.z = startVector.z + (directionVector.z * t);

    // Find the three edges of the triangle.
    DirectX::SimpleMath::Vector3 e1(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
    DirectX::SimpleMath::Vector3 e2(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
    DirectX::SimpleMath::Vector3 e3(v0.x - v2.x, v0.y - v2.y, v0.z - v2.z);

    // Calculate the normal for the first edge.
    DirectX::SimpleMath::Vector3 edgeNormal;
    e1.Cross(normal, edgeNormal);

    // Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
    DirectX::SimpleMath::Vector3 temp(Q.x - v1.x, Q.y - v1.y, Q.z - v1.z);

    float determinant = ((edgeNormal.x * temp.x) + (edgeNormal.y * temp.y) + (edgeNormal.z * temp.z));

    // Check if it is outside.
    if (determinant > 0.001f)
    {
        return false;
    }

    // Calculate the normal for the second edge
    e2.Cross(normal, edgeNormal);

    // Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
    temp.x = Q.x - v1.x;
    temp.y = Q.y - v1.y;
    temp.z = Q.z - v1.z;

    determinant = ((edgeNormal.x * temp.x) + (edgeNormal.y * temp.y) + (edgeNormal.z * temp.z));

    // Check if it is outside.
    if (determinant > 0.001f)
    {
        return false;
    }

    // Calculate the normal for the third edge.
    e3.Cross(normal, edgeNormal);

    // Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
    temp.x = Q.x - v2.x;
    temp.y = Q.y - v2.y;
    temp.z = Q.z - v2.z;

    determinant = ((edgeNormal.x * temp.x) + (edgeNormal.y * temp.y) + (edgeNormal.z * temp.z));

    // Check if it is outside.
    if (determinant > 0.001f)
    {
        return false;
    }

    // Now we have our height.
    aPos.y = Q.y;
    return true;
}

void Environment::CreateDataStrings()
{
    std::stringstream inVal;
    inVal.precision(Utility::GetNumericalPrecisionForUI());

    for (unsigned int i = 0; i < m_environs.size(); ++i)
    {
        inVal.str(std::string());
        inVal << std::fixed << m_environs[i].airDensity;
        m_environs[i].airDensityStr = inVal.str();

        inVal.str(std::string());
        inVal << std::fixed << m_environs[i].gravity;
        m_environs[i].gravityStr = inVal.str();

        inVal.str(std::string());
        inVal << std::fixed << m_environs[i].wind.x;
        m_environs[i].windXStr = inVal.str();

        inVal.str(std::string());
        inVal << std::fixed << m_environs[i].wind.y;
        m_environs[i].windYStr = inVal.str();

        inVal.str(std::string());
        inVal << std::fixed << m_environs[i].wind.z;
        m_environs[i].windZStr = inVal.str();

        inVal.str(std::string());
        inVal << std::fixed << m_environs[i].landingFrictionScale;
        m_environs[i].landingFrictionScaleStr = inVal.str();

        inVal.str(std::string());
        inVal << std::fixed << m_environs[i].landingHardnessScale;
        m_environs[i].landingHardnessScaleStr = inVal.str();
    }
}

std::vector<DirectX::VertexPositionColor> Environment::GetTerrainColorVertex()
{
    std::vector<DirectX::VertexPositionColor> vertPosColor;
    vertPosColor.clear();
    vertPosColor.resize(m_terrainModel.size());
    DirectX::XMFLOAT4 terrainColor(1.0, 1.0, 1.0, 1.0); // ToDo: for testing, implement color control

    for (unsigned int i = 0; i < vertPosColor.size(); ++i)
    {
        vertPosColor[i].position = m_terrainModel[i].position;
        vertPosColor[i].color = terrainColor;
    }

    return vertPosColor;
}

void Environment::GetLightDirectionalVectors(DirectX::SimpleMath::Vector3& aLightVec0, DirectX::SimpleMath::Vector3& aLightVec1, DirectX::SimpleMath::Vector3& aLightVec2) const
{
    aLightVec0 = m_currentEnviron.LightDirection0;
    aLightVec1 = m_currentEnviron.LightDirection1;
    aLightVec2 = m_currentEnviron.LightDirection2;
}

bool Environment::GetIsPosInPlay(const DirectX::XMFLOAT3 aPos) const
{
    if (aPos.x > m_heightMapGamePlayData.xPosMax || aPos.x < m_heightMapGamePlayData.xPosMin
        || aPos.z > m_heightMapGamePlayData.zPosMax || aPos.z < m_heightMapGamePlayData.zPosMin)
    {
        return false;
    }
    else if (aPos.y > m_heightMapGamePlayData.maxElevation)
    {
        return true;
    }
    else
    {
        float terrainHeight = GetTerrainHeightAtPos(aPos);
        if (terrainHeight > aPos.y)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
}

float Environment::GetTerrainHeightAtPos(DirectX::XMFLOAT3 aPos) const
{
    DirectX::SimpleMath::Vector3 prePos = aPos;

    bool foundHeight = false;

    for (unsigned int i = 0; i < m_heightMapGamePlayData.terrainModel.size(); ++i)
    {
        DirectX::XMFLOAT3 vertex1 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex2 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex3 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex4 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex5 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex6 = m_heightMapGamePlayData.terrainModel[i].position;
        
        if (aPos.x <= vertex2.x && aPos.x >= vertex4.x && aPos.z <= vertex2.z && aPos.z >= vertex4.z)
        {
            foundHeight = CheckTerrainTriangleHeight(aPos, vertex1, vertex2, vertex3);
            if (foundHeight == true)
            {
                return aPos.y;
            }
            foundHeight = CheckTerrainTriangleHeight(aPos, vertex6, vertex5, vertex4);
            if (foundHeight == true)
            {
                return aPos.y;
            }
            else
            {
                // to do : height check failure, add error checking
            }
        }
    }
    float errorHeight = aPos.y;
    return errorHeight;
}

DirectX::SimpleMath::Vector3 Environment::GetTerrainNormal(DirectX::SimpleMath::Vector3 aPos) const
{
    bool foundHeight = false;

    for (unsigned int i = 0; i < m_heightMapGamePlayData.terrainModel.size(); ++i)
    {
        DirectX::XMFLOAT3 vertex1 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex2 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex3 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex4 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex5 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex6 = m_heightMapGamePlayData.terrainModel[i].position;

        if (aPos.x <= vertex2.x && aPos.x >= vertex4.x && aPos.z <= vertex2.z && aPos.z >= vertex4.z)
        {
            foundHeight = CheckTerrainTriangleHeight(aPos, vertex1, vertex2, vertex3);
            if (foundHeight == true)
            {
                DirectX::SimpleMath::Vector3 p3 = vertex1;
                DirectX::SimpleMath::Vector3 p2 = vertex2;
                DirectX::SimpleMath::Vector3 p1 = vertex3;

                DirectX::SimpleMath::Vector3 U = p2 - p1;
                DirectX::SimpleMath::Vector3 V = p3 - p1;
                DirectX::SimpleMath::Vector3 terrainNormal;
                terrainNormal.x = (U.y * V.z) - (U.z * V.y);
                terrainNormal.y = (U.z * V.x) - (U.x * V.z);
                terrainNormal.z = (U.x * V.y) - (U.y * V.x);
                terrainNormal.Normalize();
                return terrainNormal;
            }
            else if (CheckTerrainTriangleHeight(aPos, vertex6, vertex5, vertex4) == true)
            {
                foundHeight = true;
                DirectX::SimpleMath::Vector3 p3 = vertex4;
                DirectX::SimpleMath::Vector3 p2 = vertex5;
                DirectX::SimpleMath::Vector3 p1 = vertex6;

                DirectX::SimpleMath::Vector3 U = p2 - p1;
                DirectX::SimpleMath::Vector3 V = p3 - p1;
                DirectX::SimpleMath::Vector3 terrainNormal;
                terrainNormal.x = (U.y * V.z) - (U.z * V.y);
                terrainNormal.y = (U.z * V.x) - (U.x * V.z);
                terrainNormal.z = (U.x * V.y) - (U.y * V.x);
                terrainNormal.Normalize();
                return terrainNormal;
            }
            else
            {
                // to do : height check failure, add error checking
            }
        }
    }

    return -DirectX::SimpleMath::Vector3::UnitX;
}

std::vector<DirectX::VertexPositionNormalColor> Environment::GetTerrainPositionNormalColorVertex(EnvironmentType aEnvironType)
{
    std::vector<DirectX::VertexPositionNormalColor> vertPosNormColor;
    vertPosNormColor.clear();
    if (aEnvironType == EnvironmentType::ENVIRONMENTTYPE_STARTSCREEN)
    {
        vertPosNormColor.resize(m_heightMapStartScreenData.terrainModel.size());
        DirectX::XMFLOAT4 terrainColor(1.0, 1.0, 1.0, 1.0); // ToDo: for testing, implement color control

        for (unsigned int i = 0; i < vertPosNormColor.size(); ++i)
        {
            vertPosNormColor[i].position = m_heightMapStartScreenData.terrainModel[i].position;
            vertPosNormColor[i].color = terrainColor;
            vertPosNormColor[i].normal = m_heightMapStartScreenData.terrainModel[i].normal;
        }
    }
    else
    {
        vertPosNormColor.resize(m_heightMapGamePlayData.terrainModel.size());
        DirectX::XMFLOAT4 terrainColor(1.0, 1.0, 1.0, 1.0); // ToDo: for testing, implement color control

        for (unsigned int i = 0; i < vertPosNormColor.size(); ++i)
        {
            vertPosNormColor[i].position = m_heightMapGamePlayData.terrainModel[i].position;
            vertPosNormColor[i].color = terrainColor;
            vertPosNormColor[i].normal = m_heightMapGamePlayData.terrainModel[i].normal;
        }
    }
    return vertPosNormColor;
}

bool Environment::GetGroundPlane(DirectX::SimpleMath::Plane& aPlane, DirectX::SimpleMath::Vector3 aPos) const
{
    bool isPlaneFound = false;
    for (unsigned int i = 0; i < m_heightMapGamePlayData.terrainModel.size(); ++i)
    {
        DirectX::XMFLOAT3 vertex1 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex2 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex3 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex4 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex5 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex6 = m_heightMapGamePlayData.terrainModel[i].position;

        DirectX::SimpleMath::Vector3 pos = aPos;

        if (aPos.x <= vertex2.x && aPos.x >= vertex4.x && aPos.z <= vertex2.z && aPos.z >= vertex4.z)
        {
            isPlaneFound = CheckTerrainTriangleHeight(aPos, vertex1, vertex2, vertex3);
            if (isPlaneFound == true)
            {
                //aHeight = aPos.y;
                //aTriVertex1 = vertex3;
                //aTriVertex2 = vertex2;
                //aTriVertex3 = vertex1;
                aPlane = DirectX::SimpleMath::Plane(vertex1, vertex2, vertex3);

                DirectX::SimpleMath::Vector3 p3 = vertex1;
                DirectX::SimpleMath::Vector3 p2 = vertex2;
                DirectX::SimpleMath::Vector3 p1 = vertex3;

                DirectX::SimpleMath::Vector3 U = p2 - p1;
                DirectX::SimpleMath::Vector3 V = p3 - p1;
                DirectX::SimpleMath::Vector3 terrainNormal;
                terrainNormal.x = (U.y * V.z) - (U.z * V.y);
                terrainNormal.y = (U.z * V.x) - (U.x * V.z);
                terrainNormal.z = (U.x * V.y) - (U.y * V.x);
                terrainNormal.Normalize();
                //aNorm = terrainNormal;
                i = static_cast<unsigned int>(m_heightMapGamePlayData.terrainModel.size());
            }
            else if (CheckTerrainTriangleHeight(aPos, vertex6, vertex5, vertex4) == true)
            {
                isPlaneFound = true;
                //aHeight = aPos.y;
                //aTriVertex1 = vertex6;
                //aTriVertex2 = vertex5;
                //aTriVertex3 = vertex4;
                aPlane = DirectX::SimpleMath::Plane(vertex4, vertex5, vertex6);

                DirectX::SimpleMath::Vector3 p3 = vertex4;
                DirectX::SimpleMath::Vector3 p2 = vertex5;
                DirectX::SimpleMath::Vector3 p1 = vertex6;

                DirectX::SimpleMath::Vector3 U = p2 - p1;
                DirectX::SimpleMath::Vector3 V = p3 - p1;
                DirectX::SimpleMath::Vector3 terrainNormal;
                terrainNormal.x = (U.y * V.z) - (U.z * V.y);
                terrainNormal.y = (U.z * V.x) - (U.x * V.z);
                terrainNormal.z = (U.x * V.y) - (U.y * V.x);
                terrainNormal.Normalize();
                //aNorm = terrainNormal;
                i = static_cast<unsigned int>(m_heightMapGamePlayData.terrainModel.size());
            }
            else
            {
                // to do : height check failure, add error checking
            }
        }
    }
    return isPlaneFound;
}

bool Environment::GetTerrainTriangleData(DirectX::SimpleMath::Vector3& aTriVertex1, DirectX::SimpleMath::Vector3& aTriVertex2, DirectX::SimpleMath::Vector3& aTriVertex3, DirectX::SimpleMath::Vector3 aPos) const
{
    bool isTriFound = false;
    for (unsigned int i = 0; i < m_heightMapGamePlayData.terrainModel.size(); ++i)
    {
        DirectX::XMFLOAT3 vertex1 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex2 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex3 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex4 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex5 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex6 = m_heightMapGamePlayData.terrainModel[i].position;

        DirectX::SimpleMath::Vector3 pos = aPos;

        if (aPos.x <= vertex2.x && aPos.x >= vertex4.x && aPos.z <= vertex2.z && aPos.z >= vertex4.z)
        {
            isTriFound = CheckTerrainTriangleHeight(aPos, vertex1, vertex2, vertex3);
            if (isTriFound == true)
            {
                //aHeight = aPos.y;
                aTriVertex1 = vertex3;
                aTriVertex2 = vertex2;
                aTriVertex3 = vertex1;
                DirectX::SimpleMath::Vector3 p3 = vertex1;
                DirectX::SimpleMath::Vector3 p2 = vertex2;
                DirectX::SimpleMath::Vector3 p1 = vertex3;

                DirectX::SimpleMath::Vector3 U = p2 - p1;
                DirectX::SimpleMath::Vector3 V = p3 - p1;
                DirectX::SimpleMath::Vector3 terrainNormal;
                terrainNormal.x = (U.y * V.z) - (U.z * V.y);
                terrainNormal.y = (U.z * V.x) - (U.x * V.z);
                terrainNormal.z = (U.x * V.y) - (U.y * V.x);
                terrainNormal.Normalize();
                //aNorm = terrainNormal;
                i = static_cast<unsigned int>(m_heightMapGamePlayData.terrainModel.size());
            }
            else if (CheckTerrainTriangleHeight(aPos, vertex6, vertex5, vertex4) == true)
            {
                isTriFound = true;
                //aHeight = aPos.y;
                aTriVertex1 = vertex6;
                aTriVertex2 = vertex5;
                aTriVertex3 = vertex4;

                DirectX::SimpleMath::Vector3 p3 = vertex4;
                DirectX::SimpleMath::Vector3 p2 = vertex5;
                DirectX::SimpleMath::Vector3 p1 = vertex6;

                DirectX::SimpleMath::Vector3 U = p2 - p1;
                DirectX::SimpleMath::Vector3 V = p3 - p1;
                DirectX::SimpleMath::Vector3 terrainNormal;
                terrainNormal.x = (U.y * V.z) - (U.z * V.y);
                terrainNormal.y = (U.z * V.x) - (U.x * V.z);
                terrainNormal.z = (U.x * V.y) - (U.y * V.x);
                terrainNormal.Normalize();
                //aNorm = terrainNormal;
                i = static_cast<unsigned int>(m_heightMapGamePlayData.terrainModel.size());
            }
            else
            {
                // to do : height check failure, add error checking
            }
        }
    }
    return isTriFound;
}

bool Environment::GetVehicleUpdateData(DirectX::SimpleMath::Vector3 aPos, DirectX::SimpleMath::Vector3& aNorm, float& aHeight, DirectX::SimpleMath::Plane& aPlane) const
{
    bool foundHeight = false;

    for (unsigned int i = 0; i < m_heightMapGamePlayData.terrainModel.size(); ++i)
    {
        DirectX::XMFLOAT3 vertex1 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex2 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex3 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex4 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex5 = m_heightMapGamePlayData.terrainModel[i].position;
        ++i;
        DirectX::XMFLOAT3 vertex6 = m_heightMapGamePlayData.terrainModel[i].position;

        DirectX::SimpleMath::Vector3 pos = aPos;

        if (aPos.x <= vertex2.x && aPos.x >= vertex4.x && aPos.z <= vertex2.z && aPos.z >= vertex4.z)
        {
            foundHeight = CheckTerrainTriangleHeight(aPos, vertex1, vertex2, vertex3);
            if (foundHeight == true)
            {
                aHeight = aPos.y;
                aPlane = DirectX::SimpleMath::Plane(vertex1, vertex2, vertex3);

                DirectX::SimpleMath::Vector3 p3 = vertex1;
                DirectX::SimpleMath::Vector3 p2 = vertex2;
                DirectX::SimpleMath::Vector3 p1 = vertex3;

                DirectX::SimpleMath::Vector3 U = p2 - p1;
                DirectX::SimpleMath::Vector3 V = p3 - p1;
                DirectX::SimpleMath::Vector3 terrainNormal;
                terrainNormal.x = (U.y * V.z) - (U.z * V.y);
                terrainNormal.y = (U.z * V.x) - (U.x * V.z);
                terrainNormal.z = (U.x * V.y) - (U.y * V.x);
                terrainNormal.Normalize();
                aNorm = terrainNormal;
                
                i = static_cast<unsigned int>(m_heightMapGamePlayData.terrainModel.size());
            }
            else if (CheckTerrainTriangleHeight(aPos, vertex6, vertex5, vertex4) == true)
            {
                foundHeight = true;
                aHeight = aPos.y;
                aPlane = DirectX::SimpleMath::Plane(vertex4, vertex5, vertex6);
                
                DirectX::SimpleMath::Vector3 p3 = vertex4;
                DirectX::SimpleMath::Vector3 p2 = vertex5;
                DirectX::SimpleMath::Vector3 p1 = vertex6;

                DirectX::SimpleMath::Vector3 U = p2 - p1;
                DirectX::SimpleMath::Vector3 V = p3 - p1;
                DirectX::SimpleMath::Vector3 terrainNormal;
                terrainNormal.x = (U.y * V.z) - (U.z * V.y);
                terrainNormal.y = (U.z * V.x) - (U.x * V.z);
                terrainNormal.z = (U.x * V.y) - (U.y * V.x);
                terrainNormal.Normalize();
                aNorm = terrainNormal;
                
                i = static_cast<unsigned int>(m_heightMapGamePlayData.terrainModel.size());
            }
            else
            {
                // to do : height check failure, add error checking
            }
        }
    }

    return foundHeight;
}

// While this could be done once per environment update, future updates could have moment to moment wind changes
float Environment::GetWindDirection() const
{ 
    DirectX::SimpleMath::Vector3 windVec = m_currentEnviron.wind;
    DirectX::SimpleMath::Vector3 zeroDirection(-1.0, 0.0, -1.0);
    float direction = DirectX::XMVectorGetX(DirectX::XMVector3AngleBetweenNormals(DirectX::XMVector3Normalize(windVec), DirectX::XMVector3Normalize(zeroDirection)));
    if (DirectX::XMVectorGetY(DirectX::XMVector3Cross(windVec, zeroDirection)) > 0.0f)
    {
        direction = -direction;
    }

    return direction;
}

bool Environment::InitializeTerrain(HeightMap& aMap)
{
    bool result = LoadHeightMap(aMap);
    if (!result)
    {
        return false;
    }

    result = CalculateTerrainNormals(aMap);
    if (!result)
    {
        return false;
    }

    result = BuildTerrainModel(aMap);
    if (!result)
    {
        return false;
    }

    ScaleTerrain(aMap);
    
    return true;
}

void Environment::InitializeHeightMapData()
{
    // initialize game play map
    m_heightMapGamePlayData.heightScale = 0.01f;
    m_heightMapGamePlayData.mapScale = 100.30f;
    m_heightMapGamePlayData.mapXtransform = -16.0f * m_heightMapGamePlayData.mapScale + 187.0f;
    m_heightMapGamePlayData.mapYtransform = 0.5f;
    m_heightMapGamePlayData.mapZtransform = -16.0f * m_heightMapGamePlayData.mapScale + 00.0f;

    m_heightMapGamePlayData.mapType = EnvironmentType::ENVIRONMENTTYPE_GAMEPLAY;
    m_heightMapGamePlayData.terrainHeight = 0;
    m_heightMapGamePlayData.terrainLength = 0;
    m_heightMapGamePlayData.terrainWidth = 0;
    
    m_heightMapGamePlayData.heightMap.clear();
    m_heightMapGamePlayData.terrainModel.clear();

    // initialize start screen map
    m_heightMapStartScreenData.heightScale = m_heightScaleStartScreen;
    m_heightMapStartScreenData.mapScale = m_mapScaleStartScreen;
    m_heightMapStartScreenData.mapXtransform = m_mapXtransformStartScreen;

    m_heightMapStartScreenData.mapYtransform = m_mapYtransformStartScreen;
    m_heightMapStartScreenData.mapZtransform = m_mapZtransformStartScreen;

    m_heightMapStartScreenData.mapType = EnvironmentType::ENVIRONMENTTYPE_STARTSCREEN;
    m_heightMapStartScreenData.terrainHeight = 0;
    m_heightMapStartScreenData.terrainLength = 0;
    m_heightMapStartScreenData.terrainWidth = 0;

    m_heightMapStartScreenData.heightMap.clear();
    m_heightMapStartScreenData.terrainModel.clear();
}

void Environment::LoadEnvironmentData()
{
    m_environs.clear();
    m_environs.resize(m_environsAvailable);

    int i = 0;

    m_environs[i].name = "Clean Test";   
    m_environs[i].airDensity = 1.225;
    m_environs[i].gravity = -9.8;
    m_environs[i].gravityVec = DirectX::SimpleMath::Vector3(0.0f, -9.8f, 0.0f);
    m_environs[i].holePosition = DirectX::SimpleMath::Vector3(3.0f, 0.0f, -0.5f); 
    m_environs[i].landingFrictionScale = 1.0;
    m_environs[i].landingHardnessScale = 1.0;
    m_environs[i].scale = 0.02;
    m_environs[i].terrainCoefficientOfRestitution = 0.4;
    m_environs[i].terrainColor = DirectX::Colors::Green;
    m_environs[i].wind = DirectX::SimpleMath::Vector3(-0.4f, 0.0f, -0.9f);
    UpdateDirectionalLightingVectors(m_environs[i]);

    ++i;
    m_environs[i].name = "Breezy";    
    m_environs[i].airDensity = 1.225;
    m_environs[i].gravity = -9.8;
    m_environs[i].gravityVec = DirectX::SimpleMath::Vector3(0.0f, -9.8f, 0.0f);
    m_environs[i].holePosition = DirectX::SimpleMath::Vector3(2.1f, 0.0f, 1.0f);
    m_environs[i].landingFrictionScale = 1.0;
    m_environs[i].landingHardnessScale = 1.0;
    m_environs[i].scale = 0.02;
    m_environs[i].terrainCoefficientOfRestitution = 0.4;
    m_environs[i].terrainColor = DirectX::Colors::Green;
    m_environs[i].wind = DirectX::SimpleMath::Vector3(-10.0f, 0.0f, 0.0f);
    UpdateDirectionalLightingVectors(m_environs[i]);

    m_environs[i].name = "Calm";
    m_environs[i].airDensity = 1.225;
    m_environs[i].gravity = -9.8;
    m_environs[i].gravityVec = DirectX::SimpleMath::Vector3(0.0f, -9.8f, 0.0f);
    m_environs[i].holePosition = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 1.5f);
    m_environs[i].landingFrictionScale = 1.0;
    m_environs[i].landingHardnessScale = 1.0;
    m_environs[i].scale = 0.02;
    m_environs[i].terrainCoefficientOfRestitution = 0.4;
    m_environs[i].terrainColor = DirectX::Colors::Green;
    m_environs[i].wind = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
    UpdateDirectionalLightingVectors(m_environs[i]);
}

bool Environment::LoadHeightMap(HeightMap& aMap)
{
    FILE* filePtr;
    const char* filename;
    if (aMap.mapType == EnvironmentType::ENVIRONMENTTYPE_STARTSCREEN)
    {
        filename = "../HoverTank1989/Art/HeightMaps/heightmapStartScreen.bmp";
    }
    else if (aMap.mapType == EnvironmentType::ENVIRONMENTTYPE_GAMEPLAY)
    {
        //filename = "../HoverTank1989/Art/HeightMaps/HeightmapRoughTerrain.bmp";
        //filename = "../HoverTank1989/Art/HeightMaps/HeightmapCleanTest.bmp";
        filename = "../HoverTank1989/Art/HeightMaps/HeightmapCleanTest6.bmp";
        //filename = "../HoverTank1989/Art/HeightMaps/HeightmapWeaponTest.bmp";
    }
    else
    {
        filename = "../HoverTank1989/Art/HeightMaps/heightmapCleanTest.bmp";
    }

    // Open the height map file 
    int error = fopen_s(&filePtr, filename, "rb");
    if (error != 0)
    {
        return false;
    }

    // Read in the file header
    BITMAPFILEHEADER bitmapFileHeader;
    size_t count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
    if (count != 1)
    {
        return false;
    }

    // Read in the bitmap info header.
    BITMAPINFOHEADER bitmapInfoHeader;
    count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
    if (count != 1)
    {
        return false;
    }

    // Save the dimensions of the terrain.
    aMap.terrainWidth = bitmapInfoHeader.biWidth;
    aMap.terrainHeight = bitmapInfoHeader.biHeight;

    // Calculate the size of the bitmap image data.
    unsigned int imageSize = aMap.terrainWidth * aMap.terrainHeight * 3 + aMap.terrainWidth;

    // Allocate memory for the bitmap image data.
    unsigned char* bitmapImage = new unsigned char[imageSize];
    if (!bitmapImage)
    {
        return false;
    }

    // Move to the beginning of the bitmap data.
    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

    UINT pitch = aMap.terrainWidth * 3;
    UINT excessPitch = 0;
    while (float(pitch / 4) != float(pitch) / 4.0)
    {
        pitch++;
        excessPitch++;
    }

    // Read in the bitmap image data.
    count = fread(bitmapImage, 1, pitch * aMap.terrainHeight, filePtr);
    if (count != imageSize)
    {
        return false;
    }

    std::vector<char> testMap;
    testMap.clear();
    for (unsigned int i = 0; i < imageSize; ++i)
    {
        testMap.push_back(bitmapImage[i]);
    }

    // Close the file.
    error = fclose(filePtr);
    if (error != 0)
    {
        return false;
    }

    // Create the structure to hold the height map data.
    aMap.heightMap.clear();
    aMap.heightMap.resize(aMap.terrainWidth * aMap.terrainHeight);

    // Initialize the position in the image data buffer.
    int k = 0;
    int index;
    unsigned char height;

    // Read the image data into the height map.
    for (int j = 0; j < aMap.terrainWidth; j++)
    {
        for (int i = 0; i < aMap.terrainHeight; i++)
        {
            height = bitmapImage[k];

            // To read values in backwards since bitmap read in is fliped
            index = (aMap.terrainWidth * (aMap.terrainHeight - 1 - j)) + i;

            aMap.heightMap[index].position.x = (float)j;
            aMap.heightMap[index].position.y = (float)height * aMap.heightScale; // scale height during input
            aMap.heightMap[index].position.z = (float)i;

            k += 3;
        }
        k += excessPitch;
    }

    // Release the bitmap image data.
    delete[] bitmapImage;
    bitmapImage = 0;

    return true;
}

void Environment::ScaleTerrain(HeightMap& aMap)
{
    float scale = 1.0;
    float xTransform = 0.0;
    float yTransform = 0.0;
    float zTransform = 0.0;
    if (aMap.mapType == EnvironmentType::ENVIRONMENTTYPE_STARTSCREEN)
    {
        scale = aMap.mapScale;
        xTransform = aMap.mapXtransform;
        yTransform = aMap.mapYtransform;
        zTransform = aMap.mapZtransform;
    }
    else if (aMap.mapType == EnvironmentType::ENVIRONMENTTYPE_GAMEPLAY)
    {
        scale = aMap.mapScale;
        xTransform = aMap.mapXtransform;
        yTransform = aMap.mapYtransform;
        zTransform = aMap.mapZtransform;
    }
    for (unsigned int i = 0; i < aMap.heightMap.size(); ++i)
    {
        aMap.heightMap[i].position.x *= scale;
        aMap.heightMap[i].position.y *= scale;
        aMap.heightMap[i].position.z *= scale;

        aMap.heightMap[i].position.x += xTransform;
        aMap.heightMap[i].position.y += yTransform;
        aMap.heightMap[i].position.z += zTransform;
    }

    for (unsigned int i = 0; i < aMap.terrainModel.size(); ++i)
    {
        aMap.terrainModel[i].position.x *= scale;
        aMap.terrainModel[i].position.y *= scale;
        aMap.terrainModel[i].position.z *= scale;

        aMap.terrainModel[i].position.x += xTransform;
        aMap.terrainModel[i].position.y += yTransform;
        aMap.terrainModel[i].position.z += zTransform;

        if (i == 0)
        {
            aMap.maxElevation = aMap.terrainModel[i].position.y;
            aMap.xPosMax = aMap.terrainModel[i].position.x;
            aMap.xPosMin = aMap.terrainModel[i].position.x;
            aMap.zPosMax = aMap.terrainModel[i].position.z;
            aMap.zPosMin = aMap.terrainModel[i].position.z;
        }

        if (aMap.terrainModel[i].position.y > aMap.maxElevation)
        {
            aMap.maxElevation = aMap.terrainModel[i].position.y;
        }
        if (aMap.terrainModel[i].position.x > aMap.xPosMax)
        {
            aMap.xPosMax = aMap.terrainModel[i].position.x;
        }
        if (aMap.terrainModel[i].position.x < aMap.xPosMin)
        {
            aMap.xPosMin = aMap.terrainModel[i].position.x;
        }
        if (aMap.terrainModel[i].position.z > aMap.zPosMax)
        {
            aMap.zPosMax = aMap.terrainModel[i].position.z;
        }
        if (aMap.terrainModel[i].position.z < aMap.zPosMin)
        {
            aMap.zPosMin = aMap.terrainModel[i].position.z;
        }
    }
}

void Environment::SetCameraPos(const DirectX::SimpleMath::Vector3 aCamPos)
{
    m_cameraPos = aCamPos;
}

void Environment::UpdateEnvironment(const int aIndex)
{
    m_currentEnviron = m_environs[aIndex];
    m_landingFriction = m_environs[aIndex].landingFrictionScale;
    m_landingHardness = m_environs[aIndex].landingHardnessScale;
}

void Environment::UpdateDirectionalLightingVectors(Environ& aEnviron)
{
    DirectX::SimpleMath::Vector3 lightVectorPrime = aEnviron.LightDirectionPrime;
    DirectX::SimpleMath::Vector3 lightVector0 = aEnviron.LightDirection0;
    DirectX::SimpleMath::Vector3 lightVector1 = aEnviron.LightDirection1;
    DirectX::SimpleMath::Vector3 lightVector2 = aEnviron.LightDirection2;
    const float offsetAngle = aEnviron.lightDirectionsOffsetAngle;
    const float rotationAngle = aEnviron.lightDirectionsOffsetAngle;
    Utility::GetDispersedLightDirectionsRotation(lightVectorPrime, offsetAngle, rotationAngle, lightVector0, lightVector1, lightVector2);
    aEnviron.LightDirection0 = lightVector0;
    aEnviron.LightDirection1 = lightVector1;
    aEnviron.LightDirection2 = lightVector2;
}
