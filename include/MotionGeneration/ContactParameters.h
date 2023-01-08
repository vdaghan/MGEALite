#pragma once

struct ContactParameters {
	double stiffness; // Default 1e6 N/m in MATLAB
	double damping; // Default 1e3 N(/m/S) in MATLAB
	double transitionRegionWidth; // Default 1e-4m in MATLAB
	double staticFriction; // Default 0.5 in MATLAB
	double dynamicFriction; // Default 0.3 in MATLAB
	double criticalVelocity; // Default 1e-3 m/s in MATLAB
};

ContactParameters defaultContactParameters();
ContactParameters bodyGroundContactParameters();
