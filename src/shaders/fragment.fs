

#version 330 core

in vec2 UV;
out vec4 FragColor;

uniform usampler3D model;
uniform sampler2D prevFrame;
uniform ivec3 modelDimms;
uniform float aspect;
uniform int time;
uniform int frameCount;


const float DEGREE = 3.1415926535897932384/180.0;
const float PI = 3.1415926525897932384;


const int NORTH = 0;
const int SOUTH = 1;
const int EAST = 2;
const int WEST = 3;
const int UP = 4;
const int DOWN = 5;



vec3 traceRayPosition;
vec3 traceRayDirection;
int traceRayHits;
int traceRayTime;
vec3 traceRayResult;



float seededRand(int seed) {
    // Thomas Wang-style integer hash
    uint x = uint(seed);
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    return float(x) / float(0xffffffffU);
}


vec3 sampleCosineHemisphere(float u1, float u2)
{
    float r = sqrt(u1);
    float phi = 2.0*PI*u2;
    float x = r * cos(phi);
    float y = r * sin(phi);
    float z = sqrt(1.0 - u1);
    return vec3(x, y, z);
}

bool hitFunction(uint voxtype, int face, vec2 location, vec3 position, vec3 incidentAngle, int hits, int time)
{
    if (voxtype == 0u)
    {
        mat3 rotationMatrix = mat3(
            cos(-15*DEGREE), -sin(-15*DEGREE), 0,
            sin(-15*DEGREE),  cos(-15*DEGREE), 0,
            0,                0,               1
        );
        vec3 dir = rotationMatrix*incidentAngle;
        float theta = atan(abs(dir.y)/sqrt(dir.x*dir.x + dir.z*dir.z));
        if (dir.y < 0)
        {
            const vec3 dullBrown = vec3(0.52941176471, 0.43137254902, 0.29411764706)*0.3;
            const vec3 dullOrange = vec3(0.84705882353, 0.52549019608, 0.23137254902)*0.7;
            if (theta < 15*DEGREE)
            {
                traceRayResult *= dullBrown*(theta/(15*DEGREE)) + dullOrange*(1-theta/(15*DEGREE));
                return true;
            }
            traceRayResult *= dullBrown;
            return true;
        }
        else
        {
            const vec3 dullOrange = vec3(0.84705882353, 0.52549019608, 0.23137254902)*0.7;
            const vec3 skyBlue = vec3(0.72156862745, 0.78039215686, 0.8);
            if (theta < 15*DEGREE)
            {
                traceRayResult *= skyBlue*(theta/(15*DEGREE)) + dullOrange*(1-theta/(15*DEGREE));
                return true;
            }
            traceRayResult *= skyBlue;
            return true;
        }
    }else if (voxtype == 1u)
    {
        if (hits < 5)
        {
            const vec3 baseColour = vec3(0.8666666666667, 0.16078431373, 0.06274509804);
            vec3 reflectedDirection = incidentAngle;
            // build masks (0.0 or 1.0)
            float m_up    = float(int(face == UP));
            float m_down  = float(int(face == DOWN));
            float m_east  = float(int(face == EAST));
            float m_west  = float(int(face == WEST));
            float m_north = float(int(face == NORTH));
            float m_south = float(int(face == SOUTH));
            // normal is (+1 for positive face, -1 for negative face) per axis
            vec3 normal = vec3(m_east - m_west, m_up - m_down, m_north - m_south);
            // flip multiplier: 1.0 normally, -1.0 when that axis's face is selected
            vec3 flip = vec3(
                1.0 - 2.0 * (m_east + m_west),
                1.0 - 2.0 * (m_up + m_down),
                1.0 - 2.0 * (m_north + m_south)
            );
            // apply flip to reflectedDirection components
            reflectedDirection *= flip;
            
            float cosTheta = max(0.0, dot(normal, normalize(incidentAngle*(-1.0))));
            const float R_0 = 0.5;
            float reflectionCoefficient = R_0 + (1-R_0)*pow((1-cosTheta), 5.0);
            float u1 = seededRand(time);
            float u2 = seededRand(time+1);
            vec3 valueInLobe = sampleCosineHemisphere(u1, u2);
            float u3 = seededRand(time+2);
            vec3 perpNormal = normalize(cross(normal, incidentAngle));
            vec3 direction;
            
            direction = (int(u3 > reflectionCoefficient))*(normal*valueInLobe.z + perpNormal*valueInLobe.x + normalize(cross(normal, perpNormal))*valueInLobe.z) + (1-int(u3 > reflectionCoefficient))*(reflectedDirection*valueInLobe.z + perpNormal*valueInLobe.x + normalize(cross(perpNormal, reflectedDirection))*valueInLobe.y);
            
            traceRayResult *= baseColour;

            traceRayPosition = position;
            traceRayDirection = direction;
            traceRayHits = hits+1;
            traceRayTime = time+3;
            return false;
        }
        else
        {
            traceRayResult *= vec3(0.0, 0.0, 0.0);
            return true;
        }
    }else
    {
        traceRayResult *= vec3(0.0, 0.0, 0.0);
        return true;
    }
}

