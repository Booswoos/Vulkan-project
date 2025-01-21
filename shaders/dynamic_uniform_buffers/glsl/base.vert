#version 450
/* Copyright (c) 2019-2024, Sascha Willems
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 the "License";
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 normal;

layout (binding = 0) uniform UboView 
{
	mat4 projection;
	mat4 view;
    vec4 ambientLightColor;
    vec3 lightPosition;
    vec4 lightColor;
} uboView;

layout (binding = 1) uniform UboInstance 
{
	mat4 model;
} uboInstance;

layout (location = 0) out vec3 outColor;

out gl_PerVertex 
{
	vec4 gl_Position;
};

void main() 
{
	outColor = inColor;
	// To avoid calculating this for every vertex, matrix multiplications could be moved to the CPU-side
	mat4 modelView = uboView.view * uboInstance.model;
	vec3 worldPos = vec3(modelView * vec4(inPos, 1.0));
	gl_Position = uboView.projection * modelView * vec4(inPos.xyz, 1.0);

	//
    mat3 normalMatrix = transpose(inverse(mat3(uboInstance.model)));
    vec3 normalWorldSpace = normalize(normalMatrix * normal);

	vec3 directionToLight = uboView.lightPosition - worldPos.xyz;
    float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance squared
	vec3 lightColor = uboView.lightColor.xyz * uboView.lightColor.w;
	vec3 ambientLight = uboView.ambientLightColor.xyz * uboView.ambientLightColor.w;
	float diffuseFactor = max(dot(normalWorldSpace, normalize(directionToLight)), 0.0);
	vec3 diffuseLight = lightColor * diffuseFactor;

	//outColor = vec3(0.7,0.0,0.0) * (lightColor);
	outColor = (diffuseLight + ambientLight) * inColor;
	//outColor = vec3(attenuation);

	//outColor = normalWorldSpace;
}
