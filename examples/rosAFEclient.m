%clear all;
%close all;
%clc;
p=0.25;

%% Paths
addpath(genpath('~/openrobots/lib/matlab'));
addpath(genpath('~/TwoEars/AuditoryModel/TwoEars-1.2/RosAFE/'));
startRosAFE;

%% Genom
client = genomix.client;
bass = client.load('bass');
rosAFE = client.load('rosAFE');
rosAFE.connect_port('Audio', 'bass/Audio');

%% Params
sampleRate = 44100;
nFramesPerChunk = 2205;
nChunksOnPort = 20;
bufferSize_s = 1;

%% ILD Params
% Parameters of crosscorrelation processor
fb_lowFreqHz  = 8;
ild_wSizeSec  = 2;
ild_hSizeSec  = 1;

% Summary of parameters 
par = genParStruct('ild_wSizeSec',ild_wSizeSec,...
                   'ild_hSizeSec',ild_hSizeSec,...
                   'fb_lowFreqHz',fb_lowFreqHz); 
               
%% BASS
acquire = bass.Acquire('-a', 'hw:2,0', sampleRate, nFramesPerChunk, nChunksOnPort);
pause(p);

%% Manager
mObj = manager_rosAFE(rosAFE);

mObj.addProcessor('ild'); % With default parameters
pause(p);
mObj.addProcessor('ild', par); % With given parameters
pause(p);

mObj.ModifyParameter('ild_innerhaircell_filterbank_time_input_0_0_0_0_0', 'ild_wSizeSec', '3');
mObj.deleteProcessor('filterbank_time_input_0_0_0');
mObj.cleanup();