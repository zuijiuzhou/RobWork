require("sdurw_core")
require("sdurw")
using("sdurw")
require("sdurw_math")
using("sdurw_math")

rpy = RPY(Pi,Pi/2,0);
print("RPY: " .. tostring(rpy));
rotationFromRPY = rpy:toRotation3D();
print("Rotation from RPY: " .. tostring(rotationFromRPY));

rot = Rotation3D(-1,0,0,0,0,1,0,1,0);
print("Rotation: " .. tostring(rot));
rpyFromRotation = RPY(rot);
print("RPY from Rotation: " .. tostring(rpyFromRotation));