% clear all;
% close all;
% clc;
p=0.25;

%% Paths
addpath(genpath('~/openrobots/lib/matlab'));

addpath(genpath('/home/musabini/TwoEars/AuditoryModel/TwoEars-1.2/AuditoryFrontEnd'));
startAuditoryFrontEnd;

addpath(genpath('~/genom_ws/rosAFE/matlabFiles'));
startRosAFE;

%% Genom
client = genomix.client;
pause(p);
bass = client.load('bass');
pause(p);
rosAFE = client.load('rosAFE');
pause(p);

%% Parameters for rosAFE
sampleRate = 44100;

bufferSize_s_bass = 1;
bufferSize_s_rosAFE_port = 1;
bufferSize_s_rosAFE_getSignal = 1;
bufferSize_s_matlab = 10;

inputDevice = 'hw:1,0'; % Check your input device by bass.ListDevices();

framesPerChunk = 12000; % Each chunk is (framesPerChunk/sampleRate) seconds.

%% Data Object
dObj = dataObject_RosAFE( bass, rosAFE, inputDevice, sampleRate, framesPerChunk, bufferSize_s_bass, ...
                                                                                 bufferSize_s_rosAFE_port, ...
                                                                                 bufferSize_s_rosAFE_getSignal, ...
                                                                                 bufferSize_s_matlab );

%% Manager
mObj = manager_RosAFE(dObj);

%% Parameter settings
pp_bRemoveDC  = 1;
pp_cutoffHzDC  = 5000;
 
% Summary of parameters 
par = genParStruct('pp_bRemoveDC',pp_bRemoveDC,...
                    'pp_cutoffHzDC',pp_cutoffHzDC); 

mObj.addProcessor('ild'); % With given parameters

mObj.modifyParameter( 'time_0', 'pp_bRemoveDC', '0' );

mObj.processChunk( );

mObj.deleteProcessor( 'time_0' );