vec3 traceRayInWorldPosition;
vec3 traceRayInWorldNormDir;
int traceRayInWorldHits;
ivec3 traceRayInWorldGridPosition;
vec3 traceRayInWorldDistances;
vec3 traceRayInWorldPerBlockDistances;
bvec3 traceRayInWorldDirections;
int traceRayInWorldTime;
bool traceRayInWorldResult;

bool traceRayInWorld()
{
    if (traceRayInWorldDistances.x < traceRayInWorldDistances.y && traceRayInWorldDistances.x < traceRayInWorldDistances.z)
    {
        if (traceRayInWorldDirections.x)
        {
            ivec3 newGridPosition = ivec3(traceRayInWorldGridPosition.x+1, traceRayInWorldGridPosition.yz);
            vec3 newDistances = vec3(traceRayInWorldPerBlockDistances.x, traceRayInWorldDistances.y-traceRayInWorldDistances.x, traceRayInWorldDistances.z-traceRayInWorldDistances.x);
            vec3 newPosition = traceRayInWorldPosition+traceRayInWorldNormDir*traceRayInWorldDistances.x;
            if (newGridPosition.x >= modelDimms.x)
            {
                traceRayInWorldResult = hitFunction(0u, UP, vec2(0.0, 0.0), newPosition, traceRayInWorldNormDir, traceRayInWorldHits, traceRayInWorldTime);
                return true;
            }
            else if (texelFetch(model, newGridPosition, 0).r != 0u)
            {
                traceRayInWorldResult = hitFunction(texelFetch(model, newGridPosition, 0).r, WEST, vec2(1.0-(newPosition.z - floor(newPosition.z)), newPosition.y - floor(newPosition.y)), newPosition, traceRayInWorldNormDir, traceRayInWorldHits, traceRayInWorldTime);
                return true;
            }
            else
            {
                traceRayInWorldPosition = newPosition;
                traceRayInWorldGridPosition = newGridPosition;
                traceRayInWorldDistances = newDistances;
                return false;
            }
        }
        else
        {
            ivec3 newGridPosition = ivec3(traceRayInWorldGridPosition.x-1, traceRayInWorldGridPosition.yz);
            vec3 newDistances = vec3(traceRayInWorldPerBlockDistances.x, traceRayInWorldDistances.y-traceRayInWorldDistances.x, traceRayInWorldDistances.z-traceRayInWorldDistances.x);
            vec3 newPosition = traceRayInWorldPosition+traceRayInWorldNormDir*traceRayInWorldDistances.x;
            if (newGridPosition.x < 0)
            {
                traceRayInWorldResult = hitFunction(0u, UP, vec2(0.0, 0.0), newPosition, traceRayInWorldNormDir, traceRayInWorldHits, traceRayInWorldTime);
                return true;
            }
            else if (texelFetch(model, newGridPosition, 0).r != 0u)
            {
                traceRayInWorldResult = hitFunction(texelFetch(model, newGridPosition, 0).r, EAST, vec2((newPosition.z - floor(newPosition.z)), (newPosition.y - floor(newPosition.y))), newPosition, traceRayInWorldNormDir, traceRayInWorldHits, traceRayInWorldTime);
                return true;
            }
            else
            {
                traceRayInWorldPosition = newPosition;
                traceRayInWorldGridPosition = newGridPosition;
                traceRayInWorldDistances = newDistances;
                return false;
            }
        }
    }
    else if (traceRayInWorldDistances.y < traceRayInWorldDistances.z)
    {
        if (traceRayInWorldDirections.y)
        {
            ivec3 newGridPosition = ivec3(traceRayInWorldGridPosition.x, traceRayInWorldGridPosition.y+1, traceRayInWorldGridPosition.z);
            vec3 newDistances = vec3(traceRayInWorldDistances.x-traceRayInWorldDistances.y, traceRayInWorldPerBlockDistances.y, traceRayInWorldDistances.z-traceRayInWorldDistances.y);
            vec3 newPosition = traceRayInWorldPosition+traceRayInWorldNormDir*traceRayInWorldDistances.y;
            if (newGridPosition.y >= modelDimms.y)
            {
                traceRayInWorldResult = hitFunction(0u, UP, vec2(0.0, 0.0), newPosition, traceRayInWorldNormDir, traceRayInWorldHits, traceRayInWorldTime);
                return true;
            }
            else if (texelFetch(model, newGridPosition, 0).r != 0u)
            {
                traceRayInWorldResult = hitFunction(texelFetch(model, newGridPosition, 0).r, DOWN, vec2(newPosition.x - floor(newPosition.x), newPosition.z - floor(newPosition.z)), newPosition, traceRayInWorldNormDir, traceRayInWorldHits, traceRayInWorldTime);
                return true;
            }
            else
            {
                traceRayInWorldPosition = newPosition;
                traceRayInWorldGridPosition = newGridPosition;
                traceRayInWorldDistances = newDistances;
                return false;
            }
        }
        else
        {
            ivec3 newGridPosition = ivec3(traceRayInWorldGridPosition.x, traceRayInWorldGridPosition.y-1, traceRayInWorldPosition.z);
            vec3 newDistances = vec3(traceRayInWorldDistances.x-traceRayInWorldDistances.y, traceRayInWorldPerBlockDistances.y, traceRayInWorldDistances.z-traceRayInWorldDistances.y);
            vec3 newPosition = traceRayInWorldPosition+traceRayInWorldNormDir*traceRayInWorldDistances.y;
            if (newGridPosition.y < 0)
            {
                traceRayInWorldResult = hitFunction(0u, UP, vec2(0.0, 0.0), newPosition, traceRayInWorldNormDir, traceRayInWorldHits, traceRayInWorldTime);
                return true;
            }
            else if (texelFetch(model, newGridPosition, 0).r != 0u)
            {
                traceRayInWorldResult = hitFunction(texelFetch(model, newGridPosition, 0).r, UP, vec2((newPosition.x - floor(newPosition.x)), (newPosition.z - floor(newPosition.z))), newPosition, traceRayInWorldNormDir, traceRayInWorldHits, traceRayInWorldTime);
                return true;
            }
            else
            {
                traceRayInWorldPosition = newPosition;
                traceRayInWorldGridPosition = newGridPosition;
                traceRayInWorldDistances = newDistances;
                return false;
            }
        }
    }
    else
    {
        if (traceRayInWorldDirections.z)
        {
            ivec3 newGridPosition = ivec3(traceRayInWorldGridPosition.x, traceRayInWorldGridPosition.y, traceRayInWorldGridPosition.z+1);
            vec3 newDistances = vec3(traceRayInWorldDistances.x-traceRayInWorldDistances.z, traceRayInWorldDistances.y-traceRayInWorldDistances.z, traceRayInWorldPerBlockDistances.z);
            vec3 newPosition = traceRayInWorldPosition+traceRayInWorldNormDir*traceRayInWorldDistances.z;
            if (newGridPosition.z >= modelDimms.z)
            {
                traceRayInWorldResult = hitFunction(0u, UP, vec2(0.0, 0.0), newPosition, traceRayInWorldNormDir, traceRayInWorldHits, traceRayInWorldTime);
                return true;
            }
            else if (texelFetch(model, newGridPosition, 0).r != 0u)
            {
                traceRayInWorldResult = hitFunction(texelFetch(model, newGridPosition, 0).r, SOUTH, vec2(newPosition.x - floor(newPosition.x), newPosition.y - floor(newPosition.y)), newPosition, traceRayInWorldNormDir, traceRayInWorldHits, traceRayInWorldTime);
                return true;
            }
            else
            {
                traceRayInWorldPosition = newPosition;
                traceRayInWorldGridPosition = newGridPosition;
                traceRayInWorldDistances = newDistances;
                return false;
            }
        }
        else
        {
            ivec3 newGridPosition = ivec3(traceRayInWorldGridPosition.x, traceRayInWorldGridPosition.y, traceRayInWorldPosition.z-1);
            vec3 newDistances = vec3(traceRayInWorldDistances.x-traceRayInWorldDistances.z, traceRayInWorldDistances.y-traceRayInWorldDistances.z, traceRayInWorldPerBlockDistances.z);
            vec3 newPosition = traceRayInWorldPosition+traceRayInWorldNormDir*traceRayInWorldDistances.z;
            if (newGridPosition.z < 0)
            {
                traceRayInWorldResult = hitFunction(0u, UP, vec2(0.0, 0.0), newPosition, traceRayInWorldNormDir, traceRayInWorldHits, traceRayInWorldTime);
                return true;
            }
            else if (texelFetch(model, newGridPosition, 0).r != 0u)
            {
                traceRayInWorldResult = hitFunction(texelFetch(model, newGridPosition, 0).r, NORTH, vec2(1.0-(newPosition.x - floor(newPosition.x)), (newPosition.y - floor(newPosition.y))), newPosition, traceRayInWorldNormDir, traceRayInWorldHits, traceRayInWorldTime);
                return true;
            }
            else
            {
                traceRayInWorldPosition = newPosition;
                traceRayInWorldGridPosition = newGridPosition;
                traceRayInWorldDistances = newDistances;
                return false;
            }
        }
    }
}


