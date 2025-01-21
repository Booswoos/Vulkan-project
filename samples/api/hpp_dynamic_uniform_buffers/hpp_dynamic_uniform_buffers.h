/* Copyright (c) 2021-2024, NVIDIA CORPORATION. All rights reserved.
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

/*
 * Demonstrates the use of dynamic uniform buffers, using vulkan.hpp
 *
 * Instead of using one uniform buffer per-object, this example allocates one big uniform buffer
 * with respect to the alignment reported by the device via minUniformBufferOffsetAlignment that
 * contains all matrices for the objects in the scene.
 *
 * The used descriptor type vk::DescriptorType::eUniformBufferDynamic then allows to set a dynamic
 * offset used to pass data from the single uniform buffer to the connected shader binding point.
 */

#pragma once

#include <hpp_api_vulkan_sample.h>

#define OBJECT_INSTANCES 15

class HPPDynamicUniformBuffers : public HPPApiVulkanSample
{
  public:
	HPPDynamicUniformBuffers();
	~HPPDynamicUniformBuffers();

  private:
	// One big uniform buffer that contains all matrices
	// Note that we need to manually allocate the data to cope for GPU-specific uniform buffer offset alignments
	struct UboDataDynamic
	{
		glm::mat4 *model = nullptr;
	};

	struct UboVS
	{
		glm::mat4 projection;
		glm::mat4 view;
        glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .02f};
        glm::vec3 lightPosition{-16.0f, -0.0f, -16.0f};
        alignas(16) glm::vec4 lightColor{1.0f, 1.0f, 1.0f, 1.0f};
	};

	struct UniformBuffers
	{
		std::unique_ptr<vkb::core::BufferCpp> view;
		std::unique_ptr<vkb::core::BufferCpp> dynamic;
	};

	struct Vertex
	{
		float pos[3];
		float color[3];
        float normal[3];
        Vertex() {}

        Vertex(const glm::vec3 &position, const glm::vec3 &color, const glm::vec3 &normal)
        {
            set_position(position);
            set_color(color);
            set_normal(normal);

        }

        void set_position(const glm::vec3 &position)
        {
            pos[0] = position.x;
            pos[1] = position.y;
            pos[2] = position.z;
        }

        void set_color(const glm::vec3 &color)
        {
            this->color[0] = color.r;
            this->color[1] = color.g;
            this->color[2] = color.b;
        }
        void set_normal(const glm::vec3 &normal)
        {
            this->normal[0] = normal.x;
            this->normal[1] = normal.y;
            this->normal[2] = normal.z;
        }

        // assignment for color, not really generic
        // and wont work for position!
        Vertex &operator=(const glm::vec3 &new_color)
        {
            set_color(new_color);
            return *this;
        }

	};

  private:
	static void *aligned_alloc(size_t size, size_t alignment);
	static void  aligned_free(void *data);

  private:
	// from vkb::Application
	bool prepare(const vkb::ApplicationOptions &options) override;
	bool resize(const uint32_t width, const uint32_t height) override;

	// from HPPApiVulkanSample
	void render(float delta_time) override;
	void build_command_buffers() override;

	vk::DescriptorPool      create_descriptor_pool();
	vk::DescriptorSetLayout create_descriptor_set_layout();
	vk::Pipeline            create_pipeline();
	void                    draw();
	void                    generate_cube();
	void                    prepare_camera();
	void                    prepare_uniform_buffers();
	void                    update_descriptor_set();
	void                    update_dynamic_uniform_buffer(float delta_time, bool force = false);
	void                    update_uniform_buffers();
    void                    update_colors(Vertex& vertex);

  private:
	float                                 animation_timer = 0.0f;
	vk::DescriptorSet                     descriptor_set;
	vk::DescriptorSetLayout               descriptor_set_layout;
	size_t                                dynamic_alignment = 0;
	std::unique_ptr<vkb::core::BufferCpp> index_buffer;
	uint32_t                              index_count = 0;
	vk::Pipeline                          pipeline;
	vk::PipelineLayout                    pipeline_layout;
	glm::vec3                             rotations[OBJECT_INSTANCES];        // Store random per-object rotations
	glm::vec3                             rotation_speeds[OBJECT_INSTANCES];
	UboDataDynamic                        ubo_data_dynamic;
	UboVS                                 ubo_vs;
	UniformBuffers                        uniform_buffers;
	std::unique_ptr<vkb::core::BufferCpp> vertex_buffer;
};

std::unique_ptr<vkb::Application> create_hpp_dynamic_uniform_buffers();
