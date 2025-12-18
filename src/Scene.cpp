module;

#include <functional>
#include <string_view>
#include <vector>

export module moonstone:scene;

import :error;

export namespace moonstone
{
class scene
{
public:
	static std::vector<std::reference_wrapper<scene>> registered_scenes;
	scene() = default;
	virtual ~scene() = default;
	virtual error::result<> on_update(float delta_time)
	{
		return {};
	};
	virtual error::result<> on_render()
	{
		return {};
	};
	virtual error::result<> on_imgui_render()
	{
		return {};
	};
	virtual bool operator==(const scene& other)
	{
		return this->get_name() == other.get_name();
	};
	[[nodiscard]] virtual const char* get_name() const
	{
		return nullptr;
	};
	static void register_scene(scene& scene)
	{
		registered_scenes.emplace_back(scene);
	}
};

} // namespace moonstone
std::vector<std::reference_wrapper<moonstone::scene>>
	moonstone::scene::registered_scenes{};
