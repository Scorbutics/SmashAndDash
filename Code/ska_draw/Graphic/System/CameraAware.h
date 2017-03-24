#pragma once
namespace ska {
	class CameraSystem;

	class CameraAware {
	public:
	    CameraAware() = default;
	    virtual ~CameraAware() = default;
		virtual void linkCamera(CameraSystem* cs) = 0;
	};
}
