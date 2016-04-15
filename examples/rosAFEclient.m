clear all;
close all;
clc;
p=0.25;

%% Paths
addpath(genpath('~/openrobots/lib/matlab'));
addpath(genpath('~/genom_ws/rosAFE/AuditoryModel'));
startRosAFE;

%% Genom
client = genomix.client;
bass = client.load('bass');
rosAFE = client.load('rosAFE');

%% Data Object
sampleRate = 44100;
dObj = dataObject_RosAFE( bass, rosAFE, 'hw:2,0', sampleRate );

%% Manager
mObj = manager_RosAFE(dObj);
mObj.addProcessor('ild'); % With default parameters
pause(p);

mObj.RosAFE.SendToMatlab();
dObj_RosAFE = mObj.RosAFE.dataObj();

mObj.ModifyParameter('time_input_0_0', 'pp_bRemoveDC', '3');
tic
mObj.processChunk( );
toc
%% ILD Params
% Parameters of crosscorrelation processor
fb_lowFreqHz  = 80;
fb_nChannels  = 3;
ild_wSizeSec  = 2;
ild_hSizeSec  = 10;

% Summary of parameters 
par = genParStruct('ild_wSizeSec',ild_wSizeSec,...
                   'ild_hSizeSec',ild_hSizeSec,...
                   'fb_lowFreqHz',fb_lowFreqHz,...
                   'fb_nChannels',fb_nChannels); 
               





mObj.addProcessor('ild', par); % With given parameters
pause(p);

mObj.deleteProcessor('filterbank_time_input_0_0_0');
mObj.cleanup();