function [ bass, rosAFE, client ] = initRosAFE( openRobotsMatlabPath, twoEarsPath, rosAFEPath )
%INITROSAFE [ bass, rosAFE, client ] = initRosAFE( openRobotsPath, twoEarsPath )
%   Initialization of the needed modules.

if ( nargin == 0 )
    openRobotsMatlabPath = '~/openrobots/lib/matlab';
    twoEarsPath = '~/TwoEars/AuditoryModel/TwoEars-1.2/AuditoryFrontEnd/';
    rosAFEPath = '~/genom_ws/rosAFE/matlabFiles';
end

%% Paths
addpath(genpath(openRobotsMatlabPath));
addpath(genpath(twoEarsPath));
startAuditoryFrontEnd;

addpath(genpath(rosAFEPath));
startRosAFE;

%% Genom Modules
client = genomix.client;
bass = client.load('bass');
rosAFE = client.load('rosAFE');

end