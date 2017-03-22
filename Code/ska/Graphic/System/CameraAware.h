#pragma once
namespace ska {
	class CameraSystem;

	class CameraAware {
	public:
		virtual void linkCamera(CameraSystem* cs) = 0;
	};
}