bool traceRay()
{

    vec3 normDir = normalize(traceRayDirection);

    if ((abs(traceRayPosition.x) > modelDimms.x/2) || (abs(traceRayPosition.y) > modelDimms.y/2) || (abs(traceRayPosition.z) > modelDimms.z/2))
    {
        float distXP = 0.0;
        float distXN = 0.0;
        float distYP = 0.0;
        float distYN = 0.0;
        float distZP = 0.0;
        float distZN = 0.0;
        float minDist = (float(modelDimms.x)+float(modelDimms.y)+float(modelDimms.z))*3;

        if (normDir.x != 0.0)
        {
            distXP = ((-modelDimms.x/2)-traceRayPosition.x) / normDir.x;
            distXN = (( modelDimms.x/2)-traceRayPosition.x) / normDir.x;
        }
        if (normDir.y != 0.0)
        {
            distYP = ((-modelDimms.y/2)-traceRayPosition.y) / normDir.y;
            distYN = (( modelDimms.y/2)-traceRayPosition.y) / normDir.y;
        }
        if (normDir.z != 0.0)
        {
            distZP = ((-modelDimms.z/2)-traceRayPosition.z) / normDir.z;
            distZN = (( modelDimms.z/2)-traceRayPosition.z) / normDir.z;
        }
        if (distXP < minDist && distXP > 0)
        {
            minDist = distXP;
        }
        if (distXN < minDist && distXN > 0)
        {
            minDist = distXN;
        }
        if (distYP < minDist && distYP > 0)
        {
            minDist = distYP;
        }
        if (distYN < minDist && distYN > 0)
        {
            minDist = distYN;
        }
        if (distZP < minDist && distZP > 0)
        {
            minDist = distZP;
        }
        if (distZN < minDist && distZN > 0)
        {
            minDist = distZN;
        }

        if (minDist>(float(modelDimms.x)+float(modelDimms.y)+float(modelDimms.z))*2)
        {
            return hitFunction(0u, UP, vec2(0.0, 0.0), traceRayPosition, traceRayDirection, traceRayHits, traceRayTime);
        }
        else
        {
            traceRayPosition += normDir*minDist;
            return false;
        }
    }
    else
    {
        vec3 worldPosition = traceRayPosition + vec3(float(modelDimms.x)/2, float(modelDimms.y)/2, float(modelDimms.z)/2);
        ivec3 worldGridPosition = ivec3(int(floor(worldPosition.x)), int(floor(worldPosition.y)), int(floor(worldPosition.z)));
        bvec3 directions = bvec3(traceRayDirection.x>0, traceRayDirection.y>0, traceRayDirection.z>0);
        vec3 xDir;
        if (normDir.x != 0)
        {
            xDir = normDir/normDir.x;
        }else
        {
            xDir = vec3(float(modelDimms.x)*2, 0.0, 0.0);
        }
        vec3 yDir;
        if (normDir.y != 0)
        {
            yDir = normDir/normDir.y;
        }else
        {
            yDir = vec3(float(modelDimms.y)*2, 0.0, 0.0);
        }
        vec3 zDir;
        if (normDir.z != 0)
        {
            zDir = normDir/normDir.z;
        }else
        {
            zDir = vec3(float(modelDimms.z)*2, 0.0, 0.0);
        }
        vec3 perBlockDistances = vec3(length(xDir), length(yDir), length(zDir));
        vec3 initialDistances;
        initialDistances.x = int(directions.x)*(perBlockDistances.x * (ceil(worldPosition.x) - worldPosition.x))+(1-int(directions.x))*(perBlockDistances.x * (worldPosition.x - floor(worldPosition.x)));
        initialDistances.y = int(directions.y)*(perBlockDistances.y * (ceil(worldPosition.y) - worldPosition.y))+(1-int(directions.y))*(perBlockDistances.y * (worldPosition.y - floor(worldPosition.y)));
        initialDistances.z = int(directions.z)*(perBlockDistances.z * (ceil(worldPosition.z) - worldPosition.z))+(1-int(directions.z))*(perBlockDistances.z * (worldPosition.z - floor(worldPosition.z)));
        
        traceRayInWorldPosition = traceRayPosition;
        traceRayInWorldNormDir = normDir;
        traceRayInWorldHits = traceRayHits;
        traceRayInWorldGridPosition = worldGridPosition;
        traceRayInWorldDistances = initialDistances;
        traceRayInWorldPerBlockDistances = perBlockDistances;
        traceRayInWorldDirections = directions;
        traceRayInWorldTime = time;
        while (!traceRayInWorld())
        {
            continue;
        }
        return traceRayInWorldResult;
    }

}






void main()
{
    vec2 uv = vec2(UV.x, UV.y/aspect);
    vec3 direction = normalize(vec3(uv.x*0.5, uv.y*0.5, 1.0));
    mat3 rotationMatrix = mat3(
        1.0, 0.0              ,  0.0,   // first column
        0.0, cos(-22.0*DEGREE), -sin(-22.0*DEGREE),   // second column
        0.0, sin(-22.0*DEGREE),  cos(-22.0*DEGREE)    // third column
    );

    traceRayPosition = vec3(0.0, 300.0, -850.0);
    traceRayDirection = rotationMatrix*direction;
    traceRayHits = 0;
    traceRayTime = time*int(floor(uv.x*10000))*int(floor(uv.y*10000));
    traceRayResult = vec3(1.0, 1.0, 1.0);
    while (!traceRay())
    {
        continue;
    }



    vec2 UV2 = UV*0.5+vec2(0.5, 0.5);
    vec3 prev = texture(prevFrame, UV2).rgb;
    vec3 current = traceRayResult; // your new raytraced color
    float alpha = 1.0 / float(frameCount + 1);
    vec3 accum = mix(prev, current, alpha);
    FragColor = vec4(accum, 1.0);

}

