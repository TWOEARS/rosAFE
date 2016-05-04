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
bufferSize_s = 10;

dObj = dataObject_RosAFE( bass, rosAFE, 'hw:2,0', sampleRate, bufferSize_s );

%% Manager
mObj = manager_RosAFE(dObj);

%% Parameter settings
pp_bRemoveDC  = 1;
pp_cutoffHzDC  = 5000;
 
% Summary of parameters 
par = genParStruct('pp_bRemoveDC',pp_bRemoveDC,...
                    'pp_cutoffHzDC',pp_cutoffHzDC); 

mObj.addProcessor('time', par); % With given parameters
pause(p);

mObj.addProcessor('ild'); % With default parameters
pause(p);

mObj.processChunk( );

mObj.deleteProcessor('filterbank_time_input_0_0_0');
% mObj.cleanup(); %TODO : FIX This

audiowrite('input.wav',[dObj.input_0{1}.Data(:) dObj.input_0{2}.Data(:)], sampleRate);
audiowrite('preProc.wav',[dObj.time_input_0_0{1}.Data(:) dObj.time_input_0_0{2}.Data(:)], sampleRate);

plot(dObj.input_0{1}.Data(:),'b');
hold on;
plot(dObj.time_input_0_0{1}.Data(:),'y');
