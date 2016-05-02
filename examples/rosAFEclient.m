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
mObj.addProcessor('ild'); % With default parameters
pause(p);
mObj.ModifyParameter('time_input_0_0', 'pp_bRemoveDC', '3');

while ( 1 )
    mObj.processChunk( );
end

%% ILD Params
% Parameters of crosscorrelation processor
fb_lowFreqHz  = 800;
fb_nChannels  = 30;
ild_wSizeSec  = 20;
ild_hSizeSec  = 100;

% Summary of parameters 
par = genParStruct('ild_wSizeSec',ild_wSizeSec,...
                   'ild_hSizeSec',ild_hSizeSec,...
                   'fb_lowFreqHz',fb_lowFreqHz,...
                   'fb_nChannels',fb_nChannels); 
 
mObj.addProcessor('ild', par); % With given parameters
pause(p);

mObj.deleteProcessor('filterbank_time_input_0_0_0');
% mObj.cleanup(); %TODO : FIX This

audiowrite('input.wav',[dObj.input_0{1}.Data(:) dObj.input_0{2}.Data(:)],44100)
audiowrite('preProc.wav',[dObj.time_input_0_0{1}.Data(:) dObj.time_input_0_0{2}.Data(:)],44100)

plot(dObj.time_input_0_0{1}.Data(:));
hold on;
plot(dObj.input_0{1}.Data(:),'y');