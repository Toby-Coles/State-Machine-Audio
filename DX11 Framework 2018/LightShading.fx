


struct PointLight {

    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

    float3 Position;
    float Range;

    float3 Attenuation;
    float pad;
};

struct SpotLight {
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

    float3 Position;
    float Range;
    float3 Attenuation;
    float3 Direction;
    float Spot;
    float pad;
};

struct Material {
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Reflect;
};

void ProcessSpotLight(Material material, SpotLight light, float3 pos, float3 normal, float3 toEye, out float4 ambient, out float4 diffuse, out float4 spec) {
    
    // Initialize outputs.
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // vector from the surface to the light.
    float3 lightVec = light.Position - pos;

    //distance from surface to light.
    float d = length(lightVec);
    
    // Range test.
    if (d > light.Range)
        return;

    // Normalize the light vector.
    lightVec /= d;

    // Ambient term.
    ambient = material.Ambient * light.Ambient;

    // Add diffuse and specular term, provided the surface is in
    // the line of site of the light.
    float diffuseFactor = dot(lightVec, normal);

    // Flatten to avoid dynamic branching.
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);

        float specFactor = pow(max(dot(v, toEye), 0.0f), material.Specular.w);

        diffuse = diffuseFactor * material.Diffuse * light.Diffuse;
        spec = specFactor * material.Specular * light.Specular;
    }

    // Scale by spotlight factor and attenuate.
    float spot = pow(max(dot(-lightVec, light.Direction), 0.0f), light.Spot);

    // Scale by spotlight factor and attenuate.
    float att = spot / dot(light.Attenuation, float3(1.0f, d, d * d));

    //ambient *= spot;
    //diffuse *= att;
    //spec *= att;


}
void ProcessPointLight(Material material, PointLight light, float3 pos, float3 normal, float3 toEye, out float4 ambient,
    out float4 diffuse, out float4 specular)
{

    //Initiallize outputs
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    //vector from the surface to the light
    float3 lightVec = light.Position - pos;

    //Distance from surface to light
    float distance = length(lightVec);

    //Range test
    if (distance > light.Range)
    {
        return;
    }

    //Normalize light vector
    lightVec /= distance;

    //Ambient term
    ambient = material.Ambient * light.Ambient;

    //add diffuse and specular term, provided surface is in line of site
    float diffuseFactor = dot(lightVec, normal);
    
    //Flatten to avoid dynamic branching
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), material.Specular.w);

        diffuse = diffuseFactor * material.Diffuse * light.Diffuse;
        specular = specFactor * material.Specular * light.Specular;
    
    }

    //Attenuate
    float att = 1.0f / dot(light.Attenuation, float3(1.0f, distance, distance * distance));

    diffuse *= att;
    specular *= att;

}