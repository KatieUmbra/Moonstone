module;

#include <functional>
#include <string>
#include <vector>

export module moonstone:scene;

export namespace moonstone
{
class scene
{
public:
	static std::vector<std::reference_wrapper<scene>> registered_scenes;
	scene() = default;
	virtual ~scene() = default;
	virtual void on_update(float delta_time)
	{
	}
	virtual void on_render()
	{
	}
	virtual void on_imgui_render()
	{
	}
	virtual bool operator==(const scene& other)
	{
		return false;
	}
	[[nodiscard]] virtual std::string get_name() const
	{
		return "";
	}
	static void register_scene(scene& scene)
	{
		registered_scenes.emplace_back(scene);
	}
};

} // namespace moonstone
std::vector<std::reference_wrapper<moonstone::scene>> moonstone::scene::registered_scenes{};
